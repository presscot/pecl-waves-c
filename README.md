Waves PECL extension

# DESCRIPTION

Waves is a PECL extension providing interface to the `libwaves_c` library.


# INSTALLATION

```
phpize --clean && phpize
aclocal && libtoolize --force && autoreconf
./configure --with-waves
make clean
make
make test
make install
```
Note, after the steps above, the PECL package can be created with the following command:

```
pecl package
```

# COPYRIGHT

Copyright © 2019 Waves Platform
