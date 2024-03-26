/***************************************************************************
    queue.h  -  Definition of Queue template class.
                             -------------------
    begin                : October 14 2002
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
  \file queue.h
  \brief Definition of Queue template class.

  This file contains definition of Queue template class.
*/

#ifndef QUEUE_H
#define QUEUE_H

#include "defs.h"
#include "collection.h"
#include "debug.h"


/*!
  \brief Template class representing linked list on which Queue operates.
 */
template<class T_> struct QueueLinkedList
{
  //! Pointer to next item of the list.
  struct QueueLinkedList<T_> *next_;
  
  //! Pointer to the data.
  T_ *data_;

}; //QueueLinkedList


/*!
  \brief Template class representing queue.
 */
template<class T_> class Queue : public Collection<T_>
{
public:
  /*!
    \brief A constructor.

    Calls initQueue()
   */
  Queue(void) : Collection<T_>() { initQueue(); }

  /*!
    \brief A destructor.

    Calls clear()
   */
  virtual ~Queue(void) { clear(); }



  /*!
    \brief Get the data from the first item in the queue.

    The item isn't tremoved.

    \returns Pointer to the data.
   */
  virtual T_ *first(void)
  { 
    if (!firstNode_)
      return 0;
    else
      {
	return firstNode_->data_;
      }
  }


  /*!
    \brief Get the data from the last item in the queue.

    The item isn't tremoved.

    \returns Pointer to the data.
   */
  virtual T_ *last(void)
  { 
    if (!lastNode_)
      return 0;
    else
      {
	return lastNode_->data_;
      }
  }



  /*!
    \brief Enqueue data.

    \param item Pointer to enqueued data.
   */
  virtual void enqueue(T_ *item)
  {
    QueueLinkedList<T_> *pom;

    NEW(pom, QueueLinkedList<T_>);

    pom->data_ = item;
    pom->next_ = 0;

    if (!lastNode_)
      firstNode_ = pom;
    else
      lastNode_->next_ = pom;

    lastNode_ = pom;

    cnt_++;
  }



  /*!
    \brief Dequeue data.

    \return Pointer to data.
   */
  virtual T_ *dequeue(void)
  {
    if (!firstNode_)
      return 0;
    else
      {
	QueueLinkedList<T_> *pom;
	T_ *data;
	
	pom = firstNode_;
	data = firstNode_->data_;

	firstNode_ = firstNode_->next_;

	if (!firstNode_)
	  lastNode_ = 0;

	cnt_--;

	DELETE(pom);

	return data;
      }
  }



  /*!
    \brief Empty the queue.

    If auto deletion is enabled, all data items will be removed from memory.

    \sa setAutoDelete()
   */
  virtual void clear(void)
  {
    QueueLinkedList<T_> *pom;

    while (firstNode_)
      {
	pom = firstNode_;
	firstNode_ = firstNode_->next_;

	if (autoDelete_)
	  DELETE(pom->data_);

	DELETE(pom);
      }

    firstNode_ = 0;
    lastNode_ = 0;
    cnt_ = 0;
  }


  

protected:
  //! Linked list of items of the queue.
  struct QueueLinkedList<T_> *firstNode_;

  //! Pointer to the last item of the queue.
  struct QueueLinkedList<T_> *lastNode_;

  //! Initialize the queue.
  virtual void initQueue(void)
  {
    firstNode_ = 0;
    lastNode_ = 0;
  }

}; //Queue


#endif //QUEUE_H
