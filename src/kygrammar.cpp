/***************************************************************************
    kygrammar.cpp  -  Definitions of class KYGrammar methods.
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
  \file kygrammar.cpp
  \brief Definitions of KYGrammar class methods
  
  This file contains the definitions of KYGrammar class
*/


#ifdef __GNUG__
# pragma implementation
#endif


#include "kygrammar.h"



/*!
  \brief Decrease the length of the input queue by given amount.

  \param _amount_ Given amount.
 */
#define DECREASE_INPUT_QUEUE_LENGTH(_amount_)		inputQueueLength -= _amount_


/*!
  \brief Increase the length of the input queue by given amount.

  If necessary, maxInputQueueLength is adjusted.

  \param _amount_ Given amount.
 */
#define INCREASE_INPUT_QUEUE_LENGTH(_amount_)	\
{						\
  inputQueueLength += _amount_;			\
  if (inputQueueLength > maxInputQueueLength)	\
    maxInputQueueLength = inputQueueLength;	\
}


/*!
  \brief Pop one tested rule from the stack.

  Value of the top item on the \a _stack_ is stored in \a _val_.

  \param _val_ Value of the top item on the stack.
  \param _stack_ The stack with tested rules.
  \param _tmpStack_ Temporary variable for deleting the top item from the stack.
 */
#define TESTEDRULES_POP(_val_, _stack_, _tmpStack_)	\
{							\
  _val_ = _stack_->rel;					\
  _tmpStack_ = _stack_;					\
  _stack_ = _stack_->next;				\
  DELETE(_tmpStack_);					\
}
  

/*!
  \brief Push one tested rule onto the stack.

  New tested rule with value \a _val_ is pushed onto the \a _stack_

  \param _val_ Value of the new item.
  \param _stack_ The stack with tested rules.
  \param _tmpStack_ Temporary variable for creating the top item on the stack.
 */
#define TESTEDRULES_PUSH(_val_, _stack_, _tmpStack_)	\
{							\
  NEW(_tmpStack_, TestedElement);			\
  _tmpStack_->rel = _val_;				\
  _tmpStack_->next = _stack_;				\
  _stack_ = _tmpStack_;					\
}
  

/*!
  \brief Remove specified digram.

  Remove digram (\a el1, \a el2) from list of digrams for symbol \a el1.
*/
#define DIGRAM_REMOVE(_el1_, _el2_)						\
{										\
  if (_el1_->type == Terminal)							\
    {nd = terminalDigrams->find(_el1_->value);}					\
  else										\
    {nd = variableDigrams->find(_el1_->rule->id);}				\
										\
  if (nd)									\
    for (r = nd->neighbours.first(); r; r = nd->neighbours.next())		\
      if (r->origin == _el2_)							\
	{									\
	  nd->neighbours.remove();						\
          DELETE(r);								\
	  break;								\
	}									\
}


/*!
  \brief Add specified digram.

  Add digram (\a el1, \a el2) to list of digrams for symbol \a el1 (\a el2 is located in rule \a rulePtr).
*/
#define DIGRAM_ADD(_el1_, _el2_, _rulePtr_)					\
{										\
  bool _needInsert_ = true;							\
  if (_el1_->type == Terminal)							\
    {										\
      if (!(nd = terminalDigrams->find(_el1_->value)))				\
	{									\
          NEW(nd, NeighboursDescription);					\
          terminalDigrams->insert(_el1_->value, nd);				\
        }									\
    }										\
  else										\
    {										\
      if (!(nd = variableDigrams->find(_el1_->rule->id)))			\
	{									\
          NEW(nd, NeighboursDescription);					\
          variableDigrams->insert(_el1_->rule->id, nd);				\
        }									\
    }										\
										\
  for (r = nd->neighbours.first(); r; r = nd->neighbours.next())		\
    if (r->origin->type == _el2_->type && r->origin->rule == _el2_->rule)	\
      {										\
	_needInsert_ = false;							\
	break;									\
      }										\
										\
 if (_needInsert_)								\
   {										\
     NEW(r, RuleElementNeighbour);						\
     r->origin = _el2_;								\
     r->rule = _rulePtr_;							\
     nd->neighbours.append(r);							\
   }										\
}



/*!
  \brief Change of the rule info in specified digram

  Change rule info for digram (\a el1, \a el2). Used when this digram goes to another rule (with pointer \a rulePtr).
*/
#define DIGRAM_CHANGE(_el1_, _el2_, _rulePtr_)					\
{										\
  if (_el1_->type == Terminal)							\
    nd = terminalDigrams->find(_el1_->value);					\
  else										\
    nd = variableDigrams->find(_el1_->rule->id);				\
										\
  if (nd)									\
    for (r = nd->neighbours.first(); r; r = nd->neighbours.next())		\
      if (r->origin == _el2_)							\
	{									\
	  r->rule = _rulePtr_;							\
	  break;								\
	}									\
}


/*!
  \brief Append symbol to specified rule.

  Symbol \a el is appended to the end of \a rule.
*/
#define RIGHT_SIDE_APPEND(_rule_, _el_)	\
if (!_rule_->body)			\
{					\
  _rule_->body = _el_;			\
  _rule_->last = _el_;			\
  _el_->next = 0;			\
  _el_->prev = 0;			\
}					\
else					\
{					\
  _rule_->last->next = _el_;		\
  _el_->prev = _rule_->last;		\
  _el_->next = 0;			\
  _rule_->last = _el_;			\
}


/*!
  \brief Test whether given rule is empty.

  This macro returns \a true when \a rule is empty, otherwise it returns \a false.
*/
#define RIGHT_SIDE_IS_EMPTY(_rule_)	(_rule_->last == 0)


/*!
  \brief Append new rule to the list of representatives of the rule element.

  The _rule_ is appended only if is not contained in the list.

  \param _list_ Linked list of representatives of given rule element.
  \param _rule_ RUle that represents the rule element.
 */
#define REPRESENTED_BY_APPEND(_list_, _rule_)			\
{								\
  bool _needInsert_ = true;					\
								\
  for (Rule *_rr_ = _list_.first(); _rr_; _rr_ = _list_.next())	\
    {								\
      if (_rr_ == _rule_)					\
        {							\
	  _needInsert_ = false;					\
	  break;						\
        }							\
    }								\
  if (_needInsert_)						\
    _list_.append(_rule_);					\
}



/*!
  Method initKYGrammar() with parameter TERMINAL_ALPHABET_DEFAULT_SIZE (default size of terminal alphabet) is called.
*/
KYGrammar::KYGrammar(void)
  : GrammarBase()
{
  initKYGrammar();
}



