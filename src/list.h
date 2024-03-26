/***************************************************************************
    list.h  -  Definition of List template class.
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
  \file list.h
  \brief Definition of List template class.

  This file contains definition of List template class.
*/

#ifndef LIST_H
#define LIST_H

#include "defs.h"
#include "collection.h"
#include "debug.h"



/*!
  \brief Template class representing doubly-linked list on which List operates.
 */
template<class T_> struct ListLinkedList
{
  //! Pointer to the previous item.
  struct ListLinkedList<T_> *prev_;
  
  //! Pointer to the previous item.
  struct ListLinkedList<T_> *next_;
  
  //! Pointer to the data.
  T_ *data_;

}; //SLListLinkedList



/*!
  \brief Template class representing doubly-linked list.
 */
template<class T_> class List : public Collection<T_>
{
public:
  /*!
    \brief A constructor.

    Calls initList()
   */
  List(void) : Collection<T_>() { initList(); }

  /*!
    \brief A destructor

    Calls clear()
  */
  virtual ~List(void) { clear(); }


  /*!
    \brief Get pointer of the data of the first item.

    \return Pointer to the data.

    \sa last(), current()
   */
  virtual T_ *first(void)
  { 
    if (!firstNode_)
      return 0;
    else
      {
	currentNode_ = firstNode_;
	return firstNode_->data_;
      }
  }


  /*!
    \brief Get pointer of the data of the last item.

    \return Pointer to the data.

    \sa first(), current()
   */
  virtual T_ *last(void)
  { 
    if (!lastNode_)
      return 0;
    else
      {
	currentNode_ = lastNode_;
	return lastNode_->data_;
      }
  }


  /*!
    \brief Get pointer of the data of the current item.

    \return Pointer to the data.

    \sa first(), last()
   */
  virtual T_ *current(void)
  { 
    if (!currentNode_)
      return 0;
    else
      {
	return currentNode_->data_;
      }
  }


  /*!
    \brief Get pointer of the data of the next item.

    This next item becomes the current item. If there is no next item, current item will be NULL.
    
    \return Pointer to the data or NULL if there is no next item.

    \sa current()
   */
  virtual T_ *next(void)
  {
    if (!currentNode_)
      return 0;
    else
      if (!currentNode_->next_)
	{
	  currentNode_ = 0;
	  return 0;
	}
      else
	{
	  currentNode_ = currentNode_->next_;
	  return currentNode_->data_;
	}
  }


  /*!
    \brief Get pointer of the data of the previous item.

    This previous item becomes the current item. If there is no previous item, current item will be NULL.
    
    \return Pointer to the data or NULL if there is no previous item.

    \sa current()
   */
  virtual T_ *prev(void)
  {
    if (!currentNode_)
      return 0;
    else
      if (!currentNode_->prev_)
	{
	  currentNode_ = 0;
	  return 0;
	}
      else
	{
	  currentNode_ = currentNode_->prev_;
	  return currentNode_->data_;
	}
  }


  /*!
    \brief Insert item at current position.

    The new item will become the current item. If current() is NULL, the will become the first item of the list.

    \param item Pointer to the data.

    \sa remove()
   */
  virtual void insert(T_ *item)
  {
    ListLinkedList<T_> *pom;

    NEW(pom, ListLinkedList<T_>);

    pom->data_ = item;


    if (!firstNode_)
      {
	pom->prev_ = 0;
	pom->next_ = 0;
	firstNode_ = pom;
	lastNode_ = pom;
      }
    else
      {
	if (!currentNode_)
	  {
	    pom->prev_ = 0;
	    pom->next_ = firstNode_;
	    firstNode_->prev_ = pom;
	  }
	else
	  {
	    pom->prev_ = currentNode_;
	    pom->next_ = currentNode_->next_;
	    currentNode_->next_ = pom;
	    
	    if (!pom->next_)
	      lastNode_ = pom;
	    else
	      pom->next_->prev_ = pom;
	  }
      }

    currentNode_ = pom;

    cnt_++;
  }


  /*!
    \brief Append item to the list.

    The new item will become the current item.

    \param item Pointer to the data.

    \sa remove(), prepend()
   */
  virtual void append(T_ *item)
  {
    ListLinkedList<T_> *pom;

    NEW(pom, ListLinkedList<T_>);

    pom->next_ = 0;
    pom->prev_ = lastNode_;
    pom->data_ = item;

    if (!firstNode_)
      firstNode_ = pom;
    else
      lastNode_->next_ = pom;

    lastNode_ = pom;
    currentNode_ = pom;

    cnt_++;
  }


  /*!
    \brief Prepend item to the list.

    The new item will become the current item.

    \param item Pointer to the data.

    \sa remove(), append()
   */
  virtual void prepend(T_ *item)
  {
    ListLinkedList<T_> *pom;

    NEW(pom, ListLinkedList<T_>);

    pom->prev_ = 0;
    pom->next_ = firstNode_;
    pom->data_ = item;


    !lastNode_ ? lastNode_ = pom : firstNode_->prev_ = pom;

    firstNode_ = pom;

    currentNode_ = pom;

    cnt_++;
  }


  /*!
    \brief Remove the first item of the list and return its data.
    
    \return Pointer to the data.
   */
  virtual T_ *getFirst(void)
  {
    if (!firstNode_)
      return 0;
    else
      {
	ListLinkedList<T_> *pom;
	T_ *data;
	
	pom = firstNode_;
	data = firstNode_->data_;


	firstNode_ = firstNode_->next_;
	currentNode_ = firstNode_;


	if (firstNode_)
	  {
	    firstNode_->prev_ = 0;
	  }

	cnt_--;

	DELETE(pom);
	return data;
      }
  }


  /*!
    \brief Remove current item.

    After removal, the current item will be the item following the removed one, or NULL. If auto deletion is enabled, also the data of the item are removed from memory.

    \retval true Item has been removed.
    \retval false Item hasn't been removed.
    
    \sa setAutoDelete(), insert()
   */
  virtual bool remove(void)
  {
    if (!currentNode_)
      return false;
    else
      {
	ListLinkedList<T_> *pom1, *pom2;
	
	pom1 = currentNode_->prev_;
	pom2 = currentNode_->next_;


	if (pom1)
	  pom1->next_ = pom2;
	else
	  firstNode_ = pom2;

	if (pom2)
	  pom2->prev_ = pom1;
	else
	  lastNode_ = pom1;

	if (autoDelete_)
	  {
	    DELETE(currentNode_->data_);
	  }


	DELETE(currentNode_);
	cnt_--;

	currentNode_ = pom2;


	return true;
      }
  }



  /*!
    \brief Remove item with given data.

    After removal, the current item will be the item following the removed one, or NULL. If auto deletion is enabled, also the data of the item are removed from memory.

    \retval true Item has been removed.
    \retval false Item hasn't been removed.
    
    \sa setAutoDelete(), insert()
   */
  virtual bool remove(T_ *item)
  {
    ListLinkedList<T_> *pom;

    pom = firstNode_;

    while (pom)
      {
	if (pom->data_ == item)
	  {
	    currentNode_ = pom;
	    return remove();
	  }
	else
	  pom = pom->next_;
      }
    
    return false;
  }



  /*!
    \brief Test if list contains given item.

    \param item Pointer to the data.

    \retval true List contains the item.
    \retval false List doesn't contain the item.
   */
  virtual bool contains(T_ *item)
  {
    ListLinkedList<T_> *pom;

    pom = firstNode_;

    while (pom)
      {
	if (pom->data_ == item)
	  return true;
	else
	  pom = pom->next_;
      }
    
    return false;
  }


  /*!
    \brief Clear the list.

    If auto deletion is enabled, also the data of the items are removed from memory.

    \sa setAutoDelete()
   */
  virtual void clear(void)
  {
    ListLinkedList<T_> *pom;

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
    currentNode_ = 0;
    cnt_ = 0;
  }


protected:
  //! Pointer to the first node of the list.
  struct ListLinkedList<T_> *firstNode_;

  //! Pointer to the last node of the list.
  struct ListLinkedList<T_> *lastNode_;

  //! Pointer to the current node of the list.
  struct ListLinkedList<T_> *currentNode_;


  //! Initialization of the list.
  virtual void initList(void)
  {
    firstNode_ = 0;
    lastNode_ = 0;
    currentNode_ = 0;
  }

}; //List


#endif //LIST_H
