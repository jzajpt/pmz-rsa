/*
 * RsaKey.h - RSA klic.
 *
 * Copyright (c) 2006 Jiri Zajpt, <jzajpt@blueberry.cz>
 */

#ifndef _RSA_RSAKEY__H
#define _RSA_RSAKEY__H


#include "../Common/Types.h"
#include "../Common/Exceptions.h"


using std::ifstream;
using std::ofstream;


/**
 * \brief Typ RSA klice.
 */
enum RsaKeyType
{
	RSA_KEY_PRIVATE, // Privatni klic
	RSA_KEY_PUBLIC,  // Verejny klic
	RSA_KEY_INVALID
};


/**
 * \brief Trida reprezentujici Rsa klic.
 *
 * \author Jiri Zajpt.
 */
class RsaKey
{
public:
	/**
	 * \brief Implicitni konstruktor.
	 */
	RsaKey();

	/**
	 * \brief Konstruktor.
	 *
	 * \param keyType Typ klice. 
	 * \param keySize Velikost klice (v bitech).
	 */
	RsaKey( RsaKeyType keyType, size_t keySize, const BigNum& p, const BigNum& q, const BigNum& n, const BigNum& d, const BigNum& e );

	/**
	 * \brief Generuje klic.
	 *
	 * \param bits Velikost modulusu klice.
	 * \param e Verejny exponent.
	 *
	 * \return True pokud se podarilo vygenerovat klic, jinak false.
	 */
	bool GenerateKey( size_t bits, BigNum e );

	/**
	 * \brief Ulozi klic do souboru.
	 */
	bool SaveKeyToFile( const char* filename );

	/**
	 * \brief Nacte klic ze souboru.
	 */
	bool LoadKeyFromFile( const char* filename );

	/**
	 * \brief Zkontroluje zda-li je klic platny.
	 *
	 * \return True pokud je klic platny, jinak false.
	 */
	bool IsValid() const;

	/**
	 * \brief Vraci prvni faktor modulusu.
	 */
	BigNum GetP( void ) const { return mP; };

	/**
	 * \brief Vraci druhy faktor modulusu.
	 */
	BigNum GetQ( void ) const { return mQ; };

	/**
	 * \brief Vraci modulus.
	 */
	BigNum GetN( void ) const { return mN; };

	/**
	 * \brief Vraci verejny exponent.
	 */
	BigNum GetE( void ) const { return mE; };

	/**
	 * \brief Vraci privatni exponent.
	 */
	BigNum GetD( void ) const { return mD; };

	BigNum GetDp( void ) const { return mDp; };
	BigNum GetDq( void ) const { return mDq; };
	BigNum GetQinv( void ) const { return mQinv; };

	/**
	 * \brief Vraci typ klice.
	 *
	 * \return Typ klice.
	 */
	RsaKeyType GetKeyType( void ) const { return mKeyType; };
	
	/**
	 * \brief Nastavi typ klice.
	 *
	 * \param keyType Typ klice.
	 */
	void SetKeyType( RsaKeyType keyType ) { mKeyType = keyType; };

	/**
	 * \brief Vraci velikost modulusu klice v bitech.
	 * 
	 * \return Velikost klice (v bitech).
	 */
	size_t GetKeySize( void ) const { return mKeySize; };

private:
	/**
	 * \brief Typ klice.
	 */
	RsaKeyType mKeyType;

	/**
	 * \brief Velikost klice v bitech.
	 */
	size_t     mKeySize;

	/**
	 * \brief Prvni faktor cisla N.
	 */
	BigNum     mP;

	/**
	 * \brief Druhy faktor cisla N.
	 */
	BigNum     mQ;

	/**
	 * \brief Modulus. Je soucasti verejneho i privatniho klice.
	 */
	BigNum     mN;

	/**
	 * CRT - dP
	 */
	BigNum     mDp;

	/**
	 * CRT - dQ
	 */
	BigNum     mDq;

	/**
	 * CRT - qInv
	 */
	BigNum     mQinv;

	/**
	 * \brief Privatni exponent.
	 */
	BigNum     mD;
	
	/**
	 * \brief Verejny exponent.
	 *
	 * Vetsinou ma hodnotu 3 nebo 65537.
	 */
	BigNum     mE;
};


#endif // _RSA_RSAKEY__H