/*!
  Some necessary initialization actions are performed.
*/
void KYGrammar::initKYGrammar(void)
{
  size = 0;
  newSymbolToInstall = 0;
  inputQueueLength = maxInputQueueLength = 0;

  totalTime = totalSymbols = 0;
  variablesEncoded = 0;
  terminalsEncoded = 0;

  eatDataPeriodicity = KY_GRAMMAR_EATDATA_PERIODICITY;

  NEW(flushStack, RuleElement*[KY_GRAMMAR_FLUSH_STACK_SIZE]);
  flushStackPos = -1;

  NEW(ruleStack, Rule*[KY_GRAMMAR_RULE_STACK_SIZE]);
  ruleStackPos = -1;

  outputDevice = 0;

  context = 0;
  useContextForOutput = false;

  NEW(ruleSet, RuleSet);

  ruleCounter = 0;

  //no input
  inputFirst = inputLast = 0;
  NEW(testedRules, TestedRules);

  NEW(availableRuleNumbers, Queue<RuleId>);

  availableRuleNumbers->setAutoDelete(true);

  runCount = 1;

  rootRule = 0;
  createRootRule();

  NEW(terminalDigrams, TerminalDigrams);
  terminalDigrams->setAutoDelete(true);
 
  NEW(variableDigrams, VariableDigrams);
  variableDigrams->setAutoDelete(true);

  //initially, I = false (set of variables is unchanged)
  I = false;
}



/*!
  Contents of the grammar are freed.
*/
KYGrammar::~KYGrammar(void)
{
  //force processing of the unprocessed data
  flush();

  DELETE_ARRAY(flushStack);
  DELETE_ARRAY(ruleStack);

  if (ExaltOptions::getOption(ExaltOptions::PrintGrammar) == ExaltOptions::Yes)
    print();

  if (ExaltOptions::getOption(ExaltOptions::Verbose) == ExaltOptions::Yes)
    {
      OUTPUTENL("KY grammar statistics");
      OUTPUTE("  Total symbols processed: \t\t");
      if (useContextForOutput)
	{
	  //compression
	  OUTPUTENL(totalSymbols);
	}
      else
	{
	  //decompression
	  OUTPUTENL(runCount-1);
	}

      OUTPUTENL("  Symbols appended to the root rule: \t" << runCount-1);
      OUTPUTENL("  Number of rules: \t\t\t" << ruleSet->count());

      if (useContextForOutput)
	{
	  //compression
	  OUTPUTENL("  Variables encoded: \t\t\t" << variablesEncoded);
	  OUTPUTENL("  Terminals encoded: \t\t\t" << terminalsEncoded);
	  OUTPUTENL("  Maximum length of the input queue: \t" << maxInputQueueLength);
	  OUTPUTENL("  Average time per symbol: \t\t" << (double)totalTime/totalSymbols << " microsecs");
	}

      OUTPUTEENDLINE;
    }


  DELETE(ruleSet);

  DELETE(testedRules);
  DELETE(availableRuleNumbers);

  DELETE(terminalDigrams);
  DELETE(variableDigrams);

  deleteDefaultTextCodec();
}



/*!
  Empties the grammar, deletes all the rules and creates a new (emtpy) grammar.
 */
void KYGrammar::purge(void)
{
  DELETE(flushStack);
  DELETE(ruleStack);

  DELETE(ruleSet);

  DELETE(testedRules);
  DELETE(availableRuleNumbers);

  DELETE(terminalDigrams);
  DELETE(variableDigrams);


  NEW(flushStack, RuleElement*[KY_GRAMMAR_FLUSH_STACK_SIZE]);
  flushStackPos = -1;

  NEW(ruleStack, Rule*[KY_GRAMMAR_RULE_STACK_SIZE]);
  ruleStackPos = -1;

  NEW(ruleSet, RuleSet);

  NEW(testedRules, TestedRules);

  NEW(availableRuleNumbers, Queue<RuleId>);

  availableRuleNumbers->setAutoDelete(true);

  runCount = 1;
  ruleCounter = 0;

  rootRule = 0;
  createRootRule();

  NEW(terminalDigrams, TerminalDigrams);
  terminalDigrams->setAutoDelete(true);
 
  NEW(variableDigrams, VariableDigrams);
  variableDigrams->setAutoDelete(true);

  //initially, I = false (set of variables is unchanged)
  I = false;


  size = 0;
}



/*!
  This method sets the base size of the alphabet of the arithmetic context.

  \param size The size of the alphabet.
 */
void KYGrammar::setAlphabetBaseSize(size_t size)
{
  if (context)
    {
      if (!context->isInitialized())
	{
	  //initialize the context only if it is not initialized yet
	  context->setType(size, DynamicContext);
	  context->initialize();
	  
	  //store the size of the alphabet in output device
	  char fibBuf[10];
	  size_t nrItems = Fibonacci::encodeToBuffer(fibBuf, SIZEOF_CHAR, size);
	  outputDevice->writeData(fibBuf, nrItems);
	}
    }

  //output device is needed only for storing the alphabet size
  outputDevice = 0;
}



/*!
  The device used for the output of data is set.

  \param device The output device.
 */
void KYGrammar::setOutputDevice(IODevice *device)
{
  outputDevice = device;
}


/*!
  This method creates new root rule.
  \warning If the grammar has a root rule already, it will be overwritten by the new one. This can cause fatal consequences.
  \return Pointer to newly created root rule.
*/
Rule *KYGrammar::createRootRule(void)
{
  if (rootRule)
    {
      WRN("Root roole already exists --> cannot create root rule.");
      return 0;
    }
  else
    {
      NEW(rootRule, Rule);

      rootRule->counter = 1;
      
      if (!rootRule)
	return 0;
      
      rootRule->refCount = 1;
      rootRule->id = ruleCounter;
      rootRule->body = 0;
      rootRule->last = 0;

      ruleCounter++;

      ruleSet->append(rootRule);

      return rootRule;
    }
}



/*!
  This method creates new rule with unique id. The right side of the rule consists of symbols \a start and \a last.

  \param start First symbol of the right side of the rule.
  \param last Last symbol of the right side of the rule.

  \return Pointer to newly created rule.
*/
Rule *KYGrammar::createRule(RuleElement *start, RuleElement *last)
{
  Rule *rule;

  NEW(rule, Rule);


  rule->counter = 1;

  rule->matchRun = 0;
  rule->matchResult = Ignore;

  rule->body = start;
  rule->last = last;

  rule->refCount = 0;


  if (availableRuleNumbers->isEmpty())
    {
      //if no of the old ids can be reused, use ruleCounter
      rule->id = ruleCounter;

      //context->installSymbol((long)(context->lastFixedSymbol() + ruleCounter));
      newSymbolToInstall = context->lastFixedSymbol() + ruleCounter;

      DBG("Installing symbol: " << (long)(context->lastFixedSymbol() + ruleCounter) << "(" << context->lastFixedSymbol() << "," << ruleCounter << ")");
      ruleCounter++;
    }
  else
    {
      //otherwise use some old id
      RuleId *rid;

      rid = availableRuleNumbers->dequeue();

      rule->id = *rid;

      DELETE(rid);
    }

  ruleSet->append(rule);

  return rule;
}



/*!
  This method finds the rule with given rule.

  \param id The id of the rule.

  \return Pointer to the rule, or NULL.
 */
Rule *KYGrammar::findRule(RuleId id)
{
  for (Rule *r = ruleSet->first(); r; r = ruleSet->next())
    {
      if (r->id == id)
	return r;
    }

  return 0;
}



/*!
  Tests if given rule matches the current input string. If \a flush is FALSE, also the rules that represents more than the current input are considered.

  \param testedRule Pointer to the rule that is being tested.
  \param flush Indication if we are in the flush-mode.

  \retval TRUE The rule matches.
  \retval FALSE The rule doesn't match.
 */
