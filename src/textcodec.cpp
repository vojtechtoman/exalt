/***************************************************************************
    textcodec.cpp  -  Definitions of TextCodec class methods.
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
  \file textcodec.cpp
  \brief Definitions of TextCodec class methods.

  This file contains definitions of TextCodec class methods.
*/


#ifdef __GNUG__
# pragma implementation
#endif


#include "textcodec.h"


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


/*!
  \brief the table for converting US ASCII to UTF-8.
 */
static int US_ASCII_to_UTF8_table[] =
{
  //character codes greater than 127 are all marked as invalid
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1
};



/*!
  \brief the table for converting ISO-8859-2 to UTF-8.
 */
static int ISO_8859_2_to_UTF8_table[] =
{
  0xc280, 0xc281, 0xc282, 0xc283, 0xc284, 0xc285, 0xc286, 0xc287,
  0xc288, 0xc289, 0xc28a, 0xc28b, 0xc28c, 0xc28d, 0xc28e, 0xc28f,
  0xc290, 0xc291, 0xc292, 0xc293, 0xc294, 0xc295, 0xc296, 0xc297,
  0xc298, 0xc299, 0xc29a, 0xc29b, 0xc29c, 0xc29d, 0xc29e, 0xc29f,
  0xc2a0, 0xc484, 0xcb98, 0xc581, 0xc2a4, 0xc4bd, 0xc59a, 0xc2a7,
  0xc2a8, 0xc5a0, 0xc59e, 0xc5a4, 0xc5b9, 0xc2ad, 0xc5bd, 0xc5bb,
  0xc2b0, 0xc485, 0xcb9b, 0xc582, 0xc2b4, 0xc4be, 0xc59b, 0xcb87,
  0xc2b8, 0xc5a1, 0xc59f, 0xc5a5, 0xc5ba, 0xcb9d, 0xc5be, 0xc5bc,
  0xc594, 0xc381, 0xc382, 0xc482, 0xc384, 0xc4b9, 0xc486, 0xc387,
  0xc48c, 0xc389, 0xc498, 0xc38b, 0xc49a, 0xc38d, 0xc38e, 0xc48e,
  0xc490, 0xc583, 0xc587, 0xc393, 0xc394, 0xc590, 0xc396, 0xc397,
  0xc598, 0xc5ae, 0xc39a, 0xc5b0, 0xc39c, 0xc39d, 0xc5a2, 0xc39f,
  0xc595, 0xc3a1, 0xc3a2, 0xc483, 0xc3a4, 0xc4ba, 0xc487, 0xc3a7,
  0xc48d, 0xc3a9, 0xc499, 0xc3ab, 0xc49b, 0xc3ad, 0xc3ae, 0xc48f,
  0xc491, 0xc584, 0xc588, 0xc3b3, 0xc3b4, 0xc591, 0xc3b6, 0xc3b7,
  0xc599, 0xc5af, 0xc3ba, 0xc5b1, 0xc3bc, 0xc3bd, 0xc5a3, 0xcb99
};



/*!
  \brief Map a single byte encoding to UTF-8 by filling in the Expat info structure.

  \param _enc_ The name of the encoding (for example: ISO-8859-2).
  \param _info_ The Expat info structure.
*/
#define EXPAT_MAP_SINGLE_BYTE_ENCODING_TO_UTF8(_enc_, _info_)	\
{								\
  size_t i;							\
								\
  for (i = 0; i < 128; i++)					\
    _info_->map[i] = i;						\
								\
  for (i = 128; i < 256; i++)					\
    _info_->map[i] = _enc_ ## _to_UTF8_table[i-128];		\
}



/*!
  \brief Return the size of the encoding if it matches given encoding.

  \param _mib_ The MIB of the encoding.
  \param _mibGiven_ The MIB of matched encoding.
  \param _size_ The size of the encoding.
 */
#define ENCODING_SIZE(_mib_, _mibGiven_, _size_)	\
if (_mib_ == _mibGiven_)				\
  return _size_;



/*!
  \brief Return the MOB of the encoding if it matches given encoding.

  \param _enc_ The name of the encoding.
  \param _encGiven_ The name of matched encoding.
  \param _mib_ The MIB of the encoding.
 */
#define ENCODING_MIB(_enc_, _encGiven_, _mib_)		\
if (!xmlchar_cstrcmp(_encGiven_, _enc_))		\
  return _mib_;						\



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


/*!
  \param mib The MIB of the encoding.

  \return The base size of the alphabet.
 */
