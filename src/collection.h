/***************************************************************************
    collection.h  -  Definition of Stack template class.
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
  \file collection.h
  \brief Definition of Collection template class.

  This file contains definition of Collection template class.
*/


#ifndef COLLECTION_H
#define COLLECTION_H

#include "defs.h"

/*!
  \brief Base template class for all collection types (Stack, List, etc.)

  Inherit this class whenever you intend to create a new collection type.
 */
template<class T_> class Collection
{
public:
  /*!
    \brief A constructor.

    Calls init_()
  */
  Collection(void) { init_(); }

  /*!
    \brief Clear contents of the collection.

    Behaviour is dependend on auto deletion flag. If auto deletion is enabled, the memory occupied by the items in the collection will be freed; otherwise it will be preserved.

    \sa setAudoDelete()
  */
  virtual void clear(void) = 0;

  /*!
    \brief Return the number of items in the collection.
    
    \return Number of items.
  */
  virtual size_t count(void) { return cnt_; }

  /*! 
    \brief Test whether the collection is empty.

    \retval true Collection is empty.
    \retval false Collection isn't empty.
  */
  virtual bool isEmpty(void) { return cnt_ == 0; }

  /*!
    \brief Set auto delete flag.

    If auto deletion is enabled, the memory occupied by the items in the collection will be freed when they are removed from the collection; otherwise they will be preserved.

    Auto deletion is implicitly disabled.

    \param ad Turn on/off auto deletion (true/false).
  */
  virtual void setAutoDelete(bool ad) { autoDelete_ = ad; }


  

protected:
  //! Number of items in the collection.
  size_t cnt_;

  //! Auto deletion flag.
  bool autoDelete_;


  //! Initialization of the collection.
  virtual void init_(void)
  {
    cnt_ = 0;
    autoDelete_ = false;
  }

}; //Collection

#endif //COLLECTION_H
