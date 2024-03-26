## The binary target
bin_PROGRAMS =		exalt



############################################################



## Additional files to be removed during 'make clean'
CLEANFILES =		*~



############################################################



## Helper headers
HELPERHEADERS =		exceptions.h		\
			collection.h		\
			list.h			\
			stack.h			\
			queue.h			\
			hashtable.h		\
			iodevice.h		\
			filedevice.h		\
			funneldevice.h		\
			encodings.h		\
			textcodec.h		\
			fibonacci.h		\
			saxemitter.h		\
			saxreceptor.h		\
			outputsaxreceptor.h	\
			options.h		\
			exaltcodec.h


## Helper sources
HELPERSOURCES =		filedevice.cpp		\
			funneldevice.cpp	\
			textcodec.cpp		\
			fibonacci.cpp		\
			saxemitter.cpp		\
			outputsaxreceptor.cpp	\
			exaltcodec.cpp		\
			options.cpp



## XML relevant headers
XMLHEADERS = 		xmldefs.h		\
			xmlchar.h		\
			saxbase.h		\
			xmlparserbase.h		\
			xmlparser.h		\
			xmlmodelevents.h	\
			xmlmodelbase.h		\
			xmlsimplemodel.h	\
			xmladaptivemodel.h	\
			elementmodel.h		\
			xmlcodecbase.h		\
			xmlcodec.h

## XML relevant sources
XMLSOURCES =		xmlchar.cpp		\
			xmlparser.cpp		\
			xmlsimplemodel.cpp	\
			xmladaptivemodel.cpp	\
			elementmodel.cpp	\
			xmlcodec.cpp



## Arithmetic coding relevant headers
ARITHHEADERS =		arithdefs.h		\
			arithcodecbase.h	\
			arithcodec.h		\
			contextbase.h		\
			context.h		\
			unroll.h

## Arithmetic coding relevant sources
ARITHSOURCES =		context.cpp		\
			arithcodec.cpp



## Kieffer-Yang coding relevant headers
KYHEADERS =		grammardefs.h		\
			grammarbase.h		\
			kydefs.h		\
			kygrammar.h

## Kieffer-Yang coding relevant sources
KYSOURCES =		kygrammar.cpp



############################################################



exalt_SOURCES =		main.cpp

exalt_LDADD =           libexalt.la


lib_LTLIBRARIES = 	libexalt.la


libexalt_la_SOURCES =	$(XMLSOURCES)		\
			$(ARITHSOURCES)		\
			$(KYSOURCES)		\
			$(HELPERSOURCES)


include_HEADERS = 	defs.h			\
			debug.h			\
			$(XMLHEADERS)		\
			$(ARITHHEADERS)		\
			$(KYHEADERS)		\
			$(HELPERHEADERS)



