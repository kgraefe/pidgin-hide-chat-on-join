#!/bin/bash

set -e

test -f VERSION
test -f configure.ac.in

languages=""
for f in po/*.po; do
	test -f $f && languages="$languages $(basename $f .po)"
done

sed \
	-e "s/@@VERSION@@/$(cat VERSION)/" \
	-e "s/@@LANGUAGES@@/$(echo $languages)/" \
configure.ac.in >configure.ac

