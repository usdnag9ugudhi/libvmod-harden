# libvmod-harden

## About

**libvmod-harden** tightens HTTPS when you have **several certificates**:
Varnish otherwise may use the **last** certificate as a fallback for a
non-matching SNI, which can show the wrong cert for the wrong host. This
VMOD **aborts the TLS handshake** unless the certificate matches the
client's SNI (hostname or IP). It also aborts if there is no SNI or no
certificate.

`harden.close()` closes the client connection without sending an HTTP
page—similar in spirit to nginx `return 444`. Use it only from
client-side VCL (e.g. `vcl_recv`).

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

Import `harden` in VCL. Handshake checks run automatically once the VMOD
is loaded. Optionally call `harden.close()` to drop a client (no
response body from that call).

```
import harden;

sub vcl_recv {
    if (req.http.X-Drop == "1") {
        harden.close();
    }
}
```

## License

This VMOD is licensed under the Unlicense. See LICENSE for details.

### Note

1. Using Homebrew, https://github.com/Homebrew/brew/.