bool KYGrammar::ruleMatchesInput(TestedRule *testedRule, bool flush = false)
{
  RuleElement *rel;
  unsigned int length;
  InputItem *i, *ii;
  RuleElement *pom;

  if (testedRule->rule->matchResult == DoesntMatch && testedRule->rule->matchRun == runCount)
    return false;

  //  if (!flush)
    {
      if (testedRule->rule->matchResult == Ignore && testedRule->rule->matchRun == runCount)
	{
//  	  if (flush)
//  	    cerr << "FLUSH: ";
	  testedRule->rule->matchResult = Matches;
	  testedRule->matchesCompletely = true;
	  //cerr << "rule " << testedRule->rule->id << " matches!" << endl;
	  return false;
	}
    }

  testedRule->rule->matchRun = runCount;


  if (!flush)
    {
      TestedElement *stack = testedRule->currentElements;
      TestedElement *s;
      //not in the "flush" mode
      
      if (!stack)
	{
	  //the stack of elements is empty --> tested rule should be initialized
	  rel = testedRule->rule->body;
	  CHECK_POINTER(rel);
	  length = 0;
	  i = inputFirst;
	}
      else
	{ 
	  TESTEDRULES_POP(rel, stack, s);
	  length = testedRule->matchedLength;
	  i = testedRule->lastInputItem->next;
	}





      //for sure
      ii = i;


      while (i)
	{
	  if (rel->type == Terminal)
	    {
	      //terminal --> simply compare value with current input item
	  
	      if (rel->value != i->value)
		{
		  //the rule doesn't match...
		  testedRule->rule->matchResult = DoesntMatch;
		  testedRule->rule->matchRun = runCount;

		    {
		      while (stack)
			{
			  TESTEDRULES_POP(rel, stack, s);
			  
			  //first element!
  			  if (stack && !stack->rel)
  			    {
  			      TESTEDRULES_POP(pom, stack, s);
  			      rel->rule->matchRun = runCount;
  			      rel->rule->matchResult = DoesntMatch;
  			    }
			}
		    }
		  testedRule->currentElements = 0;
		  return false;
		}
	      else
		{
		  //rule element matches --> increase the length and move to the next rule element
		  length++;
		  rel = rel->next;
	      
		  ii = i;
		  i = i->next;

		  if (!rel)
		    {
		      //we have examined the complete rule --> check the stack if the "parent" rule element is present


		      while (stack && !rel)
			{
			  //jump up if rel is the last element of the rule
			  TESTEDRULES_POP(rel, stack, s);

			  //first element!
			  if (stack && !stack->rel)
			    {
			      TESTEDRULES_POP(pom, stack, s);

			      //indicate that the subrule rule matches
			      //it matches the prefix of the data
			      //rel->rule->matchesCompletely = true;
			      if (rel)
				{
				  rel->rule->matchResult = Ignore;	//workaround for matches completely :(
				  rel->rule->matchRun = runCount;
				  
				  if (length > bestTestedRule.length)
				    {
				      bestTestedRule.rule = rel->rule;
				      bestTestedRule.length = length;
				    }
				}


			    }

			  rel = rel->next;
			}

		      if (!stack && !rel)
			{
			  //the stack is empty --> we have examined whole rule tree
			  //the rule matches and matches completely
			  testedRule->currentElements = 0;
			  testedRule->matchedLength = length;
			  testedRule->matchesCompletely = true;
			  testedRule->rule->matchResult = Matches;
			  return true;
			}
		    }
		}
	    }
	  else
	    {

	      if (!length)
		{
		  if (rel->rule->matchRun == runCount && rel->rule->matchResult == DoesntMatch)
		    {
		      //      cerr << "pushed nonmatching rule!" << endl;
		      testedRule->rule->matchRun = runCount;
		      testedRule->rule->matchResult = DoesntMatch;

		      while (stack)
			{
			  TESTEDRULES_POP(rel, stack, s);
			  if (stack && !stack->rel)
			    {
			      TESTEDRULES_POP(pom, stack, s);
			    }
			  rel->rule->matchRun = runCount;
			  rel->rule->matchResult = DoesntMatch;
			}

		      //flushStackPos = -1;
		      return false;
		    }
		  else
		    {

		      //  		  rel->rule->matchRun = runCount;
		      //  		  rel->rule->matchResult = DoesntMatch;
		      //		beginning = 1;
		      //  		  cerr << "zero length!" << endl;
		      
		      //to indicate the first rule
		      TESTEDRULES_PUSH(0, stack, s);
		    }
		}

	      //variable --> push current rel to the stack and jump to the referenced rule
	      TESTEDRULES_PUSH(rel, stack, s);
	      //cerr << "pushing rule" << rel->rule->id << "	length = " << length << endl;

	      rel = rel->rule->body;
	    }
	}

      //the rule represents more than the input
      //push the last visited rule element
      TESTEDRULES_PUSH(rel, stack, s);
      testedRule->currentElements = stack;
      testedRule->matchesCompletely = false;
      testedRule->matchedLength = length;
      testedRule->lastInputItem = ii;
      testedRule->rule->matchResult = Matches;
  
      return true;
    }
  else
    {
      //      cerr << "testing rule" << testedRule->rule->id << endl;

      flushStackPos = -1;
      //the "flush" mode
      rel = testedRule->rule->body;
      length = 0;
      i = inputFirst;

      //for sure
      ii = i;

      while (i)
	{
	  if (rel->type == Terminal)
	    {
	      //terminal --> simply compare value with current input item
	  
	      if (rel->value != i->value)
		{
		  testedRule->rule->matchRun = runCount;
		  testedRule->rule->matchResult = DoesntMatch;

		  while (flushStackPos != -1)
		    {
		      rel = flushStack[flushStackPos];
		      flushStackPos--;
			
//  		      if (length == 1)
//  			{
//  			  rel->rule->matchRun = runCount;
//  			  rel->rule->matchResult = DoesntMatch;
//  			}
		    }
		  //		  cerr << "  DOES NOT MATCH" << endl;

		  return false;
		}
	      else
		{
		  //rule element matches --> increase the length and move to the next rule element
		  length++;
		  rel = rel->next;
	      
		  ii = i;
		  i = i->next;

		  if (!rel)
		    {
		      //we have examined the complete rule --> check the stack if the "parent" rule element is present

		      while (flushStackPos != -1 && !rel)
			{
			  //jump up if rel is the last element of the rule
			  rel = flushStack[flushStackPos]->next;


			  flushStackPos--;
			}

		      if (flushStackPos == -1 && !rel)
			{
			  //the stack is empty --> we have examined whole rule tree
			  //the rule matches and matches completely
			  testedRule->matchedLength = length;
			  testedRule->matchesCompletely = true;
			  testedRule->rule->matchRun = runCount;
			  testedRule->rule->matchResult = Matches;
			  //  cerr << "  MATCHES" << endl;
			  return true;
			}
		    }
		}
	    }
	  else
	    {
	      //variable --> push current rel to the stack and jump to the referenced rule
	      flushStackPos++;
	      if (flushStackPos == KY_GRAMMAR_FLUSH_STACK_SIZE)
		{
		  FATAL("Size of KY flush stack exceeded! Please redefine the KY_GRAMMAR_FLUSH_STACK_SIZE constant in kygrammar.h");
		}
	      flushStack[flushStackPos] = rel;

	      //cerr << "  pushing rule" << rel->rule->id << "	length = " << length << endl;

	      if (!length)
		{
		  //		  cerr << "  zero length!" << endl;
		  if (rel->rule->matchRun == runCount && rel->rule->matchResult == DoesntMatch)
		    {
		      //      cerr << "pushed nonmatching rule!" << endl;
		      testedRule->rule->matchRun = runCount;
		      testedRule->rule->matchResult = DoesntMatch;

		      while (flushStackPos != -1)
			{
			  rel = flushStack[flushStackPos];

			  rel->rule->matchRun = runCount;
			  rel->rule->matchResult = DoesntMatch;
			  flushStackPos--;
			}

		      //flushStackPos = -1;
		      return false;
		    }
		}


	      rel = rel->rule->body;
	    }
	}

      testedRule->rule->matchRun = runCount;
      testedRule->rule->matchResult = DoesntMatch;

      //      cerr << "  DOES NOT MATCH" << endl;
  
      return false;
    }
}



