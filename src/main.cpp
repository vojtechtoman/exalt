/***************************************************************************
    main.cpp  -  Main program
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
  \file main.cpp
  \brief The main program.
  
  The main() function parses the command line options, constructs an exalt codec, and starts (de)compression.
*/


#include <cstdio>
#include <fstream>


#include "defs.h"
#include "encodings.h"
#include "exaltcodec.h"
#include "list.h"
#include "options.h"


//! Display the program version.
#define PRINT_VERSION						\
{								\
  OUTPUTNL("This is Exalt, XML data compressor.");		\
  OUTPUTNL("Version " << VERSION << ", build date: "		\
	   << __DATE__);					\
  OUTPUTNL("Written by Vojtech Toman, 2002-2003");		\
  OUTPUTNL("Using Expat " << EXPAT_VERSION);			\
}


/*!
  \brief Test if the current command line argument matches with specified option.

  \param _x_ The name of the option.
 */
#define OPT(_x_)		!strcmp(argv[i], (_x_))



/*!
  \brief Report an error when an option without required argument occurs.

  \param _i_ The position of erroneous command line parameter.
 */
#define OPT_ERROR(_i_)						\
{								\
  ERR("'" << argv[_i_-1] << "' requires an argument");		\
  throw ExaltOptionException();					\
}



/*!
  \brief Report unknown option.

  \param _opt_ The name of the option.
 */
#define OPT_UNKNOWN(_opt_)					\
{								\
  ERR("Unknown option: '" << _opt_ << "'.");			\
  throw ExaltOptionException();					\
}



/*!
  \brief Report an error when no input files have been specified.
 */
#define OPT_NONE						\
{								\
  ERR("No input files specified.");				\
  throw ExaltOptionException();					\
}



/*!
  \brief Report unknown encodings.

  \param _name_ The name of the encoding.
 */
#define ENCODING_UNKNOWN(_name_)				\
{								\
  ERR("Unknown encoding: '" << _name_ << "'.");			\
  throw ExaltUnknownEncodingException();			\
}



/*!
  \brief Set output encoding.

  \param _enc_ Output encoding.
 */
#define SET_OUTPUT_ENCODING(_enc_)				\
{								\
  size_t j = 0;							\
  Encodings::MIB mib = Encodings::Unknown;			\
								\
  while (encodingNames[j].name)					\
    {								\
      if (!strcmp(_enc_, encodingNames[j].name))		\
	mib = encodingNames[j].mib;				\
      j++;							\
    }								\
  if (mib != Encodings::Unknown)				\
    encodingMIB = mib;						\
  else								\
    {								\
      ENCODING_UNKNOWN(_enc_);					\
    }								\
}



/*!
  \brief Perform compression/decompression based on the given parameter.

  \param _action_ The name of the method of XmlCodec (encode() or decode())
 */
#define ENCODE_DECODE(_action_)					\
{								\
  bool canContinue = true;					\
								\
  if (fileName)							\
    {								\
      ifstream inFile(fileName);				\
								\
      if (inFile)						\
        inFile.close();						\
      else							\
        {							\
          ERR("'" << fileName << "' doesn't exist!");		\
          canContinue = false;					\
        }							\
    }								\
								\
  if (!useStdout && canContinue)				\
    {								\
      if (outputFileNameLength != -1)				\
	{							\
	  ifstream outFile(outputFileName);			\
								\
	  if (outFile && !force)				\
	    {							\
	      char c;						\
	      outFile.close();					\
			  					\
	      OUTPUTE("File '" << outputFileName		\
		      << "' exists. Overwrite (y or n)? ");	\
			  					\
	      do { c = getchar(); } while (c == '\n');		\
			 					\
	      canContinue = (c == 'y' || c == 'Y');		\
								\
	      if (!canContinue && verbose)			\
		OUTPUTENL("Not overwritten.");			\
	    }							\
	}							\
    }								\
	      							\
  if (canContinue && outputFileNameLength != -1)		\
    {								\
      if (exaltCodec._action_(fileName, outputFileName))	\
	{							\
	  if (erase && fileName)				\
	    remove(fileName);					\
	}							\
      else							\
	throw ExaltIOException();				\
    }								\
}



/*!
  \brief Test if the file name ends with given suffix.

  \param str The file name.
  \param suf The suffix.
  \return The position in \a str where the suffix starts, or -1 if the file name doesn't end with given suffix.
 */
