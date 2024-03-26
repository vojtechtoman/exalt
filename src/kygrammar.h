/***************************************************************************
    kygrammar.h  -  Definition of KYGrammar class
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
  \file kygrammar.h
  \brief Definition of KYGrammar class.
  
  This file contains the definition of KYGrammar class.
*/

#ifdef __GNUG__
# pragma interface
#endif

#ifndef KYGRAMMAR_H
#define KYGRAMMAR_H


#include "defs.h"
#include "grammarbase.h"
#include "queue.h"
#include "stack.h"
#include "kydefs.h"
#include "iodevice.h"
#include "fibonacci.h"
#include "contextbase.h"

#ifdef __WIN32__
# include <Winsock2.h>
#else
# include <sys/time.h>
#endif



/*!
  \brief The maximum size of the stack with tested rules (used when flushing the grammar).
 */
#define KY_GRAMMAR_FLUSH_STACK_SIZE	8192


/*!
  \brief The maximum size of the stack with tested rules.
 */
#define KY_GRAMMAR_RULE_STACK_SIZE	8192


/*!
  \brief The maximum amount of memory occupied by the grammar.

  When the grammars grows larger, it is purged and new (empty) grammar is formed.
 */
#define KY_GRAMMAR_MAX_SIZE		8388608


/*!
  \brief The periodicity of the eatData() calls.

  Frequent calls to eatData() cause slower performance because of the many operations needed to be done when eating the data. By calling eatData() not so frequently, we gain in speed.
 */
#define KY_GRAMMAR_EATDATA_PERIODICITY	800



/*!
  \brief Class implementing the Kieffer-Yang grammar based data compression.
  
  Grammar structure implementing the Kieffer-Yang grammar-based 
  compression algorithm.
*/
class KYGrammar : public GrammarBase
{
public:
  //! A constructor.
  KYGrammar(void);

  //! A destructor.
  virtual ~KYGrammar(void);

  //! Purge the grammar.
  virtual void purge(void);

  //! Set the base size of the terminal alphabet.
  virtual void setAlphabetBaseSize(size_t size);

  //! Set context for encoding and decoding.
  virtual void setContext(ContextBase *ctxt, bool useForOutput = true);

  //! Set output device for output.
  void setOutputDevice(IODevice *device);

  //! Append one terminal symbol to the root rule of the grammar.
  virtual void append(TerminalValue);

  //! Append one symbol to the root rule.
  virtual void appendToRootRule(RuleElement *);

  //! Ensure that all appended terminal symbols have been processed.
  virtual void flush(void);

  //! Find rule with given id.
  virtual Rule *findRule(RuleId id);

  //! Print contents of the grammar.
  virtual void print(void);


  //! Reconstruct data represented by the grammar.
  virtual void reconstructInput(void);


  //! Print list of all digrams occurring in the rules.
  virtual void printDigrams(void);


protected:

  //! A coding context.
  ContextBase *context;

  //! Indication is the arithmetic context is used for output (when compressing).
  bool useContextForOutput;

  //! A value of the symbol that has to be installed.
  long newSymbolToInstall;

  /*!
    \brief Last symbol fixed in the initial table of the context.

    Used for encoding newly created variables. Each new variable is encoded by a number which is a sum of the variable id and the number of lastFixedContextSymbol.
  */
  long lastFixedContextSymbol;


  //! The amount of the memory occupied by the grammar.
  size_t size;

  /*!
    \brief Output device.

    In some cases the grammar needs to output some data to the output stream (for example, the information about the alphabet size etc.). In order to be this possible, the grammar should have direct access to the output device.

    \sa setOutputDevice().
  */
  IODevice *outputDevice;

  //! First item of the input queue.
  InputItem *inputFirst;

  //! Last item of the input queue.
  InputItem *inputLast;

  //! Rules that match the current input during eating the data.
  TestedRules *testedRules;


