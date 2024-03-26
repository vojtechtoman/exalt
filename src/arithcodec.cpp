/***************************************************************************
    arithcodec.cpp  -  Definitions of ArithCodec methods
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

/* Based on code by:                                                       */

/***************************************************************************
Authors: 	John Carpinelli   (johnfc@ecr.mu.oz.au)
	 	Wayne Salamonsen  (wbs@mundil.cs.mu.oz.au)
  		Lang Stuiver      (langs@cs.mu.oz.au)
  		Radford Neal      (radford@ai.toronto.edu)

Purpose:	Data compression using a revised arithmetic coding method.

Based on: 	A. Moffat, R. Neal, I.H. Witten, "Arithmetic Coding Revisted",
		Proc. IEEE Data Compression Conference, Snowbird, Utah, 
		March 1995.

		Low-Precision Arithmetic Coding Implementation by 
		Radford M. Neal



Copyright 1995 John Carpinelli and Wayne Salamonsen, All Rights Reserved.
Copyright 1996 Lang Stuiver, All Rights Reserved.

These programs are supplied free of charge for research purposes only,
and may not sold or incorporated into any commercial product.  There is
ABSOLUTELY NO WARRANTY of any sort, nor any undertaking that they are
fit for ANY PURPOSE WHATSOEVER.  Use them at your own risk.  If you do
happen to find a bug, or have modifications to suggest, please report
the same to Alistair Moffat, alistair@cs.mu.oz.au.  The copyright
notice above and this statement of conditions must remain an integral
part of each and every copy made of these files.
****************************************************************************/


/*!
  \file arithcodec.cpp
  \brief Definitions of class ArithCodec methods.
  
  This file contains definitions of class ArithCodec methods.
*/


#ifdef __GNUG__
#pragma implementation
#endif

#include "arithcodec.h"


/*!
  \brief Output one bit to output device.

  When BYTE_SIZE bits are queued in outBuffer, these are output to the output device.

  \param b Value of the bit.
 */
#define OUTPUT_BIT(b)			\
{					\
  outBuffer <<= 1;			\
  if (b)				\
    outBuffer |= 1;			\
  outBitsToGo--;			\
  if (outBitsToGo == 0)			\
    {					\
      outputDevice->putChar(outBuffer);	\
      numberOfBytes++;			\
      outBitsToGo = BYTE_SIZE;		\
      outBuffer = 0;			\
    }					\
}						



/*!
 \brief Return bit to input stream.

 Only guaranteed to be able to backup by 1 bit.

 \param b Value of the bit.
 */
#define UNGET_BIT(b)			\
{					\
  inBitPtr <<= 1;			\
					\
  if (inBitPtr == 0)			\
    inBitPtr = 1;			\
					\
  /* Only keep bits still to be read */	\
  inBuffer = inBuffer & (inBitPtr - 1);	\
  if (b)				\
    /* Replace bit */			\
    inBuffer |= inBitPtr; 		\
}



/*!
  \brief Read one bit from input device.

  Macro shifts \v left and sets its leas significant bit to the value of read bit.
	
  \param v Buffer of bits.
  \param garbageBits Number of bits past end of file.
 */
#define ADD_NEXT_INPUT_BIT(v, garbageBits)				\
{									\
  if (inBitPtr == 0)							\
    {									\
      if (inputDevice->getChar(&inBuffer) == EndOfFile)			\
        {								\
          inGarbage++;							\
          if ((FreqValue)((inGarbage-1)*8) >= garbageBits)		\
            FATAL("Bad input file - attempted read past end of file.");	\
        }								\
      inBitPtr = (1<<(BYTE_SIZE-1));					\
    }									\
    v <<= 1;								\
									\
  if (inBuffer & inBitPtr)						\
    v++;								\
									\
  inBitPtr >>= 1;							\
}



/*!
 Output given bit and an opposite number of bit equal to the value stored in bitsOutstanding.
 
 \param b Value of the but.
*/
#define ORIG_BIT_PLUS_FOLLOW(b)		\
{ 	  			        \
  OUTPUT_BIT(b);			\
					\
  while (outBitsOutstanding > 0)	\
    { 					\
      OUTPUT_BIT(!b);			\
      outBitsOutstanding--;    		\
    } 	                		\
}


#ifdef FRUGAL_BITS
/*!
  Output one bit and bit based on bitsOutstanding. First bit is ignored.

  \param x Value of the bit.
 */
