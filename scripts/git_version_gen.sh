#!/bin/sh

DIR="."

if [ "$#" -gt "0" ]; then
	DIR=$1
fi

GVF=version
DEF_VER=UNKNOWN

LF='
'

cd $DIR


if [ -d .git ] ||  [ -d ../.git ]
then
 	VN=$(git describe --match "[0-9]*\.[0-9]*" HEAD 2>/dev/null)
 	case "$VN" in
	*$LF*) (exit 1) ;;
	[0-9]*)
		git update-index -q --refresh
		test -z "$(git diff-index --name-only HEAD --)" ||
		VN="$VN-dirty" ;;
	esac
	VN="$VN"
else
	#Git failed to find version number. Falling back to Debian changelog.
	DEBIAN_VERSION=`head ../debian/changelog  -n 1 | cut -d'(' -f2 |cut -d')' -f1`
	VN="${DEBIAN_VERSION}"
fi
if test -r $GVF
then
	VC=$(cat $GVF)
else
	VC=unset
fi
test "$VN" = "$VC" || {
	echo "$VN" > $GVF
	echo "writing $2"
	echo "constexpr auto version = \"$VN\";" > $2
}
