/***************************************************************************
    xmlcodec.h  -  Definition of XmlCodec class
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
  \file xmlcodec.cpp
  \brief Definitions of XmlCodec class methods.
  
  This file contains the definitions of XmlCodec class methods.
*/


#ifdef __GNUG__
# pragma implementation
#endif

#include "xmlcodec.h"


//! Display a brief compression summary.
#define DISPLAY_COMPRESSION_SUMMARY							\
{											\
  OUTPUTENL("Compression performance");							\
  OUTPUTENL("  Input data size: \t\t\t" << parseResult << " B");			\
  double outSize = arithCodec->numberOfOutputBytes() + strlen(VERSION) + 2*SIZEOF_CHAR;	\
  OUTPUTENL("  Output data size: \t\t\t" << outputDevice->bytesWritten() << " B");	\
  OUTPUTENL("  Compression ratio: \t\t\t1:" << (double)(outSize)/parseResult		\
	    << " (" << (double)(outSize*100)/parseResult << "%)");			\
  OUTPUTENL("  Compression rate: \t\t\t" << (outSize / parseResult) * 8 << " bpc");	\
}


//! Display a brief decompression summary.
#define DISPLAY_DECOMPRESSION_SUMMARY						\
{										\
  OUTPUTENL("Decompression performance");					\
  OUTPUTENL("  Input data size: \t\t\t" << inDevice->bytesRead() << " B");	\
  OUTPUTENL("  Restored data size: \t\t\t" << funnelDevice->bytesWritten()	\
	    << " B (passed to the model for final processing)");		\
}



/*!
  Does nothing special.
*/
XmlCodec::XmlCodec(void)
  : XmlCodecBase()
{
}



/*!
  Deletes the default text codec (if any).
 */
XmlCodec::~XmlCodec(void)
{
  deleteDefaultTextCodec();
}


/*!
  Initializes the codec in PUSH mode.

  \param outDevice The output device used for output of compressed data.
 */
void XmlCodec::initializePushCoder(IODevice *outDevice)
{
  if (coderType == UnknownCoder)
    //coder is now PUSH
    coderType = PushCoder;
  else
    {
      if (coderType == PullCoder)
	//PULL coder used in PUSH mode
	throw ExaltCoderIsPullException();
      else
	//An attempt to re-initialize a PUSH coder
	throw ExaltCoderIsPushException();
    }

  inputDevice = 0;
  outputDevice = outDevice;

  //Create a KY grammar
  NEW(kyGrammar, KYGrammar);

  //Create a coding context for the grammar
  NEW(context, Context);
  kyGrammar->setContext(context);

  kyGrammar->setOutputDevice(outputDevice);
  kyGrammar->setTextCodec(textCodec);

  //Create arithmetic coder/decoder...
  NEW(arithCodec, ArithCodec);

  //...and bind it with the contexts
  context->setArithCodec(arithCodec);

  //Create a XML parser with specified input device
  NEW(xmlParser, XmlParser);
  //xmlParser->setInputDevice(inputDevice);

  //Create a XML model...
  if (ExaltOptions::getOption(ExaltOptions::Model) == ExaltOptions::SimpleModel)
    {
      NEW(xmlModel, XmlSimpleModel);
      outputDevice->putChar(0);		//to indicate the use of the simple model
    }
  else
    {
      NEW(xmlModel, XmlAdaptiveModel);
      outputDevice->putChar(1);		//to indicate the use of the adaptive model
    }


  //...and bind it with the parser...
  xmlParser->setXmlModel(xmlModel);

  //...plus the grammar
  xmlModel->setGrammar(kyGrammar);



  //write the version string (excluding terminating \0)
  outputDevice->writeData(FILE_STAMP, strlen(FILE_STAMP));
  
  //store F_BITS and B_BITS being used in output
  outputDevice->putChar(F_BITS);
  outputDevice->putChar(B_BITS);
  
  //prepare arithCodec for encoding
  arithCodec->setOutputDevice(outputDevice);
  arithCodec->startOutputtingBits();
  arithCodec->startEncode();
}



/*!
  The coder encodes given chunk of data.
  
  \param data The chunk of XML data.
  \param length The length of the data.
  \param isFinal Indication whether the coder received the last chunk of data.
*/
bool XmlCodec::encodePush(const char *data, int length, bool isFinal = false)
{
  if (coderType == PullCoder)
    //PULL coder used in PUSH mode
    throw ExaltCoderIsPullException();
  else
    if (coderType == UnknownCoder)
      //Unitialized coder used in PUSH mode
      throw ExaltPushCoderNotInitializedException();


  bool parseResult = xmlParser->parsePush(data, length, isFinal);

  if (isFinal)
    {
      //Delete all objects
      DELETE(xmlParser);
      DELETE(xmlModel);

      DELETE(kyGrammar);


      if (!parseResult)
	{
	  //something was wrong --> cleanup needed
	  //!!! delete file, etc.
	}
      else
	{
	  //encode "end of message"
	  context->encodeEndOfMessage();

	  //stop arithCodec
	  arithCodec->finishEncode();
	  arithCodec->doneOutputtingBits();
	}


      if (parseResult && ExaltOptions::getOption(ExaltOptions::Verbose) == ExaltOptions::Yes)
	{
	  DISPLAY_COMPRESSION_SUMMARY;
	}

      DELETE(arithCodec);
      DELETE(context);

      DELETE(xmlModel);
    }

  return parseResult;
}