void KYGrammar::eatData(bool forced)
{
  TestedRule testedRule;

  if (forced)
    {
      //time for preparation of rule testing
      bestTestedRule.rule = 0;
      bestTestedRule.length = 1;

      //filter rules in ruleSet against the input
      for (Rule *r = ruleSet->first(); r; r = ruleSet->next())
	{
	  //ignore the root rule
	  if (r == rootRule)
	    continue;

	  if (r->matchRun == runCount && r->matchResult == DoesntMatch)
	    continue;

	  if (r->body->type == Terminal && r->body->value != inputFirst->value)
	    {
	      r->matchResult = DoesntMatch;
	      r->matchRun = runCount;
	      continue;
	    }

	  //DBG("! Testing rule " << r->id);
	  testedRule.rule = r;
	  testedRule.currentElements = 0;
	  //	  if ()
	  if (ruleMatchesInput(&testedRule, true))
	    {
	      //DBG("! Rule " << r->id << " matches input");
	      if (testedRule.matchesCompletely &&
		  testedRule.matchedLength > bestTestedRule.length)
		{
		  //DBG("! Rule " << r->id << " is current best ! (" << testedRule.matchedLength << ")");
		  bestTestedRule.rule = r;
		  bestTestedRule.length = testedRule.matchedLength;	//because position in queue starts with 0
		  //no need to add rule to testedRules; it is already stored in bestTestedRule
		}
	    }
	}
      
      RuleElement *rel;
      NEW(rel, RuleElement);

      if (bestTestedRule.rule)
	//use matched rule
	{
	  rel->type = Variable;
	  rel->value = 0;
	  rel->rule = 0;	//for safe
	  rel->rule = bestTestedRule.rule;
	  //increase rule reference count
	  rel->rule->refCount++;
	  
	  //delete first N input symbols represented by the rule
	  InputItem *ii;
	  for (size_t i = 0 ; i < bestTestedRule.length; i++)
	    {
	      ii = inputFirst->next;
	      DELETE(inputFirst);
	      inputFirst = ii;
	    }
	  DECREASE_INPUT_QUEUE_LENGTH(bestTestedRule.length);
	  
	  if (!inputFirst)
	    //complete input queue deleted
	    inputLast = 0;
	}
      else
	//use head of input queue instead
	{
	  rel->type = Terminal;
	  rel->value = 0;
	  rel->rule = 0;	//for safe
	  rel->value = inputFirst->value;
	  
	  InputItem *ii = inputFirst->next;
	  DELETE(inputFirst);
	  inputFirst = ii;
	  DECREASE_INPUT_QUEUE_LENGTH(1);
	  if (!inputFirst)
	    //complete input queue deleted
	    inputLast = 0;
	}
      //append new symbol to root rule
      appendToRootRule(rel);
    }
  else
    {
      bool canEnd = false;
      NeighboursDescription *nd;
      RuleElementNeighbour *ren;
      Rule *r, *rr;

      while (!canEnd)
	{
	  if (!inputFirst)
	    return;
	  
	  if (testedRules->isEmpty())
	    {
	      //no rule candidate from previous runs
	      //let's start new tests
	      //time for preparation of rule testing
	      bestTestedRule.rule = 0;
	      bestTestedRule.length = 1;
	      
	      
	      
	      //find all of the elements following current input value
	      nd = terminalDigrams->find(inputFirst->value);

	      if (nd)
		{

		  ruleStackPos = -1;

		  for (ren = nd->neighbours.first(); ren; ren = nd->neighbours.next())
		    {
		      //testing direct match

		      //we are interested only in the elements that are on the second position in the rule
		      //i.e. the current input value is the first element in the rule
		      if (ren->origin == ren->rule->body->next)
			{
			  if (ren->rule->matchRun != runCount)
			    {
			      ruleStackPos++;
			      if (ruleStackPos == KY_GRAMMAR_RULE_STACK_SIZE)
				{
				  FATAL("Size of the rule stack exceeded!");
				}
			      ruleStack[ruleStackPos] = ren->rule;
			      for (rr = nd->representedBy.first(); rr; rr = nd->representedBy.next())
				{
				  if (rr->matchRun != runCount)
				    {
				      //rule candidate that hasn't been tested in this run
				      ruleStackPos++;
				      if (ruleStackPos == KY_GRAMMAR_RULE_STACK_SIZE)
					{
					  FATAL("Size of the rule stack exceeded!");
					}

				      ruleStack[ruleStackPos] = rr;
				      //  DBG("EATDATA (mode 0) pushed rule: " << rr->id);
				    }
				}
			    }
			}
		      else
			continue;

		      while (ruleStackPos > -1)
			{
			  r = ruleStack[ruleStackPos];
			  ruleStackPos--;
		      
			  //ignore the root rule
			  if (r == rootRule)
			    continue;
			  
			  //ignore rules already processed in the current run
			  if (r->matchRun == runCount)
			    {
			      //  			      DBG("Omitting rule: " << r->id);
			      continue;
			    }
			  
			  //DBG("! Testing rule " << r->id);
			  testedRule.rule = r;
			  testedRule.currentElements = 0;
			  if (ruleMatchesInput(&testedRule))
			    {
			      //DBG("! Rule " << r->id << " matches input");
			      if (testedRule.matchesCompletely)
				{
				  //DBG("! Complete match !");
				  
				  if (testedRule.matchedLength > bestTestedRule.length)
				    {
				      //DBG("! Rule " << r->id << " is current best ! (" << testedRule.matchedLength << ")");
				      bestTestedRule.rule = r;
				      bestTestedRule.length = testedRule.matchedLength;	//because position in queue starts with 0
				      //no need to add rule to testedRules; it is already stored in bestTestedRule
				    }
				}
			      else
				{
				  //rule represents more than input, so store it for future
				  //DBG("! Rule represents more than input");
				  //if not force, we are interested also in the rules that represent more than current input
				  TestedRule *tr;
				  
				  NEW(tr, TestedRule);
				  tr->rule = testedRule.rule;
				  tr->matchedLength = testedRule.matchedLength;
				  tr->matchesCompletely = testedRule.matchesCompletely;
				  tr->lastInputItem = testedRule.lastInputItem;
				  tr->currentElements = testedRule.currentElements;
				  
				  testedRules->append(tr);
				}
			    }
			}
		    }
		}
	    }
	  else
	    {
	      TestedRule *tr = testedRules->first();
	      while (tr)
		{
		  //DBG("!! Testing rule " << tr->rule->id);
		  if (ruleMatchesInput(tr))
		    {
		      //DBG("!! Rule " << tr->rule->id << " matches input, length: " << tr->matchedLength);
		      if (tr->matchesCompletely)
			{
			  //DBG("!! Complete match (" << tr->matchedLength << ")!");

			  if (tr->matchedLength > bestTestedRule.length && tr->matchedLength > 1)
			    {
			      //DBG("!! Rule " << tr->rule->id << " is current best ! (" << tr->matchedLength << ")");
			      bestTestedRule.rule = tr->rule;
			      bestTestedRule.length = tr->matchedLength;	//because position in queue starts with 0
			      testedRules->remove();
			      tr = testedRules->current();
			      continue;
			    }
			  else
			    {
			      //DBG("!! Rule " << tr->rule->id << " matches input, but not best --> removing");
			      testedRules->remove();
			      tr = testedRules->current();
			      continue;
			    }
			}
		      else
			{
			  canEnd = true;
			  //rule represents more than input, so store it for future
			  //DBG("!! Rule represents more than input");

			}
		    }
		  else
		    {
		      //DBG("!! Rule " << tr->rule->id << " doesn't match input --> removing from testedRules");
		      testedRules->remove();
		      tr = testedRules->current();
		      continue;
		    }

		  tr = testedRules->next();

		}

	    }

	  //if no rules remain in testedRules, check bestMatchedRule
	  //if any rule matched, use it, otherwise add first item in the input queue to the right side of the root rule
	  if (testedRules->isEmpty())
	    {
	      RuleElement *rel;
	      NEW(rel, RuleElement);

	      if (bestTestedRule.rule)
		//use matched rule
		{
		  rel->type = Variable;
		  rel->value = 0;
		  rel->rule = 0;	//for safe
		  rel->rule = bestTestedRule.rule;
		  //increase rule reference count
		  rel->rule->refCount++;

		  //delete first N input symbols represented by the rule
		  InputItem *ii;
		  for (size_t i = 0 ; i < bestTestedRule.length; i++)
		    {
		      ii = inputFirst->next;
		      DELETE(inputFirst);
		      inputFirst = ii;
		    }
		  DECREASE_INPUT_QUEUE_LENGTH(bestTestedRule.length);


		  if (!inputFirst)
		    //complete input queue deleted
		    inputLast = 0;

		}
	      else
		//use head of input queue, because no rule matches
		{
		  rel->type = Terminal;
		  rel->value = 0;
		  rel->rule = 0;	//for safe
		  rel->value = inputFirst->value;

		  InputItem *ii = inputFirst->next;
		  DELETE(inputFirst);
		  inputFirst = ii;
		  DECREASE_INPUT_QUEUE_LENGTH(1);

		  if (!inputFirst)
		    //complete input queue deleted
		    inputLast = 0;
		}

	      //append new symbol to root rule
	      appendToRootRule(rel);
	    }
	  else
	    //we have some rules that represnt more than the input
	    canEnd = true;
	}
    }
}


