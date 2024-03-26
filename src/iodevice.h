/***************************************************************************
    iodevice.h  -  Definition of IODevice class.
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
  \file iodevice.h
  \brief Definition of the IODevice class.
  
  This file contains the definition of the IODevice class.
*/


#ifndef IODEVICE_H
#define IODEVICE_H



#include <cstdlib>
#include "defs.h"
#include "exceptions.h"


//! Type for size measurements.
typedef size_t IOSize;



/*!
  \brief An enum with values reporting the device state.

  In every moment, the device is in a certain state. This state influences the behaviour of the device.
 */
enum IOState
{
  //! The read operation was successful.
  ReadOk,

  //! The read operation was not successful.
  ReadError,

  //! The write operation was successful.
  WriteOk,

  //! The write operation was successful.
  WriteError,

  //! An end of file occurred.
  EndOfFile,

  //! Device is not prepared for work.
  DeviceNotPrepared

}; //IOState



/*!
  \brief The abstract parent of all input/output devices.

  Libexalt is designed to support various kinds of inputs and outputs. In order to make this possible, it provides an abstraction of an input/output device. This generic device defines an interface that has to be implemented by all the devices used for the input/output.

  The IODevice class defines also the style of the work with the devices. A typical example would look as follows (using FileDevice, which  represents a file input/output):

  \code
  ...

  FileDevice file;
  char buffer[BUFFER_SIZE]

  file.prepare("filename.txt");
  while (file.readData(buffer, BUFFER_SIZE) == ReadOk)
    {
      //...do something with the buffer
    }

  if (file.errorOccurred())
    {
      //...an error occurred.
    }

  file.finish();

  ...
  \endcode

  The device uses the C++ exceptions to report errors (see exceptions.h).

  \sa FileDevice, FunnelDevice.
 */
class IODevice
{
public:
  /*!
    \brief A constructor.

    Initializes the device as unprepared.
   */
  IODevice() { prepared = false;}

  /*!
    \brief Prepare the device.

    This method prepares the device for further work. In a case of a file device, for example, it can open a file. An attempt to work with unprepared device will cause an ExaltIOException to be raised.
  */
  virtual void prepare(void) throw (ExaltIOException) = 0;

  /*!
    \brief Flush the device.

    Ensures that all the data is written to the device.
  */
  virtual void flush(void) throw (ExaltIOException) = 0;

  /*!
    \brief Finish the work with the device.

    This method ends the work with the device. In a case of a file device, for example, it may close a file. An attempt to work with finished device will cause an ExaltIOException to be raised.
  */
  virtual void finish(void) throw (ExaltIOException) = 0;

  /*!
    \brief Read up to the specified number of bytes into the buffer.

    Reads up to \a length bytes from the device and stores them in \a buffer. If an error occurred, ExaltIOException is raised.

    \param buf Pointer to the buffer.
    \param length Length of the buffer.

    \return State of the device after the read operation.
  */
  virtual IOState readData(char *buf, IOSize length) throw (ExaltIOException) = 0;

  /*!
    \brief Read one char.

    Reads one character from the device. If an error occurred, ExaltIOException is raised.

    \param c Pointer where the read character is stored.

    \return State of the device after the read operation.
  */
  virtual IOState getChar(int *c) throw (ExaltIOException) = 0;

  /*!
    \brief Write the specified number of bytes from the buffer.

    Writes \a length bytes from the buffer \a buf to the device. If an error occurred, ExaltIOException is raised.

    \param buf Pointer to the buffer.
    \param length Length of the buffer.

    \return State of the device after the write operation.
  */
  virtual IOState writeData(const char *buf, IOSize length) throw (ExaltIOException) = 0;

  /*!
    \brief Write one char.

    Writes one character to the device. If an error occurred, ExaltIOException is raised.

    \param c The character.

    \return State of the device after the read operation.
  */
  virtual IOState putChar(int c) throw (ExaltIOException) = 0;

  /*!
    \brief Return the number of bytes read so far.

    Returns the total number of bytes read from the device.

    \return Number of bytes.
  */
  virtual IOSize bytesReadTotal(void) = 0;

  /*!
    \brief Return the number of bytes read by the last read operation.

    Returns the number of bytes read from the device by the last read operation.

    \return Number of bytes.
  */
  virtual IOSize bytesRead(void) = 0;


  /*!
    \brief Return the number of bytes written so far.

    Returns the total number of bytes written to the device.

    \return Number of bytes.
  */
  virtual IOSize bytesWritten(void) = 0;

  /*!
    \brief Informs whether an error occurred.

    If any of the IO operations was unsuccessful, this method can be used to test it.

    \retval TRUE An error occurred.
    \retval FALSE No error occurred.
  */
  virtual bool errorOccurred(void) = 0;

  /*!
    \brief Informs whether the end of file occurred.

    This method can be used for tests of the "end of file".

    \retval TRUE End of file occurred.
    \retval FALSE End of file didn't occurred.
  */
  virtual bool eof(void) = 0;

  /*!
    \brief Informs whether the device is prepared.

    If the device is prepared, this method returns \a TRUE.

    \retval TRUE The device is prepared.
    \retval FALSE The device is not prepared.
  */
  virtual bool isPrepared(void) { return prepared; }

protected:
  //! Indication whether the device is prepared.
  bool prepared;

}; //IODevice



#endif //IODEVICE_H
