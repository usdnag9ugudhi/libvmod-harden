AM_CFLAGS = $(VARNISHAPI_CFLAGS)

# Modules

vmod_LTLIBRARIES = \
	libvmod_harden.la

libvmod_harden_la_LDFLAGS = $(VMOD_LDFLAGS) $(LIBSSL_LIBS)

libvmod_harden_la_SOURCES = \
	vmod_harden.c

nodist_libvmod_harden_la_SOURCES = \
	vcc_harden_if.c \
	vcc_harden_if.h

@BUILD_VMOD_HARDEN@

DISTCLEANFILES = vmod_vcs_version.txt

# Test suite

AM_TESTS_ENVIRONMENT = \
	PATH="$(abs_builddir):$(VARNISH_TEST_PATH):$(PATH)" \
	LD_LIBRARY_PATH="$(VARNISH_LIBRARY_PATH)"
TEST_EXTENSIONS = .vtc
VTC_LOG_COMPILER = varnishtest -v
AM_VTC_LOG_FLAGS = \
	-p vcl_path="$(abs_top_srcdir)/vcl:$(VARNISHAPI_VCLDIR)" \
	-p vmod_path="$(abs_builddir)/.libs:$(vmoddir):$(VARNISHAPI_VMODDIR)"

TESTS = \
	vtc/vmod_harden_tls.vtc \
	vtc/vmod_harden_close.vtc

# Documentation

EXTRA_DIST = \
	etc/example.com.pem

dist_doc_DATA = \
	vmod_harden.vcc \
	$(TESTS)

dist_man_MANS = \
	vmod_harden.3