////////////////////////////////////////////////////////////




/*!
  New terminal symbol with value \a value is inserted into the input queue.

  \param value Value of inserted terminal symbol.

  \sa eatData()
*/
void KYGrammar::append(TerminalValue value)
{
//    cerr << (unsigned char)value;

  InputItem *i;

  NEW(i, InputItem);

  i->next = 0;
  i->value = value;

  if (!inputFirst)
    inputFirst = i;
  else
    inputLast->next = i;

  inputLast = i;

  INCREASE_INPUT_QUEUE_LENGTH(1);
  eatDataPeriodicity--;

  totalSymbols++;
  

  if (!eatDataPeriodicity)
    {
      struct timeval t1, t2, t3;
      timerclear(&t1);
      timerclear(&t2);
      gettimeofday(&t1, 0);
      
      eatData();
      
      gettimeofday(&t2, 0);
      timersub(&t2, &t1, &t3);
      totalTime+=t3.tv_usec;
      eatDataPeriodicity = KY_GRAMMAR_EATDATA_PERIODICITY;
    }
}



/*!
  After call to this method, all symbols in the input queue are processed.

  \sa eatData()
*/
void KYGrammar::flush(void)
{
  DBG("Flushing the grammar...");
  while (inputFirst)
    {
      eatData(true);
    }
}



