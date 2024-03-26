/***************************************************************************
    xmlchar.h  -  Declarations of some XmlChar specific functions.
                             -------------------
    begin                : October 10 2002
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
  \file xmlchar.h
  \brief Declaration of some XmlChar specific functions.
  
  This file contains declarations XmlChar specific functions.
*/

#ifndef XMLCHAR_H
#define XMLCHAR_H


#include "defs.h"
#include "xmldefs.h"



/*!
  \brief Calculate the length of the XmlChar string.

  This function is an simple equivalent to strlen(), except that it works with XmlChar based string.

  \param str XmlChar string.

  \return Number of XmlChars in string.

  \sa xmlchar_strcpy()
  \sa xmlchar_strncpy()
  \sa xmlchar_strcat()
  \sa xmlchar_strncat()
 */
size_t xmlchar_strlen(const XmlChar *str);



/*!
  \brief Copy XmlChar string.

  This function is an simple equivalent to strcpy(), except that it works with XmlChar based string. It copies \a src to \a dest. Terminating 0 is included.

  \param dest Destination string.
  \param src Source string.

  \return Pointer to the destination string.

  \sa xmlchar_strlen()
  \sa xmlchar_strncpy()
  \sa xmlchar_strcat()
  \sa xmlchar_strncat()
 */
XmlChar *xmlchar_strcpy(XmlChar *dest, const XmlChar *src);



/*!
  \brief Copy char * string to XmlChar string.

  This function is an simple equivalent to strcpy(), except that it works with XmlChar based string. It copies \a src to \a dest. Terminating 0 is included.

  \param dest Destination string.
  \param src Source string.

  \return Pointer to the destination string.

  \sa xmlchar_strlen()
  \sa xmlchar_strncpy()
  \sa xmlchar_strcat()
  \sa xmlchar_strncat()
  \sa xmlchar_cstrncpy()
 */
XmlChar *xmlchar_cstrcpy(XmlChar *dest, const char *src);



/*!
  \brief Copy up to a given number of characters of a XmlChar string.

  This function is an simple equivalent to strncpy(), except that it works with XmlChar based string. It copies up to \a n XmlChars from \a src to \a dest. If there is no terminating 0 in the \a n XmlChars of \a src, \a dest will not be zero terminated.

  \param dest Destination string.
  \param src Source string.
  \param n Maximum number of copied XmlChars.

  \return Pointer to the destination string.

  \sa xmlchar_strlen()
  \sa xmlchar_strcpy()
  \sa xmlchar_strcat()
  \sa xmlchar_strncat()
 */
XmlChar *xmlchar_strncpy(XmlChar *dest, const XmlChar *src, size_t n);



/*!
  \brief Copy up to a given number of characters of char * string to a XmlChar string.

  This function is an simple equivalent to strncpy(), except that it works with XmlChar based string. It copies up to \a n XmlChars from \a src to \a dest. If there is no terminating 0 in the \a n XmlChars of \a src, \a dest will not be zero terminated. Characters of \a src are simply overtyped to XmlChar.

  \param dest Destination string.
  \param src Source string.
  \param n Maximum number of copied XmlChars.

  \return Pointer to the destination string.

  \sa xmlchar_strlen()
  \sa xmlchar_strcpy()
  \sa xmlchar_strcat()
  \sa xmlchar_strncat()
  \sa xmlchar_cstrcpy()
 */
XmlChar *xmlchar_cstrncpy(XmlChar *dest, const char *src, size_t n);



/*!
  \brief Concatenate two XmlChar strings.

  This function is an simple equivalent to strcat(), except that it works with XmlChar based string. It appends \a src to \a dest. Terminating 0 is included.

  \param dest Destination string.
  \param src Source string.

  \return Pointer to the destination string.

  \sa xmlchar_strlen()
  \sa xmlchar_strcpy()
  \sa xmlchar_strncpy()
  \sa xmlchar_strncat()
 */
XmlChar *xmlchar_strcat(XmlChar *dest, const XmlChar *src);



/*!
  \brief Append char * string to XmlChar string.

  This function is an simple equivalent to strcat(), except that it works with XmlChar based string. It appends \a src to \a dest. Terminating 0 is included. Characters of \a src are simply overtyped to XmlChar.

  \param dest Destination string.
  \param src Source string.

  \return Pointer to the destination string.

  \sa xmlchar_strlen()
  \sa xmlchar_strcpy()
  \sa xmlchar_strncpy()
  \sa xmlchar_strncat()
  \sa xmlchar_cstrncat()
*/
XmlChar *xmlchar_cstrcat(XmlChar *dest, const char *src);



