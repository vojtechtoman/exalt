/***************************************************************************
    filedevice.cpp  -  Definitions of the FileDevice class methods.
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
  \file filedevice.cpp
  \brief Definitions of the FileDevice class methods.
  
  This file contains the definitions of the FileDevice class methods.
*/


#ifdef __GNUG__
# pragma implementation
#endif


#include "filedevice.h"



/*!
  Initialization is performed.
 */
FileDevice::FileDevice(void)
  : IODevice()
{
  bWritten = 0;
  bRead = 0;
}



/*!
  The device is finished.
 */
FileDevice::~FileDevice(void)
{
  if (!isPrepared())
    finish();
}



/*!
  Prepares the device for work with file \a fileName in mode \a mode. If \a fileName is \a NULL, and mode is \a ios::in (or \a ios::out), standard input is used (or standard output).

  \param fileName The name of the file (or NULL).
  \param mode The mode.
 */
void FileDevice::prepare(const char *fileName, int mode) throw (ExaltIOException)
{
  //if fileName is NULL, use stdout or stdin
  if (!fileName)
    {
      if (mode == ios::in)
	attach(0);
      else
	if (mode == ios::out)
	  attach(1);
	else
	  throw ExaltPrepareDeviceIOException();
    }
  else
    open(fileName, mode);

  prepared = !fail();

  if (!prepared)
    throw ExaltPrepareDeviceIOException();
}


/*!
  Ensures that all the data is written to the device.
*/
void FileDevice::flush(void) throw (ExaltIOException)
{
  fstream::flush();

  if (errorOccurred())
    throw ExaltFlushDeviceIOException();
}


/*!
  This method ends the work with the device. It flushes the file and closes it. An attempt to work with finished device will cause an ExaltIOException to be raised.
*/
void FileDevice::finish(void) throw (ExaltIOException)
{
  prepared = false;
  bWritten = 0;
  bRead = 0;

  flush();
  close();

  if (errorOccurred())
    throw ExaltFinishDeviceIOException();
}


/*!
  Reads up to \a length bytes from the device and stores them in \a buffer. If an error occurred, ExaltIOException is raised.
  
  \param buf Pointer to the buffer.
  \param length Length of the buffer.
  
  \return State of the device after the read operation.
*/
IOState FileDevice::readData(char *buf, IOSize length) throw (ExaltIOException)
{
  if (isPrepared())
    {
      if (read(buf, length))
	{
	  bRead += gcount();
	  return ReadOk;
	}
      else
	{
	  bRead += gcount();

	  if (eof())
	    return EndOfFile;
	  else
	    throw ExaltDeviceReadIOException();
	}
    }
  else
    throw ExaltDeviceNotPreparedIOException();
}


/*!
  Reads one character from the device. If an error occurred, ExaltIOException is raised.
  
  \param c Pointer where the read character is stored.
  
  \return State of the device after the read operation.
*/
IOState FileDevice::getChar(int *c) throw (ExaltIOException)
{
  if (isPrepared())
    {
      *c = 0;
      if (get((unsigned char &)*c))
	{
	  bRead += gcount();
	  return ReadOk;
	}
      else
	{
	  bRead += gcount();
	  if (eof())
	    return EndOfFile;
	  else
	    throw ExaltDeviceWriteIOException();
	}
    }
  else
    throw ExaltDeviceNotPreparedIOException();
}


/*!
  Writes \a length bytes from the buffer \a buf to the device. If an error occurred, ExaltIOException is raised.
  
  \param buf Pointer to the buffer.
  \param length Length of the buffer.
  
  \return State of the device after the write operation.
*/
IOState FileDevice::writeData(const char *buf, IOSize length) throw (ExaltIOException)
{
  if (isPrepared())
    {
      if (write(buf, length))
	{
	  bWritten += length;
	  return WriteOk;
	}
      else
	throw ExaltDeviceWriteIOException();
    }
  else
    throw ExaltDeviceNotPreparedIOException();
}


/*!
  Writes one character to the device. If an error occurred, ExaltIOException is raised.
  
  \param c The character.
  
  \return State of the device after the read operation.
*/
IOState FileDevice::putChar(int c) throw (ExaltIOException)
{
  if (isPrepared())
    {
      if (put((unsigned char)c))
	{
	  bWritten++;
	  return WriteOk;
	}
      else
	throw ExaltDeviceWriteIOException();
    }
  else
    throw ExaltDeviceNotPreparedIOException();
}


/*!
  Returns the total number of bytes read from the device.
  
  \return Number of bytes.
*/
IOSize FileDevice::bytesReadTotal(void)
{
  return bRead;
}


/*!
  Returns the number of bytes read from the device by the last read operation.
  
  \return Number of bytes.
*/
IOSize FileDevice::bytesRead(void)
{
  return gcount();
}


/*!
  Returns the total number of bytes written to the device.
  
  \return Number of bytes.
*/
IOSize FileDevice::bytesWritten(void)
{
  return bWritten;
}


/*!
  If any of the IO operations was unsuccessful, this method can be used to test it.
  
  \retval TRUE An error occurred.
  \retval FALSE No error occurred.
*/
bool FileDevice::errorOccurred(void)
{
  return bad();
}

/*!
  This method can be used for tests of the end of file.
  
  \retval TRUE End of file occurred.
  \retval FALSE End of file didn't occurred.
*/
bool FileDevice::eof(void)
{
  return fstream::eof();
}


