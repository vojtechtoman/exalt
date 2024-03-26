/***************************************************************************
    kydefs.h  -  Definitions of types and macros used by KYGrammar class
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
  \file kydefs.h
  \brief Definitions of types and macros used by KYGrammar class.
  
  This file contains definitions of types and macros used by KYGrammar class.
*/

#ifndef KYDEFS_H
#define KYDEFS_H


#include "defs.h"
#include "arithdefs.h"
#include "grammardefs.h"
#include "hashtable.h"
#include "list.h"
#include "stack.h"


/*!
  \brief Default size of terminal set.
 */
#define TERMINAL_ALPHABET_DEFAULT_SIZE	256


/*!
  \brief Type for rule reference counts.
 */
typedef unsigned int RuleFreq;


/*!
  \brief Type for rule identifications.
 */
typedef unsigned long RuleId;



struct Rule;	//forward definition of the Rule structure



/*!
  \brief One element of a Rule.

  Rule is represented as a doubly-linked list of this rule elements (terminals and variables).
 */
struct RuleElement
{
  //! Type of the element.
  SymbolType type;

  //! Pointer to the previous element in the rule.
  RuleElement *prev;

  //! Pointer to the next element in the rule.
  RuleElement *next;
  

  //! Value of the element.
  union
  {
    //! Value of terminal symbol.
    TerminalValue value;

    //! Value of a variable (pointer to a rule).
    Rule *rule;
  };
};


/*!
  \brief Results of rule-matching operations.

  Used when matching rules against the contents of the input queue.
 */
enum RuleMatchResult
{
  //! Rule matches.
  Matches = 1,

  //! Rule doesn't match.
  DoesntMatch = 2,

  //! No result.
  Ignore = 3
};


/*!
 \brief Structure representing one rule.

 Each rule has its reference counter, right side (represented by a doubly-linked list of RuleElement), pointer to the last element of the right side, and unique rule identifier.
 */
struct Rule
{
public:
  //! Reference count of the rule.
  RuleFreq refCount;

  //! Counter used during arithmetic coding.
  FreqValue counter;

  //! Body of the rule.
  RuleElement *body;

  //! Last element of the body.
  RuleElement *last;

  //! Unique identifier of the rule.
  RuleId id;

  //! Identification of the run when the rule was matched.
  unsigned long matchRun;

  //! Result of last rule-matching operation.
  RuleMatchResult matchResult;
};


/*!
  \brief Definition of a rule set.

  Implemented as a doubly-linked list of elements with type Rule.
 */
typedef List<Rule> RuleSet;


/*!
  \brief Description of symbol following given symbol - a neighbour.

  This description is used in digrams to store neccessary information about element's neighbour.
 */
struct RuleElementNeighbour
{
  //! Pointer to element's original location.
  RuleElement *origin;

  //! Pointer to the rule which contains the neighbour.
  Rule *rule;
};


/*!
  \brief Linked list containg descriptions of all element's neighbours.
 */
typedef List<RuleElementNeighbour> NeighboursDescriptionList;


/*!
  \brief Linked list of rules that represent a string (equal or longer) than the string represented by given rule element.
 */
typedef List<Rule> RuleElementRepresentedByList;


/*!
  \brief Description of a rule element's neighbour.
 */
struct NeighboursDescription
{
  //! %List of elements neighbours.
  NeighboursDescriptionList neighbours;

  //! %List of rules that represent the element.
  RuleElementRepresentedByList representedBy;
};


/*!
  \brief Hash array mapping unsigned long to lists with neighbours descriptions for terminals.
 */
typedef HashTable<unsigned long, NeighboursDescription, List, 3079> TerminalDigrams;


/*!
  \brief Hash array mapping unsigned long to lists with neighbours descriptions for variables.
 */
typedef HashTable<unsigned long, NeighboursDescription, List, 98317> VariableDigrams;


/*!
  \brief Structure for description how many input symbol matched the (whole) rule.
 */
struct MatchingRule
{
  //! Pointer to the rule
  Rule *rule;

  //! Matched length (in terminals).
  unsigned int length;
};


/*!
  \brief One item of the input queue.
 */
struct InputItem
{
  //! Pointer to the next item.
  InputItem *next;

  //! Value of the item.
  TerminalValue value;
};


/*!
  \brief Description of the currently examined rule element.
 */
struct TestedElement
{
  //! Pointer to the rule element.
  RuleElement *rel;

  //! Pointer to the next tested rule element.
  TestedElement *next;
};


/*!
  \brief Description of currently tested rule.
 */
struct TestedRule
{
  //! Pointer ot the rule.
  Rule *rule;

  //! Number of symbols in the input queue that the rule matches.
  unsigned int matchedLength;

  //! Indication if the rule matches completely, or if it represents more than the current string in th einput queue.
  bool matchesCompletely;

  //! List of tested elements of the rule (elements that lead to other rules are pushed here).
  TestedElement *currentElements;

  //! Pointer to the last item of the input queue that was tested.
  InputItem *lastInputItem;
};


/*!
  \brief Linked list of tested rules.
 */
typedef List<TestedRule> TestedRules;



#endif //KYDEFS_H



