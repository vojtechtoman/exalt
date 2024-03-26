/***************************************************************************
    hashtable.h  -  Definition of template class HashTable.
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
  \file hashtable.h
  \brief Definition of HashTable template class.

  This file contains definition of HashTable template class.
*/


#ifndef HASHTABLE_H
# define HASHTABLE_H


#include <cstring>	//for strcmp()

#include "defs.h"
#include "collection.h"
#include "debug.h"
#include "xmldefs.h"
#include "xmlchar.h"





/*!
  \brief Simple hash function for hashing XmlChar strings.

  This function hashes given XmlChar string depending on the values of its characters.

  \param s Hashed XmlChar string.
  \return Number representing hashed string.
 */
static inline unsigned long hashXmlString(const XmlChar *s)
{
  unsigned long h = 0; 

  for ( ; *s; ++s)
    h = 5 * h + *s;
  
  return h;
}



/*!
  \brief Template structure for hashing various data types.

  \param Key_ Class representing type for key values.
  \param size_ The size of the hash array.
 */
template<unsigned long size_> struct HashTableHash
{
  //! Hashing of a \a XmlChar \a string.
  //unsigned long operator()(XmlChar *s) const { return hashXmlString_(s) % size_; }

  //! Hashing of a \a const \a XmlChar \a string.
  unsigned long operator()(const XmlChar *s) const { return hashXmlString(s) % size_; }

  //! Hashing of a \a char.
  unsigned long operator()(char x) const { return x % size_; }

  //! Hashing of an \a unsigned char.
  unsigned long operator()(unsigned char x) const { return x % size_; }

  //! Hashing of a \a short.
  //unsigned long operator()(short x) const { return x % size_; }

  //! Hashing of an \a unsigned \a short.
  //unsigned long operator()(unsigned short x) const { return x % size_; }

  //! Hashing of an \a integer.
  unsigned long operator()(int x) const { return x % size_; }

  //! Hashing of an \a unsigned \a integer.
  unsigned long operator()(unsigned int x) const { return x % size_; }

  //! Hashing of a \a long.
  unsigned long operator()(long x) const { return x % size_; }

  //! Hashing of an \a unsigned \a long .
  unsigned long operator()(unsigned long x) const { return x % size_; }
};





/*!
  \brief Template structure for hashing various data types.

  \param Key_ Class representing type for key values.
  \param size_ The size of the hash array.
 */
struct HashTableComparator
{
  //! Compare two XmlChars.
  bool compare(XmlChar x, XmlChar y) { return x == y; }

  //! Compare two chars.
  bool compare(unsigned char x, unsigned char y) { return x == y; }
  //bool compare(short x, short y) { return x == y; }
  //bool compare(unsigned short x, unsigned short y) { return x == y; }

  //! Compare two integers.
  bool compare(int x, int y) { return x == y; }

  //! Compare two unsigned integers.
  bool compare(unsigned int x, unsigned int y) { return x == y; }

  //! Compare two longs.
  bool compare(long x, long y) { return x == y; }

  //! Compare two unsigned longs.
  bool compare(unsigned long x, unsigned long y) { return x == y; }

  //! Compare two XmlChar strings.
  bool compare(XmlChar *x, XmlChar *y) { return !xmlchar_strcmp(x, y); }
};



/*!
  \brief Template class representing one item in the HashTable.
  
  \param Key_ Class representing the type of a key.
  \param T_ Class representing the type of data.
*/
template<class Key_, class T_> struct HashTableItem
{
  //! Key assigned to the item.
  Key_ key_;

  //! Data of the item.
  T_ *data_;
};



/*!
  \brief Template class representing one bucket in the HashTable.

  \param Key_ Class representing the type of a key.
  \param T_ Class representing the type of data.
  \param HashTableBucketContainer_ Template class representing the container type that forms a base for the HashTableBucket.
*/
template<class Key_, class T_, template<class T_>
 class HashTableBucketContainer_> class HashTableBucket
#ifndef DOXYGEN_SHOULD_SKIP_THIS
   : public HashTableBucketContainer_< HashTableItem<Key_, T_> >
#endif //DOXYGEN_SHOULD_SKIP_THIS
{
public:
  //! Comparator of some basic data types.
  HashTableComparator comparator_;

  /*!
    \brief Insert item to the bucket.

    \param item Pointer to inserted HashTableItem.
   */
  virtual void insertItem(HashTableItem<Key_, T_> *item)
  {
    prepend(item);
  }


  /*!
    \brief Find item by a key.

    \param key Value of the key.
    \return Pointer to the data of the item or \a NULL if the item couldn't be found.
   */
  virtual T_ *find(Key_ key)
  {
    HashTableItem<Key_, T_> *pom;

    for (pom = first(); pom; pom = next())
      {
	if (comparator_.compare(pom->key_, key))
	  return pom->data_;
      }
    
    return 0;
  }



  /*!
    Remove the item with given key.

    Data of the item is not deleted unless setAutoDelete was changed.

    \param key Value of the key.
    \retval true Item has been removed.
    \retval false Item hasn't been removed (for example, because it was not in the hash array).
   */
  virtual bool removeItem(Key_ key)
  {
    HashTableItem<Key_, T_> *pom;

    pom = first();

    while (pom)
      {
	if (comparator_.compare(pom->key_, key))
	  {
	    if (autoDelete_)
	      //if neccessary, delete the contents of found item
	      DELETE(pom->data_);
	    
	    //delete the item
	    return remove();
	  }

	pom = next();
      }
    
    return false;
  }
};