unsigned long TextCodec::suggestAlphabetBaseSize(Encodings::MIB mib) throw (ExaltUnknownEncodingException)
{
  ENCODING_SIZE(Encodings::UTF_8, mib, 256);
  ENCODING_SIZE(Encodings::UTF_16, mib, 256);
  ENCODING_SIZE(Encodings::ISO_8859_1, mib, 256);
  ENCODING_SIZE(Encodings::ISO_8859_2, mib, 256);
  ENCODING_SIZE(Encodings::US_ASCII, mib, 128);

  throw ExaltUnknownEncodingException();
  return DEFAULT_ALPHABET_BASE_SIZE;
}



/*!
  \param mib The MIB of the encoding.

  \retval TRUE The text codec is able to convert to the encoding.
  \retval FALSE The text codec is not able to convert to the encoding.
 */
bool TextCodec::isAbleToConvert(Encodings::MIB mib)
{
  switch (mib)
    {
    case Encodings::US_ASCII:
      //(expat will probably never cause this :)
      //just for illustration
      return true;

    case Encodings::ISO_8859_2:
      return true;

    default:
      return false;
    }
}



/*!
  This method fills in the Expat's info structure (see the Expat documentation) according to the given MIB.

  \param info Pointer to the Expat info structure.
  \param mib The MIB of the encoding.
 */
void TextCodec::fillInMapArray(XmlEncoding *info, Encodings::MIB mib)
{
  //support for UTF-8 and UTF-16 missing!
  //when XML declaration contains for example: encoding="utf8", textcodec
  //has to "convert" this!

  //IMPORTANT
  //we should to take care about how expat has been compiled
  //here we silently suppose that it uses UTF-8 encoding internally,
  //which may be simply wrong! (It can use UTF-16 as well)

  switch (mib)
    {
    case Encodings::US_ASCII:
      //the first 128 characters map directly to Unicode values (i believe)
      //next 128 are invalid
      //(expat will probably never cause this :)
      EXPAT_MAP_SINGLE_BYTE_ENCODING_TO_UTF8(US_ASCII, info);
      break;

    case Encodings::ISO_8859_2:
      EXPAT_MAP_SINGLE_BYTE_ENCODING_TO_UTF8(ISO_8859_2, info);

      break;
      
    default:
      //unable to convert: all items of the map array are marked as invalid
      for (size_t i = 0; i < 256; i++)
	info->map[i] = -1;
    }
}



/*!
  Converts a char to given encoding.

  \param s Pointer to the character.
  \param mib The MIB of the encoding.

  \return The converted value of the character.
 */
int TextCodec::convert(const char *s, Encodings::MIB mib)
{
  switch (mib)
    {
    case Encodings::US_ASCII:
      //return first char of the byte sequence
      //should be sufficient
      //(expat will probably never cause this :)
      return *s;
  
    default:
      //stupid: s is always invalid sequence
      return -1;
    }
}


/*!
  This function gets called by Expat after performing the conversion. Here it does nothing.

  \param mib The MIB of the encoding.
 */
void TextCodec::release(Encodings::MIB mib)
{
  //nothing special
}



/*!
  This method tests whether the text codec knows given encoding.

  \note If the text codec knows an the encoding, it doesn't mean that it is also able to covert to/from it!

  \param mib The MIB of the encoding.

  \retval TRUE The text codec knows the encoding.
  \retval FALSE The text codec doesn't know the encoding.
 */
bool TextCodec::knowsMIB(Encodings::MIB mib)
{
  size_t i = 0;
  Encodings::EncodingName encodingNames[] = { ENCODING_NAMES };	//ENCODING_NAMES defined in encodings.h

  while (encodingNames[i].name)
    {
      if (encodingNames[i].mib == mib)
	return true;

      i++;
    }

  return false;
}



/*!
  Returns the MIB of the encoding that was specified by its name.

  \param encoding The name of the encoding.
  
  \return The MIB of the encoding.
 */