/*!
  This method appends one element (terminal or variable) to the root rule. Depending on the previous state of the grammar, various steps are performed. If the appending of the symbol causes some digram to appear twice in the range of the grammar, Reduction %Rule 2 (digram appears twice within one rule) or Reduction %Rule 3 (digram appears twice in two distinct rules) is performed, in some cases followed by Reduction %Rule 1 (some rule appears only once).

  \param newElement Pointer to appended element.

  \sa reductionRule1()
  \sa reductionRule2()
  \sa reductionRule3()
*/
void KYGrammar::appendToRootRule(RuleElement *newElement)
{
  RuleElement *last;
  NeighboursDescription *nd;
  RuleElementNeighbour *r;
  Rule *newRule;
  bool collision;


  if (!textCodec)
    //use default TextCodec if none has been specified
    createDefaultTextCodec();
      
  if (context)
    {
      if (!context->isInitialized())
	{
	  //context is not initialized yet
	  //some character data occurred before the XML declaration
	  //initialize the context with some default base size (for example UTF_8) ...
	  setAlphabetBaseSize(textCodec->suggestAlphabetBaseSize(Encodings::UTF_8));
	}
    }

  //reconstruct appended terminal/variable to the output device
  //only for decoding
  if (outputDevice)
    {
      if (newElement->type == Terminal)
	{
	  outputDevice->writeData((const char *)&newElement->value, SIZEOF_XML_CHAR);
	}
      else
	reconstructRule(newElement->rule, outputDevice);
    }


  runCount++;

  //increase the size of the grammar by 1
  size++;

  RuleElement appendedElement;
  appendedElement = *newElement;


  last = rootRule->last;
  RIGHT_SIDE_APPEND(rootRule, newElement);

  if (last)
    //inserted symbol isn't the first symbol of the rule
    {
      if (last->type == Terminal)
	nd = terminalDigrams->find(last->value);
      else
	nd = variableDigrams->find(last->rule->id);

      if (nd)
	//symbol is in digrams
	{
	  //    	  DBG("Adding to digrams");

	  collision = false;

	  //check whether there is a record about inserted symbol
	  for (RuleElementNeighbour *n = nd->neighbours.first(); n; n = nd->neighbours.next())
	    {
	      if ((n->origin->type == Terminal && newElement->type == Terminal && n->origin->value == newElement->value)
		  || (n->origin->type == Variable && newElement->type == Variable && n->origin->rule == newElement->rule))
		//this should be a safe test....
		{
		  if (n->origin == last)
		    {
		      //    		      DBG("Triple --> ignoring");
		      collision = true;

		      //set of variables remains unchanged
		      I = false;
		      break;
		    }
		  else
		    {
		      //    		      DBG("ACCEPTED DIGRAM: " << n << " - rule: " << n->rule << " origin: " << n->origin << " ");
		      //  		      DBG("origintype: " << n->origin->type << " originrule: " << n->origin->rule);
		      if (n->rule == rootRule)
			//found the same pair in root rule
			{
			  //    			  DBG("Collision (2x pair in root rule) --> RR2");

			  if (I == false)
			    //I equals 0 --> apply RR2 and set I to 1
			    //(one variable has been created)
			    {
			      reductionRule2(n->origin->prev, last);
			      I = true;
			    }
			  else
			    //I equals 1 --> apply RR2, then RR1 and set I to 0
			    //(one variable has been created and one has been deleted)
			    {
			      newRule = reductionRule2(n->origin->prev, last);
			      reductionRule1(newRule);
			    }
			}
		      else
			//found the same pair in distinct rules
			{
			  //  			  DBG(n->rule->body << " : " << n->origin->prev->rule << " ; " << (char)n->origin->value);
			  //  			  DBG(last->rule << " ; " << (char)last->next->value);

			  if (n->origin->prev->prev || n->origin->next)
			    {
			      //apply RR3 only if the original rule is longer than 2
			      //  			      DBG("Collision (2x pair in distinct rules) --> RR3");

			      if (I == false)
				//I equals 0 --> apply RR3 and set I to 1
				//(one variable has been created)
				{
				  reductionRule3(n->rule, n->origin->prev, last);
				  I = true;
				}
			      else
				//I equals 1 --> apply RR3, then RR1 and set I to 0
				//(one variable has been created and one has been deleted)
				{
				  newRule = reductionRule3(n->rule, n->origin->prev, last);
				  reductionRule1(newRule);
				}
			    }
			  else
			    I = false;
			}

		      //indicate that a digrams collision occurred and end the cycle
		      collision = true;
		      break;
		    }
		}
	    }

	  if (!collision)
	    //if no collision occurred, insert new (root) digram for last element of the root rule
	    {
	      //  	      DBG("no collision");
	      NEW(r, RuleElementNeighbour);
	      r->origin = newElement;

	      r->rule = rootRule;
	      nd->neighbours.append(r);

	      //set of variables remains unchanged
	      I = false;

	    }
	}
      else
	//symbol is not in digrams
	{
	  //  	  DBG("Inserting into digrams");
	  DIGRAM_ADD(last, newElement, rootRule);

	  //set of variables remains unchanged
	  I = false;
	}
    }


  //now comes time for encoding the newly created grammar
#ifdef SEQUENTIAL_ALGORITHM
  if (useContextForOutput)
    {
      if (context)
	{
	  if (appendedElement.type == Terminal)
	    {
	      //DBG(":" << (unsigned int)appendedElement.value);
	      //encoding of a terminal

 	      if (context->encode((int)(unsigned char)appendedElement.value) == ContextBase::NotKnown)
 		{
 		  FATAL("encoding unknown terminal: " << (unsigned char)appendedElement.value);
 		}

	      terminalsEncoded++;
	    }
	  else
	    {
	      //encoding of a variable
	      long symbolId = context->lastFixedSymbol() + appendedElement.rule->id;

	      //DBG("VARENC: " << symbolId <<endl;//- lastFixedContextSymbol);

   	      if (context->encode(symbolId) == ContextBase::NotKnown)
		{
		  FATAL("Encoding unknown symbol: " << symbolId);
		}

	      variablesEncoded++;
	    }
	}
    }

  //if new symbol has to be installed to the encoding context
  //install it now
  //it is IMPORTANT to install the symbol after the encoding operation
  //otherwise the decoder will be unable to decode "modified" arithmetic code!
  if (newSymbolToInstall)
    {
      context->installSymbol(newSymbolToInstall);
      newSymbolToInstall = 0;
    }

//    if (newSymbolToInstall < 1000)
//      {
//        context->installSymbol(newSymbolToInstall);
//        newSymbolToInstall = 0;
//      }


  if (size == KY_GRAMMAR_MAX_SIZE)
    {
      DBG("PURGING THE GRAMMAR");
      purge();
    }
#endif

}



/*!
  Right side of the rule that appears only once is inserted into the rule that references this rule.

  \param rule Pointer to the rule that starts with a variable that is used only once.
*/
void KYGrammar::reductionRule1(Rule *rule)
{
  RuleElement *rel;
  RuleElement *origRuleEl;
  Rule *origRule;
  NeighboursDescription *nd;
  RuleElementNeighbour *r;

  CHECK_POINTER(rule);



  //DBG("Applying reduction rule 1.");

  //DBG("Grammar before applying RR1:");

  //print();
  //printDigrams();

  //get the first element of rule
  rel = rule->body;

  if (rel->type == Terminal)
    {
      FATAL("First element is not a variable!");
    }

  //get the rule referenced by rel
  origRule = rel->rule;

  RuleId *rid;

  NEW(rid, RuleId(rel->rule->id));

  availableRuleNumbers->enqueue(rid);

  //change all digrams of origRule to rule
  // :( some optimalisations would be fine...
  origRuleEl = origRule->body;
  while (origRuleEl->next)
    {
      DIGRAM_CHANGE(origRuleEl, origRuleEl->next, rule);
      origRuleEl = origRuleEl->next;
    }


  //replace rel with origRule contents in rule
  origRuleEl->next = rel->next;

  if (rel->next)
    {
      rel->next->prev = origRuleEl;

      //add digram last element of the origRule to second element of the rule
      DIGRAM_ADD(origRuleEl, rel->next, rule);

      //remove digram about rel and rel->next
      DIGRAM_REMOVE(rel, rel->next);
    }

  rule->body = origRule->body;



  //remove the rule from the representedBy list of the first element
  //of the rule referenced by that variable

  NeighboursDescription *ndRep;
  RuleElement *relRep = rel->rule->body;

  if (relRep->type == Terminal)
    ndRep = terminalDigrams->find(relRep->value);
  else
    ndRep = variableDigrams->find(relRep->rule->id);
  
  CHECK_POINTER(ndRep);
  if (ndRep)
    {
      //        DBG("RR1 remove (a) from " << (unsigned int)rel->rule->body->value << ":: " << rule->id);
      if (!ndRep->representedBy.remove(rule))
	{
	  //  	  ERR("unable to remove!!!");
	}
    }
  
  
  
  if (origRule->body->type == Variable)
    {
      relRep = origRule->body;

      while (relRep->type != Terminal)
	relRep = relRep->rule->body;
      
      ndRep = terminalDigrams->find(relRep->value);
      
      CHECK_POINTER(ndRep);
      if (ndRep)
	{
	  //  	  DBG("RR1 remove from " << (unsigned int)origRule->body->value << ":: " << origRule->id);
	  while (ndRep->representedBy.remove(origRule));
	  
	  //  	  DBG("RR1 append to " << (unsigned int)relRep->value << ":: " << rule->id);
	  REPRESENTED_BY_APPEND(ndRep->representedBy, rule);
	  //	  ndRep->representedBy.append(rule);
	}
    }
  

  DELETE(rel);


  ruleSet->remove(origRule);
  DELETE(origRule);


  //the size of the grammar has to be decreased by one
  size--;

}



