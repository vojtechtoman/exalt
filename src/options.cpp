/***************************************************************************
    options.cpp  -  Static initialization block for class ExaltOptions.
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
  \file options.cpp
  \brief Static initialization block for class ExaltOptions.
  
  This file contains a static initialization block for class ExaltOptions.
*/

#include "options.h"

ExaltOptions::OptionValue ExaltOptions::options[] = {
  ExaltOptions::No,		//Not in verbose mode
  ExaltOptions::AdaptiveModel,	//Using the adaptive model
  DEFAULT_OUTPUT_ENCODING,	//Use the default output encoding
  ExaltOptions::No,		//Don't print the grammar
  ExaltOptions::No,		//Don't print the element models
  ExaltOptions::None};