/*!
  \brief Template class representing a hash array.

  Hash array is implemented as an array (with given \a size_) of buckets. Buckets are represented by \a BucketContainer_ and hold data of type \a T_ which are accessible by keys of type \a Key_. For best performance, the \a size_ should be a suitably large prime number.

  %List of some useful prime numbers:
  \code
  53,           97,           193,         389,         769,  
  1543,         3079,         6151,        12289,       24593,  
  49157,        98317,        196613,      393241,      786433,  
  1572869,      3145739,      6291469,     12582917,    25165843,  
  50331653,     100663319,    201326611,   402653189,   805306457,   
  1610612741,   3221225473,   4294967291
  \endcode


  \param Key_ Class representing the type of a key.
  \param T_ Class representing the type of data.
  \param BucketContainer_ Template class representing container for buckets.
  \param size_ Size of the hash array.
 */
template<class Key_, class T_, template<class T_> class HashTableBucketContainer_, unsigned long size_> class HashTable : public Collection<T_>
{
public:

  /*!
    \brief A constructor.
   */
  HashTable(void) : Collection<T_>()
  {
    NEW(table_, (HashTableBucket<Key_, T_, HashTableBucketContainer_> *[size_]));

    for (unsigned long i = 0; i < size_; i++)
      table_[i] = 0;
  }

  /*!
    \brief A destructor.

    Clears the contents of the hash array.
   */
  virtual ~HashTable(void)
  {
    clear();
    DELETE_ARRAY(table_);
  }


  /*!
    \brief Find an item by given key.

    This function finds an item by given key.

    \param key The value of the key.

    \return Pointer to found data, or \a NULL.
   */
  virtual T_ *find(Key_ key)
  {
    unsigned long pos = hash_(key);

    if (!table_[pos])
      return 0;
    else
      return table_[pos]->find(key);
  }


  /*!
    \brief Insert an item with gven key and data.

    This function inserts new item into the hash array using appropriate hash function.

    \param key The value of the key.
    \param item Data of the item.
   */
  virtual void insert(Key_ key, T_ *item)
  {
    unsigned long pos = hash_(key);

    if (!table_[pos])
      {
	NEW(table_[pos], (HashTableBucket<Key_, T_, HashTableBucketContainer_>));
      }

    if (!contains(key))
      {
	HashTableItem<Key_, T_> *tableItem;

	NEW(tableItem, (HashTableItem<Key_, T_>));
	
	tableItem->key_ = key;
	tableItem->data_ = item;
	table_[pos]->insertItem(tableItem);
	cnt_++;
      }
  }


  /*!
    \brief Remove an item with given key.

    Data of the item is not deleted unless setAutoDelete was changed.

    \param key The value of the key.
    
    \retval true Item has been removed.
    \retval false Item hasn't been removed.
   */
  virtual bool remove(Key_ key)
  {
    unsigned long pos = hash_(key);

    if (!table_[pos])
      return false;
    else
      {
	if (table_[pos]->removeItem(key))
	  {
	    cnt_--;
	    return true;
	  }
	else
	  return false;
      }
  }


  /*!
    \brief Test whether hash array contains an item with given key.

    \param key The value of the key.
    
    \retval true Hash array contains the item.
    \retval false Hash array doesn't contain the item.
   */
  virtual bool contains(Key_ key)
  {
    unsigned long pos = hash_(key);

    if (!table_[pos])
      return false;
    else
      return (table_[pos]->find(key) != 0);
  }


  /*!
    \brief Clear the hash array.

    Removes all items from the hash array. The data of the items aren't removed unless setAutoDelete is called.
   */
  virtual void clear(void)
  {
    for (unsigned long i = 0; i < size_; i++)
      {
	if (autoDelete_)
	  {
	    if (table_[i])
	      {
		//HashTableItem<Key_, T_> *pom;

		//for (pom = table_[i]->first(); pom; pom = table_[i]->next())
		//  DELETE(pom->data_);
	      }
	  }

	if (table_[i])
	  {
	    DELETE(table_[i]);
	    //	    table_[i] = 0;
	  }
      }
  }

  
  /*!
    Turn on/off auto deletion of item data.

    If auto deletion is turned off (the default), removing an item doesn't delete the data of the item. If auto deletion is turned odn, the data is deleted together with the item.

    \param ad Flag of auto deletion (\a true or \a false).
   */
  virtual void setAutoDelete(bool ad)
  {
    for (unsigned long i = 0; i < size_; i++)
      {
	if (table_[i])
	  table_[i]->setAutoDelete(ad);
      }

    autoDelete_ = ad;
  }

protected:
  //! An array of buckets.
  HashTableBucket<Key_, T_, HashTableBucketContainer_> **table_;

  //! Hash structure.
  HashTableHash<size_> hash_;
};

#endif //HASHTABLE_H

