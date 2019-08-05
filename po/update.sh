#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

pushd $DIR
pushd ..
find src/ -name "*.cpp" > po/POTFILES.in
xgettext --keyword=_ -C --add-comments --sort-output -o po/timeit-temp.pot  --files-from=po/POTFILES.in
popd

CHANGED_LINES=`diff --ignore-matching-lines="POT-Creation-Date.*" timeit.pot timeit-temp.pot |wc -l`
echo "$1"
if [ "$CHANGED_LINES" != "0" ] || [ "--force" == "$1" ]
then
	cp timeit-temp.pot timeit.pot
	echo "updating all po-files"
	for file in *.po
	do
		echo
		echo "$file:"
		msgmerge --update $file timeit.pot
	done
fi
popd
