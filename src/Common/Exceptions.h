/*
 * Common/Exception.h - Definice trid vyjimek.
 *
 * Prakticka maturitni zkouska, 2005 - 2006
 * Implementace sifrovaci algoritmu RSA.
 * 
 * Copyright (c) 2006 Jiri Zajpt, <jzajpt@blueberry.cz>
*/

#ifndef _BIGNUM_EXCEPTIONS__H
#define _BIGNUM_EXCEPTIONS__H


#include <string>


class Exception
{
public:
	Exception();
	Exception( std::string messsage );

	std::string mMessage;	
};


class AllocationFailedException
{
public:
	AllocationFailedException() {};
};


class DivisionByZeroException
{
public:
	DivisionByZeroException() {};
};


class NotImplementedException
{
public:
	NotImplementedException() {};
};


class UnableToOpenFileException : public Exception
{
public:
	UnableToOpenFileException( std::string message, std::string fileName );	
	UnableToOpenFileException( std::string fileName );

	std::string mFileName;
};


class ArithmeticException
{
public:
	ArithmeticException() {};
};


class InvalidKeyException
{
public:
	InvalidKeyException() {};
};


extern Exception* gpLastException;


#endif // _BIGNUM_EXCEPTIONS__H