/*!
  Both occurences of the digram that appears twice within one rule are replaced by a new variable representing this digram.

  \param elOrig1 Pointer to the first symbol of the first digram.
  \param elNew1 Pointer to the second symbol of the second digram.

  \return Pointer to newly created rule.
*/
Rule *KYGrammar::reductionRule2(RuleElement *elOrig1, RuleElement *elNew1)
  //parametr je na puvodni prvni prvek a na novy prvni prvek (oba v rootRule)
{
  RuleElement *elOrig2;
  RuleElement *elNew2;
  RuleElement *var1, *var2;
  Rule *newRule;
  NeighboursDescription *nd;
  RuleElementNeighbour *r;

  //              DBG("Applying reduction rule 2.");

  CHECK_POINTER(elOrig1);
  CHECK_POINTER(elNew1);

  elOrig2 = elOrig1->next;
  CHECK_POINTER(elOrig2);

  //if any of elOrig1 or elOrig2 were variables,
  //decrease the reference count of corresponding rule(s)
  if (elOrig1->type == Variable)
    elOrig1->rule->refCount--;

  if (elOrig2->type == Variable)
    elOrig2->rule->refCount--;

  //create non-terminal substituting first pair (elOrig1, elOrig2)
  NEW(var1, RuleElement);
  var1->type = Variable;
  var1->value = 0;
  var1->rule = 0;	//for safe
  var1->next = elOrig2->next;

  //omitted test on 0, because 0 shouldn't never happen!
  elOrig2->next->prev = var1;
  var1->prev = elOrig1->prev;

  //remove digram with successor of elOrig2
  DIGRAM_REMOVE(elOrig2, var1->next);



  elNew2 = elNew1->next;
  CHECK_POINTER(elNew2);

  //create non-terminal substituting second pair (elNew1, elNew2)
  NEW(var2, RuleElement);
  var2->type = Variable;
  var2->value = 0;
  var2->rule = 0;	//for safe
  var2->next = 0;
  var2->prev = elNew1->prev;
  elNew1->prev->next = var2;

  //last symbol of the rule is now var2
  rootRule->last = var2;

  if (!elOrig1->prev)
    //elOrig1 was the first element
    //var1 is now the head of the list
    rootRule->body = var1;
  else
    elOrig1->prev->next = var1;

  //create rule from original pair
  elOrig1->prev = 0;
  elOrig2->next = 0;

    


  //create new rule with right side containing elOrig1 and elOrig2
  newRule = createRule(elOrig1, elOrig2);

  //digram (elOrig1, elOrig2) is now in newRule
  DIGRAM_CHANGE(elOrig1, elOrig2, newRule);

  //bind created variables with new rule
  var1->rule = newRule;
  var2->rule = newRule;

  //appropriately adjust reference count fo the rule
  newRule->refCount = 2;


  //update digrams
  DIGRAM_REMOVE(elNew1->prev, elNew1);

  if (var1->prev)
    DIGRAM_REMOVE(var1->prev, elOrig1);

  DIGRAM_ADD(var1, var1->next, rootRule);
  DIGRAM_ADD(var2->prev, var2, rootRule);


  //delete second pair of terminals
  DELETE(elNew1);
  DELETE(elNew2);


  RuleElement *relRep;
  NeighboursDescription *ndRep;

  //if the first symbol of the new rule is a variable
  //append the rule to the representedBy list of the first element
  //of the rule referenced by that variable
  if (elOrig1->type == Variable)
    {
      relRep = elOrig1;
      //DBG (">>> " << elOrig1->rule->id << " : body = " << elOrig1->rule->body->value);

      while (relRep->type != Terminal)
	relRep = relRep->rule->body;

      ndRep = terminalDigrams->find(relRep->value);
      
      CHECK_POINTER(ndRep);
      if (ndRep)
	{
	  //	  DBG("RR2 append to "  << (unsigned int)relRep->value << ":: " << newRule->id);
	  REPRESENTED_BY_APPEND(ndRep->representedBy, newRule);
	  //ndRep->representedBy.append(newRule);
	}

    }
  


  //the size doesn't change: size-2*2 + 2*1 + 1*2 (replace two digrams with two variables and create new rule with size 2)
  //return pointer to the new rule
  return newRule;
}



/*!
  Both occurences of the digram that appears in two distinct rules (one of the rules is always the root rule) are replaced by a new variable representing this digram.

  \param ruleOrig Pointer to the non-root rule.
  \param elOrig1 Pointer to the first symbol of the first digram.
  \param elNew1 Pointer to the second symbol of the second digram.

  \return Pointer to newly created rule.
*/
Rule *KYGrammar::reductionRule3(Rule *ruleOrig, RuleElement *elOrig1, RuleElement *elNew1)
{
  RuleElement *elOrig2;
  RuleElement *elNew2;
  RuleElement *var1, *var2;
  Rule *newRule;
  NeighboursDescription *nd;
  RuleElementNeighbour *r;

  //              DBG("Applying reduction rule 3.");

  CHECK_POINTER(ruleOrig);
  CHECK_POINTER(elOrig1);
  CHECK_POINTER(elNew1);

  elOrig2 = elOrig1->next;
  CHECK_POINTER(elOrig2);

  //if any of elOrig1 or elOrig2 were variables,
  //decrease the reference count of responding rule(s)
  if (elOrig1->type == Variable)
    elOrig1->rule->refCount--;

  if (elOrig2->type == Variable)
    elOrig2->rule->refCount--;

  //create non-terminal substituting first pair (elOrig1, elOrig2)
  NEW(var1, RuleElement);
  var1->type = Variable;
  var1->value = 0;
  var1->rule = 0;	//for safe

  var1->next = elOrig2->next;
  if (elOrig2->next)
    {
      //remove digram with successor of elOrig2
      //       DBG("ELORIG2, remove 1: " << elOrig2);
      DIGRAM_REMOVE(elOrig2, var1->next);
      elOrig2->next->prev = var1;
    }
  else
    //var1 is now the last element of ruleOrig
    ruleOrig->last = var1;


  var1->prev = elOrig1->prev;
  if (!elOrig1->prev)
    //elOrig1 was the first element
    //var1 is now the first element of the ruleOrig
    ruleOrig->body = var1;
  else
    elOrig1->prev->next = var1;


  elNew2 = elNew1->next;
  CHECK_POINTER(elNew2);

  //create non-terminal substituting second pair (elNew1, elNew2)
  NEW(var2, RuleElement);
  var2->type = Variable;
  var2->value = 0;
  var2->rule = 0;	//for safe
  var2->next = 0;
  var2->prev = elNew1->prev;

  if (elNew1->prev)
    elNew1->prev->next = var2;
  else
    //var2 is now the only element of roorRule
    rootRule->body = var2;

  //last symbol of the rule is now var2
  rootRule->last = var2;


  //create rule from original pair
  elOrig1->prev = 0;
  elOrig2->next = 0;


  //create new rule with right side containing elOrig1 and elOrig2
  newRule = createRule(elOrig1, elOrig2);

  //digram (elOrig1, elOrig2) is now owned by newRule
  DIGRAM_CHANGE(elOrig1, elOrig2, newRule);

  //bind created variables with new rule
  var1->rule = newRule;
  var2->rule = newRule;

  //appropriately adjust reference count fo the rule
  newRule->refCount = 2;


  //update digrams
  //    DBG("ELNEW1->PREV, remove 2: " << elNew1->prev);
  DIGRAM_REMOVE(elNew1->prev, elNew1);
  if (var1->prev)
    {
      //        DBG("VAR1->prev, remove 3" << var1->prev);
      DIGRAM_REMOVE(var1->prev, elOrig1);
    }

  if (var1->next)
    DIGRAM_ADD(var1, var1->next, ruleOrig);

  if (var1->prev)
    DIGRAM_ADD(var1->prev, var1, ruleOrig);

  DIGRAM_ADD(var2->prev, var2, rootRule);


  //delete second pair of terminals
  DELETE(elNew1);
  DELETE(elNew2);


  NeighboursDescription *ndRep;
  RuleElement *relRep;
  


  if (!var1->prev)
    {
      relRep = var1;

      while (relRep->type != Terminal)
	relRep = relRep->rule->body;

      ndRep = terminalDigrams->find(relRep->value);

      if (ndRep)
	{
	  REPRESENTED_BY_APPEND(ndRep->representedBy, ruleOrig);
	}
    }
  //if the first symbol of the new rule is a variable
  //append the rule to the representedBy list of the first element
  //of the rule referenced by that variable
  if (elOrig1->type == Variable)
    {
      if (var1->prev)
	{
	  relRep = elOrig1;
	  
	  while (relRep->type != Terminal)
	    relRep = relRep->rule->body;

	  ndRep = terminalDigrams->find(relRep->value);

	}
      
      CHECK_POINTER(ndRep);
      if (ndRep)
	{
	  //  	  DBG(newRule->id);
	  //  	  DBG("RR3 append (a) to "  << (unsigned int)relRep->value << ":: " << newRule->id);
	  REPRESENTED_BY_APPEND(ndRep->representedBy, newRule);
	}
    }


  //the size doesn't change: size-2*2 + 2*1 + 1*2 (replace two digrams with two variables and create new rule with size 2)
  
  //return pointer to the new rule
  return newRule;
}



