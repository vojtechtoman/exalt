/***************************************************************************
    xmlchar.cpp  -  Definitions of some XmlChar specific functions.
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
  \file xmlchar.cpp
  \brief Definitions of some XmlChar specific functions.
  
  This file contains definitions XmlChar specific functions.
*/


#include "xmlchar.h"



size_t xmlchar_strlen(const XmlChar *str)
{
  size_t i;

  if (!str)
    return 0;

  for (i = 0; str[i]; i++);

  return i;
}





XmlChar *xmlchar_strcpy(XmlChar *dest, const XmlChar *src)
{
  size_t i;

  for (i = 0; src[i]; i++)
    {
      dest[i] = src[i];
    }

  dest[i] = 0;

  return dest;
}

XmlChar *xmlchar_cstrcpy(XmlChar *dest, const char *src)
{
  size_t i;

  for (i = 0; src[i]; i++)
    {
      dest[i] = (XmlChar)src[i];
    }

  dest[i] = 0;

  return dest;
}



XmlChar *xmlchar_strncpy(XmlChar *dest, const XmlChar *src, size_t n)
{
  size_t i;

  for (i = 0; src[i] && i < n; i++)
    {
      dest[i] = src[i];
    }

  if (i < n)
    dest[i] = 0;


  return dest;

}



XmlChar *xmlchar_cstrncpy(XmlChar *dest, const char *src, size_t n)
{
  size_t i;

  for (i = 0; src[i] && i < n; i++)
    {
      dest[i] = (XmlChar)src[i];
    }

  if (i < n)
    dest[i] = 0;


  return dest;

}



XmlChar *xmlchar_strcat(XmlChar *dest, const XmlChar *src)
{
  size_t i, j;

  for (j = 0; dest[j]; j++);

  for (i = 0; src[i]; i++)
    {
      dest[j+i] = src[i];
    }

  dest[j+i] = 0;

  return dest;
}


XmlChar *xmlchar_cstrcat(XmlChar *dest, const char *src)
{
  size_t i, j;

  for (j = 0; dest[j]; j++);

  for (i = 0; src[i]; i++)
    {
      dest[j+i] = (XmlChar)src[i];
    }

  dest[j+i] = 0;

  return dest;
}


XmlChar *xmlchar_strncat(XmlChar *dest, const XmlChar *src, size_t n)
{
  size_t i, j;

  for (j = 0; dest[j]; j++);

  for (i = 0; src[i] && i < n; i++)
    {
      dest[j+i] = src[i];
    }

  if (i < n)
    dest[j+i] = 0;

  return dest;
}


XmlChar *xmlchar_cstrncat(XmlChar *dest, const char *src, size_t n)
{
  size_t i, j;

  for (j = 0; dest[j]; j++);

  for (i = 0; src[i] && i < n; i++)
    {
      dest[j+i] = (XmlChar)src[i];
    }

  if (i < n)
    dest[j+i] = 0;

  return dest;
}



int xmlchar_strcmp(const XmlChar *s1, const XmlChar *s2)
{
  size_t i;

  i = 0;

  if (!s1 && s2)
    //s1 is null
    return -1;
  else
    if (s1 && !s2)
      //s2 is null
      return 1;
    else
      if (!s1 && !s2)
	//both null
	return 0;


  while (s1[i] && s2[i])
    {
      if (s1[i] == s2[i])
	//characters are equal
	i++;
      else
	if (s1[i] < s2[i])
	  //s1 is smaller
	  return -1;
	else
	  //s1 is greater
	  return 1;
    }

  if (s1[i])
    //s1 is longer
    return 1;
  else
    if (s2[i])
      //s1 is shorter
      return -1;
    else
      //s1 and s2 are equal
      return 0;
}


int xmlchar_strncmp(const XmlChar *s1, const XmlChar *s2, size_t n)
{
  size_t i;

  i = 0;

  if (!s1 && s2)
    //s1 is null
    return -1;
  else
    if (s1 && !s2)
      //s2 is null
      return 1;
    else
      if (!s1 && !s2)
	//both null
	return 0;

  while (s1[i] && s2[i] && i < n)
    {
      if (s1[i] == s2[i])
	//characters are equal
	i++;
      else
	if (s1[i] < s2[i])
	  //s1 is smaller
	  return -1;
	else
	  //s1 is greater
	  return 1;
    }

  if (s1[i] && s2[i])
    //equal (n reached)
    return 0;
  else
    if (!s1[i] && !s2[i])
      //equal
      return 0;

  if (s2[i])
    //s1 is shorter
    return -1;
  else
    //s1 is larger
    return 1;
}


int xmlchar_cstrcmp(const XmlChar *s1, const char *s2)
{
  size_t i;

  i = 0;

  if (!s1 && s2)
    //s1 is null
    return -1;
  else
    if (s1 && !s2)
      //s2 is null
      return 1;
    else
      if (!s1 && !s2)
	//both null
	return 0;


  while (s1[i] && s2[i])
    {
      if (s1[i] == s2[i])
	//characters are equal
	i++;
      else
	if (s1[i] < s2[i])
	  //s1 is smaller
	  return -1;
	else
	  //s1 is greater
	  return 1;
    }

  if (s1[i])
    //s1 is longer
    return 1;
  else
    if (s2[i])
      //s1 is shorter
      return -1;
    else
      //s1 and s2 are equal
      return 0;
}


int xmlchar_cstrncmp(const XmlChar *s1, const char *s2, size_t n)
{
  size_t i;

  i = 0;

  if (!s1 && s2)
    //s1 is null
    return -1;
  else
    if (s1 && !s2)
      //s2 is null
      return 1;
    else
      if (!s1 && !s2)
	//both null
	return 0;

  while (s1[i] && s2[i] && i < n)
    {
      if (s1[i] == s2[i])
	//characters are equal
	i++;
      else
	if (s1[i] < s2[i])
	  //s1 is smaller
	  return -1;
	else
	  //s1 is greater
	  return 1;
    }

  if (s1[i] && s2[i])
    //equal (n reached)
    return 0;
  else
    if (!s1[i] && !s2[i])
      //equal
      return 0;

  if (s2[i])
    //s1 is shorter
    return -1;
  else
    //s1 is larger
    return 1;
}