# define BIT_PLUS_FOLLOW(x)			\
   {						\
     if (ignoreFirstBit)			\
       ignoreFirstBit = 0;			\
     else					\
       ORIG_BIT_PLUS_FOLLOW(x);			\
   }

#else

/*!
  Output one bit and bit based on bitsOutstanding.

  \param x Value of the bit.
 */
# define BIT_PLUS_FOLLOW(x)		ORIG_BIT_PLUS_FOLLOW(x)

#endif

/*!
 Output code bits until the range has been expanded to above Quarter. With FRUGAL_BITS option, ignore first zero bit output (a redundant zero will otherwise be emitted every time the encoder is started).
*/
#define ENCODE_RENORMALISE		\
{					\
  while (out_R <= Quarter)		\
    {					\
      if (out_L >= Half)		\
        {				\
          BIT_PLUS_FOLLOW(1);		\
          out_L -= Half;		\
        }				\
      else				\
        if (out_L+out_R <= Half)	\
          {				\
            BIT_PLUS_FOLLOW(0);		\
          }				\
        else 				\
          {				\
            outBitsOutstanding++;	\
            out_L -= Quarter;		\
          }				\
      out_L <<= 1;			\
      out_R <<= 1;			\
  }					\
}


#ifdef FRUGAL_BITS

/*!
 Input code bits until range has been expanded to more than Quarter. Mimics encoder. FRUGAL_BITS option also keeps track of bitstream input so it can work out exactly how many disambiguating bits the encoder put out (1, 2 or 3).
*/
# define DECODE_RENORMALISE			\
   {						\
     while (in_R <= Quarter)			\
       {					\
         in_R <<= 1;				\
         in_V <<= 1;				\
         ADD_NEXT_INPUT_BIT(in_D,B_BITS);	\
         if (in_D & 1)				\
           in_V++;				\
       }					\
   }

#else

/*!
 Input code bits until range has been expanded to more than Quarter. Mimics encoder. FRUGAL_BITS option also keeps track of bitstream input so it can work out exactly how many disambiguating bits the encoder put out (1, 2 or 3).
*/
# define DECODE_RENORMALISE		\
   {					\
     while (in_R <= Quarter)		\
       {				\
         in_R <<= 1;			\
         ADD_NEXT_INPUT_BIT(in_D,0);	\
       }				\
   }

#endif



/*!
  Initialization steps are performed.
*/
ArithCodec::ArithCodec(void)
  : ArithCodecBase()
{
  outputDevice = 0;


#ifdef FRUGAL_BITS
  ignoreFirstBit = 1;
  firstMessage = 1;
#endif

  numberOfBytes = 0;	//number of output bytes

  inBuffer = 0;		//I/O buffer
  inBitPtr = 0;		//bits left in buffer
  inGarbage = 0;	//bytes read beyond eof
  
  outBuffer = 0;	//I/O buffer
  outBitsToGo = 0;	//bits to fill buffer
}




/*!
  Variables related to bit output are intialised.
*/
void ArithCodec::startOutputtingBits(void)
{
  outBuffer = 0;
  outBitsToGo = BYTE_SIZE;
}

/*!
  Variables related to bit input are initialized.
*/
void ArithCodec::startInputtingBits(void)
{
  inGarbage = 0;	//Number of bytes read past end of file
  inBitPtr = 0;		//No valid bits yet in input buffer
}



/*!
  Output remaining bits.
*/
void ArithCodec::doneOutputtingBits(void)
{
  if (outBitsToGo != BYTE_SIZE)
    {
      outputDevice->putChar(outBuffer << outBitsToGo);
      numberOfBytes++;
    }
  outBitsToGo = BYTE_SIZE;
}

/*!
 Clear input bit buffer.
*/
void ArithCodec::doneInputtingBits(void)
{
  inBitPtr = 0;		//"Wipe" buffer (in case more input follows)
}



//  /*!
//    Sets output device for the encoder.

//    \param od Output device.
//   */
//  void ArithCodec::setOutputDevice(IODevice *od)
//  {
//    if (outputDevice)
//      {
//        WRN("ArithCodec: Output device already specified, ommiting new.");
//      }
//    else
//      outputDevice = od;
//  }



//  /*!
//    Sets input device for the decoder.

