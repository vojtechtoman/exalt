/***************************************************************************
    textcodec.h  -  Definition of TextCodec class.
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
  \file textcodec.h
  \brief Definition of TextCodec class.

  This file contains definition of TextCodec class.
*/

#ifdef __GNUG__
# pragma interface
#endif

#ifndef TEXTCODEC_H
#define TEXTCODEC_H





#include "defs.h"
#include "xmldefs.h"
#include "xmlchar.h"
#include "encodings.h"
#include "iodevice.h"


//! The default size of the alphabet.
#define DEFAULT_ALPHABET_BASE_SIZE	256



/*!
  \brief A simple class for character sets conversion.

  The functionality of the codec if rather basic, but it offers an unified interface for character sets conversions.
 */
class TextCodec
{
public:
  //! Is the codec able to convert specified encoding?
  virtual bool isAbleToConvert(Encodings::MIB mib);

  //! Fill in the expat encoding structure.
  virtual void fillInMapArray(XmlEncoding *info, Encodings::MIB mib);

  //! Convert given char to gven encoding.
  virtual inline int convert(const char *s, Encodings::MIB mib);

  //! Release function called by expat.
  virtual inline void release(Encodings::MIB mib);

  //! Get the MIB of the encoding.
  virtual Encodings::MIB getMIB(const XmlChar *encoding) throw (ExaltUnknownEncodingException);

  //! Does the codec know the encoding?
  virtual bool knowsMIB(Encodings::MIB mib);

  //! Suggest suitable alphabet size for given encoding.
  virtual inline unsigned long suggestAlphabetBaseSize(Encodings::MIB mib) throw (ExaltUnknownEncodingException);

  //! Output a char converted to given encoding.
  virtual inline void output(IODevice *device, XmlChar c, Encodings::MIB toEncoding) throw (ExaltEncodingException, ExaltIOException);

  //! Output a string converted to given encoding.
  virtual inline void output(IODevice *device, const XmlChar *str, Encodings::MIB toEncoding) throw (ExaltEncodingException, ExaltIOException);

  //! Output a string of given length converted to given encoding.
  virtual inline void output(IODevice *device, const XmlChar *str, size_t length, Encodings::MIB toEncoding) throw (ExaltEncodingException, ExaltIOException);
};



/*!
  \brief Simple interface for accessing the TextCodec functionality.

  All of the classes in Exalt that use the TextCodec inherit this class.
 */
class UserOfTextCodec
{
public:
  /*!
    \brief A constructor.

    Initialization is performed.
   */
  UserOfTextCodec(void) { textCodec = 0; defaultTextCodecUsed = false; }

  /*!
    \brief Set the text codec.

    Sets a (non-default) text codec.

    \param codec The text codec.
   */
  virtual void setTextCodec(TextCodec *codec) { textCodec = codec; defaultTextCodecUsed = false; }

protected:
  //! Pointer to the text codec instance.
  TextCodec *textCodec;

  //! Are we using the default text codec?
  bool defaultTextCodecUsed;

  /*!
    \brief Create a default text codec.

    This method can be used if no text codec has been specified by setTextCodec().
   */
  void createDefaultTextCodec(void)
  {
    NEW(textCodec, TextCodec);
    defaultTextCodecUsed = true;
  }

  /*!
    \brief Delete the default text codec.

    Deletes the default text codec.
   */
  void deleteDefaultTextCodec(void)
  {
    if (defaultTextCodecUsed && textCodec)
      {
	DELETE(textCodec);
	defaultTextCodecUsed = false;
      }
  }
};

#endif //TEXTCODEC_H



