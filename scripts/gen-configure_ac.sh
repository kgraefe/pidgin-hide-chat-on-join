#!/bin/bash

set -e

test -f configure.ac.in

VERSION=$(scripts/gen-version.sh)

languages=""
for f in po/*.po; do
	test -f $f && languages="$languages $(basename $f .po)"
done

sed \
	-e "s/@@VERSION@@/$VERSION/" \
	-e "s/@@LANGUAGES@@/$(echo $languages)/" \
configure.ac.in >configure.ac

