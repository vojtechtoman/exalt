/***************************************************************************
    winconfig.h  -  Definitions for Windows.
                             -------------------
    begin                : June 21 2002
    copyright            : (C) 2003 by Vojtìch Toman
    email                : vtoman@lit.cz
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*!
  \file winconfig.h
  \brief Configuration header for Windows.
*/


/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* Define if you have the ANSI C header files.  */
/* #undef STDC_HEADERS */

/* Size of XML_Char */
#define SIZEOF_XML_CHAR 1

/* The number of bytes in a char.  */
#define SIZEOF_CHAR 1

/* The number of bytes in a unsigned long.  */
#define SIZEOF_UNSIGNED_LONG 4

/* Define if you have the <cstdlib> header file.  */
#define HAVE_CSTDLIB 1

/* Define if you have the <cstring> header file.  */
#define HAVE_CSTRING 1

/* Define if you have the <ctime> header file.  */
#define HAVE_CTIME 1

/* Define if you have the <cmath> header file.  */
#define HAVE_CMATH 1

/* Define if you have the <fstream> header file.  */
#define HAVE_FSTREAM 1

/* Define if you have the <iomanip> header file.  */
#define HAVE_IOMANIP 1

/* Define if you have the <iostream> header file.  */
#define HAVE_IOSTREAM 1

/* Define if you have the expat library (-lexpat).  */
#define HAVE_LIBEXPAT 1

/* Name of package */
#define PACKAGE "exalt"

/* Version number of package */
#define VERSION "0.1.0"

