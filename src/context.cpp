/***************************************************************************
    context.cpp  -  Definitions of Context methods
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
  \file context.cpp
  \brief Definitions of class Context methods.
  
  This file contains definitions of class Context methods.
*/


#ifdef __GNUG__
# pragma implementation
#endif


#include "context.h"



//! Returns minimum of \a a and \a b
#define MIN(a,b)		((a) < (b) ? (a) : (b))



/*!
  \brief Increments the specified \a symbol probability by the \a inc amount. 

  If the most probable symbol is maintined at the end of the coding range (MOST_PROB_AT_END #defined), then both INCR_SYMBOL_PROB_ACTUAL and INCR_SYMBOL_PROB_MPS are used.  Otherwise, just INCR_SYMBOL_PROB_ACTUAL is used.

  \param symbol Symbol.
  \param inc Increment.
*/
#define INCR_SYMBOL_PROB_ACTUAL(symbol, inc)	\
  FreqValue _inc = (inc);			\
  int p = symbol;				\
  /* Increment stats */				\
  while (p > 0)					\
    {						\
      tree[p] += _inc;				\
      p = BACK(p);				\
    }						\
  total += _inc;				\



/*!
  Update most frequent symbol, assuming \a symbol was just incremented.

  \param symbol Incremented symbol.
*/
#define INCR_SYMBOL_PROB_MPS(symbol)			\
  {							\
    if (symbol == mostFreqSymbol)			\
      mostFreqCount += _inc;				\
    else						\
      if ((_high) - (_low) + (_inc) > mostFreqCount)	\
        {						\
          mostFreqSymbol = symbol;			\
          mostFreqCount = (_high) - (_low) + (_inc);	\
          mostFreqPos   = _low;				\
        }						\
      else						\
        if (symbol < mostFreqSymbol)			\
          mostFreqPos += _inc;				\
  }



#ifdef MOST_PROB_AT_END
/*!
 Increase symbol probability. Definition depends on whether most probable symbol needs to be remembered.
*/
#define INCR_SYMBOL_PROB(symbol, low1, high1, inc1)	\
{							\
  FreqValue _low = low1;				\
  FreqValue _high = high1;				\
  INCR_SYMBOL_PROB_ACTUAL(symbol, inc1)			\
  INCR_SYMBOL_PROB_MPS(symbol)				\
}

#else

/*!
 Increase symbol probability. Definition depends on whether most probable symbol needs to be remembered.
*/
#define INCR_SYMBOL_PROB(symbol, low1, high1, inc1)	\
{							\
  INCR_SYMBOL_PROB_ACTUAL(symbol, inc1)			\
}
#endif






/*!
  Get cumulative frequency of \a symbol

  \param symbol Symbol.
  \param c Resulting cumulative frequency.
*/
#define GET_COUNT(symbol, c)			\
{						\
  if ((symbol) & 1)				\
    c = tree[symbol];				\
  else						\
    {						\
      int q = symbol + 1;			\
      int z = MIN(FORW(symbol), maxLength + 1);	\
      c = tree[symbol];				\
      while (q < z)				\
        {					\
          c -= tree[q];				\
          q  = FORW(q);				\
        }					\
    }						\
}



/*!
  Zero frequency probability is specified as a count out of the total frequency count.  It is stored as the first item in the tree (item 1).  Luckily, a Moffat tree is defined such that we can read the value of item 1 directly (tree[1]), although it cannot be updated directly. After each symbol is coded, ADJUST_ZERO_FREQ() is called to ensure that the zero frequency probability stored in the tree is still accurate (and changes it if it has changed).
*/
#define ADJUST_ZERO_FREQ()			\
{						\
  FreqValue diff;				\
  diff = ZERO_FREQ_PROB - tree[1];		\
  if (diff != 0)				\
    INCR_SYMBOL_PROB(1, 0, tree[1], diff);	\
}



