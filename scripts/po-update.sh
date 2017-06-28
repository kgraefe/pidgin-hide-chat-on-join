#!/bin/bash

set -e

cd "$(dirname "$0")/.."
ls -1 src/*.c > po/POTFILES.in

cd po
intltool-update --pot --gettext-package=pidgin-hide-chat

for f in *.po; do
	test -f $f && intltool-update ${f%.po}
done

exit 0

