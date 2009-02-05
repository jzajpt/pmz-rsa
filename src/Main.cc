/*
 * Main.cc
 *
 * Prakticka maturitni zkouska, 2005 - 2006
 * Implementace sifrovaci algoritmu RSA.
 * 
 * Copyright (c) 2005-2006 Jiri Zajpt, <jzajpt@blueberry.cz>
 */


#include <iostream>
#ifdef __WIN__
#include <windows.h>
#endif # __WIN__
#include "BigNum/BigNum.h"
#include "Rsa/RsaKey.h"
#include "Rsa/Rsa.h"


using namespace std;


const int port = 8666;


void
PrintHelp( char* pProgramName )
{
	cout << "Implementace sifrovaciho algoritmu RSA" << endl;
	cout << "Prakticka maturitni zkouska 2005 - 2006" << endl;
	cout << "Copyright (c) 2005,2006 Jiri Zajpt" << endl << endl;
	cout << "Pouziti: " << endl;
	cout << "\t" << pProgramName << " genkey <vystupni-soubor> <delka-klice>" << endl;
	cout << "\t\t" << "Vygeneruje RSA klic o dane bitove delce a ulozi jej do souboru." << endl << endl;
	cout << "\t" << pProgramName << " publickey <privatni-klic> <verejny-klic>" << endl;
	cout << "\t\t" << "Nacte RSA klic ze souboru privatni klic a vezme z nej " << endl << "\t\tverejne parametry a ulozi je do verejneho klice." << endl << endl;
	cout << "\t" << pProgramName << " checkkey <soubor-s-klicem>" << endl;
	cout << "\t\t" << "Zkontroluje platnost RSA klice." << endl << endl;
	cout << "\t" << pProgramName << " encrypt <soubor-s-klicem> <vstup> <vystup>" << endl;
	cout << "\t\t" << "Zasifruje soubor." << endl << endl;
	cout << "\t" << pProgramName << " decrypt <soubor-s-klicem> <vstup> <vystup>" << endl;
	cout << "\t\t" << "Desifruje soubor." << endl;
}


void
term_func()
{
	if ( gpLastException )
		cout << "Nastal vyjimka: " << gpLastException->mMessage << endl;
		
	exit( 1 );
}


int
main( int argc, char** argv )
{
	if ( argc < 2 )
	{
		PrintHelp( argv[ 0 ] );
		return 1;
	}
	
	char* pAction = argv[ 1 ];
	
	set_terminate( term_func );

	if ( strncmp( pAction, "genkey", 6 ) == 0 )
	{
		if ( argc != 4 )
		{
			PrintHelp( argv[ 0 ] );
			return 1;
		}

		RsaKey newKey;
		size_t bits;
		char*  pKeyFile = argv[ 2 ];

		bits = atoi( argv[ 3 ] );

		if ( bits < 128 || bits > 2048 )
			cerr << "Neplatna delka klice (musi byt 128 az 2048 bitu). " << endl;

		cout << "Generuji klic o velikosti " << bits << " bitu..." << endl;

		// Seedujeme generator nahodnych cisel
		BigNum::SeedRandom( ( unsigned int ) time( NULL ) );

		// Vygenerujeme klic
		newKey.GenerateKey( bits, 0x10001 );

		cout << "Hotovo." << endl;

		// Ulozime klic do souboru
		if ( !newKey.SaveKeyToFile( pKeyFile ) )
		{
			cerr << "Nepodarilo se ulozit klic do souboru: " << pKeyFile << "!" << endl;
			return 1;
		}
		
		return 0;
	}
	else if ( strncmp( pAction, "publickey", 8 ) == 0 )
	{
		if ( argc != 4 )
		{
			PrintHelp( argv[ 0 ] );
			return 1;
		}
		
		char*  pKeyFile       = argv[ 2 ];
		char*  pPublicKeyFile = argv[ 3 ];
		RsaKey key;
		
		// Pokusime se nacist klic ze souboru
		if ( !key.LoadKeyFromFile( pKeyFile ) )
		{
			cerr << "Nepodarilo se nacist klic ze souboru: " << "!" << pKeyFile << endl;
			return 1;
		}
		
		key.SetKeyType( RSA_KEY_PUBLIC );

		// Ulozime verejny klic do souboru
		if ( !key.SaveKeyToFile( pPublicKeyFile ) )
		{
			cerr << "Nepodarilo se ulozit klic do souboru: " << pKeyFile << "!" << endl;
			return 1;
		}
		
		cout << "Verejny klic byl uspesne ulozen do souboru: " << pPublicKeyFile << "." << endl;
		
		return 0;
	}
	else if ( strncmp( pAction, "checkkey", 8 ) == 0 )
	{
		if ( argc != 3 )
		{
			PrintHelp( argv[ 0 ] );
			return 1;
		}

		char*  pKeyFile = argv[ 2 ];
		RsaKey key;

		// Pokusime se nacist klic ze souboru
		if ( !key.LoadKeyFromFile( pKeyFile ) )
		{
			cerr << "Nepodarilo se nacist klic ze souboru: " << "!" << pKeyFile << endl;
			return 1;
		}

		// Zkontrolujeme zda-li je nacteny klic platny.
		if ( !key.IsValid() )
		{
			cout << "Klic je neplatny!" << endl;
			return 1;
		}
		else
			cout << "Klic je platny!" << endl;

		return 0;
	}
	else if ( strncmp( pAction, "encrypt", 7 ) == 0 )
	{
		if ( argc != 5 )
		{
			PrintHelp( argv[ 0 ] );
			return 1;
		}

		char*  pKeyFile = argv[ 2 ];
		char*  pInputFile = argv[ 3 ];
		char*  pOutputFile = argv[ 4 ];
		RsaKey key;

		// Pokusime se nacist klic ze souboru
		if ( !key.LoadKeyFromFile( pKeyFile ) )
		{
			cerr << "Nepodarilo se nacist klic ze souboru: " << pKeyFile << "!" << endl;
			return 1;
		}
		
		Rsa rsa( key );
	
		cout << "Sifruji soubor " << pInputFile << " klicem " << pKeyFile << endl;
		
		rsa.EncryptFileToFile( pInputFile, pOutputFile );

		return 0;
	}
	else if ( strncmp( pAction, "decrypt", 7 ) == 0 )
	{
		if ( argc != 5 )
		{
			PrintHelp( argv[ 0 ] );
			return 1;
		}

		char*  pKeyFile = argv[ 2 ];
		char*  pInputFile = argv[ 3 ];
		char*  pOutputFile = argv[ 4 ];
		RsaKey key;

		if ( !key.LoadKeyFromFile( pKeyFile ) )
		{
			cerr << "Nepodarilo se nacist klic ze souboru: " << pKeyFile << "!" << endl;
			return 1;
		}

		Rsa rsa( key );

		cout << "Desifruji soubor " << pInputFile << " klicem " << pKeyFile << endl;

		rsa.DecryptFileToFile( pInputFile, pOutputFile );

		return 0;
	}
	else
	{
		PrintHelp( argv[ 0 ] );
	}

	return 0;
}
