#!/bin/bash

set -e

cd "$(dirname "$0")/.."
ls -1 src/*.c > po/POTFILES.in

cd po
intltool-update --pot --gettext-package=pidgin-hide-chat
sed -i 's/charset=CHARSET/charset=UTF-8/' pidgin-hide-chat.pot
