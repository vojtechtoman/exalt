/***************************************************************************
    saxreceptor.h  -  Definition of SAXReceptor class
                             -------------------
    begin                : October 24 2002
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
  \file saxreceptor.h
  \brief Definition of SAXReceptor class.
  
  This file contains the definition of SAXReceptor class.
*/



#ifndef SAXRECEPTOR_H
#define SAXRECEPTOR_H


#include "defs.h"
#include "options.h"
#include "saxbase.h"
#include "xmldefs.h"


/*!
  \brief An abstract predecessor of all SAX receptors.

  Inherit this class whenever you want to implement your own SAX receptor.
 */
class SAXReceptor : public SAXBase
{
public:
  //! A Constructor.
  SAXReceptor(void) : SAXBase() {}
};

#endif //SAXRECEPTOR_H
