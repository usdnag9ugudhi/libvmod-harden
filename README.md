# libvmod-harden

## About

**libvmod-harden** tightens Varnish's sni-nomatch-abort strictness.
It does not reject connections when SNI is not present at all. This
VMOD aborts the TLS handshake if so. Once this VMOD is loaded, there
is no possibility to unload it.

`harden.close()` drops the client without an HTTP response.
It is restricted to client subroutines only. ESIs are not compatible
with this.

## Requirements

To build this VMOD you will need:

* make
* a C compiler, e.g. GCC or clang
* pkg-config
* python3-docutils or docutils in macOS [1]
* Varnish 9.1 or later from https://varnish.org/ (including `varnish-dev`
  or `varnish-devel` and `vtest`)
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

Import `harden` in VCL; TLS checks run on load. With `harden.close()`, use
an immediate `return (synth(...));` or `return (fail);`.

```
import harden;

sub vcl_recv {
    if (req.http.X-Drop == "1") {
        harden.close();
        return (synth(503, "Connection closed"));
    }
}
```

## License

This VMOD is licensed under the Unlicense. See LICENSE for details.

### Note

1. Using Homebrew, https://github.com/Homebrew/brew/.