//    \param id Input device.
//   */
//  void ArithCodec::setInputDevice(IODevice *id)
//  {
//    if (inputDevice)
//      {
//        WRN("ArithCodec: Input device already specified, ommiting new.");
//      }
//    else
//      inputDevice = id;
//  }



/*!
  The following pseudocode is a concise (but slow due to arithmetic calculations) description of what is calculated in this method. Note that the division is done before the multiplication. This is one of the key points of this version of arithmetic coding. This means that much larger frequency values can be accomodated, but that the integer ratio R / total (code range / frequency range) becomes a worse approximation as the total frequency count is increased.  Therefore less than the whole code range will be allocated to the frequency range. The whole code range is used by assigning the symbol at the end of the frequency range (where high == total) this excess code range above and beyond its normal code range. This of course distorts the probabilities slightly, see Moffat's paper for details and compression results. Restricting the total frequency range means that the division and multiplications can be done to low precision with shifts and adds.

  The following code describes this function. (The actual code is only written less legibly to improve speed.)
  
  \code
  L += low * (R / total);                   //Adjust low bound
  
  if (high < total)
    R = (high - low) * (R / total);         //Restrict range
  else                                      //If symbol at end of range.
    R = R - low * (R / total);              //Restrict & allocate excess codelength to it.

  ENCODE_RENORMALISE;                       //Expand code range and output bits
  
  if (bitsOutstanding > MAX_BITS_OUTSTANDING)
    abort();                                //EXTREMELY improbable
                                            //(see comments in the source)
  \endcode
  \par

  \param low Low bound of the subinterval.
  \param high High bound of the subinterval.
  \param total Total range of the interval.
*/
void ArithCodec::arithmeticEncode(FreqValue low, FreqValue high, FreqValue total)
{ 
  CodeValue temp; 

#ifdef MULT_DIV
  {
    DivValue out_r;			
    out_r = out_R/total;	//Calc range:freq ratio
    temp = out_r*low;		//Calc low increment
    out_L += temp;		//Increase L
    if (high < total)
      out_R = out_r*(high-low);	//Restrict R
    else
      out_R -= temp;		//If at end of freq range

    //Give symbol excess code range
  }
#else
  {
    CodeValue A, M;		//A = numerator, M = denominator
    CodeValue temp2;

    
    //calculate r = R/total, temp = r*low and temp2 = r*high
    //using shifts and adds  (r need not be stored as it is implicit in the loop)

    A = out_R;
    temp = 0;
    temp2 = 0;

    M = total << (B_BITS - F_BITS - 1);
    if (A >= M) { A -= M; temp += low; temp2 += high; }

#define UNROLL_NUM	B_BITS - F_BITS - 1
#define UNROLL_CODE				\
    A <<= 1; temp <<= 1; temp2 <<= 1;		\
    if (A >= M)					\
      {						\
        A -= M; temp += low; temp2 += high;	\
      }

#include "unroll.h"		//unroll the above code

    out_L += temp;
    if (high < total)
      out_R = temp2 - temp;
    else
      out_R -= temp;
  }
#endif

  ENCODE_RENORMALISE;

  if (outBitsOutstanding > MAX_BITS_OUTSTANDING)
    //For MAX_BITS_OUTSTANDING to be exceeded is extremely improbable, but
    //it is possible. For this to occur the COMPRESSED file would need to
    //contain a sequence MAX_BITS_OUTSTANDING bits long (eg: 2^31 bits, or
    //256 megabytes) of all 1 bits or all 0 bits. This possibility is
    //extremely remote (especially with an adaptive model), and can only
    //occur if the compressed file is greater than MAX_BITS_OUTSTANDING
    //long. Assuming the compressed file is effectively random,
    //the probability for any 256 megabyte section causing an overflow
    //would be 1 in 2^(2^31). This is a number approximately 600 million
    //digits long (decimal).
    FATAL("Bits_outstanding limit reached - File too large!");
}



/*!
 The following code describes this function (The actual code is only written less legibly to improve speed, including storing the ratio in_r = R/total for use by arithmeticDecode()):

 \code
 target = D / (R / total);	//D = V - L.  (Old terminology)
 				//D is the location within the range R
 				//that the code value is located
 				//Dividing by (R / total) translates it
 				//to its correspoding frequency value
  				
 if (target < total)		//The approximate calculations mean the
   return target;		//encoder may have coded outside the
 else				//valid frequency range (in the excess
   return total - 1;		//code range). This indicates that the
 				//symbol at the end of the frequency
 				//range was coded.  Hence return end of
 				//range (total - 1)
 \endcode
 \par

 \param total Current total frequency.

 \return Decoded target.
 *
 */
