/// \file waEncode.cpp
/// ×Ö·û´®BASE64¡¢URI¡¢MD5±àÂëº¯ÊıÊµÏÖÎÄ¼ş

#include <cstdio>
#include <cstring>
#include <ctime>
#include <cctype>
#include <cassert>
#include <fstream>
#include <iostream>
#include "waEncode.h"

using namespace std;

/// Web Application Library namaspace
namespace webapp {
	
/// \defgroup waEncode waEncode×Ö·û´®¼ÓÃÜ±àÂëº¯Êı¿â
	
////////////////////////////////////////////////////////////////////////////////
// URI±àÂë½âÂë

// HEX×Ö·û×ª»»ÎªASC×Ö·û
// ÓÉº¯Êıuri_decode()µ÷ÓÃ
char hex_to_asc( const string &src ) {
	char digit;
	digit = ( src[0]>='A' ? ((src[0]&0xdf)-'A')+10 : (src[0]-'0') );
	digit *= 16;
	digit += ( src[1]>='A' ? ((src[1]&0xdf)-'A')+10 : (src[1]-'0') );
	
	return digit;
}

/// \ingroup waEncode
/// \fn string uri_encode( const string &source )
/// URI±àÂë
/// \param source Ô­×Ö·û´®
/// \return ±àÂë½á¹û×Ö·û´®
string uri_encode( const string &source ) {
	const char reserved[] = ";/?:@&=+\0";
	const char unsafe[] = " \"#%<>\0";
	const char other[] = "'`[],~!$^(){}|\\\r\n";

	char element[3];
	char chr;
	string res;
	res.reserve( source.length()*3 );
	
	for ( size_t i=0; i<source.length(); ++i ) {
		chr = source[i];
		if ( strchr(reserved,chr) || strchr(unsafe,chr) || strchr(other,chr) ) {
			sprintf( element, "%c%02X", '%', chr ); 
			res += element;
		} else {
			res += chr;
		}
	}
	
	return res;
}

/// \ingroup waEncode
/// \fn string uri_decode( const string &source )
/// URI½âÂë
/// \param source URI±àÂë×Ö·û´®
/// \return ½âÂë½á¹û
string uri_decode( const string &source ) {
	size_t pos = 0;
	string seq;
	string str = source;
	string rest = str;
	
	for ( str=""; ((pos=rest.find('%'))!=rest.npos); ) {
		if ( (pos+2)<rest.length() && isalnum(rest[pos+1]) && isalnum(rest[pos+2]) ) {
			seq = rest.substr( pos+1, 2 );
			str += rest.substr( 0, pos ) + hex_to_asc( seq );
			rest = rest.erase( 0, pos+3 );
		} else {
			str += rest.substr( 0, pos+1 );
			rest = rest.erase( 0, pos+1 );
		}
	}

	str += rest;
	return str;
}

////////////////////////////////////////////////////////////////////////////////
// BASE64±àÂë

//*********************************************************************
//* C_Base64 - a simple base64 encoder and decoder.
//*
//*     Copyright (c) 1999, Bob Withers - bwit@pobox.com
//*
//* This code may be freely used for any purpose, either personal
//* or commercial, provided the authors copyright notice remains
//* intact.
//
//* Reformated by James W. Anderson.  Added declarations to use
//* enhanced version.
//*********************************************************************

// class to encode and decode strings using Base64
class Base64 {
public:
	static std::string encode(const std::string& data); ///< returns the Base64 version of \c data as a string.
	static std::string decode(const std::string& data); ///< returns the decoded version of Base64 encoded \c data as a string.
	static bool isPrintable(const std::string& s); ///< Tests a string \c s to see if all the characters are printable (whether to call encode())).
	static const std::string Base64Table;
	
private:
	static const std::string::size_type DecodeTable[];
	static const char fillchar;
	static const std::string::size_type np;
};

//*********************************************************************
//* Base64 - a simple base64 encoder and decoder.
//*
//*     Copyright (c) 1999, Bob Withers - bwit@pobox.com
//*
//* This code may be freely used for any purpose, either personal
//* or commercial, provided the authors copyright notice remains
//* intact.
//*
//* Enhancements by Stanley Yamane:
//*     o reverse lookup table for the decode function 
//*     o reserve string buffer space in advance
//*
//*
//* Reformated by James W. Anderson.  Added isPrintable method.
//*********************************************************************

const char Base64::fillchar = '=';
const string::size_type Base64::np = string::npos;