/*!
  \brief Defines the count for the escape symbol.
  
  We implemented a variation of the XC method (which we call AX). We create a special escape symbol, which we keep up to date with the count of "number of singletons + 1". To achieve this, but still be efficient with static contexts, we falsely increment the number of singletons at the start of modeling for dynamic contexts, and keep it at 0 for static contexts. This way, nSingletons is always our zero frequency probability, without the need to check if the context is static or dynamic (remember that this operation would be done for each symbol coded).
*/
#define ZERO_FREQ_PROB		((FreqValue)nSingletons)




/*!
  Calls construcotr of class ContextBase.
 */
Context::Context(void)
  : ContextBase()
{
  tree = 0;
}



/*!
  Calls initContext().

  \param size Initial size of the context.
  \param type Type of the context.
 */
Context::Context(int size, ContextType type)
  : ContextBase()
{
  tree = 0;
  setType(size, type);
}



/*!
  Frees the memory occupied by Fenwick's tree.
 */
Context::~Context(void)
{
  if (tree)
    free(tree);
}



/*!
  If context is dynamic, nSingletons will be equal to incr` otherwise it will be 0.
 */
void Context::initZeroFreq()
{
  /* nSingletons is now actually nSingletons + 1, but this means
     we can use nSingletons directly as ZERO_FREQ_PROB */
  if (type == DynamicContext)
    nSingletons += incr;
  else
    nSingletons = 0;
}



/*!
  Create a new frequency table using a binary index tree. Table may be static or dynamic depending on the type parameter. Dynamic tables may grow above their initial length as new symbols are installed. Maximum length (maxLength) is set to 2^ceil(log_2 length).

  Valid tree indices are 1..maxLength - 1 (maxLength refers to the maximum length of the structure before it needs to expand).

  \param len Initial size of the context.
  \param ct type of the context.
 */
void Context::setType(int len, ContextType ct)
{
  int i;
  int size = 1;

  //add one entry for EOM symbol
  len++;
  endOfMessage = length = len;

  //increment length to accommodate the fact 
  //that symbol 0 is stored at pos 2 in the array.
  //(Escape symbol is at pos 1, pos 0 is not used).
  length+=2;

  //round length up to next power of two
  while (size < length-1)
    size = size << 1;

  //malloc context structure and array for frequencies
  if (!(tree = (FreqValue *)malloc((size+1)*sizeof(FreqValue))))
    FATAL("Not enough memory to create context!");

  //initialBaseSize = length;	//for more sensible initial installation of symbols
  initialSize = size;	//save for purging later
  length = 1;		//current no. of symbols
  total = 0;            //total frequency
  nSymbols = 1;         //count of symbols
  type = ct;            //is context DynamicContext or StaticContext
  maxLength = size;     //no. symbols before growing

  mostFreqSymbol = -1;  //Initially no mostFreqSymbol
  mostFreqCount = 0;
  mostFreqPos = 0;
    
  //initialize contents of tree array to zero
  for (i = 0; i <= maxLength; i++)
    tree[i] = 0;

  //increment is initially 2^f
  incr = (FreqValue) 1 << F_BITS; 
  nSingletons = 0;

  initZeroFreq();
  ADJUST_ZERO_FREQ();
}



/*!
  Install all symbols of the initial context.

  \return Index of last symbol in the table.
 */
int Context::initialize(void)
{
  //this is better and less "vasting"
  for (int i=0; i < endOfMessage; i++)
    installSymbol(i);
//   for (int i=0; i < initialSize; i++)
//     installSymbol(i);

  if (installSymbol(endOfMessage) == TooManySymbols)
    FATAL("TOO MANY SYMBOLS: Couldn't install initial symbols. "
	  "(Perhaps F_BITS  is too small?)");

  initialized = true;
  return endOfMessage;
}



int Context::lastFixedSymbol(void)
{
  return endOfMessage;
}