static int testFileNameSuffix(const char *str, const char *suf)
{
  int i = strlen(str)-1;
  int j = strlen(suf)-1;

  while (j > -1)
    {
      if (str[i] != suf[j])
	return -1;

      i--;
      j--;
    }

  return i+1;
}




/*!
  \brief Print basic program usage.
 */
static void usage(void)
{
  OUTPUTNL("Usage: " << PACKAGE << " [options and files in any order]");
  OUTPUTNL("where options are:");
  OUTPUTENDLINE;
  OUTPUTNL("  -s|--suffix .suf      use suffix .suf on compressed files");
  OUTPUTNL("  -d|--decompress       decompress");
  OUTPUTNL("  -f|--force            overwrite files, don't stop on errors");
  OUTPUTNL("  -c|--stdout           write on standard output");
  OUTPUTNL("  -a|--adaptive         use the adaptive model");
  OUTPUTNL("  -x|--erase            erase source files after (de)compression");
  OUTPUTNL("  -e|--encoding enc     set decompressed output encoding (to be implemented)");
  OUTPUTNL("  -l|--list-encodings   list recognized encodings");
  OUTPUTNL("  -v|--verbose          be verbose");
  OUTPUTNL("  -m|--print-models     display the models of the elements (requires the adaptive model)");
  OUTPUTNL("                        (warning: the models may be huge!)");
  OUTPUTNL("  -g|--print-grammar    display the content of the resulting grammar");
  OUTPUTNL("                        (warning: the grammar may be huge!)");
  OUTPUTNL("  -V|--version          display version number");
  OUTPUTNL("  -L|--license          display version number and software license");
  OUTPUTNL("  -h|--help             show this help");

  OUTPUTENDLINE;
  OUTPUTNL("The default action is to compress. If no file names are given,");
  OUTPUTNL("or if a file name is '-', " << PACKAGE << " compresses or decompresses");
  OUTPUTNL("from standard input to standard output.");
  
}



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////



/*!
  \brief The main program

  Function parses the command line options, constructs an exalt codec, and starts (de)compression.
  \param argc number of command line parameters
  \param argv an array of command line parameters
  \retval EXIT_SUCCESS on success
  \retval EXIT_FAILURE otherwise

 */
