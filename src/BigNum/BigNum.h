/*
 * Bignum/Bignum.h - Implementace tridy pro praci s libovolne velkymi cisly.
 *
 * Prakticka maturitni zkouska, 2005 - 2006
 * Implementace sifrovaci algoritmu RSA.
 * 
 * Copyright (c) 2005-2006 Jiri Zajpt, <jzajpt@blueberry.cz>
 */

#ifndef _BIGNUM_BIGNUM__H
#define _BIGNUM_BIGNUM__H


#define WORD_VALUE  65536
#define DWORD_VALUE 0x100000000LL

#define BITS_PER_BYTE   8
#define BITS_PER_WORD  16
#define BITS_PER_DWORD 32

#define is_even( x ) \
        ( ( x & 1 ) == 0 )

#define is_odd( x ) \
        ( ( x & 1 ) == 1 )


#include "../Common/Types.h"
#include "../Common/Exceptions.h"
#include "NumberBuffer.h"


using std::string;


/**
 * \brief Trida BigNum reprezentuje cislo o jakekoliv velikosti.
 *
 * \author Jiri Zajpt
 */
class BigNum
{
public:
	enum Sign { Positive, Negative };
	enum IntegerType { Composite, ProbablyPrime, Prime };
	

	/**
	 * \brief Implicitni konstruktor.
	 *
	 * Cislo ktere bude zinicializovano pomoci tohoto konstruktoru bude mit
	 * hodnotu 0.
	 */
	BigNum();


	/**
	 * \brief Kopirovaci konstruktor s parametrem size.
	 *
	 * Kopiruje cislo rBn do noveho bufferu o velikost size.
	 *
	 * \param rFrom Cislo ktere bude zkopirovano.
	 * \param size  Velikost bufferu pro cislo.
	 */
	BigNum( const BigNum& rFrom, size_t size );


	/**
	 * \brief Konstruktor pro prirazeni hodnoty cisla ze stringu.
	 *
	 * \param rStrValue Hodnota cisla v hexadecimalni reprezentaci.
	 */
	BigNum( const std::string& rStrValue );


	/**
	 * \brief Konstruktor pro prirazeni hodnoty cisla ze 32bitoveho integeru.
	 *
	 * \param intValue Hodnota cisla.
	 */
	BigNum( int intValue );

	
	/**
	 * \brief Destruktor.
	 */
	~BigNum();


	/**
	 * \brief Operator pro prirazeni hodnoty ze stringu.
	 *
	 * \param rStrValue Hodnota cisla v hexadecimalnim tvaru.
	 *
	 * \return Reference na prirazeny objekt.
	 */
	BigNum& operator = ( const std::string& rStrValue );


	/**
	 * \brief Operator pro prirazeni hodnoty ze integeru.
	 *
	 * \param intValue Hodnota cisla.
	 *
	 * \return Reference na prirazeny objekt.
	 */
	BigNum& operator = ( int intValue );

	
	/**
	 * \brief Operator pro bitovy posuv doprava.
	 *
	 * \param bits Pocet bitu.
	 *
	 * \return Posunute cislo.
	 */
	BigNum operator >> ( unsigned int bits ) const;


	/**
	 * \brief Operator pro prirazeni s bitovym posuvem doprava.
	 *
	 * \param bits Pocet bitu.
	 *
	 * \return Reference na objekt nad kterym byl proveden posuv.
	 */
	BigNum& operator >>= ( unsigned int bits );

	/**
	 * \brief Operator pro bitovy posuv doleva.
	 *
	 * \param bits Pocet bitu.
	 *
	 * \return Posunute cislo.
	 */
	BigNum operator << ( unsigned int bits ) const;

	/**
	 * \brief Operator pro prirazeni s bitovym posuvem doleva.
	 *
	 * \param bits Pocet bitu.
	 *
	 * \return Reference na objekt nad kterym byl proveden posuv.
	 */
	BigNum& operator <<= ( unsigned int bits );


	/**
	 * \brief Operator scitani.
	 *
	 * \return Vysledek scitani.
	 */
	BigNum operator + ( const BigNum& rBn ) const;


	/**
	 * \brief Operator pro prirazeni s pricitanim.
	 */
	BigNum& operator += ( const BigNum& rBn );


