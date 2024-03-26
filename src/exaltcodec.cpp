/***************************************************************************
    exaltcodec.cpp  -  Definition of ExaltCodec class methods
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
  \file exaltcodec.cpp
  \brief Definition of ExaltCodec class methods.
  
  This file contains the definition of ExaltCodec class methods.
*/


#ifdef __GNUG__
# pragma implementation
#endif


#include "exaltcodec.h"


/*!
  \brief Displays total running time of compression/decompression.
 */
#define SHOW_RUNNING_TIME(_t1_, _t2_)								\
{												\
  OUTPUTENL("  Running time: \t\t\t" << (double)(_t2_ - _t1_) / CLOCKS_PER_SEC << " s");	\
  OUTPUTEENDLINE;										\
}



/*!
  Creates an instance of XmlCodec.
 */
ExaltCodec::ExaltCodec(void)
  : XmlCodecBase()
{
  NEW(xmlCodec, XmlCodec());
}



/*!
  Deletes the instance of XmlCodec.
 */
ExaltCodec::~ExaltCodec(void)
{
  if (xmlCodec)
    DELETE(xmlCodec);

  deleteDefaultTextCodec();
}
/*!
  Initializes the codec in PUSH mode with given output device.

  \param outDevice The output device used for output of compressed data.
 */
void ExaltCodec::initializePushCoder(IODevice *outDevice)
{
  xmlCodec->initializePushCoder(outDevice);
}



/*!
  Initializes the codec in PUSH mode with given output file name.

  \param outFileName The file used for output of compressed data.
 */
void ExaltCodec::initializePushCoder(const char *outFileName)
{
  FileDevice *outFileDevice;

  NEW(outFileDevice, FileDevice);
  outFileDevice->prepare(outFileName, ios::out);

  xmlCodec->initializePushCoder(outFileDevice);
}



/*!
  The coder encodes given chunk of data.
  
  \param data The chunk of XML data.
  \param length The length of the data.
  \param isFinal Indication whether the coder received the last chunk of data.
*/
bool ExaltCodec::encodePush(const char *data, int length, bool isFinal = false)
{
  return xmlCodec->encodePush(data, length, isFinal);
}





/*!
  The codec encodes the XML data from one device to another.

  \param inDevice The input device.
  \param outDevice The output device.
*/
bool ExaltCodec::encode(IODevice *inDevice, IODevice *outDevice)
{
  bool res;

  clock_t t1, t2;

  t1 = clock();

  if (!textCodec)
    //use default TextCodec if none has been specified
    createDefaultTextCodec();

    {
      if (textCodec)
	xmlCodec->setTextCodec(textCodec);

      res = xmlCodec->encode(inDevice, outDevice);
    }

  t2 = clock();
  if (ExaltOptions::getOption(ExaltOptions::Verbose) == ExaltOptions::Yes)
    {
      SHOW_RUNNING_TIME(t1, t2);
    }

  return res;
}



/*!
  The codec encodes the XML data from one file to another.

  \param inFileName The input file.
  \param outFileName The output file.
*/
bool ExaltCodec::encode(const char *inFileName, const char *outFileName)
{
  FileDevice *inFileDevice, *outFileDevice;

  NEW(inFileDevice, FileDevice);
  inFileDevice->prepare(inFileName, ios::in);

  NEW(outFileDevice, FileDevice);
  outFileDevice->prepare(outFileName, ios::out);

  bool res = encode(inFileDevice, outFileDevice);

  inFileDevice->finish();
  outFileDevice->finish();

  DELETE(inFileDevice);
  DELETE(outFileDevice);

  return res;
}



/*!
  The codec decodes the XML data from one device to another.

  \param inDevice The input device.
  \param outDevice The output device.
*/
bool ExaltCodec::decode(IODevice *inDevice, IODevice *outDevice)
{
  OutputSAXReceptor *receptor;
  bool res;

  clock_t t1, t2;


  t1 = clock();

  if (!textCodec)
    //create default text codec if none has been specified
    createDefaultTextCodec();

  //we create default SAX receptor which outputs to the outDevice
  NEW(receptor, OutputSAXReceptor(outDevice));


    {
      if (textCodec)
	xmlCodec->setTextCodec(textCodec);

      res = xmlCodec->decode(inDevice, receptor);

    }

  t2 = clock();

  if (ExaltOptions::getOption(ExaltOptions::Verbose) == ExaltOptions::Yes)
    {
      SHOW_RUNNING_TIME(t1, t2);
    }

  DELETE(receptor);

  return res;
}



/*!
  The codec decodes the XML data from one file to another.

  \param inFileName The input file.
  \param outFileName The output file.
*/
bool ExaltCodec::decode(const char *inFileName, const char *outFileName)
{
  FileDevice *inFileDevice, *outFileDevice;

  NEW(inFileDevice, FileDevice);
  inFileDevice->prepare(inFileName, ios::in);

  NEW(outFileDevice, FileDevice);
  outFileDevice->prepare(outFileName, ios::out);

  bool res = decode(inFileDevice, outFileDevice);

  outFileDevice->finish();

  DELETE(inFileDevice);
  DELETE(outFileDevice);

  return res;
}



/*!
  The codec decodes the XML data from the input device and sends SAX events to specified SAX receptor.

  \param inDevice The input device.
  \param receptor The SAX event receptor.
  \param userData The pointer to the user data structure.
*/
bool ExaltCodec::decode(IODevice *inDevice, SAXReceptor *receptor, void *userData = 0)
{
  XmlCodec *xmlCodec;
  bool res;

  NEW(xmlCodec, XmlCodec);

  res = xmlCodec->decode(inDevice, receptor, userData);

  DELETE(xmlCodec);

  return res;
}



/*!
  The codec decodes the XML data from given file and sends SAX events to specified SAX receptor.

  \param inFileName The input file.
  \param receptor The SAX event receptor.
  \param userData The pointer to the user data structure.
*/
bool ExaltCodec::decode(const char *inFileName, SAXReceptor *receptor, void *userData = 0)
{
  FileDevice *inFileDevice;

  NEW(inFileDevice, FileDevice);

  try
    {
      inFileDevice->prepare(inFileName, ios::in);
    }
  catch (ExaltException)
    {
      FATAL("Unable to set \"" << inFileName <<"\" as input device!");
    }

  bool res = decode(inFileDevice, receptor, userData);

  DELETE(inFileDevice);

  return res;
}
