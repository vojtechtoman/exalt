/***************************************************************************
    funneldevice.cpp  -  Definitions of the FunnelDevice class methods.
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
  \file funneldevice.cpp
  \brief Definitions of the FunnelDevice class methods.
  
  This file contains the definitions of the FunnelDevice class methods.
*/


#ifdef __GNUG__
# pragma implementation
#endif

#include "funneldevice.h"




/*!
  Initialization is performed.

  In order to use the device, one has to specify the object that will receive the data (it must inherit UserOfFunnelDevice). The device stores the data in an inner buffer. If the buffer is full, the data is delivered to the receiving object. If \a isImmediate is set to \a TRUE, the data is not buffered.

  \param rcvr Pointer to the receiver.
  \param isImmediate Is the data delivered immediately?
  \param size Size of the device's inner buffer.
 */
FunnelDevice::FunnelDevice(UserOfFunnelDevice *rcvr, bool isImmediate = false, size_t size = DEFAULT_FUNNEL_BUFFER_SIZE)
  : IODevice()
{
  receiver = rcvr;

  bufferSize = size;
  buffer = 0;

  immediate = isImmediate;

  bw = 0;

  //device is "empty"
  dataLength = 0;
  dataPresent = false;
}


/*!
  Deletes the inner buffer.
 */
FunnelDevice::~FunnelDevice(void)
{
  if (buffer)
    DELETE_ARRAY(buffer);
}

/*!
  This method prepares the device for further work. An attempt to work with unprepared device will cause an ExaltIOException to be raised.
*/
void FunnelDevice::prepare(void) throw (ExaltIOException)
{
  NEW(buffer, XmlChar[bufferSize]);

  dataLength = 0;
}

/*!
  Ensures that all the data is written to the device. This means that all the data is delivered to the receiver.
*/
void FunnelDevice::flush(void) throw (ExaltIOException)
{
  if (dataPresent)
    {
      try
	{
	  receiver->receiveData(buffer, dataLength);
	}
      catch (ExaltException)
	{
	  throw ExaltIOException();
	}
      
      //buffer is now empty
      dataLength = 0;
      dataPresent = false;
    }
}


/*!
  This method ends the work with the device. An attempt to work with finished device will cause an ExaltIOException to be raised.
*/
void FunnelDevice::finish(void) throw (ExaltIOException)
{
  flush();

  //buffer is empty
  dataLength = 0;
  dataPresent = false;
  bw = 0;
}


/*!
  Reads up to \a length bytes from the device and stores them in \a buffer. If an error occurred, ExaltIOException is raised.
  
  \param buf Pointer to the buffer.
  \param length Length of the buffer.
  
  \return State of the device after the read operation.
*/
IOState FunnelDevice::readData(char *buf, IOSize length) throw (ExaltIOException)
{
  return ReadOk;
}

/*!
  Reads one character from the device. If an error occurred, ExaltIOException is raised.
  
  \param c Pointer where the read character is stored.
  
  \return State of the device after the read operation.
*/
IOState FunnelDevice::getChar(int *c) throw (ExaltIOException)
{
  *c = 0;
  return ReadOk;
}

/*!
  Writes \a length bytes from the buffer \a buf to the device. If an error occurred, ExaltIOException is raised.
  
  \param buf Pointer to the buffer.
  \param length Length of the buffer.
  
  \return State of the device after the write operation.
*/
IOState FunnelDevice::writeData(const char *buf, IOSize length) throw (ExaltIOException)
{
  bw += length;

  if (!buffer)
    {
      throw ExaltDeviceNotPreparedIOException();
      return WriteError;
    }

  if (length + dataLength > bufferSize)
    {
      throw ExaltDeviceFullIOException();
      return WriteError;
    }

  for (size_t i = 0; i < length; i++)
    {
      buffer[dataLength + i] = ((XmlChar *)buf)[i];
    }

  if (length)
    dataPresent = true;
  else
    dataPresent = false;

  dataLength += length;

  if (immediate)
    flush();

  return WriteOk;
}


/*!
  Writes one character to the device. If an error occurred, ExaltIOException is raised.
  
  \param c The character.
  
  \return State of the device after the read operation.
*/
IOState FunnelDevice::putChar(int c) throw (ExaltIOException)
{
  bw++;

  if (!buffer)
    {
      throw ExaltDeviceNotPreparedIOException();
      return WriteError;
    }

  if (dataLength >= bufferSize)
    {
      throw ExaltDeviceFullIOException();
      return WriteError;
    }


  buffer[dataLength] = (XmlChar)c;
  dataPresent = true;
  dataLength++;


  if (immediate)
    flush();


  return ReadOk;
}

/*!
  Returns the total number of bytes read from the device. Always returns 0, since the device only supports write operations.
  
  \return 0 Zero bytes have been read.
*/
IOSize FunnelDevice::bytesReadTotal(void)
{
  return 0;
}

/*!
  Returns the number of bytes read from the device by the last read operation. Always returns 0, since the device only supports write operations.
  
  \return 0 Zero bytes have been read.
*/
IOSize FunnelDevice::bytesRead(void)
{
  return 0;
}

/*!
  Returns the total number of bytes written to the device.
  
  \return Number of bytes.
*/
IOSize FunnelDevice::bytesWritten(void)
{
  return bw;
}

/*!
  If any of the IO operations was unsuccessful, this method can be used to test it. Always returns \a FALSE.
  
  \retval FALSE No error occurred.
*/
bool FunnelDevice::errorOccurred(void)
{
  return false;
}

/*!
  This method can be used for tests of the end of file. Always returns \a FALSE.
  
  \retval FALSE End of file didn't occurred.
*/
bool FunnelDevice::eof(void)
{
  return false;
}