/*!
  Install a new symbol in a context's frequency table.

  \param symbol Installed symbol.

  \retval Context::Ok Success
  \retval Context::TooManySymbols Too many symbols in the table.
  \retval Context::NoMemory No memory left for the symbol.
*/
int Context::installSymbol(int symbol)
{
  int i;
  FreqValue low, high;

  //Increment because first user symbol (symbol 0)
  //is stored at array position 2
  symbol+=2;


  //if the symbol is already installed, don't do anything
  if (symbol <= length)
    {
//       DBG("NOT INSTALLED SYMBOL: " << symbol << " length: "<< length);
      return Context::Ok;
    }

  //if new symbol is greater than current array length then double length 
  //of array 
  while (symbol > maxLength) 
    {
      //DBG("MAXLENGTH = " << maxLength << " SYMBOL = " << symbol);
      tree = (FreqValue *)realloc(tree, (2*maxLength+1) * sizeof(FreqValue));
      if (!tree)
        {
	  ERR("Not enough memory to expand context!");
	  return Context::NoMemory;
        }

      //clear new part of table to zero
      for (i = maxLength+1; i <= 2*maxLength; i++)
 	tree[i] = 0;
      
      maxLength <<= 1;
      //DBG("NEW MAXLENGTH = " << maxLength);
    }

  //check that we are not installing too many symbols
  if ((unsigned long)((nSymbols + 1) << 1) >= MAX_FREQUENCY)
    //cannot install another symbol as all frequencies will 
    //halve to one and an infinite loop will result
    return Context::TooManySymbols;
    
  if (symbol > length)	//update length if necessary
    length = symbol;
  nSymbols++;		//increment count of symbols

  getInterval(&low, &high, symbol);

  //update the number of singletons if context is DynamicContext
  INCR_SYMBOL_PROB(symbol, low, high, incr);
  if (type == DynamicContext)
    nSingletons += incr;

  ADJUST_ZERO_FREQ();

  //halve frequency counts if total greater than Max_frequency
  while (total > MAX_FREQUENCY)
    halveContext();

  return Context::Ok;
}


int Context::encodeEndOfMessage() throw (ExaltContextNotInitializedException, ExaltIOException)
{
  if (!initialized)
    {
      throw ExaltContextNotInitializedException();
    }
  else
    return encode(endOfMessage);
}

/*!
 Encode a symbol given its context. The lower and upper bounds are determined using the frequency table, and then passed on to the coder. If the symbol has zero frequency, code an escape symbol and return Context::NotKnown, otherwise return Context::Ok.

 \param symbol Encoded symbol

 \retval Context::NotKnown Encoding unknown symbol.
 \retval Context::Ok Success.
 */
int Context::encode(int symbol) throw (ExaltContextNotInitializedException, ExaltIOException)
{
  FreqValue low, high, low_w, high_w;

  if (!initialized)
    {
      throw ExaltContextNotInitializedException();
    }

  symbol+=2;
  if ((symbol > 0) && (symbol <= maxLength))
    {
      if (mostFreqSymbol == symbol)
        {
          low  = mostFreqPos;
          high = low + mostFreqCount;
        }
      else
	getInterval(&low, &high, symbol);
    }
  else
    low = high = 0;
    
  if (low == high)
    {
      if (ZERO_FREQ_PROB == 0)
      	FATAL("Cannot code zero-probability novel symbol!");

      //encode the escape symbol if unknown symbol
      symbol = 1;
      if (mostFreqSymbol == 1)
        {
	  low = mostFreqPos;
	  high = low + mostFreqCount;
        }
      else
        getInterval(&low, &high, symbol);
    }

  //Shift high and low if required so that most probable symbol
  //is at the end of the range
  //(Shifted values are low_w, and high_w, as original values
  //are needed when updating the stats)

#ifdef MOST_PROB_AT_END
  if (symbol > mostFreqSymbol)
    {
      low_w  = low  - mostFreqCount;
      high_w = high - mostFreqCount;
    }
  else
    if (symbol == mostFreqSymbol)
      {
	low_w  = total - mostFreqCount;
	high_w = total;
      }
    else
      {
	low_w  = low;
	high_w = high;
      }
#else
  low_w  = low;
  high_w = high;
#endif

  //call the coder with the low, high and total for this symbol
  //(with low_w, high_w:  Most probable symbol moved to end of range)

  arithCodec->arithmeticEncode(low_w, high_w, total);

  if (symbol != 1)	//If not the special ESC / NOT_KNOWN symbol
    {
      //update the singleton count if symbol was previously a singleton
      if (type == DynamicContext && high-low == incr)
	nSingletons -= incr;

      //increment the symbol's frequency count
      INCR_SYMBOL_PROB(symbol, low, high, incr);
    }

  ADJUST_ZERO_FREQ();

  while (total > MAX_FREQUENCY)
    halveContext();

  if (symbol == 1)
    return Context::NotKnown;

  return Context::Ok;
}




