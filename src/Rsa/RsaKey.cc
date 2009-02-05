/*
 * RsaKey.h - RSA klic.
 *
 * Copyright (c) 2006 Jiri Zajpt, <jzajpt@blueberry.cz>
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "../Common/Base64.h"
#include "../BigNum/BigNum.h"
#include "RsaKey.h"


const char cPublicKeyId[]  = "-- PMZ_RSA PUBLIC KEY --";
const char cPrivateKeyId[] = "-- PMZ_RSA PRIVATE KEY --";
const int  cLineLength = 48;


RsaKey::RsaKey()
	: mKeyType( RSA_KEY_INVALID ), mKeySize( 0 )
{
}


RsaKey::RsaKey(
	RsaKeyType keyType, size_t keySize,
	const BigNum& p, const BigNum& q, const BigNum& n,
	const BigNum& d, const BigNum& e
)
	: mKeyType( keyType ), mKeySize( keySize ),
	mP( p ), mQ( q ), mN( n ),
	mDp( 0 ), mDq( 0 ), mQinv( 0 ), mD( d ), mE( e )
{
}


bool
RsaKey::GenerateKey( size_t bits, BigNum e )
{
	size_t bitsP = ( bits + 1 ) / 2;
	size_t bitsQ = bits - bitsP;

	while ( true )
	{
		mP = BigNum::GeneratePrime( bitsP, 2 );
		std::cout << ".";
		flush( std::cout );
		if ( BigNum::Gcd( mP, e ) == 1 )
			break;
	}

	while ( true )
	{
		mQ = BigNum::GeneratePrime( bitsQ, 2 );
		std::cout << ".";
		flush( std::cout );
		if ( BigNum::Gcd( mQ, e ) == 1 )
			break;
	}

	/*
	 * n = p * q
	 */
	mN = mP * mQ;

	/*
	 * e
	 */
	mE = e;

	/* 
	 * lambda = ( p - 1 )( q - 1 )
	 */
	BigNum lambda = BigNum::Lcm( ( mP - 1 ), ( mQ - 1 ) );

	/*
	 * d
	 */
	mD = BigNum::ModularInverse( mE, lambda );

	mDp = mD % ( mP - 1 ); 

	mDq = mD % ( mQ - 1 ); 

	mQinv = BigNum::ModularInverse( mQ, mP );

	mKeyType = RSA_KEY_PRIVATE;
	mKeySize = bits;
	
	std::cout << std::endl;

	return true;
}


bool
RsaKey::SaveKeyToFile( const char* pFilename )
{
	if ( mKeyType == RSA_KEY_INVALID )
		return false;

	ofstream ofs( pFilename );
	if ( !ofs.is_open() )
		throw UnableToOpenFileException( "Nepodarilo se vytvorit soubor", pFilename );

	if ( mKeyType == RSA_KEY_PUBLIC )
	{
		ofs.write( cPublicKeyId, strlen( cPublicKeyId ) );
		ofs.put( '\n' );

		std::string       buffer;
		std::string       temp;
		std::stringstream ss;

		ss << std::hex << mKeySize;
		ss >> temp;
		buffer += temp;
		buffer += "\n";

		temp = mN.ToHexString();
		buffer += temp;
		buffer += "\n";		  

		temp = mE.ToHexString();
		buffer += temp;
		buffer += "\n";
		
		char*  encoded = Base64EncodeString( buffer.c_str(), buffer.length() );
		size_t encodedLen = strlen( encoded );
		int i;
		int times = encodedLen / cLineLength;

		for ( i = 0 ; i < times  ; i++ )
		{
			ofs.write( encoded + ( i * cLineLength ), cLineLength );
			ofs.put( '\n' );
		}
		ofs.write( encoded + ( i * cLineLength ), encodedLen % cLineLength );
		delete[] encoded;
	}
	else if ( mKeyType == RSA_KEY_PRIVATE )
	{
		ofs.write( cPrivateKeyId, strlen( cPrivateKeyId ) );
		ofs.put( '\n' );

		std::string buffer;
		std::string temp;
		std::stringstream ss;
		ss << std::hex << mKeySize;
		ss >> temp;

		buffer += temp;
		buffer += "\n";

		temp = mP.ToHexString();

		buffer += temp;
		buffer += "\n";

		temp = mQ.ToHexString();
		buffer += temp;
		buffer += "\n";

		temp = mDp.ToHexString();
		buffer += temp;
		buffer += "\n";

		temp = mDq.ToHexString();
		buffer += temp;
		buffer += "\n";

		temp = mQinv.ToHexString();
		buffer += temp;
		buffer += "\n";

		temp = mN.ToHexString();
		buffer += temp;
		buffer += "\n";

		temp = mE.ToHexString();
		buffer += temp;
		buffer += "\n";

		temp = mD.ToHexString();
		buffer += temp;
		buffer += "\n";

		char*  encoded = Base64EncodeString( buffer.c_str(), buffer.length() );
		size_t encodedLen = strlen( encoded );
		int i;
		int times = encodedLen / cLineLength;
		char c;

		for ( i = 0 ; i < times  ; i++ )
		{
			ofs.write( encoded + ( i * cLineLength ), cLineLength );
			ofs.put( '\n' );
		}
		ofs.write( encoded + ( i * cLineLength ), encodedLen % cLineLength );
		delete[] encoded;
	}

	return true;
}


