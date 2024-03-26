/***************************************************************************
    funneldevice.h  -  Definition of the FunnelDevice class.
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
  \file funneldevice.h
  \brief Definition of the FunnelDevice class.
  
  This file contains the definition of the FunnelDevice class.
*/


#ifdef __GNUG__
# pragma interface
#endif

#ifndef CHANNELDEVICE_H
#define CHANNELDEVICE_H

#include "defs.h"
#include "debug.h"
#include "exceptions.h"
#include "iodevice.h"
#include "xmldefs.h"



//! The size of the FunnelDevice inner buffer.
#define DEFAULT_FUNNEL_BUFFER_SIZE	4098


class UserOfFunnelDevice;



/*!
  \brief A device that acts like a 'funnel'.

  This device can be used for one-directional XmlChar data exchange between two objects. The object that receives the data has to be derived from the UserOfFunnelDevice class, and has to implement the UserOfFunnelDevice::receiveData() method. The sender can send the data to the receiver using standard IODevice write functions.

  The data written to FunnelDevice are buffered until flush() is called, or the size of the inner buffer is exceeded. The buffering can be disabled in constructor to make the data delivery immediate.

  \sa IODevice, FileDevice.
 */
class FunnelDevice : public IODevice
{
public:
  //! A constructor.
  FunnelDevice(UserOfFunnelDevice *rcvr, bool isImmediate = false, size_t size = DEFAULT_FUNNEL_BUFFER_SIZE);

  //! A destructor.
  virtual ~FunnelDevice(void);

  //! Prepare the device.
  virtual void prepare(void) throw (ExaltIOException);

  //! Flush the device.
  virtual void flush(void) throw (ExaltIOException);

  //! Finish the work with the device.
  virtual void finish(void) throw (ExaltIOException);

  //! Read up to the specified number of bytes into the buffer (unused).
  virtual IOState readData(char *buf, IOSize length) throw (ExaltIOException);

  //! Read a char (unused).
  virtual IOState getChar(int *) throw (ExaltIOException);

  //! Write the specified number of bytes from the buffer.
  virtual IOState writeData(const char *buf, IOSize length) throw (ExaltIOException);

  //! Write a char.
  virtual IOState putChar(int) throw (ExaltIOException);

  //! Return the number of bytes read so far.
  virtual IOSize bytesReadTotal(void);

  //! Return the number of bytes read by the last read operation.
  virtual IOSize bytesRead(void);

  //! Return the number of bytes written so far (unused).
  virtual IOSize bytesWritten(void);

  //! Informs about an error.
  virtual bool errorOccurred(void);

  //! Informs about an end of file (unused).
  virtual bool eof(void);

protected:
  //! The receiver of the data.
  UserOfFunnelDevice *receiver;

  //! The size of the inner data buffer.
  size_t bufferSize;

  //! The length of the data in the buffer.
  size_t dataLength;

  //! Indication whether the buffer contains any data.
  bool dataPresent;

  //! Is the data delivery immediate, or buffered?
  bool immediate;

  //! The number of bytes written to the device.
  IOSize bw;

  //! The data buffer.
  XmlChar *buffer;
};


/*!
  \brief Class capable of receiving XmlChar strings via the FunnelDevice.

  Inherit this class whenever you want your class to be able to receive XmlChar strings via the FunnelDevice.
 */
class UserOfFunnelDevice
{
public:
  /*!
    \brief Receive the data of given size.

    This method is called when the data arrive from the FunnelDevice. 

    \param data Pointer to the incoming data.
    \param size The size of incoming data.
  */
  virtual void receiveData(XmlChar *data, size_t size) = 0;
};




#endif //FUNNELDEVICE_H