/*!
  This method reconstructs the data represented by the grammar. Output is done via \a outputDevice. If the device is NULL, standard output is used.

  \sa reconstructRule()
*/
void KYGrammar::reconstructInput(void)
{
  //    DBG("Reconstructed data follow on the next line:");
  reconstructRule(rootRule, outputDevice);
  //    DBG("Reconstructed data end.");
}



/*!
  Use of this method causes recursive reconstruction of the data represented by given rule. Output is done via \a outputDevice. If the device is NULL, standard output is used.

  \param rule Pointer to the rule to be reconstructed.
  \param outputDevice pointer of the IO device.

  \sa reconstructInput()
*/
void KYGrammar::reconstructRule(Rule *rule, IODevice *outputDevice)
{
  RuleElement *rel;

  rel = rule->body;

  while (rel)
    {
      if (rel->type == Terminal)
	{
	  if (!outputDevice)
	    OUTPUT((char)rel->value);
	  else
	    {
	      outputDevice->writeData((const char *)&rel->value, SIZEOF_XML_CHAR);
	    }
	}
      else
	reconstructRule(rel->rule, outputDevice);

      rel = rel->next;
    }
}



size_t KYGrammar::getRuleLength(Rule *rule)
{
  RuleElement *rel;
  size_t size = 0;

  rel = rule->body;

  while (rel)
    {
      if (rel->type == Terminal)
	size++;
      else
	size = size + getRuleLength(rel->rule);

      rel = rel->next;
    }

  return size;
}



/*!
  This method prints human readable representation of the grammar on the standard input.
*/
void KYGrammar::print(void)
{
  Rule *rule;
  RuleElement *rel;

  OUTPUTENL("KY grammar content");
  //OUTPUTENL("Current run: " << (int)runCount);
  //OUTPUTENL("  Number of rules: \t" << ruleSet->count());

  for (rule = ruleSet->first(); rule; rule = ruleSet->next())
    {
      OUTPUTE("  R" << std::dec << rule->id << " (length: " << getRuleLength(rule) << ", used: " << rule->refCount << "x) ---> \'");

      if (RIGHT_SIDE_IS_EMPTY(rule))
	{
	  OUTPUTE("empty");
	}
      else
	{
	  for (rel = rule->body; rel; rel = rel->next)
	    {
	      if (rel->type == Terminal)
		{
		  if ((int)rel->value > 31)
		    {
		      //output the character
		      OUTPUTE((char)rel->value << " ");
		    }
		  else
		    {
		      //output the hexadecimal code
		      OUTPUTE("0x");
		      OUTPUTE(std::hex << std::setw(2) << std::setfill('0') << (int)rel->value << " ");
		    }
		}
	      else
		{
		  OUTPUTE("R" << rel->rule->id << " ");
		}
	    }
	}

      OUTPUTE("\'");
      OUTPUTEENDLINE;
    }

  OUTPUTE(std::dec);
  OUTPUTEENDLINE;

  //   OUTPUTENL("I = " << I);
}


/*!
  Prints all digrams that appear within the grammar.
*/
void KYGrammar::printDigrams(void)
{
  NeighboursDescription *nd;
  
  OUTPUTENL("** TERMINAL DIGRAMS");
  for (long i = 0; i < 9000; i++)
    {
      nd = terminalDigrams->find(i);
      if (nd && nd->neighbours.count())
	{
	  OUTPUTEENDLINE;
	  OUTPUTENL("DIGRAMS FOR TERMINAL: " << (int)i);
	  OUTPUTE("		represented by rules: ");
	  for (Rule *r = nd->representedBy.first(); r; r = nd->representedBy.next())
	    {
	      OUTPUTE(r->id << ", ");
	    }
	  OUTPUTEENDLINE;
	  
	  for (RuleElementNeighbour *r = nd->neighbours.first(); r; r = nd->neighbours.next())
	    {
	      if (r->origin->type == Terminal)
		OUTPUTE("  \"" << (int)r->origin->value << "\"");
	      else
		OUTPUTE("  N" << r->origin->rule->id);

	      OUTPUTENL("	[" << r->origin << ":" << r->rule << "]");
	    }
	}
    }

  OUTPUTENL("** VARIABLE DIGRAMS");

  for (long i = 0; i < 9000; i++)
    {
      nd = variableDigrams->find(i);
      if (nd && nd->neighbours.count())
	{
	  OUTPUTEENDLINE;
  	  OUTPUTENL("DIGRAMS FOR VARIABLE: " << i);
	  for (RuleElementNeighbour *r = nd->neighbours.first(); r; r = nd->neighbours.next())
	    {
	      if (r->origin->type == Terminal)
		OUTPUTE("  \"" << (int)r->origin->value << "\"");
	      else
		OUTPUTE("  N" << r->origin->rule->id);

	      OUTPUTENL("	[" << r->origin << ":" << r->rule << "]");
	    }
	}
    }

  OUTPUTEENDLINE;
}



/*!
  Sets the arithmetic context used by the grammar.

  \param ctxt Pointer to the context.
  \param useForOutput Indication whether the context should be used for the output of data (only compression).
 */
void KYGrammar::setContext(ContextBase *ctxt, bool useForOutput = true)
{
  context = ctxt;
  useContextForOutput = useForOutput;
}

