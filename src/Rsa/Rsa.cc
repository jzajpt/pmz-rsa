/*
 * Rsa.cc - RSA sifra.
 *
 * Prakticka maturitni zkouska, 2005 - 2006
 * Implementace sifrovaci algoritmu RSA.
 *
 * Copyright (c) 2006 Jiri Zajpt, <jzajpt@blueberry.cz>
 */

#include <iostream>
#include <fstream>
#include <string>
#include "../BigNum/BigNum.h"
#include "RsaKey.h"
#include "Rsa.h"


BigNum
StringToBigNum( const char* pInputString, size_t inputLength )
{
	BigNum bn( 0, inputLength + 3 / 4 );
	size_t i = inputLength;

	while ( i-- != 0 )
	{
		bn.SetByte( i, pInputString[ i ] );
	}
	
	return bn;
}


char*
BigNumToString( const BigNum& rBn )
{
	size_t i = rBn.GetActiveBytes();
	char* pOutputBuffer = new char[ i + 1 ];

	// Zkontrolujeme zda-li alokace probehla uspesne
	if ( pOutputBuffer == NULL )
		throw Exception( "BigNumToString(): Nepodarilo se alokovat pamet!" );

	while ( i-- != 0 )
	{
		pOutputBuffer[ i ] = rBn.GetByte( i );
	}
	// Ukoncime string
	pOutputBuffer[ rBn.GetActiveBytes() ] = '\0';

	return pOutputBuffer;
} 


Rsa::Rsa( const RsaKey& rKey )
	: mRsaKey( rKey )
{
}


char*
Rsa::EncryptBlock( const char* pInputBuffer, size_t inputSize )
{
	uint keyByteSize = mRsaKey.GetKeySize() / 8; 

	if ( inputSize >= keyByteSize )
		throw Exception( "Rsa::EncryptBlock(): Vstupni data jsou prilis velka" );

	BigNum m = StringToBigNum( pInputBuffer, inputSize );
	
	if ( m >= mRsaKey.GetN() )
		throw Exception( "Rsa::EncryptBlock(): Vstupni data jsou prilis velka" );

	BigNum c = BigNum::ModularExponentiation( m, mRsaKey.GetE(), mRsaKey.GetN() );

	return BigNumToString( c ); 
}


char*
Rsa::DecryptBlock( const char* pInputBuffer, size_t inputSize )
{

	BigNum c = StringToBigNum( pInputBuffer, inputSize );
	BigNum m = BigNum::ModularExponentiation( c, mRsaKey.GetD(), mRsaKey.GetN() ); 
	return BigNumToString( m );
} 


void
Rsa::EncryptFileToFile( const char* pInputFileName, const char* pOutputFileName )
{

	// Pokud mame neplatny klic nebo verejny
	if ( ( mRsaKey.GetKeyType() == RSA_KEY_INVALID ) )
		throw Exception( "Pro sifrovani je nutny soukromy klic!" );

	FILE* fpInput = fopen( pInputFileName, "rb" );
	if ( !fpInput )
		throw UnableToOpenFileException(
			"Rsa::EncryptFileToFile(): Nepodarilo se vytvorit vystupni soubor",
			pInputFileName
		);

	FILE* fpOutput = fopen( pOutputFileName, "wb" );
	if ( !fpOutput )
		throw UnableToOpenFileException(
			"Rsa::EncryptFileToFile(): Nepodarilo se vytvorit vystupni soubor",
			pOutputFileName
		);

	/*
	 * Budeme sifrovat po blocich, ktere budou mit velikost jako
	 * nas klic minus dva znaky (tj. 16 bitu). Jeden byte zustava
	 * nevyuzity, protoze by pak nemusela platit podminka P1 (viz nize)
	 *
	 * Napr. je-li velikost klice 512 bitu, coz je 64 bytu, potom
	 * bude delka vstupniho bloku 63 bytu.
	 */

	uint   keyByteSize    = mRsaKey.GetKeySize() / 8; 
	size_t inblockSize    = keyByteSize - 2;
	size_t outblockSize   = keyByteSize;
	char*  inputBuffer    = new char[ inblockSize + 1 ];;
	char*  encryptedBlock = NULL;
	size_t n;
	float  progress;
	
	if ( !inputBuffer )
		throw Exception(
			"Rsa::DecryptFileToFile(): Nepodarilo se alokovat pamet!"
		);
		
	fseek( fpInput, 0, SEEK_END );
	long inputFileSize = ftell( fpInput );
	fseek( fpInput, 0, SEEK_SET);
	
	uint blocks = ( inputFileSize + keyByteSize - 1 ) / ( keyByteSize - 2 );
	uint i = 0;

	std::cout.precision( 4 );
	std::cout << std::endl;
	
	while ( ( n = fread( inputBuffer + 1, 1, inblockSize, fpInput ) ) != 0 )
	{
		// Na zacatek bufferu vlozime jeho delku
		inputBuffer[ 0 ] = static_cast< uint8_t >( n );

		// Zasifrujeme blok
		encryptedBlock = EncryptBlock( inputBuffer, n + 1 );

		// Zapiseme zasifrovany blok
		fwrite( encryptedBlock, 1, outblockSize, fpOutput );
		
		// Uvolnime alokovanou pamet
		delete[] encryptedBlock;
		
		// Spocitame prubeh v procentech
		progress = i / static_cast< float >( blocks ) * 100.0f;
		
		// Vypiseme prubeh
		std::cout << "Prubeh: " << progress << "% \r";
		
		i++;
	}
	
	std::cout << "Sifrovani souboru probehlo uspesne!" << std::endl;
	delete[] inputBuffer;
	
	fclose( fpInput );
	fclose( fpOutput );
}


