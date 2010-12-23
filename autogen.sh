#! /bin/sh
set -x

test -f VERSION || exit
test -f ChangeLog|| exit
test -f configure.in.in || exit
test -f COPYING || exit

languages=""
for f in po/*.po
do test -f $f && languages="$languages $(basename $f .po)"
done

sed -e "s/@@VERSION@@/$(cat VERSION)/" -e "s/@@LANGUAGES@@/$(echo $languages)/" configure.in.in >configure.in || exit
aclocal || exit
autoheader || exit
libtoolize --copy || exit
automake --add-missing --copy || exit
autoconf || exit
libtoolize --copy --install || exit
intltoolize --copy --force  || exit
