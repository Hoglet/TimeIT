## Process this file with automake to produce Makefile.in
## Created by Anjuta

ACLOCAL_AMFLAGS = -I m4

man1_MANS = timeit.man

SUBDIRS = src po \
	doc  images

timeitdocdir = ${datadir}/doc/timeit
timeitdoc_DATA = \
	README\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS

timeitlauncherdir = ${prefix}/share/applications

timeitlauncher_DATA = \
	timeit.desktop

EXTRA_DIST = $(timeitdoc_DATA) \
	$(images_DATA) \
	TODO.tasks \
	gpl-2.0.txt \
	debian \
	$(timeitlauncher_DATA)



# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done