/*!
 Decode symbol based on current context.

 \return Decoded symbol.
*/
int Context::decode(void) throw (ExaltContextNotInitializedException, ExaltIOException)
{
  int symbol;
  int p, m, e;
  FreqValue low, high, target;
  int n = maxLength;


  if (!initialized)
    {
      throw ExaltContextNotInitializedException();
    }
 
  target = arithCodec->arithmeticDecodeTarget(total);

#ifdef MOST_PROB_AT_END
  //Check if most probable symbol (shortcut decode)
  if (target >= total - mostFreqCount)
    {
      arithCodec->arithmeticDecode( total - mostFreqCount, total, total);
      symbol = mostFreqSymbol;
      low    = mostFreqPos;
      high   = low + mostFreqCount;

      INCR_SYMBOL_PROB(symbol, low, high, incr);
      
      if (symbol != 1) 
	if (type == DynamicContext && high-low == incr)
	  nSingletons -= incr;
    }
  else
    //Not MPS, have to decode slowly
    {
      if (target >= mostFreqPos)
	target += mostFreqCount;
#endif
      p = 1; low = 0;
      while ( ((p << 1) <= maxLength ) && (tree[p] <= target))
	{
	  target -= tree[p];
	  low    += tree[p];
	  p      <<= 1;
	}

      symbol = p;
      m = p >> 1;
      e = 0;

      while (m >= 1)
	{
	  if (symbol + m <= n)
	    {
	      e += tree[symbol + m];
	      if (tree[symbol] - e <= target) 
		{
		  target    -= tree[symbol] - e;
		  low       += tree[symbol] - e;
		  if (symbol != 1) tree[symbol] += incr;
		  symbol    += m;
		  e         =  0;
		}
	    }
	  m >>= 1;
	}
      if (symbol!= 1) tree[symbol] += incr;

      if (symbol & 1)
        high = low + tree[symbol];
      else {
        GET_COUNT(symbol, high);
        high += low;
      }
      if (symbol != 1) high -= incr;

#ifdef MOST_PROB_AT_END
      if (low >= mostFreqPos)
	//Ie: Was moved
	arithCodec->arithmeticDecode(low  - mostFreqCount,
				     high - mostFreqCount,
				     total);
      else
#endif
        arithCodec->arithmeticDecode(low, high, total);

      //update the singleton count if symbol was previously a singleton
      if (symbol != 1)
	{
	  if (type == DynamicContext && high-low == incr)
	    nSingletons -= incr;

	  total += incr;

	  if (symbol == mostFreqSymbol)
	    mostFreqCount += incr;
	  else
	    if (high-low+incr > mostFreqCount)
	      { 
		mostFreqSymbol = symbol;
		mostFreqCount  = high - low + incr;
		mostFreqPos    = low;
	      }
	    else
	      if (symbol < mostFreqSymbol)
		mostFreqPos += incr;
	}
      
#ifdef MOST_PROB_AT_END
    }  //If not MPS
#endif

  ADJUST_ZERO_FREQ();

  //halve all frequencies if necessary
  while (total > MAX_FREQUENCY)
    halveContext();

  if (symbol == 1)
    return Context::NotKnown;

  return symbol - 2;
}



