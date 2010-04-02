#! /bin/sh

ls VERSION >/dev/null && 
ls configure.in.in >/dev/null &&
ls COPYING >/dev/null &&
echo generating configure.in &&
sed "s/@@VERSION@@/$(cat VERSION)/" configure.in.in >configure.in &&
echo aclocal &&
aclocal &&
echo autoheader &&
autoheader &&
echo libtoolize --copy &&
libtoolize --copy &&
echo automake --add-missing --copy &&
automake --add-missing --copy &&
echo autoconf &&
autoconf &&
echo libtoolize --copy --install &&
libtoolize --copy --install &&
echo intltoolize --copy --force &&
intltoolize --copy --force 