void
Rsa::DecryptFileToFile( const char* pInputFileName, const char* pOutputFileName )
{
	// Pokud mame neplatny nebo verejny klic => nemuzeme desifrovat
	if ( ( mRsaKey.GetKeyType() == RSA_KEY_INVALID ) ||
		( mRsaKey.GetKeyType() == RSA_KEY_PUBLIC ) )
		throw Exception( "Neplatny klic pro desifrovani!" );

	FILE* fpInput = fopen( pInputFileName, "rb" );
	if ( !fpInput )
		throw UnableToOpenFileException(
			"Rsa::EncryptFileToFile(): Nepodarilo se vytvorit vystupni soubor",
			pInputFileName
		);

	FILE* fpOutput = fopen( pOutputFileName, "wb" );
	if ( !fpOutput )
		throw UnableToOpenFileException(
			"Rsa::EncryptFileToFile(): Nepodarilo se vytvorit vystupni soubor",
			pOutputFileName
		);

	uint    keyByteSize    = mRsaKey.GetKeySize() / 8; 
	size_t  inblockSize    = keyByteSize;
	size_t  outblockSize   = keyByteSize - 2;
	char*   inputBuffer    = new char[ inblockSize + 1 ];;
	char*   decryptedBlock = NULL;
	size_t  n;
	uint8_t size;
	float   progress;
	
	if ( !inputBuffer )
		throw Exception(
			"Rsa::DecryptFileToFile(): Nepodarilo se alokovat pamet!"
		);

	fseek( fpInput, 0, SEEK_END );
	long inputFileSize = ftell( fpInput );
	fseek( fpInput, 0, SEEK_SET);
	
	uint blocks = ( inputFileSize + keyByteSize - 1 ) / ( keyByteSize - 2 );
	uint i = 0;
	
	std::cout.precision( 4 );
	std::cout << std::endl;

	while ( ( n = fread( inputBuffer, 1, inblockSize, fpInput ) ) != 0 )
	{
		// Desifrujeme jeden blok
		decryptedBlock = DecryptBlock( inputBuffer, n );
		size = decryptedBlock[ 0 ];

		// Zapiseme desifrovany blok do souboru
		fwrite( decryptedBlock + 1, 1, size, fpOutput );
		
		// Uvolnime alokovanou pamet
		delete[] decryptedBlock;
		
		// Spocitame prubeh v procentech
		progress = i / static_cast< float >( blocks ) * 100.0f;
		
		// Vypiseme prubeh
		std::cout << "Prubeh: " << progress << "% \r";
		
		i++;
	}

	std::cout << "Desifrovani souboru probehlo uspesne!" << std::endl;
	delete[] inputBuffer;
	
	fclose( fpInput );
	fclose( fpOutput );
}