	/**
	 * \brief Operator odcitani.
	 *
	 * \return Vysledek odcitani.
	 */
	BigNum operator - ( const BigNum& rBn ) const;


	/**
	 * \brief Operator pro prirazeni s odcitanim.
	 */
	BigNum& operator -= ( const BigNum& rBn );


	/**
	 * \brief Operator nasobeni.
	 *
	 * \return Vysledek nasobeni.
	 */
	BigNum operator * ( const BigNum& rBn ) const;


	/**
	 * \brief Operator prirazeni s nasobenim.
	 *
	 * \return Reference na prirazeny objekt.
	 */
	BigNum& operator *= ( const BigNum& rBn );


	/**
	 * \brief Operator deleni.
	 *
	 * \return Vysledek deleni.
	 */
	BigNum operator / ( const BigNum& rBn ) const;


	/**
	 * Operator deleni s prirazenim.
	 *
	 * \return Reference na prirazeny objekt.
	 */
	BigNum& operator /= ( const BigNum& rBn );


	/**
	 * \brief Operator modulo - zbytek po deleni.
	 *
	 * \return Zbytek po deleni.
	 */
	BigNum operator % ( const BigNum& rBn ) const;


	/**
	 * Operator modulo s prirazenim.
	 *
	 * \return Reference na prirazeny objekt.
	 */
	BigNum& operator %= ( const BigNum& rBn );


	/**
	 * \brief Operator umocneni.
	 *
	 * \deprecated
	 */
	BigNum operator ^ ( unsigned int n ) const;


	/**
	 * \brief Operator umocneni.
	 */
	BigNum operator ^ ( const BigNum& rBn ) const;


	/**
	 * \brief Operator zmeny znamenka.
	 */
	BigNum operator - () const;


	/**
	 * \brief Operator zmeny znamenka.
	 */
	BigNum operator + () const;

	
	/**
	 * \brief Operator 'rovna se'.
	 *
	 * \return True pokud se cisla rovnaji, jinak false.
	 */
	bool operator == ( const BigNum& rBn ) const;


	/**
	 * \brief Operator 'nerovna se'.
	 *
	 * \return True pokud se cisla nerovnaji, jinak false.
	 */
	bool operator != ( const BigNum& rBn ) const;


	/**
	 * \brief Operator 'vetsi nez'.
	 */
	bool operator > ( const BigNum& rBn ) const;


	/**
	 * \brief Operator 'vetsi nez nebo rovno'.
	 */
	bool operator >= ( const BigNum& rBn ) const;


	/**
	 * \brief Operator 'mensi nez'.
	 */
	bool operator < ( const BigNum& rBn ) const;
	

	/**
	 * \brief Operator 'mensi nez nebo rovno'.
	 */
	bool operator <= ( const BigNum& rBn ) const;


	/**
	 * \brief Nastavi hodnotu cisla z stringu.
	 *
	 * Pokud bude string obsahovat jakykoliv jiny znak nez
	 * hexadecimalni (0-9, a-f, A-F), funkce vraci false a hodnotu nenastavi.
	 *
	 * \param rStrValue Hodnota cisla v hexadecimalni reprezantaci.
	 *
	 * \return True pokud nastaveni hodnoty probehlo uspesne, jinak false.
	 */
	bool SetValue( const std::string& rStrValue );


	/**
	 * \brief Nastavi hodnotu cisla z integeru.
	 *
	 * \param intValue Hodnota cisla.
	 *
	 * \return True pokud nastaveni hodnoty probehlo uspesne, jinak false.
	 */
	bool SetValue( uint intValue );


	/**
	 * \brief Vraci hodnotu cisla v 32bitovem integeru.
	 *
	 * \return Hodnota cisla.
	 */
	int32_t ToInt32( void ) const;


	/**
	 * \brief Vraci cislo v hexadecimalnim tvaru ve stringu.
	 *
	 * \return Hodnota cisla v sestnackovem tvaru ve stringu.
	 */
	std::string ToDecString( void ) const;


	/**
	 * \brief Vraci cislo v decimalnim tvaru ve stringu.
	 *
	 * \return Hodnota cisla v desitkovem tvaru ve stringu.
	 */
	std::string ToHexString( void ) const;