/*!
  \brief Append up to a given number of characters to XmlChar string.
  
  This function is an simple equivalent to strncat(), except that it works with XmlChar based string. It appends up to \a n XmlChars from \a src to \a dest. If there is no terminating 0 in the \a n XmlChars of \a src, \a dest will not be zero terminated.

  \param dest Destination string.
  \param src Source string.
  \param n Maximum number of appended XmlChars.

  \return Pointer to the destination string.

  \sa xmlchar_strlen()
  \sa xmlchar_strcpy()
  \sa xmlchar_strncpy()
  \sa xmlchar_strcat()
*/
XmlChar *xmlchar_strncat(XmlChar *dest, const XmlChar *src, size_t n);



/*!
  \brief Append up to a given number of characters from char * string to XmlChar string.

  This function is an simple equivalent to strncat(), except that it works with XmlChar based string. It appends up to \a n XmlChars from \a src to \a dest. If there is no terminating 0 in the \a n XmlChars of \a src, \a dest will not be zero terminated. Characters of \a src are simply overtyped to XmlChar.

  \param dest Destination string.
  \param src Source string.
  \param n Maximum number of appended XmlChars.

  \return Pointer to the destination string.

  \sa xmlchar_strlen()
  \sa xmlchar_strcpy()
  \sa xmlchar_strncpy()
  \sa xmlchar_strcat()
  \sa xmlchar_cstrcat()
 */
XmlChar *xmlchar_cstrncat(XmlChar *dest, const char *src, size_t n);


/*!
  \brief Compare two XmlChar strings.

  This function is an simple equivalent to strcmp(), except that it works with XmlChar based string. It returns -1, 0 or 1 if first string is less, equal or more than the second string.

  \param s1 XmlChar string.
  \param s2 XmlChar string.

  \return -1, 0, 1.

  \sa xmlchar_strncmp()
  \sa xmlchar_strlen()
  \sa xmlchar_strcpy()
  \sa xmlchar_strcat()
 */
int xmlchar_strcmp(const XmlChar *s1, const XmlChar *s2);



/*!
  \brief Compare up to \a n characters of two XmlChar strings.

  This function is an simple equivalent to strncmp(), except that it works with XmlChar based string. It returns -1, 0 or 1 if first string is less, equal or more than the second string.

  \param s1 XmlChar string.
  \param s2 XmlChar string.
  \param n Number of compared characters.

  \return -1, 0, 1.

  \sa xmlchar_strcmp()
  \sa xmlchar_strlen()
  \sa xmlchar_strcpy()
  \sa xmlchar_strcat()
 */
int xmlchar_strncmp(const XmlChar *s1, const XmlChar *s2, size_t n);


/*!
  \brief Compare two XmlChar strings.

  This function is an simple equivalent to strcmp(), except that it works with XmlChar based string. It returns -1, 0 or 1 if first string is less, equal or more than the second string.

  \param s1 XmlChar string.
  \param s2 XmlChar string.

  \return -1, 0, 1.

  \sa xmlchar_cstrncmp()
  \sa xmlchar_strcmp()
  \sa xmlchar_strncmp()
  \sa xmlchar_strlen()
  \sa xmlchar_strcpy()
  \sa xmlchar_strcat()
 */
int xmlchar_cstrcmp(const XmlChar *s1, const char *s2);



/*!
  \brief Compare up to \a n characters of two XmlChar strings.

  This function is an simple equivalent to strncmp(), except that it works with XmlChar based string. It returns -1, 0 or 1 if first string is less, equal or more than the second string.

  \param s1 XmlChar string.
  \param s2 XmlChar string.
  \param n Number of compared characters.

  \return -1, 0, 1.

  \sa xmlchar_cstrcmp()
  \sa xmlchar_strcmp()
  \sa xmlchar_strncmp()
  \sa xmlchar_strlen()
  \sa xmlchar_strcpy()
  \sa xmlchar_strcat()
 */
int xmlchar_cstrncmp(const XmlChar *s1, const char *s2, size_t n);


#endif //XMLCHAR_H



