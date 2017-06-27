#!/bin/bash

set -e

test -f Makefile.am.in
test -f COPYING

./scripts/gen-configure_ac.sh

headers=""
for f in src/*.h; do
	test -f $f && headers="$headers $f"
done

sed \
	-e "s#@@HEADERFILES@@#$(echo $headers)#" \
Makefile.am.in >Makefile.am

mkdir -p m4
intltoolize --force --copy --automake
autoreconf --force --install --verbose

