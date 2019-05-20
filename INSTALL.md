INSTALLATION OF WAVES-C PECL EXTENSION
==================================

Currently GNU/Linux platforms supported only.

*It is highly recommended to install `pgk-config`*, if it is not installed yet.

AUTOMATIC INSTALLATION
----------------------

TODO: installation via pecl (custom) repository.

MANUAL INSTALLATION
-------------------

Checkout the project or download it as archive. In the package directory run:

```
phpize
./configure --with-waves
make
```

Additionally, you may take advantage of the following flags:

--enable-waves-debug         Enable waves-c internal debugging

Optionally test the extension:

```
make test
```

Install files with root priveleges:

```
# make install
```

In php.ini, or some other configuration like
</usr/local/etc/php/conf.d/waves.ini> write:

```
extension=waves.so
```

FINALLY
-------

Restart the SAPI server (Apache, PHP-FPM etc.), if appropriate.