	/**
	 * \brief Vraci cislo v decimalnim tvaru ve stringu s byty oddelenymi
	 * dvojteckou.
	 *
	 * \return Hodnota cisla v sestnackovem tvaru ve stringu.
	 */	
	std::string ToHexString2( void ) const;


	/**
	 * \brief Pomocna funkce pro debugovani.
	 */
	void DebugPrint( void ) const;


	/**
	 * \brief Testuje zda-li je cislo prvocislem.
	 *
	 * Otestuje zda-li je cislo pravdepodobne prvocislo za pomoci Fermatova
	 * maleho teoremu.
	 * 
	 * \param times Pocet pokusu pro zjisteni prvociselnosti. Vetsi cislo 
	 *  udava vetsi jistotu.
	 *
	 * \return True pokud cislo je pravdepodobne prvocislem, false pokud neni.
	 */
	bool IsProbablyPrime( unsigned int times ) const;


	/**
	 * \brief Testuje prvociselnost pomoci Rabin-Millerova testu.
	 *
	 * \param times Pocet pokusu pro zjisteni prvociselnosti. Vetsi cislo
	 *  udava vetsi jistotu.
	 *
	 * \return True pokud cislo je pravdepodobne prvocislem, false pokud neni.
	 */
	IntegerType RabinMillerPrimalityTest( uint accuracy ) const;


	/**
	 * \brief Testuje zda-li je hodnota cisla nula.
	 *
	 * \return True pokud cislo ma hodnotu nula, jinak false.
	 */
	bool IsZero( void ) const;


	/**
	 * \brief Testuej zda-li je cislo zaporne.
	 *
	 * \return True pokdu je cislo zaporne, jinak false.
	 */
	bool IsNegative( void ) const { return mSign == Negative; };


	/**
	 * \brief Testuje zda-li je cislo kladne.
	 *
	 * \return True pokdu je cislo kladne, jinak false.
	 */
	bool IsPositive( void ) const { return mSign == Positive; };


	/**
	 * \brief Testuje zda-li je cislo sude.
	 *
	 * \return True pokud je cislo sude, jinak false.
	 */
	bool IsEven( void ) const;


	/**
	 * \brief Testuje zda-li je cislo liche.
	 *
	 * \return True pokud je cislo liche, jinak false.
	 */
	bool IsOdd( void ) const;


	/**
	 * \brief Vraci velikost cisla ve wordech.
	 */
	size_t GetSize( void ) const { return mNb.GetSize(); };


	/**
	 * \brief Vraci pocet aktivnich wordu v cisle.
	 */
	size_t GetActiveSize( void ) const { return mNb.GetActiveSize(); };
	size_t GetActiveBytes( void ) const;


	/**
	 * \brief Vraci velikost cisla v bytech.
	 */
	size_t GetByteSize( void ) const;


	/**
	 * \brief Vraci pocet bitu cisla.
	 */
	size_t GetBitCnt( void ) const;


	/**
	 * \brief Vraci velikost cisla v bitech.
	 *
	 * \return Velikost cisla v bitech;
	 */
	size_t GetBitSize( void ) const;


	/**
	 * \brief Vraci dany bajt.
	 *
	 * \param index Index bytu. Pocita se od nuly.
	 *
	 * \return Hodnota bytu.
	 */
	uint8_t GetByte( size_t index ) const;


	/**
	 * \brief Nastavi hodnotu daneho bajtu.
	 *
	 * \param index Index bytu. Pocita se od nuly.
	 * \param value Hodnota.
	 * 
	 * \return Vraci true pokud se podarilo nastavit hodnotu.
	 */
	bool SetByte( size_t index, uint8_t value );


	/**
	 * \brief Vraci pocet bajtu v cisle.
	 */
	size_t GetByteCnt( void ) const;


	/**
	 * \brief Vraci dany double word.
	 */
	uint32_t GetDoubleWord( size_t index ) const;


	/**
	 * \brief Nastavy dany double word.
	 *
	 * \return True pokud nastaveni probehlo uspesne, jinak false.
	 */
	bool SetDoubleWord( size_t index, uint32_t value );


	/**
	 * \brief Vraci hodnotu bitu.
	 *
	 * \param bit Pozadovany bit.
	 *
	 * \return Hodnota bitu.
	 */
	int TestBit( size_t bit ) const;


