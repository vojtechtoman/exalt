SUBDIRS =		src doc

CLEANFILES =		*~



pkgdoc_stddir =		${pkgdocdir}
pkgdoc_std_DATA =	${srcdir}/AUTHORS ${srcdir}/COPYING ${srcdir}/NEWS ${srcdir}/ChangeLog ${srcdir}/INSTALL ${srcdir}/README ${srcdir}/TODO

EXTRA_DIST =		${pkgdoc_std_DATA}


apidoc:
	@if which doxygen; then								\
	  rm -f doc/api-doc/*.html doc/api-doc/*.gif doc/api-doc/*.css;			\
	  exec doxygen;									\
	else										\
	  echo "Doxygen isn't installed - unable to generate API documentation.";	\
	fi