  /*!
    \brief Set of all grammar rules.

    The set is represented by a linked list with items of type Rule.
  */
  RuleSet *ruleSet;

  /*!
    \brief Pointer to the root rule.
    
    Used mainly for optimization and lucidity.
  */
  Rule *rootRule;

  /*!
    \brief Structure with information about dograms starting with terminals.

    The structure is implemented as a hash array that maps values of terminal symbols to lists of elements which follow given terminal.
  */
  TerminalDigrams *terminalDigrams;

  /*!
    \brief Structure with information about digrams starting with variables.

    The structure is implemented as a hash array that maps id numbers of variable symbols to lists of elements which follow given variable.
  */
  VariableDigrams *variableDigrams;


  /*!
    \brief Description of best rule matching the contents of the input queue.

    This item contains information about the best rule matching current contents of the input queue.

    \sa testedRules
    \sa append()
  */
  MatchingRule bestTestedRule;

  /*!
    \brief Indicator of extension of variable set.

    Whenever new variable is introduced, this indicator equals \a true; otherwise it equals \a false.
  */
  bool I;

  /*!
    \brief Counter for numbering rules.
    \warning Number of rules is limited by the range of \a unsigned \a long type.

    Used for generation of unique rule numbers during createRule(). When availableRuleNumbers queue is non-empty, the first available rule id from it is used, otherwise the ruleCounter is used.

    \sa availableRuleNumbers, createRule()
  */
  RuleId ruleCounter;


  /*!
    \brief Queue of available rule identificators.

    After applying RR1, the identifier of deleted rule can be reused again. This identifier is stored in the queue to be available when newRule() is called. 

    \sa ruleCounter, createRule()
   */
  Queue<RuleId> *availableRuleNumbers;


//    Stack<Rule> ruleStack;

  /*!
    \brief Number of the "transformation runs".

    New input run occurs when some symbol was appended to the right side of the root rule.
   */
  unsigned long runCount;


  //! The size of the input queue.
  unsigned long inputQueueLength;

  //! The maximum size of the input queue reached.
  unsigned long maxInputQueueLength;


  //! Total time needed for compression/decompression.
  size_t totalTime;

  //! Total symbols processed during compression.
  size_t totalSymbols;

  //! Total number of variables encoded during compression.
  size_t variablesEncoded;

  //! Total number of terminals encoded during compression.
  size_t terminalsEncoded;

  //! The periodicity of calls to eatData().
  size_t eatDataPeriodicity;

  //! Create root rule in empty grammar.
  virtual Rule *createRootRule(void);


  //! Create new rule with give right side.
  virtual Rule *createRule(RuleElement *, RuleElement *);


  //! Test if the rule matches the current input.
  virtual bool ruleMatchesInput(TestedRule *testedRule, bool flush = false);

  //! Process all the data in input queue
  virtual void eatData(bool = false);

  //! Reconstruct data represented by one rule.
  virtual void reconstructRule(Rule *rule, IODevice *outputDevice);

  //! Get the length of the data string represented by the rule.
  virtual size_t getRuleLength(Rule *rule);

  //! Apply Reduction %Rule 1 (as given by Kieffer and Yang)
  virtual void reductionRule1(Rule *);

  //! Apply Reduction %Rule 2 (as given by Kieffer and Yang)
  virtual Rule *reductionRule2(RuleElement *, RuleElement *);

  //! Apply Reduction %Rule 3 (as given by Kieffer and Yang)
  virtual Rule *reductionRule3(Rule *, RuleElement *, RuleElement *);


  //! Perform some basic initialization steps.
  virtual void initKYGrammar(void);


  //! The stack of rule elements used when flushing the grammar.
  RuleElement **flushStack;

  //! The position in the flush stack.
  int flushStackPos;

  //! The stack of rules used when eating the data.
  Rule **ruleStack;

  //! The position in the rule stack.
  int ruleStackPos;
};



#endif //KYGRAMMAR_H

