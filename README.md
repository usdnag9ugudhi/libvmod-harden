# libvmod-harden

## About

A Varnish VMOD that aborts the TLS handshake when there is no server
certificate matching the client's SNI (Server Name Indication). Varnish
uses the **last declared certificate** as the fallback when no cert
matches the requested hostname or IP; this VMOD ensures that the
fallback is not used for a different host by installing a certificate
callback on every `SSL_CTX`. Both hostname and IP address certificates
(SAN dNSName and iPAddress) are supported.

## Requirements

To build this VMOD you will need:

* make
* a C compiler, e.g. GCC or clang
* pkg-config
* python3-docutils or docutils in macOS [1]
* Varnish 7.5 or later from https://varnish.org/
* libssl-dev in Debian/Ubuntu, openssl-devel in Fedora/RHEL.
  See also https://www.openssl.org/

If you are building from Git, you will also need:

* autoconf
* automake
* libtool

You will also need to set `PKG_CONFIG_PATH` to the directory where
**varnishapi.pc** is located before running `./bootstrap` and
`./configure`. For example:

```
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
```

## Installation

### From the Git repository

To install from Git, clone this repository and run:

```
./bootstrap
```

And then follow the instructions above for installing from a tarball.

## Usage

Load the VMOD in VCL; no functions need to be called. The behaviour is
active as soon as the VMOD is loaded (at VCL warm). Connections where
the client sends no SNI, or the SNI does not match the selected
certificate, will have the TLS handshake aborted.

```
import harden;

sub vcl_recv {
    # harden runs automatically; no per-request calls
}
```

## License

This VMOD is licensed under the Unlicense. See LICENSE for details.

### Note

1. Using Homebrew, https://github.com/Homebrew/brew/.