/*!
  The codec creates all of the necessary underlying components and encodes the data.

  \param inDevice The input device.
  \param outDevice The output device.
*/
bool XmlCodec::encode(IODevice *inDevice, IODevice *outDevice)
{
  inputDevice = inDevice;
  outputDevice = outDevice;

  //Create a KY grammar
  NEW(kyGrammar, KYGrammar);

  //Create a coding context for the grammar
  NEW(context, Context);

  kyGrammar->setOutputDevice(outputDevice);

  kyGrammar->setTextCodec(textCodec);

  kyGrammar->setContext(context);

  //Create arithmetic coder/decoder...
  NEW(arithCodec, ArithCodec);

  //...and bind it with the contexts
  context->setArithCodec(arithCodec);

  //Create a XML parser with specified input device
  NEW(xmlParser, XmlParser);
  xmlParser->setInputDevice(inputDevice);


  //write the version string (excluding terminating \0)
  outputDevice->writeData(FILE_STAMP, strlen(FILE_STAMP));

  //Create a XML model...
  if (ExaltOptions::getOption(ExaltOptions::Model) == ExaltOptions::SimpleModel)
    {
      NEW(xmlModel, XmlSimpleModel);
      outputDevice->putChar(0);		//to indicate the use of the simple model
    }
  else
    {
      NEW(xmlModel, XmlAdaptiveModel);
      outputDevice->putChar(1);		//to indicate the use of the simple model
    }

  //...and bind it with the parser...
  xmlParser->setXmlModel(xmlModel);

  //...plus the grammar
  xmlModel->setGrammar(kyGrammar);



  
  //store F_BITS and B_BITS being used in output
  outputDevice->putChar(F_BITS);
  outputDevice->putChar(B_BITS);
  
  //prepare arithCodec for encoding
  arithCodec->setOutputDevice(outputDevice);
  arithCodec->startOutputtingBits();
  arithCodec->startEncode();

  coderType = PullCoder;

  //parse the data
  long parseResult = xmlParser->parse();

 
  DELETE(kyGrammar);

  //Delete all objects
  DELETE(xmlParser);
  DELETE(xmlModel);

  if (!parseResult)
    {
      //something was wrong --> cleanup needed
      //!!! delete file, etc.
    }
  else
    {
      //encode "end of message"
      context->encodeEndOfMessage();

      //stop arithCodec
      arithCodec->finishEncode();
      arithCodec->doneOutputtingBits();
    }


  if (parseResult && ExaltOptions::getOption(ExaltOptions::Verbose) == ExaltOptions::Yes)
    {
      DISPLAY_COMPRESSION_SUMMARY;
    }

  DELETE(arithCodec);
  DELETE(context);


  //return the result of the parsing
  return parseResult;
}