bool
RsaKey::LoadKeyFromFile( const char* pFilename )
{
	ifstream ifs( pFilename );
	if ( !ifs.is_open() )
		throw UnableToOpenFileException( "Nepodarilo se otevrit soubor", pFilename );

	char idBuffer[ 64 ];
	ifs.getline( idBuffer, 64 );

	if ( strncmp( idBuffer, cPublicKeyId, strlen( cPublicKeyId ) ) == 0 )
	{
		mKeyType = RSA_KEY_PUBLIC;

		char         tmp[ 1024 ];
		std::string  inbuffer;
		std::string  decodedBuffer;

		while ( ifs.good() )
		{
			ifs.getline( tmp, 1024 );
			inbuffer += tmp;
		}

		decodedBuffer = Base64DecodeString( inbuffer.c_str(), inbuffer.length() );
		std::stringstream ss( decodedBuffer );
		
		// Nacteme delku klice
		ss.getline( tmp, 1024 ); 
		mKeySize = ( int ) strtol( tmp, ( char** ) NULL, 16 );

		ss.getline( tmp, 1024 );
		mN = BigNum( tmp );

		ss.getline( tmp, 1024 );
		mE = BigNum( tmp );

		return true;
	}
	else if ( strncmp( idBuffer, cPrivateKeyId, strlen( cPrivateKeyId ) ) == 0 )
	{
		mKeyType = RSA_KEY_PRIVATE;

		char         tmp[ 1024 ];
		std::string  inbuffer;
		std::string  decodedBuffer;
		
		while ( ifs.good() )
		{
			ifs.getline( tmp, 1024 );
			inbuffer += tmp;
		}

		decodedBuffer = Base64DecodeString( inbuffer.c_str(), inbuffer.length() );
		std::stringstream ss( decodedBuffer );
		
		// Nacteme delku klice
		ss.getline( tmp, 1024 ); 
		mKeySize = ( int ) strtol( tmp, ( char** ) NULL, 16 );

		ss.getline( tmp, 1024 );
		mP = BigNum( tmp );
		
		ss.getline( tmp, 1024 );
		mQ = BigNum( tmp );

		ss.getline( tmp, 1024 );
		mDp = BigNum( tmp );

		ss.getline( tmp, 1024 ); 
		mDq = BigNum( tmp );

		ss.getline( tmp, 1024 );
		mQinv = BigNum( tmp );

		ss.getline( tmp, 1024 );
		mN = BigNum( tmp );

		ss.getline( tmp, 1024 );
		mE = BigNum( tmp );

		ss.getline( tmp, 1024 );
		mD = BigNum( tmp );

		return true;
	}
	else
	{
		std::cerr << "Neplatny soubor!" << std::endl;
		return false;
	}
}


bool
RsaKey::IsValid() const
{
	if ( mKeyType == RSA_KEY_PRIVATE )
	{
		if ( !mP.IsProbablyPrime( 30 ) )
			return false;

		if ( !mQ.IsProbablyPrime( 30 ) )
			return false;

		if ( mN != ( mP * mQ ) )
			return false;

		if ( BigNum::Gcd( mP, mE ) != 1 )
			return false;
			
		if ( BigNum::Gcd( mQ, mE ) != 1 )
			return false;
	}

	return true;
}