int main(int argc, char **argv)
{

  UserAction selectedAction = Compress;		//the action of the user
  char *outputFileSuffix = 0;			//user supported suffix of output files
  bool force = false;				//overwrite files?
  bool verbose = false;				//be verbose?
  bool printGrammar = false;			//the resulting grammar is not printed by default
  bool printModels = false;			//the resulting models are not printed by default
  bool useStdout = false;			//use standard output?
  bool adaptiveModel = false;			//use adaptive model?
  bool erase = false;				//erase source files after (de)compression?

  //list of recognized encodings (not necessarily supported!)
  Encodings::EncodingName encodingNames[] = { ENCODING_NAMES };

  //default output encoding dependent on expat (UTF_8 or UTF_16)
  Encodings::MIB encodingMIB = DEFAULT_OUTPUT_ENCODING;


  List<char> unrecognizedParamList;		//list of unrecognized command line arguments
  List<char> filesToProcess;			//list of file do process

  ExaltOptions::resetOptions();

  try
    {
      for (int i = 1; i < argc; ++i)
	{
	  if (OPT("--suffix") || OPT("-s"))
	    {
	      //set compressed files suffix
	      if ((++i) >= argc)
		OPT_ERROR(i);

	      outputFileSuffix = argv[i];
	    }

	  else if (OPT("--force") || OPT("-f"))
	    {
	      //force (overwrite files, don't stop on errors)
	      force = true;
	    }

	  else if (OPT("--verbose") || OPT("-v"))
	    {
	      verbose = true;
	    }

	  else if (OPT("--print-models") || OPT("-m"))
	    {
	      printModels = true;
	    }

	  else if (OPT("--print-grammar") || OPT("-g"))
	    {
	      printGrammar = true;
	    }

	  else if (OPT("--stdout") || OPT("-c"))
	    {
	      useStdout = true;
	    }

	  else if (OPT("--decompress") || OPT("-d"))
	    {
	      selectedAction = Decompress;
	    }

	  else if (OPT("--adaptive") || OPT("-a"))
	    {
	      adaptiveModel = true;
	    }

	  else if (OPT("--encoding") || OPT("-e"))
	    {
	      //set compressed files suffix
	      if ((++i) >= argc)
		OPT_ERROR(i);

	      SET_OUTPUT_ENCODING(argv[i]);
	    }

	  else if (OPT("--list-encodings") || OPT("-l"))
	    {
	      OUTPUTNL("List of recognized input/output encodings (not necessarily supported!):");
	      OUTPUTENDLINE;
	      LIST_ENCODINGS;
	      exit(EXIT_SUCCESS);
	    }

	  else if (OPT("--erase") || OPT("-x"))
	    {
	      erase = true;
	    }

	  else if (OPT("--version") || OPT("-V"))
	    {
	      //print version info
	      PRINT_VERSION;
	      exit(EXIT_SUCCESS);
	    }

	  else if (OPT("--license") || OPT("-L"))
	    {
	      //print version info
	      PRINT_VERSION;

	      //print the license
	      OUTPUTENDLINE;
	      OUTPUTNL("This program comes with ABSOLUTELY NO WARRANTY. You may redistribute");
	      OUTPUTNL("copies of this program under the terms of the GNU General Public License.");
	      OUTPUTNL("For more information about these matters, see the file named COPYING.");

	      OUTPUTENDLINE;
	      OUTPUTNL("Arithmetic coding routines are based on the sources originally written");
	      OUTPUTNL("by Alistair Moffat. Their use is restricted by the following license:");
	      OUTPUTENDLINE;

	      OUTPUTNL("These programs are supplied free of charge for research purposes only,");
	      OUTPUTNL("and may not sold or incorporated into any commercial product. There is");
	      OUTPUTNL("ABSOLUTELY NO WARRANTY of any sort, nor any undertaking that they are");
	      OUTPUTNL("fit for ANY PURPOSE WHATSOEVER. Use them at your own risk. If you do");
	      OUTPUTNL("happen to find a bug, or have modifications to suggest, please report");
	      OUTPUTNL("the same to Alistair Moffat, alistair@cs.mu.oz.au. The copyright");
	      OUTPUTNL("notice above and this statement of conditions must remain an integral");
	      OUTPUTNL("part of each and every copy made of these files.");

	      OUTPUTNL("(Search for the license text in the sources to find the concrete files.)");
	      OUTPUTEENDLINE;

	      exit(EXIT_SUCCESS);
	    }

	  else if (OPT("--help") || OPT("-h"))
	    {
	      throw ExaltOptionException();
	    }

	  else
	    {
	      //collect unrecognized parameters/options
	      unrecognizedParamList.append(argv[i]);
	    }
	}
      
      for (char *unrecognized = unrecognizedParamList.first(); unrecognized; unrecognized = unrecognizedParamList.next())
	{
	  if (strcmp(unrecognized, "-") && unrecognized[0] == '-')
	    {
	      OPT_UNKNOWN(unrecognized);
	    }
	  else
	    filesToProcess.append(unrecognized);
	}
    }

  catch (ExaltException)
    {
      usage();
      exit(EXIT_FAILURE);
    }


  //set the options
  if (verbose)
    ExaltOptions::setOption(ExaltOptions::Verbose, ExaltOptions::Yes);
  else
    ExaltOptions::setOption(ExaltOptions::Verbose, ExaltOptions::No);

  if (printModels)
    ExaltOptions::setOption(ExaltOptions::PrintModels, ExaltOptions::Yes);
  else
    ExaltOptions::setOption(ExaltOptions::PrintModels, ExaltOptions::No);

  if (printGrammar)
    ExaltOptions::setOption(ExaltOptions::PrintGrammar, ExaltOptions::Yes);
  else
    ExaltOptions::setOption(ExaltOptions::PrintGrammar, ExaltOptions::No);

  if (adaptiveModel)
    ExaltOptions::setOption(ExaltOptions::Model, ExaltOptions::AdaptiveModel);
  else
    ExaltOptions::setOption(ExaltOptions::Model, ExaltOptions::SimpleModel);

  //set output encoding
  ExaltOptions::setOption(ExaltOptions::Encoding, encodingMIB);


  //perform selected action
  switch (selectedAction)
    {
    case Compress:
      if (filesToProcess.isEmpty())
	//if no files have been specified, use stdin and stdout
	filesToProcess.append("-");

      for (char *fileName = filesToProcess.first(); fileName; fileName = filesToProcess.next())
	{
	  char *outputFileName = 0;
	  int outputFileNameLength = 0;
	      
	  if (!strcmp(fileName, "-"))
	    {
	      OUTPUTENL("Compressing standard input to standard output.");
	      fileName = 0;
	    }

	  if (useStdout)
	    {
	      if (fileName)
		{
		  //NULL means stdout for output
		  outputFileName = 0;
		      
		  if (verbose)
		    OUTPUTENL("Encoding '" << fileName << "' to standard output.");
		}
	    }
	  else
	    {
	      if (fileName)
		{
		  outputFileNameLength = strlen(fileName);
		  if (outputFileSuffix)
		    outputFileNameLength += strlen(outputFileSuffix);
		  else
		    outputFileNameLength += strlen(DEFAULT_FILE_SUFFIX);

		  NEW(outputFileName, char[outputFileNameLength+1]);
		      
		  strcpy(outputFileName, fileName);
		  if (outputFileSuffix)
		    strcat(outputFileName, outputFileSuffix);
		  else
		    strcat(outputFileName, DEFAULT_FILE_SUFFIX);
		      
		  if (verbose)
		    OUTPUTENL("Encoding '" << fileName << "' to '" << outputFileName << "'.");
		}
	    }

	  //create the exalt codec
// 	  ExaltCodec exaltCodec;
	  try
	    {
	      ExaltCodec exaltCodec;
	      ENCODE_DECODE(encode);
	    }

	  catch (ExaltContextNotInitializedException)
	    {
	      DBG("Use of uninitialized context!");
	    }
	  catch (ExaltException)
	    {
	      if (fileName)
		ERR("Failed to compress '" << fileName << "'!");
	      else
		ERR("Failed to compress standard input!");

	      if (outputFileName)
		remove(outputFileName);

	      if (!force)
		exit(EXIT_FAILURE);
	    }


	  if (outputFileName)
	    DELETE(outputFileName);

	}
      break;

    case Decompress:
      if (filesToProcess.isEmpty())
	//if no files have been specified, use stdin and stdout
	filesToProcess.append("-");

      for (char *fileName = filesToProcess.first(); fileName; fileName = filesToProcess.next())
	{
	  char *outputFileName = 0;
	  int outputFileNameLength = 0;

	  if (!strcmp(fileName, "-"))
	    {
	      OUTPUTENL("Decompressing standard input to standard output.");
	      fileName = 0;
	    }
	  else
	    {
	      if ((outputFileNameLength = testFileNameSuffix(fileName, DEFAULT_FILE_SUFFIX)) == -1)
		if (outputFileSuffix)
		  outputFileNameLength = testFileNameSuffix(fileName, outputFileSuffix);

	      if (outputFileNameLength == -1)
		{
		  ERR("Unknown suffix, ignoring.");
		}
	    }
	  
	  
	  if (useStdout)
	    {
	      if (fileName)
		{
		  if (outputFileNameLength != -1)
		    {
		      //NULL means stdout for output
		      outputFileName = 0;
		      
		      if (verbose)
			OUTPUTENL("Decompressing '" << fileName << "' to standard output.");
		    }
		}
	    }
	  else
	    {
	      if (outputFileNameLength != -1)
		{
		  if (fileName)
		    {
		      NEW(outputFileName, char[outputFileNameLength+1]);
		      strncpy(outputFileName, fileName, outputFileNameLength);
		      outputFileName[outputFileNameLength] = 0;
		      
		      if (verbose)
			OUTPUTENL("Decoding '" << fileName << "' to '" << outputFileName << "'.");
		    }
		}
	    }


	  //construct an exalt codec
	  ExaltCodec exaltCodec;	//instance of the codec
	  
	  try
	    {
	      ENCODE_DECODE(decode);
	    }
	  
	  catch (ExaltException)
	    {
	      if (fileName)
		ERR("Failed to decompress '" << fileName << "'!");
	      else
		ERR("Failed to decompress standard input!");

	      if (outputFileName)
		remove(outputFileName);

	      if (!force)
		exit(EXIT_FAILURE);
	    }
	  
	  if (outputFileName)
	    DELETE(outputFileName);
	}
      
      break;

    default:
      ERR("Don't know what to do!");
      return EXIT_FAILURE;
    }
  
  return EXIT_SUCCESS;
}
