/***************************************************************************
    options.h  -  Definitions of ExaltOptions class.
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
  \file options.h
  \brief Definitions of ExaltOptions class.
  
  This file contains definitions of ExaltOptions class.
*/


#ifndef OPTIONS_H
#define OPTIONS_H

#include "defs.h"
#include "xmldefs.h"
#include "encodings.h"


/*!
  A static class for storing the options.
 */
class ExaltOptions
{
public:
  //! An enum of possible option names.
  enum OptionName
  {
    //! The "verbose" option.
    Verbose,

    //! The "model" option.
    Model,

    //! The "encoding" option.
    Encoding,

    //! The "print grammar" option.
    PrintGrammar,

    //! The "print models" option.
    PrintModels,

    //! Dummy option, has to be the last one!
    LastOption		//keep this option at the end of the enum!
  };

  //! A type for handling the values of the options.
  typedef int OptionValue;

  //! Some predefined values of the options.
  enum
  {
    //! No value.
    None = -1,

    //! The value "No".
    No,

    //! The value "TYes".
    Yes,

    //! Use the simple model.
    SimpleModel,

    //! Use the adaptive model.
    AdaptiveModel
  };

  /*!
    \brief Set the value of the option.

    \param name The name of the option.
    \param value the value of the option.
   */
  static void setOption(OptionName name, OptionValue value) { options[name] = value; }

  /*!
    \brief Get the value of the option.

    \param name The name of the option.

    \return The value of the option.
   */
  static OptionValue getOption(OptionName name) { return options[name]; }

  /*!
    \brief Reset all options.

    The values of the options are all set to None.
   */
  static void resetOptions(void)
  {
    for (int i = 0; i < LastOption; i++)
      options[(OptionName)i] = None;
  }

private:
  //! An array of option values.
  static OptionValue options[];
};


#endif //OPTIONS_H



