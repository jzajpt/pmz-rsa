/*
 * Rsa.h - RSA sifra.
 *
 * Prakticka maturitni zkouska, 2005 - 2006
 * Implementace sifrovaci algoritmu RSA.
 *
 * Copyright (c) 2006 Jiri Zajpt, <jzajpt@blueberry.cz>
 */

#ifndef _RSA_RSA__H
#define _RSA_RSA__H


#include "../Common/Types.h"
#include "../Common/Exceptions.h"


using std::string;
using std::ifstream;
using std::ofstream;


/**
 * \brief Prevede string na BigNum
 */
BigNum StringToBigNum( const char* pInputString, size_t inputLength );
char* BigNumToString( const BigNum& rBn, char* pOutputBuffer, size_t outputSize );


/**
 * Trida slouzi pro sifrovani pomoci RSA.
 *
 * \author Jiri Zajpt
 */
class Rsa
{
public:
	/**
	 * \brief Implicitni konstruktor.
	 */
	Rsa() {};

	Rsa( const RsaKey& rKey );

	/**
	 * \brief Vraci RSA klic.
	 */
	RsaKey GetKey( void ) const { return mRsaKey; };

	/**
	 * \brief Nastavi RSA klic.
	 */
	void SetKey( const RsaKey& key ) { mRsaKey = key; };
	
	char* EncryptBlock( const char* pInputBuffer, size_t inputSize );

	char* DecryptBlock( const char* pInputBuffer, size_t inputSize );


	/**
	 * \brief Zasifruje soubor pomoci aktualniho RSA klice.
	 *
	 * \param pFileName Vstupni soubor pro zasifrovani.
	 * \param pOut      Vystupni soubor.
	 *
	 */
	void EncryptFileToFile( const char* pFileName, const char* pOut );

	/**
	 * \brief Desifruje soubor pomoci aktualniho RSA klice.
	 *
	 * \param pFileName Vstupni soubor pro desifrovani.
	 * \param pOut      Vystupni soubor.
	 *
	 */
	void DecryptFileToFile( const char* pFileName, const char* pOut );
	

private:
	RsaKey mRsaKey;
};


#endif // _RSA_RSA__H

