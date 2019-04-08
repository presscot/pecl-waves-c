#!/bin/bash -
php -n -d extension=waves.so  -dextension_dir=./.libs "$@"