/*!
 Get the low and high limits of the frequency interval occupied by a symbol.

 \param pLow Low frequency linit.
 \param pHigh High frequency linit.
 \param symbol Symbol.
*/
void Context::getInterval(FreqValue *pLow, FreqValue *pHigh, int symbol)
{
  FreqValue low, count;
  int p, q;

  //go too far
  for (p = 1, low = 0; p < symbol; )
    {
      low += tree[p], p <<= 1;
    }

  //subtract off the extra freqs from low
  q = symbol;
  while ((q != p) && (q <= maxLength))
    {
      low -= tree[q], q = FORW(q);
    }

  GET_COUNT(symbol, count);
  
  *pLow = low;
  *pHigh = low + count;
}

 

/*!
 This method is responsible for halving all the frequency counts in a context. It halves context in linear time by converting tree to list of freqs and then back again. It ensures the most probable symbol size and range stay updated. If halving the context gives rise to a sudden drop in the ZERO_FREQ_PROB(), and if it was the most probable symbol, it will stay recorded as the most probable symbol even if it isn't. This may cause slight compression inefficiency. (The ZERO_FREQ_PROB() as implemented does not have this characteristic, and in this case the inefficiency cannot occur.)
 */
void Context::halveContext(void)
{
  int  shifts, p, symbol;

  //halve increment
  incr = (incr + MIN_INCR) >> 1;
  if (incr < MIN_INCR) 
    incr = MIN_INCR;
  nSingletons = incr;
    
  //Convert Moffat tree to array of freqs
  for (shifts=0 , p = maxLength ; p > 1 ; shifts++ ) p >>= 1;
  p  = 1 << shifts;      //p is now to 2^floor(log_2 pContext->max_length)
  while (p > 1)
    {
      symbol = p;
      while (symbol + (p >> 1) <= maxLength )
	{
	  tree[symbol] -= tree[symbol + (p >> 1)];
	  symbol += p;
	}
      p >>= 1;
    }


  //Halve the counts (ignore tree[1] as it will be changed soon)
  total = 0;
  for (p = 2; p <= maxLength; p++) 
    {
      tree[p] = (tree[p] + 1) >> 1;
      total += tree[p];
      if (tree[p] == incr)
	nSingletons += incr;
    }


  //Convert array of freqs to Moffat tree
  for (p = 2; p <= maxLength; )
    {
      symbol = p;
      while (symbol + (p >> 1) <= maxLength )
	{
	  tree[symbol] += tree[symbol + (p >> 1)];
	  symbol += p;
	}
      p <<= 1;
    }

  if (type == StaticContext)
    nSingletons = 0;

  tree[1] = ZERO_FREQ_PROB;
  total += ZERO_FREQ_PROB;

  //Recalc new mostFreqSymbol info if it exists
  //(since roundoff may mean not exactly half of previous value)

  if (mostFreqSymbol != -1) 
    {
      FreqValue low, high;

      getInterval(&low, &high, mostFreqSymbol);
      mostFreqCount = high-low;
      mostFreqPos = low;
    }

  ADJUST_ZERO_FREQ();
}


/*!
 Frees memory allocated for a context and initialize empty context of original size.
 */
void Context::purgeContext(void)
{
  int i;

  if (tree)
    free(tree);
    
  //malloc new tree of original size
  if (!(tree = (FreqValue *)malloc((initialSize + 1) * sizeof(FreqValue))))
    FATAL("Not enough memory to create context!");

  length = 1;
  total = 0;
  nSymbols = 1;		///* Start with escape symbol

  mostFreqSymbol = -1;	//Indicates no such symbol
  mostFreqCount = 0;
  mostFreqPos = 0;

  maxLength = initialSize;
  for (i = 0; i <= initialSize; i++)
    tree[i] = 0;

  //increment is initially 2 ^ f
  incr = (FreqValue) 1 << F_BITS;
  nSingletons = 0;

  initZeroFreq();
  ADJUST_ZERO_FREQ();
}

