/***************************************************************************
    filedevice.h  -  Definition of the FileDevice class.
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
  \file filedevice.h
  \brief Definition of the FileDevice class.
  
  This file contains the definition of the FileDevice class.
*/


#ifndef FILEDEVICE_H
#define FILEDEVICE_H

#ifdef __GNUG__
# pragma interface
#endif


#include <fstream>
#include "iodevice.h"
#include "defs.h"



/*!
  \brief A device for working with files.

  This class encapsulates the fstream functionality into the interface of IODevice.

  \sa IODevice, FunnelDevice.
 */
class FileDevice : public IODevice, public fstream
{
public:
  //! A constructor.
  FileDevice(void);

  //! A destructor.
  virtual ~FileDevice(void);

  //! Prepare the device with specified file name and mode.
  virtual void prepare(const char *, int) throw (ExaltIOException);

  //! Prepare the device (using standard input).
  virtual void prepare(void) throw (ExaltIOException) { prepare(0, ios::in); }

  //! Flush the device.
  virtual void flush(void) throw (ExaltIOException);

  //! Finish the work with the device.
  virtual void finish(void) throw (ExaltIOException);

  //! Read up to the specified number of bytes into the buffer.
  virtual IOState readData(char *buf, IOSize length) throw (ExaltIOException);

  //! Read one char.
  virtual IOState getChar(int *) throw (ExaltIOException);

  //! Write the specified number of bytes from the buffer.
  virtual IOState writeData(const char *buf, IOSize length) throw (ExaltIOException);

  //! Write one char.
  virtual IOState putChar(int) throw (ExaltIOException);

  //! Return the number of bytes read so far.
  virtual IOSize bytesReadTotal(void);

  //! Return the number of bytes read by the last read operation.
  virtual IOSize bytesRead(void);

  //! Return the number of bytes written so far.
  virtual IOSize bytesWritten(void);

  //! Informs about an error.
  virtual bool errorOccurred(void);

  //! Informs about an end of file.
  virtual bool eof(void);

protected:
  //! The number of bytes written so far.
  IOSize bWritten;

  //! The number of bytes read so far.
  IOSize bRead;
};



#endif //FILEDEVICE_H