FreqValue ArithCodec::arithmeticDecodeTarget(FreqValue total)
{
  FreqValue target;
    
#ifdef MULT_DIV
  in_r = in_R/total;
  target = (in_D)/in_r;
#else 
  {	
    CodeValue A, M;	//A = numerator, M = denominator

    //divide r = R/total using shifts and adds
    A = in_R;
    in_r = 0;

    M = total << ( B_BITS - F_BITS - 1 );
    if (A >= M) { A -= M; in_r++; }

#define UNROLL_NUM	B_BITS - F_BITS - 1
#define UNROLL_CODE			\
    A <<= 1; in_r <<= 1;		\
    if (A >= M)				\
      {					\
        A -= M; in_r++;			\
      }

#include "unroll.h"

    A = in_D;
    target = 0;
    if (in_r < (1 << (B_BITS - F_BITS - 1)))
      { M = in_r << F_BITS; 
      if (A >= M) { A -= M; target++; }	
      A <<= 1; target <<= 1;		
      }
    else
      {
	M = in_r << (F_BITS - 1);
      }

    if (A >= M) { A -= M; target++; }


#define UNROLL_NUM	F_BITS - 1
#define UNROLL_CODE			\
    A <<= 1; target <<= 1;		\
    if (A >= M)				\
      {					\
        A -= M; target++;		\
      }	

#include "unroll.h"

  }
#endif //shifts vs multiply

  return (target >= total ? total-1 : target);
}



/*!
 The following code describes this function (The actual code is only written less legibly to improve speed. See the comments in the source code which is essentially the same process.):

 \code
 D -= low * (R / total);	//Adjust current code value
 
 if (high < total)
   R = (high-low) * (R/total);	//Adjust range
 else
   R -= low * (R / total);	//End of range is a special case
 
 DECODE_RENORMALISE;		//Expand code range and input bits
 \endcode
 \par

 \param low Low bound of the subinterval.
 \param high High bound of the subinterval.
 \param total The range of the interval.
 */
void ArithCodec::arithmeticDecode(FreqValue low, FreqValue high, FreqValue total)
{     
  CodeValue temp;

#ifdef MULT_DIV
  //assume r has been set by decode_target
  temp = in_r*low;
  in_D -= temp;
  if (high < total)
    in_R = in_r*(high-low);
  else
    in_R -= temp;
#else
  {
    CodeValue temp2, M;
    
    //calculate r*low and r*high using shifts and adds
    temp = 0;
    temp2 = 0;

    M = in_r << F_BITS;

    if (M & Half) { temp += low; temp2 += high; }

#define UNROLL_NUM  B_BITS - F_BITS - 1
#define UNROLL_CODE				\
    M <<= 1; temp <<= 1; temp2 <<= 1;		\
    if (M & Half)				\
      {						\
        temp += low; temp2 += high;		\
      }

#include "unroll.h"


    in_D -= temp;
    if (high < total)
      in_R = temp2 - temp;
    else
      in_R -= temp;
  }
#endif //shifts vs multiply

  DECODE_RENORMALISE;
}



/*!
 With FRUGAL_BITS, ensure first bit (always 0) not actually output.
*/
void ArithCodec::startEncode(void)
{
  out_L = 0;			//Set initial coding range to
  out_R = Half;			//[0,Half)
  outBitsOutstanding = 0;
#ifdef FRUGAL_BITS
  ignoreFirstBit = 1;		//Don't ouput the leading 0
#endif
}



#ifdef FRUGAL_BITS
/*!
 Encoding is finished by outputting follow bits and one to three further bits to make the last symbol unambiguous.

 Calculate the number of bits required: value = L rounded to "nbits" of precision (followed by zeros). value + roundup = L rounded to "nbits" of precision (followed by ones). Loop, increasing "nbits" until both the above values fall within [L,L+R), then output these "nbits" of L
*/
void ArithCodec::finishEncode(void)
{
  unsigned int nbits, i;
  CodeValue roundup, bits, value;

  bits = 0;
  nbits = 0;

  for (nbits = 1; nbits <= B_BITS; nbits++)
    {
      roundup = (1 << (B_BITS - nbits)) - 1;
      bits = (out_L + roundup) >> (B_BITS - nbits);
      value = bits << (B_BITS - nbits);
      if (out_L <= value && value + roundup <= (out_L + (out_R - 1)) )
	break;
    }
  for (i = 1; i <= nbits; i++)
    //output the nbits integer bits
    BIT_PLUS_FOLLOW(((bits >> (nbits-i)) & 1));
}
#else