Encodings::MIB TextCodec::getMIB(const XmlChar *encoding) throw (ExaltUnknownEncodingException)
{
  if (!encoding)
    {
#ifdef XML_UNICODE
      //implicit expat encoding is UTF-16
      return Encodings::UTF_16;
#else
      //implicit expat encoding is UTF-8
      return Encodings::UTF_8;
#endif
    }

  ENCODING_MIB("US-ASCII", encoding, Encodings::US_ASCII);

  ENCODING_MIB("KOI8_V", encoding, Encodings::KOI8_V);
  ENCODING_MIB("KOI8_R", encoding, Encodings::KOI8_R);

  ENCODING_MIB("UTF-8", encoding, Encodings::UTF_8);
  ENCODING_MIB("UTF-16", encoding, Encodings::UTF_16);

  ENCODING_MIB("ISO-8859-1", encoding, Encodings::ISO_8859_1);
  ENCODING_MIB("ISO-8859-2", encoding, Encodings::ISO_8859_2);
  ENCODING_MIB("ISO-8859-3", encoding, Encodings::ISO_8859_3);
  ENCODING_MIB("ISO-8859-4", encoding, Encodings::ISO_8859_4);
  ENCODING_MIB("ISO-8859-5", encoding, Encodings::ISO_8859_5);
  ENCODING_MIB("ISO-8859-6", encoding, Encodings::ISO_8859_6);
  ENCODING_MIB("ISO-8859-7", encoding, Encodings::ISO_8859_7);
  ENCODING_MIB("ISO-8859-8", encoding, Encodings::ISO_8859_8);
  ENCODING_MIB("ISO-8859-9", encoding, Encodings::ISO_8859_9);
  ENCODING_MIB("ISO-8859-10", encoding, Encodings::ISO_8859_10);
  ENCODING_MIB("ISO-8859-11", encoding, Encodings::ISO_8859_11);
  ENCODING_MIB("ISO-8859-13", encoding, Encodings::ISO_8859_13);
  ENCODING_MIB("ISO-8859-14", encoding, Encodings::ISO_8859_14);
  ENCODING_MIB("ISO-8859-15", encoding, Encodings::ISO_8859_15);

  ENCODING_MIB("CP_1250", encoding, Encodings::CP_1250);
  ENCODING_MIB("CP_1251", encoding, Encodings::CP_1251);
  ENCODING_MIB("CP_1252", encoding, Encodings::CP_1252);
  ENCODING_MIB("CP_1253", encoding, Encodings::CP_1253);
  ENCODING_MIB("CP_1254", encoding, Encodings::CP_1254);
  ENCODING_MIB("CP_1255", encoding, Encodings::CP_1255);
  ENCODING_MIB("CP_1256", encoding, Encodings::CP_1256);
  ENCODING_MIB("CP_1257", encoding, Encodings::CP_1257);
  ENCODING_MIB("CP_1258", encoding, Encodings::CP_1258);


  ERR("Unknown character encoding: " << encoding);
  throw ExaltUnknownEncodingException();
  return Encodings::Unknown;
}



/*!
  Sends a character converted to given encoding to the output device.

  \param device Input device.
  \param c A character.
  \param toEncoding Output encoding.
*/
void TextCodec::output(IODevice *device, const XmlChar c, Encodings::MIB toEncoding) throw (ExaltEncodingException, ExaltIOException)
{
  switch (toEncoding)
    {
    case Encodings::UTF_8:
    case Encodings::UTF_16:
      device->writeData((const char *)&c, SIZEOF_XML_CHAR);
      break;

    default:
      //unable to output in specified encoding
      ERR("Unsupported output encoding!");
      throw ExaltUnsupportedOutputEncodingException();
    }
}



/*!
  Sends a string converted to given encoding to the output device.

  \param device Input device.
  \param str A string.
  \param toEncoding Output encoding.
*/
void TextCodec::output(IODevice *device, const XmlChar *str, Encodings::MIB toEncoding) throw (ExaltEncodingException, ExaltIOException)
{
  switch (toEncoding)
    {
    case Encodings::UTF_8:
    case Encodings::UTF_16:
      for (size_t i = 0; str[i]; i++)
	device->writeData((const char *)&str[i], SIZEOF_XML_CHAR);
      break;

    default:
      //unable to output in specified encoding
      ERR("Unsupported output encoding!");
      throw ExaltUnsupportedOutputEncodingException();
    }
}



/*!
  Sends a string of specified length converted to given encoding to the output device.

  \param device Input device.
  \param str A string.
  \param length The length of the string.
  \param toEncoding Output encoding.
*/
void TextCodec::output(IODevice *device, const XmlChar *str, size_t length, Encodings::MIB toEncoding) throw (ExaltEncodingException, ExaltIOException)
{
  switch (toEncoding)
    {
    case Encodings::UTF_8:
    case Encodings::UTF_16:
      for (size_t i = 0; i < length; i++)
	device->writeData((const char *)&str[i], SIZEOF_XML_CHAR);
      break;

    default:
      //unable to output in specified encoding
      ERR("Unsupported output encoding!");
      throw ExaltUnsupportedOutputEncodingException();
    }
}