/*!
  The codec creates all of the necessary underlying components and decodes the data. The codec sends SAX events to the SAX receptor.

  \param inDevice The input device.
  \param receptor The SAX event receptor.
  \param userData The user data structure.
*/
bool XmlCodec::decode(IODevice *inDevice, SAXReceptor *receptor, void *userData = 0)
{
  FunnelDevice *funnelDevice;
  char *fileStamp;
  size_t fileStampLength = strlen(FILE_STAMP);
  int symbol;
  int byte1, byte2;
  long lastFixedContextSymbol;
  RuleElement *rel;
  unsigned char tmpChar, mask;
  unsigned long fibCode = 0;
  bool lastWasOne = false;
  bool finished = false;
  char bits = 0;
  unsigned long alphabetBaseSize;
  SAXEmitter *saxEmitter;


  inputDevice = inDevice;

  //Create a KY grammar
  NEW(kyGrammar, KYGrammar);


  //Create a decoding context for the grammar
  NEW(context, Context);

  kyGrammar->setContext(context, false);

  //Create arithmetic coder/decoder...
  NEW(arithCodec, ArithCodec);

  //...and bind it with the contexts
  context->setArithCodec(arithCodec);



  NEW(fileStamp, char[fileStampLength + 1]);

  inputDevice->readData(fileStamp, fileStampLength);
  if (inputDevice->errorOccurred() || inputDevice->eof())
    return false;

  fileStamp[fileStampLength] = '\0';

  if (strcmp(FILE_STAMP, fileStamp))
    {
      DELETE(fileStamp);
      ERR("File format not recognized!");
      throw ExaltUnknownFileFormatException();
    }
  else
    {
      DELETE(fileStamp);
    }

  //whether to use the simple, or the adaptive model
  inputDevice->getChar(&byte1);
  if (!byte1)
    {
      //Create a xmlSimpleModel
      NEW(xmlModel, XmlSimpleModel);
    }
  else
    {
      //otherwise create the adaptive model
      NEW(xmlModel, XmlAdaptiveModel);
    }

  NEW(saxEmitter, SAXEmitter(receptor));
  xmlModel->setSAXEmitter(saxEmitter, userData);

  NEW(funnelDevice, FunnelDevice(xmlModel, 4096, true));
  funnelDevice->prepare();
  kyGrammar->setOutputDevice(funnelDevice);
  kyGrammar->setTextCodec(textCodec);

    
  inputDevice->getChar(&byte1);
  if (inputDevice->errorOccurred() || inputDevice->eof())
    return false;

  inputDevice->getChar(&byte2);
  if (inputDevice->errorOccurred() || inputDevice->eof())
    return false;

  if (byte1 != F_BITS || byte2 != B_BITS)
    FATAL("Compressed file F_BITS = " << byte1 << ", B_BITS = " << byte2 << ")! Compressor was compiled with F_BITS = " << F_BITS << ", B_BITS = " << B_BITS << ".");

  //construct reverse Fibonacci code (e.g. 1100101 instead of 1010011) from sequence of
  //following bytes. This code represents the base size of the terminal alphabet.
  while (!finished)
    {
      mask = 1 << (SIZEOF_CHAR*8 - 1);
      
      int c;

      inputDevice->getChar(&c);
      if (inputDevice->errorOccurred() || inputDevice->eof())
	return false;

      tmpChar = (unsigned char)c;
      
      while (mask)
	{
	  fibCode >>= 1;
	  bits++;
	  
	  if (tmpChar & mask)
	    {
	      fibCode = fibCode | ((unsigned long)1) << (SIZEOF_UNSIGNED_LONG*8-1);
	      
	      if (!lastWasOne)
		lastWasOne = true;
	      else
		{
		  //Fibonacci code is now complete
		  //--> decode the symbol number
		  fibCode >>= (SIZEOF_UNSIGNED_LONG*8 - bits);
		  alphabetBaseSize = Fibonacci::decode(fibCode);
		  finished = true;
		  break;
		}
	    }
	  else
	    lastWasOne = false;
	  
	  mask >>= 1;
	}
    }
	  
  //reset Fibonacci state variables
  fibCode = 0;
  bits = 0;
  finished = false;
  lastWasOne = false;




  context->setType(alphabetBaseSize, DynamicContext);

  //get the last fixed symbol of the context table (EOM)
  lastFixedContextSymbol = context->initialize();

  //prepare arithCodec for decoding
  arithCodec->setInputDevice(inputDevice);
  arithCodec->startInputtingBits();
  arithCodec->startDecode();

  for (;;)
    {
      //decode next symbol
      symbol = context->decode();


      //in case of the EOM, break
      if (symbol == context->endOfMessage)
	break;
      
      //New variable --> decode it from Fibonacci code
      //that follows...

      if (symbol == Context::NotKnown)
	{
	  FATAL("Unknown symbol decoded!!!");
	}

      if (symbol > lastFixedContextSymbol)
	{
	  //decoded a variable
  	  //--> append it to the grammar

	  Rule *rule;

  	  NEW(rel, RuleElement);
  	  rel->type = Variable;

	  //find rule with corresponding id
	  rule = kyGrammar->findRule(symbol - lastFixedContextSymbol);

	  CHECK_POINTER(rule);

	  rel->rule = rule;

	  //increase rule ref count
	  rel->rule->refCount++;
	}
      else
	{
	  //decoded a terminal
	  //--> append it to the grammar
	  NEW(rel, RuleElement);

	  rel->type = Terminal;
	  rel->value = symbol;
	}

      kyGrammar->appendToRootRule(rel);
    }


  //Delete all objects
  DELETE(kyGrammar);


  //finish decoding
  arithCodec->finishDecode();
  arithCodec->doneInputtingBits();

  if (ExaltOptions::getOption(ExaltOptions::Verbose) == ExaltOptions::Yes)
    {
      DISPLAY_DECOMPRESSION_SUMMARY;
    }

  DELETE(arithCodec);
  DELETE(context);
  DELETE(xmlModel);
  DELETE(funnelDevice);


  DELETE(saxEmitter);

  return true;
}