                               // 0000000000111111111122222222223333333333444444444455555555556666
                               // 0123456789012345678901234567890123456789012345678901234567890123
const string Base64::Base64Table("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

// Decode Table gives the index of any valid base64 character in the Base64 table
// 65 == A, 97 == a, 48 == 0, 43 == +, 47 == /
                                               
const string::size_type Base64::DecodeTable[] =
//	0  1  2  3  4  5  6  7  8  9 
{	np,np,np,np,np,np,np,np,np,np,  // 0 - 9
	np,np,np,np,np,np,np,np,np,np,  //10 -19
	np,np,np,np,np,np,np,np,np,np,  //20 -29
	np,np,np,np,np,np,np,np,np,np,  //30 -39
	np,np,np,62,np,np,np,63,52,53,  //40 -49
	54,55,56,57,58,59,60,61,np,np,  //50 -59
	np,np,np,np,np, 0, 1, 2, 3, 4,  //60 -69
	5, 6, 7, 8, 9,10,11,12,13,14,  	//70 -79
	15,16,17,18,19,20,21,22,23,24,  //80 -89
	25,np,np,np,np,np,np,26,27,28,  //90 -99
	29,30,31,32,33,34,35,36,37,38,  //100 -109
	39,40,41,42,43,44,45,46,47,48,  //110 -119
	49,50,51,np,np,np,np,np,np,np,  //120 -129
	np,np,np,np,np,np,np,np,np,np,  //130 -139
	np,np,np,np,np,np,np,np,np,np,  //140 -149
	np,np,np,np,np,np,np,np,np,np,  //150 -159
	np,np,np,np,np,np,np,np,np,np,  //160 -169
	np,np,np,np,np,np,np,np,np,np,  //170 -179
	np,np,np,np,np,np,np,np,np,np,  //180 -189
	np,np,np,np,np,np,np,np,np,np,  //190 -199
	np,np,np,np,np,np,np,np,np,np,  //200 -209
	np,np,np,np,np,np,np,np,np,np,  //210 -219
	np,np,np,np,np,np,np,np,np,np,  //220 -229
	np,np,np,np,np,np,np,np,np,np,  //230 -239
	np,np,np,np,np,np,np,np,np,np,  //240 -249
	np,np,np,np,np,np               //250 -256
};

string Base64::encode(const string& data) {
	string::size_type i;
	char c;
	string::size_type len = data.length();
	string ret;
	
	ret.reserve(len * 2);
	
	for (i = 0; i < len; ++i) {
		c = (data[i] >> 2) & 0x3f;
		ret.append(1, Base64Table[c]);
		c = (data[i] << 4) & 0x3f;
		if (++i < len)
			c |= (data[i] >> 4) & 0x0f;
		
		ret.append(1, Base64Table[c]);
		if (i < len) {
			c = (data[i] << 2) & 0x3f;
			if (++i < len)
				c |= (data[i] >> 6) & 0x03;
			
			ret.append(1, Base64Table[c]);
		} else {
			++i;
			ret.append(1, fillchar);
		}
		
		if (i < len) {
			c = data[i] & 0x3f;
			ret.append(1, Base64Table[c]);
		} else {
			ret.append(1, fillchar);
		}
	}
	
	return(ret);
} // encode

string Base64::decode(const string& data) {
	string::size_type i;
	char c;
	char c1;
	string::size_type len = data.length();
	string ret;
	
	ret.reserve(len);
	
	for (i = 0; i < len; ++i) {
		c = (char) DecodeTable[(unsigned char)data[i]];
		++i;
		c1 = (char) DecodeTable[(unsigned char)data[i]];
		c = (c << 2) | ((c1 >> 4) & 0x3);
		ret.append(1, c);
		if (++i < len) {
			c = data[i];
			if (fillchar == c)
				break;
			
			c = (char) DecodeTable[(unsigned char)data[i]];
			c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
			ret.append(1, c1);
		}
		
		if (++i < len) {
			c1 = data[i];
			if (fillchar == c1)
				break;
			
			c1 = (char) DecodeTable[(unsigned char)data[i]];
			c = ((c << 6) & 0xc0) | c1;
			ret.append(1, c);
		}
	}
	
	return(ret);
} // decode

bool Base64::isPrintable(const string& s) {
	size_t i = 0;
	while (i < s.size() && (isprint(s[i]) || isspace(s[i]))) {
		i++;
	}
	return (i == s.size());
} // isPrintable

////////////////////////////////////////////////////////////////////////////////
/// \ingroup waEncode
/// \fn string base64_encode( const string &source )
/// MIME Base64±àÂë
/// \param source Ô­×Ö·û´®
/// \return ³É¹¦·µ»Ø±àÂë½á¹û,·ñÔò·µ»Ø¿Õ×Ö·û´®
string base64_encode( const string &source ) {
	Base64 b64;
	return b64.encode( source );
}

/// \ingroup waEncode
/// \fn string base64_decode( const string &source )
/// MIME Base64½âÂë
/// \param source BASE64±àÂë×Ö·û´®
/// \return ³É¹¦·µ»Ø½âÂë½á¹û,·ñÔò·µ»Ø¿Õ×Ö·û´®
string base64_decode( const string &source ) {
	Base64 b64;
	return b64.decode( source );
}

////////////////////////////////////////////////////////////////////////////////
// MD5.CC - source code for the C++/object oriented translation and 
//          modification of MD5.

// Translation and modification (c) 1995 by Mordechai T. Abzug 

// This translation/ modification is provided "as is," without express or 
// implied warranty of any kind.

// The translator/ modifier does not claim (1) that MD5 will do what you think 
// it does; (2) that this translation/ modification is accurate; or (3) that 
// this software is "merchantible."  (Language for this disclaimer partially 
// copied from the disclaimer below).

/* based on:

   MD5.H - header file for MD5C.C
   MDDRIVER.C - test driver for MD2, MD4 and MD5

   Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.

*/

class MD5 {

public:
	// methods for controlled operation:
	MD5              ();  // simple initializer
	void  update     (unsigned char *input, unsigned int input_length);
	void  update     (istream& stream);
	void  update     (FILE *file);
	void  update     (ifstream& stream);
	void  finalize   ();
	
	// constructors for special circumstances.  All these constructors finalize
	// the MD5 context.
	MD5              (unsigned char *string); // digest string, finalize
	MD5              (istream& stream);       // digest stream, finalize
	MD5              (FILE *file);            // digest file, close, finalize
	MD5              (ifstream& stream);      // digest stream, close, finalize
	
	// methods to acquire finalized result
	unsigned char    *raw_digest ();  // digest as a 16-byte binary array
	char *            hex_digest ();  // digest as a 33-byte ascii-hex string
	friend ostream&   operator<< (ostream&, MD5 context);
	
private:
	
	// first, some types:
	typedef unsigned       int uint4; // assumes integer is 4 words long
	typedef unsigned short int uint2; // assumes short integer is 2 words long
	typedef unsigned      char uint1; // assumes char is 1 word long
	
	// next, the private data:
	uint4 state[4];
	uint4 count[2];     // number of *bits*, mod 2^64
	uint1 buffer[64];   // input buffer
	uint1 digest[16];
	uint1 finalized;
	
	// last, the private methods, mostly static:
	void init             ();               // called by all constructors
	void transform        (uint1 *buffer);  // does the real update work.  Note 
	// that length is implied to be 64.
	
	static void encode    (uint1 *dest, uint4 *src, uint4 length);
	static void decode    (uint4 *dest, uint1 *src, uint4 length);
	static void memcpy    (uint1 *dest, uint1 *src, uint4 length);
	static void memset    (uint1 *start, uint1 val, uint4 length);
	
	static inline uint4  rotate_left (uint4 x, uint4 n);
	static inline uint4  F           (uint4 x, uint4 y, uint4 z);
	static inline uint4  G           (uint4 x, uint4 y, uint4 z);
	static inline uint4  H           (uint4 x, uint4 y, uint4 z);
	static inline uint4  I           (uint4 x, uint4 y, uint4 z);
	static inline void   FF  (uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, 
		uint4 s, uint4 ac);
	static inline void   GG  (uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, 
		uint4 s, uint4 ac);
	static inline void   HH  (uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, 
		uint4 s, uint4 ac);
	static inline void   II  (uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, 
		uint4 s, uint4 ac);
	
};

// MD5.CC - source code for the C++/object oriented translation and 
//          modification of MD5.

// Translation and modification (c) 1995 by Mordechai T. Abzug 

// This translation/ modification is provided "as is," without express or 
// implied warranty of any kind.

// The translator/ modifier does not claim (1) that MD5 will do what you think 
// it does; (2) that this translation/ modification is accurate; or (3) that 
// this software is "merchantible."  (Language for this disclaimer partially 
// copied from the disclaimer below).

/* based on:

   MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm
   MDDRIVER.C - test driver for MD2, MD4 and MD5

   Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
*/

// MD5 simple initialization method

MD5::MD5(){
	init();
}

// MD5 block update operation. Continues an MD5 message-digest
// operation, processing another message block, and updating the
// context.

void MD5::update (uint1 *input, uint4 input_length) {
	
	uint4 input_index, buffer_index;
	uint4 buffer_space;                // how much space is left in buffer
	
	if (finalized){  // so we can't update!
		cerr << "MD5::update:  Can't update a finalized digest!" << endl;
		return;
	}
	
	// Compute number of bytes mod 64
	buffer_index = (unsigned int)((count[0] >> 3) & 0x3F);
	
	// Update number of bits
	if (  (count[0] += ((uint4) input_length << 3))<((uint4) input_length << 3) )
		count[1]++;
	
	count[1] += ((uint4)input_length >> 29);
	buffer_space = 64 - buffer_index;  // how much space is left in buffer
	
	// Transform as many times as possible.
	if (input_length >= buffer_space) { // ie. we have enough to fill the buffer
		// fill the rest of the buffer and transform
		memcpy (buffer + buffer_index, input, buffer_space);
		transform (buffer);
		
		// now, transform each 64-byte piece of the input, bypassing the buffer
		for (input_index = buffer_space; input_index + 63 < input_length; 
			input_index += 64)
		transform (input+input_index);
		
		buffer_index = 0;  // so we can buffer remaining
	}
	else
		input_index=0;     // so we can buffer the whole input
	// and here we do the buffering:
	memcpy(buffer+buffer_index, input+input_index, input_length-input_index);
}

// MD5 update for files.
// Like above, except that it works on files (and uses above as a primitive.)

void MD5::update(FILE *file){	
	unsigned char buffer[1024];
	int len;
	
	while ( (len=fread(buffer, 1, 1024, file)) )
		update(buffer, len);
	
	fclose (file);	
}

// MD5 update for istreams.
// Like update for files; see above.

void MD5::update(istream& stream){	
	unsigned char buffer[1024];
	int len;
	
	while (stream.good()){
		stream.read((char*)buffer, 1024); // note that return value of read is unusable.
		len=stream.gcount();
		update(buffer, len);
	}	
}

// MD5 update for ifstreams.
// Like update for files; see above.

void MD5::update(ifstream& stream){	
	unsigned char buffer[1024];
	int len;
	
	while (stream.good()){
		stream.read((char*)buffer, 1024); // note that return value of read is unusable.
		len=stream.gcount();
		update(buffer, len);
	}	
}

// MD5 finalization. Ends an MD5 message-digest operation, writing the
// the message digest and zeroizing the context.
void MD5::finalize (){	
	unsigned char bits[8];
	unsigned int index, padLen;
	static uint1 PADDING[64]={
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
	
	if (finalized){
		cerr << "MD5::finalize:  Already finalized this digest!" << endl;
		return;
	}
	
	// Save number of bits
	encode (bits, count, 8);
	
	// Pad out to 56 mod 64.
	index = (uint4) ((count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	update (PADDING, padLen);
	
	// Append length (before padding)
	update (bits, 8);
	
	// Store state in digest
	encode (digest, state, 16);
	
	// Zeroize sensitive information
	memset (buffer, 0, sizeof(*buffer));
	
	finalized=1;	
}

MD5::MD5(FILE *file){	
	init();  // must be called be all constructors
	update(file);
	finalize ();
}

MD5::MD5(istream& stream){	
	init();  // must called by all constructors
	update (stream);
	finalize();
}

MD5::MD5(ifstream& stream){	
	init();  // must called by all constructors
	update (stream);
	finalize();
}

unsigned char *MD5::raw_digest(){	
	uint1 *s = new uint1[16];
	
	if (!finalized){
		cerr << "MD5::raw_digest:  Can't get digest if you haven't "<<
		"finalized the digest!" <<endl;
		return ( (unsigned char*) "");
	}
	
	memcpy(s, digest, 16);
	return s;
}

char *MD5::hex_digest(){
	int i;
	char *s= new char[33];
	
	if (!finalized){
		cerr << "MD5::hex_digest:  Can't get digest if you haven't "<<
		"finalized the digest!" <<endl;
		return "";
	}
	
	for (i=0; i<16; i++)
		sprintf(s+i*2, "%02x", digest[i]);
	s[32]='\0';
	
	return s;
}

ostream& operator<<(ostream &stream, MD5 context){
	stream << context.hex_digest();
	return stream;
}

// PRIVATE METHODS:

void MD5::init(){
	finalized=0;  // we just started!
	
	// Nothing counted, so count=0
	count[0] = 0;
	count[1] = 0;
	
	// Load magic initialization constants.
	state[0] = 0x67452301;
	state[1] = 0xefcdab89;
	state[2] = 0x98badcfe;
	state[3] = 0x10325476;
}

// Constants for MD5Transform routine.
// Although we could use C++ style constants, defines are actually better,
// since they let us easily evade scope clashes.

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

// MD5 basic transformation. Transforms state based on block.
void MD5::transform (uint1 block[64]){
	uint4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];
	decode (x, block, 64);
	assert(!finalized);  // not just a user error, since the method is private
	
	/* Round 1 */
	FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
	FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
	FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */
	
	/* Round 2 */
	GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
	GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
	GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
	GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
	GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
	GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */
	
	/* Round 3 */
	HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
	HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */
	
	/* Round 4 */
	II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
	II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
	II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */
	
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	
	// Zeroize sensitive information.
	memset ( (uint1 *) x, 0, sizeof(x));
}

// Encodes input (UINT4) into output (unsigned char). Assumes len is
// a multiple of 4.
void MD5::encode (uint1 *output, uint4 *input, uint4 len) {	
	unsigned int i, j;
	
	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j]   = (uint1)  (input[i] & 0xff);
		output[j+1] = (uint1) ((input[i] >> 8) & 0xff);
		output[j+2] = (uint1) ((input[i] >> 16) & 0xff);
		output[j+3] = (uint1) ((input[i] >> 24) & 0xff);
	}
}

// Decodes input (unsigned char) into output (UINT4). Assumes len is
// a multiple of 4.
void MD5::decode (uint4 *output, uint1 *input, uint4 len){
	unsigned int i, j;
	
	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((uint4)input[j]) | (((uint4)input[j+1]) << 8) |
	(((uint4)input[j+2]) << 16) | (((uint4)input[j+3]) << 24);
}

// Note: Replace "for loop" with standard memcpy if possible.
void MD5::memcpy (uint1 *output, uint1 *input, uint4 len){
	unsigned int i;
	
	for (i = 0; i < len; i++)
		output[i] = input[i];
}

// Note: Replace "for loop" with standard memset if possible.
void MD5::memset (uint1 *output, uint1 value, uint4 len){
	unsigned int i;
	
	for (i = 0; i < len; i++)
		output[i] = value;
}

// ROTATE_LEFT rotates x left n bits.
inline unsigned int MD5::rotate_left  (uint4 x, uint4 n){
	return (x << n) | (x >> (32-n))  ;
}

// F, G, H and I are basic MD5 functions.
inline unsigned int MD5::F            (uint4 x, uint4 y, uint4 z){
	return (x & y) | (~x & z);
}

inline unsigned int MD5::G            (uint4 x, uint4 y, uint4 z){
	return (x & z) | (y & ~z);
}

inline unsigned int MD5::H            (uint4 x, uint4 y, uint4 z){
	return x ^ y ^ z;
}

inline unsigned int MD5::I            (uint4 x, uint4 y, uint4 z){
	return y ^ (x | ~z);
}

// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
// Rotation is separate from addition to prevent recomputation.
inline void MD5::FF(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, 
		    uint4  s, uint4 ac){
	a += F(b, c, d) + x + ac;
	a = rotate_left (a, s) +b;
}

inline void MD5::GG(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, 
		    uint4 s, uint4 ac){
	a += G(b, c, d) + x + ac;
	a = rotate_left (a, s) +b;
}

inline void MD5::HH(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, 
		    uint4 s, uint4 ac){
	a += H(b, c, d) + x + ac;
	a = rotate_left (a, s) +b;
}

inline void MD5::II(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, 
			     uint4 s, uint4 ac){
	a += I(b, c, d) + x + ac;
	a = rotate_left (a, s) +b;
}

////////////////////////////////////////////////////////////////////////////////
/// \ingroup waEncode
/// \fn string md5_encode( const string &source )
/// MD5½âÂë
/// \param source MD5±àÂë×Ö·û´®
/// \return ½âÂë½á¹û
string md5_encode( const string &source ) {
	char *buffer = NULL;
	size_t len = source.length();
	buffer = new char[ len+1 ];
	strncpy( buffer, source.c_str(), len );
	
	MD5 context;
	context.update( (unsigned char*)buffer, len );
	context.finalize();
	
	delete[] buffer; buffer = NULL;
	return context.hex_digest();
}

} // namespace