/*!
 Encoding is finished by outputting follow bits and all the bits in L (B_BITS long) to make the last symbol unambiguous. This also means the
 decoder can read them harmlessly as it reads beyond end of what would have been valid input.
*/
void ArithCodec::finishEncode(void)
{
  int nbits, i;
  CodeValue bits;
  
  nbits = B_BITS;
  bits  = out_L;
  for (i = 1; i <= nbits; i++)
    //output the nbits integer bits
    BIT_PLUS_FOLLOW(((bits >> (nbits-i)) & 1));
}
#endif


/*!
 Fills the decode value \a in_D from the bitstream. If FRUGAL_BITS is defined only the first call reads \a in_D from the bitstream. Subsequent calls will assume the excess bits that had been read but not used (sitting in \a in_V) are the start of the next coding message, and it will put these into \a in_D. (It also initializes \a in_V to the start of the bitstream.) 
 
 FRUGAL_BITS also means that the first bit (0) was not output, so only take \a B_BITS-1 from the input stream. Since there are \a B_BITS "read-ahead" in the buffer, on second and subsequent calls retrieveExcessInputBits() is used and the last bit must be placed back into the input stream.

 \sa retrieveExcessInputBits()
 */
void ArithCodec::startDecode(void)
{
  unsigned int i;
  
  in_D = 0;		//Initial offset in range is 0
  in_R = Half;		//Range = Half
  
#ifdef FRUGAL_BITS
  {
    
    if (firstMessage)
      {
	for (i = 0; i < B_BITS-1; i++)
	  ADD_NEXT_INPUT_BIT(in_D, B_BITS);
      }
    else
      {
	in_D = retrieveExcessInputBits();
	UNGET_BIT(in_D & 1);
	in_D >>=1;
      }
    
    firstMessage = 0;
    in_V = in_D;
  }
#else
  for (i = 0; i<B_BITS; i++)	//Fill D
    ADD_NEXT_INPUT_BIT(in_D, 0);
#endif

  if (in_D >= Half)
    FATAL("Corrupt input file.");
}

#ifdef FRUGAL_BITS
/*!
 Finish decoding by consuming the disambiguating bits generated by finishEncode (either 1, 2 or 3). Calculated as for finishEncode(). This will mean the coder will have read exactly B_BITS past end of valid coding output (these bits can be retrived with a call to retrieveExcessInputBits()).

 \sa finishEncode(), retrieveExcessInputBits()
*/
void ArithCodec::finishDecode(void)
{
  unsigned int nbits, i;
  CodeValue roundup, bits, value;
  CodeValue in_L;
  
  //This gets us either the real L, or L + Half.  Either way, we can work
  //out the number of bit emitted by the encoder
  in_L = (in_V & (Half-1))+ Half - in_D;
  
  for (nbits = 1; nbits <= B_BITS; nbits++)
    {
      roundup = (1 << (B_BITS - nbits)) - 1;
      bits = (in_L + roundup) >> (B_BITS - nbits);
      value = bits << (B_BITS - nbits);
      if (in_L <= value && value + roundup <= (in_L + (in_R - 1)) )
	break;
    }

  //No bits to consume
  if (nbits > B_BITS)
    return;
  else
    for (i = 1; i <= nbits; i++)
      {
	ADD_NEXT_INPUT_BIT(in_V, B_BITS);
      }
}

/*!
  With FRUGAL_BITS defined, B_BITS beyond valid coding output are read. It is these excess bits that are returned by calling this function.

  \return Excess bits.
 */
CodeValue ArithCodec::retrieveExcessInputBits(void)
{
  return (in_V & (Half + (Half-1)) );
}

#else

/*!
 Throw away B_BITS in buffer by doing nothing (encoder wrote these for us to consume). They were mangled anyway as we only kept V - L, and cannot get back to V.
 */
void ArithCodec::finishDecode(void)
{
  //No action
}
#endif

size_t ArithCodec::numberOfOutputBytes(void)
{
  return numberOfBytes;
}
