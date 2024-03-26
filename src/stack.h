/***************************************************************************
    stack.h  -  Definition of Stack template class.
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
  \file stack.h
  \brief Definition of Stack template class.

  This file contains definition of Stack template class.
*/

#ifndef STACK_H
#define STACK_H


#include "defs.h"
#include "collection.h"
#include "debug.h"


/*!
  \brief Template class representing linked list on which Stack operates.
 */
template<class T_> struct StackLinkedList
{
  //! Pointer to previous item of the list.
  struct StackLinkedList<T_> *prev_;
  
  //! Pointer to the data.
  T_ *data_;

}; //StackLinkedList


/*!
  \brief Template class representing stack.
 */
template<class T_> class Stack : public Collection<T_>
{
public:
  /*!
    \brief A constructor.

    Calls initStack()
   */
  Stack(void) : Collection<T_>() { initStack(); }

  /*!
    \brief A destructor.

    Calls clear()
   */
  virtual ~Stack(void) { clear(); }



  /*!
    \brief Get the data from the top  of the stack.

    Top item isn't tremoved.

    \returns Pointer to the data.
   */
  virtual T_ *top(void)
  { 
    if (!topNode_)
      return 0;
    else
      {
	return topNode_->data_;
      }
  }



  /*!
    \brief Push data.

    \param item Pointer to pushed data.
   */
  virtual void push(T_ *item)
  {
    StackLinkedList<T_> *pom;

    NEW(pom, StackLinkedList<T_>);

    pom->data_ = item;

    if (!topNode_)
      pom->prev_ = 0;
    else
      pom->prev_ = topNode_;

    topNode_ = pom;

    cnt_++;
  }



  /*!
    \brief Pop data.

    \return Pointer to data.
   */
  virtual T_ *pop(void)
  {
    if (!topNode_)
      return 0;
    else
      {
	StackLinkedList<T_> *pom;
	T_ *data;
	
	pom = topNode_;
	data = topNode_->data_;

	topNode_ = topNode_->prev_;

	cnt_--;

	DELETE(pom);
	return data;
      }
  }



  /*!
    \brief Empty the stack.

    If auto deletion is enabled, all data items will be removed from memory.

    \sa setAutoDelete()
   */
  virtual void clear(void)
  {
    StackLinkedList<T_> *pom;

    while (topNode_)
      {
	pom = topNode_;
	topNode_ = topNode_->prev_;

	if (autoDelete_)
	  DELETE(pom->data_);

	DELETE(pom);
      }

    topNode_ = 0;
    cnt_ = 0;
  }


  

protected:
  //! Linked list of items of the stack.
  struct StackLinkedList<T_> *topNode_;

  //! Initialize the stack.
  virtual void initStack(void)
  {
    topNode_ = 0;
  }

}; //Stack


#endif //STACK_H
