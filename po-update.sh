#!/bin/bash
set -x
cd po
intltool-update -po

for f in *.po
do test -f $f && intltool-update ${f%.po}
done

