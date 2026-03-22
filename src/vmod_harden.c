/* harden VMOD: abort TLS when no cert matches SNI (hostname or IP). */

#include "config.h"

#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <openssl/ssl.h>
#include <openssl/tls1.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <openssl/x509v3.h>

#include "cache/cache_varnishd.h"
#include "vcl.h"
#include "vcc_harden_if.h"

static pthread_once_t harden_once_ctrl = PTHREAD_ONCE_INIT;

static int harden_cert_cb(SSL *ssl, void *arg);

static void
harden_ctx_new_cb(void *parent, void *ptr, CRYPTO_EX_DATA *ad,
    int idx, long argl, void *argp)
{
	(void)ptr;
	(void)ad;
	(void)idx;
	(void)argl;
	(void)argp;
	SSL_CTX_set_cert_cb((SSL_CTX *)parent, harden_cert_cb, NULL);
}

static void
harden_init_once(void)
{
	(void)SSL_CTX_get_ex_new_index(0, NULL, harden_ctx_new_cb, NULL, NULL);
}

static int
is_ip_address(const char *name)
{
	struct in_addr a4;
	struct in6_addr a6;

	return (inet_pton(AF_INET, name, &a4) == 1 ||
	    inet_pton(AF_INET6, name, &a6) == 1);
}

/* Strip optional RFC 3986 / URL-style brackets around IPv6 for SNI. */
static const char *
sni_ip_strip_brackets(const char *sni, size_t len, size_t *out_len)
{
	if (len >= 2 && sni[0] == '[' && sni[len - 1] == ']') {
		*out_len = len - 2;
		return sni + 1;
	}
	*out_len = len;
	return sni;
}

static int
harden_cert_cb(SSL *ssl, void *arg)
{
	const char *sni;
	const char *ip_name;  /* SNI or stripped IPv6 for cert check */
	X509 *cert;
	size_t n;
	size_t ip_len;
	int match;
	char ip_buf[INET6_ADDRSTRLEN + 1];

	(void)arg;

	cert = SSL_get_certificate(ssl);
	sni = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name);
	if (cert == NULL || sni == NULL || sni[0] == '\0')
		return (0);
	n = strlen(sni);
	ip_name = sni_ip_strip_brackets(sni, n, &ip_len);

	/* Bracketed SNI (e.g. [::1]) must be copied to get a null-terminated IP string. */
	if (ip_name != sni) {
		if (ip_len == 0 || ip_len > INET6_ADDRSTRLEN)
			return (0);
		memcpy(ip_buf, ip_name, ip_len);
		ip_buf[ip_len] = '\0';
		ip_name = ip_buf;
	}
	if (is_ip_address(ip_name)) {
		match = X509_check_ip_asc(cert, ip_name, 0) == 1;
	} else
		match = X509_check_host(cert, sni, n, 0, NULL) == 1;
	if (!match)
		return (0);
	return (1);
}

VCL_VOID
vmod_close(VRT_CTX)
{
	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	Req_Fail(ctx->req, SC_TX_ERROR);
}

int
vmod_event(VRT_CTX, struct vmod_priv *priv, enum vcl_event_e e)
{
	(void)ctx;
	(void)priv;
	if (e == VCL_EVENT_WARM)
		pthread_once(&harden_once_ctrl, harden_init_once);
	return (0);
}
