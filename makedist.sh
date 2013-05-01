#!/bin/sh


if [ "$1" != "" ]; then
VERSION=`cat version`
ARCHIVE_NAME=$1-$VERSION
git archive --prefix=${ARCHIVE_NAME}/ HEAD > ${ARCHIVE_NAME}.tar
ln -s . ${ARCHIVE_NAME}
tar  --append -f ${ARCHIVE_NAME}.tar ${ARCHIVE_NAME}/version ${ARCHIVE_NAME}/src/version.h
gzip -c ${ARCHIVE_NAME}.tar > ${ARCHIVE_NAME}.tar.gz 
rm ${ARCHIVE_NAME}.tar
rm ${ARCHIVE_NAME}
fi