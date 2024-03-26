/***************************************************************************
    debug.h  -  Debugging macros.
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
  \file debug.h
  \brief Debugging macros.
  
  This file contains definitions of some debugging macros.
*/

#ifndef DEBUG_H
#define DEBUG_H


#include <iostream>

#include "exceptions.h"


//! Macro for reporting debugging messages.
//#define DBG(_x_)		std::cerr << "[DBG] " << _x_ << std::endl
#define DBG(_x_)		{}


//! Macro for printing the name of current source file and line executed.
#define PRINT_SOURCE_POSITION	std::cerr << "In file " << __FILE__ << ", line " << __LINE__ << ": "


/*!
  \brief Macro testing for NULL pointers.

  If a NULL pointer is found, current source file and the executed line are printed on stderr and ExaltNullPointerException is raised.

  \param _x_ Pointer.
*/
#define CHECK_POINTER(_x_)			\
if ( !(_x_) )					\
  {						\
    PRINT_SOURCE_POSITION;			\
    std::cerr << "Out of memory" << std::endl;	\
    throw ExaltNullPointerException();		\
  } 


/*!
  \brief Macro for safer delete.

  Macro checks if given pointer isn't NULL (with CHECK_POINTER()). This ensures that deletion of NULL pointers will not occur. After deleting the data, the pointer is set to NULL.

  \param _x_ Pointer.

  \sa DELETE_ARRAY(), CHECK_POINTER()
 */
#define DELETE(_x_)		\
{				\
  CHECK_POINTER( (_x_) );	\
  delete (_x_);			\
  (_x_) = 0;			\
}


/*!
  \brief Macro for safer delete of arrays.

  Macro checks if given pointer isn't NULL (with CHECK_POINTER()). This ensures that deletion of NULL pointers will not occur. After deleting the array, the pointer is set to NULL.

  \param _x_ Pointer.

  \sa DELETE(), CHECK_POINTER()
 */
#define DELETE_ARRAY(_x_)	\
{				\
  CHECK_POINTER( (_x_) );	\
  delete[] (_x_);		\
  (_x_) = 0;			\
}



/*!
  \brief Macro for safer new.

  After allocating memory, the macro calls CHECK_POINTER().

  \param _p_ A variable
  \param _type_ Type of the variable (or better: the parameter of new)
  \sa CHECK_POINTER()
*/
#define NEW(_p_, _type_)	\
{				\
  (_p_) = new _type_;		\
  CHECK_POINTER( (_p_) );	\
}


#endif //DEBUG_H



