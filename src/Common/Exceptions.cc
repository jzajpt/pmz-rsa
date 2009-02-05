/*
 * Common/Exception.cc
 *
 * Prakticka maturitni zkouska, 2005 - 2006
 * Implementace sifrovaci algoritmu RSA.
 * 
 * Copyright (c) 2006 Jiri Zajpt, <jzajpt@blueberry.cz>
*/

#include "Exceptions.h"

Exception* gpLastException = NULL;


Exception::Exception()
{
	gpLastException = this;
}


Exception::Exception( std::string message )
{
	mMessage = message;
	gpLastException = this;
}


UnableToOpenFileException::UnableToOpenFileException(
	std::string message, std::string fileName
)
{
	gpLastException = this;
	mMessage = message;
	mFileName = fileName;
}


UnableToOpenFileException::UnableToOpenFileException( std::string fileName )
{
	gpLastException = this;
	mFileName = fileName;
}