	/**
	 * \brief Nastavi bit na jednicku.
	 *
	 * \param bit Index bitu.
	 */
	void SetBit( size_t bit );


	/**
	 * \brief Nastavi bitu nulu.
	 *
	 * \param bit Index bitu.
	 */
	void ClearBit( size_t bit );


	/**
	 * \brief Pomocna funkce pro operatory deleni a modulo.
	 *
	 * \param u Delenec.
	 * \param v Delitel.
	 * \param q Podil.
	 * \param r Zbytek po deleni.
	 *
	 * \return
	 */
	static bool Divide( const BigNum& u, const BigNum& v, BigNum& q, BigNum& r );


	/**
	 * \brief Nejvetsi spolecny delitel dvou cisel.
	 *
	 * Funkce spocita a vrati nejvetsi spolecny delitel cisel.
	 *
	 * \param u Prvni cislo.
	 * \param v Druhe cislo.
	 * 
	 * \return Nejvetsi spolecny delitel.
	 */
	static BigNum Gcd( const BigNum& u, const BigNum& v );


	/**
	 * \brief Nejmensi spolecny nasobek dvou cisel.
	 *
	 * Funkce spocita a vrati nejmensi spolecny nasobek dvou cisel.
	 *
	 * \param u Prvni cislo.
	 * \param v Druhe cislo.
	 *
	 * \return Nejmensi spolecny nasobek.
	 */
	static BigNum Lcm( const BigNum& u, const BigNum& v );


	/**
	 * \brief Vypocita hodnotu vztahu ( b ^ e ) mod m.
	 *
	 * \param b Zaklad.
	 * \param e Exponent.
	 * \param m Modulo.
	 */
	static BigNum ModularExponentiation( const BigNum& b, const BigNum& e,
	                                     const BigNum& m );


	/**
	 * Vypocita modularni inverzi.
	 *
	 * Modularni inverze je u^(-1) mod v.
	 */
	static BigNum ModularInverse( const BigNum& u, const BigNum& v );


	/**
	 * \brief Nastavi seed pro generator pseudo-nahodnych cisel.
	 */
	static void SeedRandom( unsigned int seed );
	

	/**
	 * \brief Generuje pseudo-nahodne cislo o dane velikosti.
	 *
	 * \param wordSize Velikost cisla ve wordech.
	 *
	 * \return Vygenerovane pseudo-nahodne cislo.
	 */
	static BigNum GenerateRandom( size_t wordSize );


	/**
	 * \brief Generuje pseudo-nahodne cislo od 0 do daneho cisla rBn.
	 *
	 * \param max Strop pro generovani nahodneho cisla.
	 *
	 * \return Vygenerovane pseudo-nahodne cislo.
	 */
	static BigNum GenerateRandomMax( const BigNum& max );


	/**
	 * \brief Generuje nahodne n-bitove cislo.
	 *
	 * \warning Pokud bude pocet bitu = 1, vygenerovane cislo bude vzdy 1.
	 *
	 * \param bits Pocet bitu cisla.
	 *
	 * \return Vygenerovane pseudo-nahodne cislo.
	 */
	static BigNum GenerateRandomBits( size_t bits );


	/**
	 * \brief Generuje nahodne cislo, ktere bude s urcitou pravdepodobnosti
	 *  prvocislo.
	 *
	 * K testovani zda-li vygenerovane cislo je prvocislo se pouziva 
	 * funce BigNum::IsProbablyPrime(). Parametr topBits se pouziva predevsim
	 * pri generovani RSA klice.
	 *
	 * Pozn.: Funkce vstupuje do nekonecne smycky.
	 *
	 * \param bits Pocet bitu cisla.
	 * \param topBits Pocet hornich bitu, ktere budou 1.
	 *
	 * \return Vygenerovane prvocislo.
	 */
	static BigNum GeneratePrime( uint bits, uint topBits = 0 );


// :TODO:
// Privatni promenne:
public:
	/**
	 * \brief Buffer.
	 */
	NumberBuffer mNb;


	/**
	 * \brief Oznacuje zda-li je cislo kladne nebo zaporne.
	 */
	Sign mSign;
};


#endif // _BIGNUM_BIGNUM__H

