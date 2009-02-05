/*
 * BigNum/BigNum.cc - Implementace tridy pro praci s libovolne velkymi cisly.
 *
 * Prakticka maturitni zkouska, 2005 - 2006
 * Implementace sifrovaci algoritmu RSA.
 * 
 * Copyright (c) 2005-2006 Jiri Zajpt, <jzajpt@blueberry.cz>
 */

#include <iostream>
#include <vector>
#include <limits.h>
#include "NumberBuffer.h"
#include "BigNum.h"


/**
 * \brief Prevede hexadecimalni znak na cislo.
 *
 * \param c Znak (0-9, A-F, a-f).
 *
 * \return Hodnota znaku (0-15).
 */
uint8_t
HexCharToInt( char c )
{
	if ( c >= '0' && c <= '9' )
		return c - '0';
	else if ( c >= 'a' && c <= 'f' )
		return ( c - 'a' ) + 10;
	else if ( c >= 'A' && c <= 'F' )
		return ( c - 'A' ) + 10;
	else
		return 0;
}


/**
 * \brief Prevede cislo (0-15) na hexadecimalni znak.
 *
 * \param i Cislo (0-15).
 *
 * \return Znak (0-9, a-f)
 */
static char
IntToHexChar( uint8_t i )
{
	return ( i < 10 )
		? '0' + i
		: 'a' + ( i  - 10 );
}


BigNum::BigNum()
	: mSign( Positive )
{
}


BigNum::BigNum( const BigNum& rFrom, size_t size )
	: mNb( rFrom.mNb, size ),
	mSign( rFrom.mSign )
{
}


BigNum::BigNum( const std::string& rStrValue )
{
	SetValue( rStrValue );
}


BigNum::BigNum( int intValue )
{
	SetValue( intValue );
}


BigNum::~BigNum()
{
	mNb.Free();
}


BigNum&
BigNum::operator = ( const std::string& rStrValue )
{
	SetValue( rStrValue );
	
	return *this;
}


BigNum&
BigNum::operator = ( int intValue )
{
	SetValue( intValue );

	return *this;
}


BigNum
BigNum::operator >> ( unsigned int bits ) const
{
	BigNum bn;

	// Pokud posouvame cislo o vice bitu nez je jeho
	// celkovy pocet bitu je vysledek vzdy nula.
	if ( bits >= ( mNb.GetSize() * 32 ) )
		return bn;

	// Pokud posouvame cislo o nula bitu je vysledek
	// vzdy puvodni cislo.
	if ( bits == 0 )
		return *this;

	// Pokud je cislo, ktere chceme posunout nula, je
	// vysledek vzdy nula.
	if ( IsZero() )
		return *this;

	try
	{
		bn.mNb.Allocate( mNb.GetSize() );
	}
	catch ( AllocationFailedException& e )
	{
		throw e;
	}

	//
	// Protoze nemuzeme posouvat vice jak o 32 bitu musime 
	// nejdrive provest posunuti o (bits % 32) bitu. Pokud bylo
	// bits mene nebo rovno 32, vracime cislo. Pokud ale bitu
	// bylo vice nez 32, musime jeste provest posunuti o 
	// ( bits / 32 ) * 32. To provedeme v cyklu, vzdy po 32 bitech.
	//

	unsigned int part_bits = bits % BITS_PER_DWORD;
	uint32_t     mask      = 0;
	uint32_t     carry0    = 0;
	uint32_t     carry     = 0;
	size_t       rem_bits  = BITS_PER_DWORD - part_bits;
	size_t       i;

	// Vytvorime masku pomoci ktere ziskame bity urcene pro
	// prenos do dalsiho bajtu.
	for ( i = 0 ; i < part_bits ; i++ )
		mask |= 1 << i;

	// Provedeme posuv o (bits % 32). bitu.
	i = mNb.GetSize();
	while ( i-- )
	{
		carry0 = ( mNb.Get( i ) & mask ) << rem_bits;
		bn.mNb.Set( i, ( mNb.Get( i ) >> part_bits ) | carry );
		carry = carry0;
	}

	// Zjistime zda-li je potreba jeste provest posunuti
	unsigned int times = bits / BITS_PER_DWORD;
	if ( times == 0 )
		return bn;

	size_t j = 0;
	carry0   = 0;
	mask     = 0xFF;

	// Provedeme posunuti (bits / 32) krat.
	for ( i = 0 ; i < times ; i++ )
	{
		j = bn.mNb.GetSize();
		carry   = 0;
		while ( j-- )
		{
			carry0 = bn.mNb.Get( j );
			bn.mNb.Set( j,
				( static_cast< uint64_t >( bn.mNb.Get( j ) ) >> BITS_PER_DWORD ) | carry
			);
			carry = carry0;
		}
	}

	return bn;
}


BigNum&
BigNum::operator >>= ( unsigned int bits )
{
	*this = *this >> bits;
	return *this;
}


BigNum
BigNum::operator << ( unsigned int bits ) const
{
	BigNum bn;

	// Pokud posouvame cislo o vice bitu nez je jeho
	// celkovy pocet bitu je vysledek vzdy nula.
	//
	// :FIXME:
	if ( bits >= ( mNb.GetSize() * 32 ) )
		return bn;

	// Pokud posouvame cislo o nula bitu je vysledek
	// vzdy puvodni cislo.
	if ( bits == 0 )
		return *this;

	// Pokud je cislo, ktere chceme posunout nula, je
	// vysledek vzdy nula.
	if ( IsZero() )
		return *this;

	try
	{
		bn.mNb.Allocate( mNb.GetSize() );
	}
	catch ( AllocationFailedException& e )
	{
		throw e;
	}

	//
	// Protoze nemuzeme posouvat vice jak o 32 bitu musime 
	// nejdrive provest posunuti o (bits % 32) bitu. Pokud bylo
	// bits mene nebo rovno 32, vracime cislo. Pokud ale bitu
	// bylo vice nez 32, musime jeste provest posunuti o 
	// ( bits / 32 ) * 32. To provedeme v cyklu, vzdy po 32 bitech.
	//

	unsigned int part_bits = bits % BITS_PER_DWORD;
	uint32_t     mask   = 0;
	uint32_t     carry0 = 0;
	uint32_t     carry  = 0;
	size_t       rem_bits = BITS_PER_DWORD - part_bits;
	size_t       i;

	// Vytvorime masku pomoci ktere ziskame bity urcene pro
	// prenos do dalsiho bajtu.
	for ( i = 0 ; i < rem_bits ; i++ )
		mask |= 1 << i;
	mask = ~mask;

	// Provedeme posuv o (bits % 32) bitu.
	i = mNb.GetSize();
	for ( i = 0 ; i < bn.mNb.GetSize() ; i++ )
	{
		carry0 = ( mNb.Get( i ) & mask ) >> rem_bits;
		bn.mNb.Set( i, ( mNb.Get( i ) << part_bits ) | carry );
		carry = carry0;
	}

	// Zjistime zda-li je potreba jeste provest posunuti
	unsigned int times = bits / BITS_PER_DWORD;
	if ( times == 0 )
		return bn;

	size_t j = 0;
	carry0   = 0;
	mask     = 0xFF;

	// Provedeme posunuti (bits / 32) krat.
	for ( i = 0 ; i < times ; i++ )
	{
		carry   = 0;
		for ( j = 0 ; j < bn.mNb.GetSize() ; j++ )
		{
			carry0 = bn.mNb.Get( j );
			bn.mNb.Set( j,
				( static_cast< uint64_t >( bn.mNb.Get( j ) ) << BITS_PER_DWORD ) | carry
			);
			carry = carry0;
		}
	}

	return bn;
}


BigNum&
BigNum::operator <<= ( unsigned int bits )
{
	*this = *this << bits;
	return *this;
}


BigNum
BigNum::operator + ( const BigNum& rBn ) const
{
	// Pokud je jedno z cisel nula vracime to druhe
	if ( rBn.IsZero() )
		return *this;
	if ( IsZero() )
		return rBn;

	// Pokud k kladnemu cislu pricitame zaporne cislo
	if ( mSign == Positive && rBn.mSign == Negative )
	{
		BigNum *pBn = const_cast< BigNum* > ( &rBn );
		pBn->mSign = Positive;
		BigNum result = *this - *pBn;
		pBn->mSign = Negative;

		return result;
	}
	// Pokud k zapornemu cislu pricitame kladne cislo
	else if ( mSign == Negative && rBn.mSign == Positive )
	{
		BigNum *pThis = const_cast< BigNum* > ( this );
		pThis->mSign = Positive;
		BigNum result = rBn - *this;
		pThis->mSign = Negative;

		return result;
	}
	// Pokud k zapornemu cislu pricitame zaporne cislo
	else if ( mSign == Negative && rBn.mSign == Negative )
	{
		BigNum *pThis = const_cast< BigNum* > ( this );
		BigNum *pBn = const_cast< BigNum* > ( &rBn );

		pThis->mSign = Positive;
		pBn->mSign = Positive;

		BigNum result = ( *pThis ) + ( *pBn );

		pThis->mSign = Negative;
		pBn->mSign = Negative;
		result.mSign = Negative;

		return result;
	}

	// Zjistime pocet cislic u obou cisel
	size_t active_dwords1 = mNb.GetActiveSize();
	size_t active_dwords2 = rBn.mNb.GetActiveSize();
	size_t result_size;

	active_dwords1 > active_dwords2
		? result_size = active_dwords1 + 1
		: result_size = active_dwords2 + 1;

	BigNum result;

	try
	{
		result.mNb.Allocate( result_size );
	}
	catch ( AllocationFailedException& e )
	{
		throw e;
	}

	// V tomhle cyklu secteme obe cisla...
	uint64_t tmp;
	bool     carry = false;

	for ( size_t i = 0 ; i < result.mNb.GetSize() ; i++ )
	{
		tmp = static_cast< uint64_t >( mNb.Get( i ) )
		    + static_cast< uint64_t >( rBn.mNb.Get( i ) );

		// Zkontrolujeme zda-li nedoslo k prenosu
		if ( carry )
		{
			tmp++;
			carry = false;
		}

		// Zkontrolujeme zda-li nedoslo k preteceni
		if ( tmp >= DWORD_VALUE )
		{
			tmp &= 0xFFFFFFFF;
			carry = true;
		}

		result.mNb.Set( i, tmp );
	}

	return result;
}


BigNum&
BigNum::operator += ( const BigNum& rBn )
{
	*this = *this + rBn;
	return *this;
}


BigNum
BigNum::operator - ( const BigNum& rBn ) const
{
	// Pokud odecitame nuly
	if ( rBn.IsZero() )
		return *this;
	// Pokud odecitame od nuly
	if ( IsZero() )
		return -rBn;

	// Pokud od kladneho cisla odecitame zaporne cislo
	if ( mSign == Positive && rBn.mSign == Negative )
	{
		BigNum *pBn = const_cast< BigNum* > ( &rBn );
		pBn->mSign = Positive;
		BigNum result = *this + *pBn;
		pBn->mSign = Negative;
		return result;
	}
	// Pokud od zaporneho cisla odecitame kladne cislo
	else if ( mSign == Negative && rBn.mSign == Positive )
	{

		BigNum *pThis = const_cast< BigNum* > ( this );
		BigNum *pBn = const_cast< BigNum* > ( &rBn );

		pThis->mSign = Positive;
		pBn->mSign = Positive;

		BigNum result = ( *pThis ) + ( *pBn );

		pThis->mSign = Negative;
		pBn->mSign = Negative;
		result.mSign = Negative;

		return result;
	}
	// Pokud od zaporneho cisla odecitame zaporne cislo
	else if ( mSign == Negative && rBn.mSign == Negative )
	{
		BigNum *pThis = const_cast< BigNum* > ( this );
		BigNum *pBn = const_cast< BigNum* > ( &rBn );

		pThis->mSign = Positive;
		pBn->mSign = Positive;

		BigNum result = ( *pBn ) - ( *pThis );

		pThis->mSign = Positive;
		pBn->mSign = Negative;

		return result;
	}

	// Pokud by mel byt vysledek zaporny
	if ( rBn > *this )
	{
		BigNum *pBn = const_cast< BigNum* > ( &rBn );
		pBn->mSign = Positive;
		BigNum result = ( *pBn ) - ( *this );
		result.mSign = Negative;
		return result;
	}

	size_t activeDoubleWords1 = mNb.GetActiveSize();
	size_t activeDoubleWords2 = rBn.mNb.GetActiveSize();
	size_t result_size;

	activeDoubleWords1 > activeDoubleWords2
		? result_size = activeDoubleWords1
		: result_size = activeDoubleWords2;

	BigNum result;
	try
	{
		result.mNb.Allocate( result_size );
	}
	catch ( AllocationFailedException& e )
	{
		throw e;
	}

	int64_t tmp;
	bool    carry = false;
	for ( size_t i = 0 ; i < result.mNb.GetSize() ; i++ )
	{
		tmp = static_cast< uint64_t >( mNb.Get( i ) )
		    - static_cast< uint64_t >( rBn.mNb.Get( i ) );

		if ( carry )
		{
			tmp--;
			carry = false;
		}

		if ( tmp < 0 )
		{
			tmp += DWORD_VALUE;
			carry = true;
		}

		result.mNb.Set( i, tmp );
	}
	
	return result;
}


BigNum&
BigNum::operator -= ( const BigNum& rBn )
{
	*this = *this - rBn;
	return *this;
}


BigNum
BigNum::operator * ( const BigNum& rBn ) const
{
	BigNum result( 0 );

	// Pokud je jedno z cisel nula, vysledek je nula.
	if ( IsZero() || rBn.IsZero() )
		return result;
	
	uint64_t tmp;
	uint32_t carry;
	size_t   i;
	size_t   j;

	for ( i = 0 ; i < rBn.mNb.GetActiveSize() ; i++ )
	{
		BigNum subresult;
		try
		{
			subresult.mNb.Allocate( mNb.GetActiveSize() + i + 1 );
		}
		catch ( AllocationFailedException& e )
		{
			throw e;
		}

		carry = 0;

		for ( j = 0 ; j < mNb.GetActiveSize() ; j++ )
		{
			tmp = static_cast< uint64_t >( mNb.Get( j ) )
			    * static_cast< uint64_t >( rBn.mNb.Get( i ) )
				+ carry;
			subresult.mNb.Set( i + j, tmp % DWORD_VALUE );
			carry = tmp / DWORD_VALUE;
		}

		if ( carry > 0 )
		{
			subresult.mNb.Set( j + i, carry );
		}

		result += subresult;
	}

	if ( mSign == Negative && rBn.mSign == Positive )
		result.mSign = Negative;
	if ( mSign == Positive && rBn.mSign == Negative )
		result.mSign = Negative;

	return result;
}


BigNum&
BigNum::operator *= ( const BigNum& rBn )
{
	*this = *this * rBn;
	return *this;
}


BigNum
BigNum::operator / ( const BigNum& rBn ) const
{
	BigNum q;
	BigNum r;

	Divide( *this, rBn, q, r );
	return q;
}


BigNum&
BigNum::operator /= ( const BigNum& rBn )
{
	BigNum q;
	BigNum r;

	Divide( *this, rBn, q, r );
	*this = q;
	return *this;
}


BigNum
BigNum::operator % ( const BigNum& rBn ) const
{
	BigNum q;
	BigNum r;

	Divide( *this, rBn, q, r );
	return r;
}


BigNum&
BigNum::operator %= ( const BigNum& rBn )
{
	BigNum q;
	BigNum r;

	Divide( *this, rBn, q, r );
	*this = r;
	return *this;
}


BigNum
BigNum::operator ^ ( unsigned int n ) const
{
	if ( mSign == Negative )
		throw NotImplementedException();
	
	BigNum result( 1 );
	BigNum x = *this;

	while ( n > 0 )
	{
		if ( ( n % 2 ) == 1 )
		{
			result *= x;
			n -= 1;
		}
		if ( n > 0 )
		{
			x = x * x;
			n = n / 2;
		}
	}

	return result;
}


BigNum
BigNum::operator ^ ( const BigNum& rBn ) const
{
	if ( mSign == Negative )
		throw NotImplementedException();

	BigNum result( 1 );
	BigNum x = *this;
	BigNum n = rBn;

	while ( !n.IsZero() )
	{
		if ( ( n % 2 ) == 1 )
		{
			result *= x;
			n -= 1;
		}
		if ( n > 0 )
		{
			x = x * x;
			n = n / 2;
		}
	}

	return result;
}


BigNum
BigNum::operator - () const
{
	BigNum bn = *this;
	bn.mSign == Positive ?
		bn.mSign = Negative :
		bn.mSign = Positive;
	return bn;
}


BigNum
BigNum::operator + () const
{
	BigNum bn = *this;
	// Znamenko plus mi nikdy vysledne znameko nezmeni?
	return bn;
}


bool
BigNum::operator == ( const BigNum& rBn ) const
{
	// Pokud jsou obe cisla nulova, vracime true
	if ( IsZero() && rBn.IsZero() )
		return true;

	// Pokud je jedno cislo nula a druhe ne, vracime false
	if ( !IsZero() && rBn.IsZero() )
		return false;
	if ( IsZero() && !rBn.IsZero() )
		return false;

	// Pokud je jedno cislo kladne a druhe zaporne, nebo naopak, 
	// vracime false
	if ( mSign == Positive && rBn.mSign == Negative )
		return false;
	if ( mSign == Negative && rBn.mSign == Positive )
		return false;

	size_t activeDoubleWords1 = mNb.GetActiveSize();
	size_t activeDoubleWords2 = rBn.mNb.GetActiveSize();

	// Pokud se nerovna pocet cislic vracime false.
	if ( activeDoubleWords1 != activeDoubleWords2 )
		return false;
	
	for ( size_t i = 0 ; i < mNb.GetSize() ; i++ )
		if ( mNb.Get( i ) != rBn.mNb.Get( i ) )
			return false;

	return true;
}


bool
BigNum::operator != ( const BigNum& rBn ) const
{
	return !operator == ( rBn );
}


bool
BigNum::operator > ( const BigNum& rBn ) const
{
	if ( IsZero() && rBn.IsZero() )
		return false;

	if ( rBn.IsZero() )
		return true;
	if ( IsZero() )
		return false;

	if ( mSign == Positive && rBn.mSign == Negative )
		return true;
	if ( mSign == Negative && rBn.mSign == Positive )
		return false;

	// zjistime pocet aktivnich bitu v cislech
	size_t activeDoubleWords1 = mNb.GetActiveSize();
	size_t activeDoubleWords2 = rBn.mNb.GetActiveSize();

	if ( activeDoubleWords1 < activeDoubleWords2 )
		return false;
	else if ( activeDoubleWords1 > activeDoubleWords2 )
		return true;

	size_t i = activeDoubleWords1;
	while ( i-- )
	{
		if ( mNb.Get( i ) > rBn.mNb.Get( i ) )
			return true;
		else if ( mNb.Get( i ) < rBn.mNb.Get( i ) )
			return false;
	}

	return false;
}


bool
BigNum::operator >= ( const BigNum& rBn ) const
{
	// Pokud jsou obe cisla nulova, vracime true
	if ( IsZero() && rBn.IsZero() )
		return true;

	// Pokud je druhe cislo nula
	if ( rBn.IsZero() )
		return true;
	if ( IsZero() )
		return false;

	if ( mSign == Positive && rBn.mSign == Negative )
		return true;
	if ( mSign == Negative && rBn.mSign == Positive )
		return false;

	// zjistime pocet aktivnich bitu v cislech
	size_t activeDoubleWords1 = mNb.GetActiveSize();
	size_t activeDoubleWords2 = rBn.mNb.GetActiveSize();

	if ( activeDoubleWords1 < activeDoubleWords2 )
		return false;
	else if ( activeDoubleWords1 > activeDoubleWords2 )
		return true;

	size_t i = activeDoubleWords1;
	while ( i-- )
	{
		if ( mNb.Get( i ) > rBn.mNb.Get( i ) )
			return true;
		else if ( mNb.Get( i ) < rBn.mNb.Get( i ) )
			return false;
	}

	return true;
}


bool
BigNum::operator < ( const BigNum& rBn ) const
{
	if ( IsZero() && rBn.IsZero() )
		return false;

	// Pokud je druhe cislo nula
	if ( rBn.IsZero() )
		return false;
	if ( IsZero() )
		return true;

	if ( mSign == Negative && rBn.mSign == Positive )
		return true;
	if ( mSign == Positive && rBn.mSign == Negative )
		return false;

	// zjistime pocet aktivnich bitu v cislech
	size_t activeDoubleWords1 = mNb.GetActiveSize();
	size_t activeDoubleWords2 = rBn.mNb.GetActiveSize();

	if ( activeDoubleWords1 < activeDoubleWords2 )
		return true;
	else if ( activeDoubleWords1 > activeDoubleWords2 )
		return false;

	size_t i = activeDoubleWords1;
	while ( i-- )
	{
		if ( mNb.Get( i ) < rBn.mNb.Get( i ) )
			return true;
		else if ( mNb.Get( i ) > rBn.mNb.Get( i ) )
			return false;
	}

	return false;
}


bool
BigNum::operator <= ( const BigNum& rBn ) const
{
	// Pokud jsou obe cisla nulova, vracime true
	if ( IsZero() && rBn.IsZero() )
		return true;

	// Pokud je druhe cislo nula
	if ( rBn.IsZero() )
		return false;
	if ( IsZero() )
		return true;

	if ( mSign == Negative && rBn.mSign == Positive )
		return true;
	if ( mSign == Positive && rBn.mSign == Negative )
		return false;

	// zjistime pocet aktivnich bitu v cislech
	size_t activeDoubleWords1 = mNb.GetActiveSize();
	size_t activeDoubleWords2 = rBn.mNb.GetActiveSize();

	if ( activeDoubleWords1 < activeDoubleWords2 )
		return true;
	else if ( activeDoubleWords1 > activeDoubleWords2 )
		return false;

	size_t i = activeDoubleWords1;
	while ( i-- )
	{
		if ( mNb.Get( i ) < rBn.mNb.Get( i ) )
			return true;
		else if ( mNb.Get( i ) > rBn.mNb.Get( i ) )
			return false;
	}

	return true;
}


bool
BigNum::SetValue( const std::string& rStrValue )
{
	std::string::const_iterator iter = rStrValue.begin();

	// Zkontrolujeme zda-li neni cislo zaporne a nastavime
	// prislusne znamenko
	if ( rStrValue[ 0 ] == '-' )
	{
		mSign = Negative;
		++iter;
	}
	else
		mSign = Positive;

	// Projde vstupni retezec a zkotroluje zda-li vsechny znaky sou platne.
	for ( ; iter != rStrValue.end() ; ++iter )
		if ( !isxdigit( *iter ) )
		{
			std::cerr << "Neplatny znak na vstupu SetValue()!" << std::endl;
			return false;
		}

	// Pokud je buffer alokovany, musime jej nejdrive uvolnit.
	if ( mNb.IsBufferAllocated() )
		mNb.Free();

	size_t str_len = rStrValue.length();
	// Alokuje buffer o potrebne velikosti.
	size_t size = ( str_len + 7 ) / 8;

	try
	{
		mNb.Allocate( size );
	}
	catch ( AllocationFailedException& e )
	{
		throw e;
	}

	// Pouzijeme reverzni iterator k pruchodu vstupniho rerezce od posledniho 
	// znaku k prvnim a vlozime cislice do bufferu.
	size_t i = 0;
	uint   realIdx;
	std::string::const_reverse_iterator riter;
	std::string::const_reverse_iterator riter_end =
		mSign == Negative ? rStrValue.rend() + 1 : rStrValue.rend() ;
	for ( riter = rStrValue.rbegin() ; riter != riter_end ; ++riter, i++ )
	{
		realIdx = i / 8;
		switch ( i % 8 )
		{
		case 0:
			mNb.Set( realIdx, mNb.Get( realIdx ) & 0xFFFFFFF0 );
			mNb.Set( realIdx, mNb.Get( realIdx ) | HexCharToInt( *riter ) );
			break;
		case 1:
			mNb.Set( realIdx, mNb.Get( realIdx ) & 0xFFFFFF0F );
			mNb.Set( realIdx, mNb.Get( realIdx ) | ( ( HexCharToInt( *riter ) << 4 ) ) );
			break;
		case 2:
			mNb.Set( realIdx, mNb.Get( realIdx ) & 0xFFFFF0FF );
			mNb.Set( realIdx, mNb.Get( realIdx ) | ( ( HexCharToInt( *riter ) << 8 ) ) );
			break;
		case 3:
			mNb.Set( realIdx, mNb.Get( realIdx ) & 0xFFFF0FFF );
			mNb.Set( realIdx, mNb.Get( realIdx ) | ( ( HexCharToInt( *riter ) << 12 ) ) );
			break;
		case 4:
			mNb.Set( realIdx, mNb.Get( realIdx ) & 0xFFF0FFFF );
			mNb.Set( realIdx, mNb.Get( realIdx ) | ( ( HexCharToInt( *riter ) << 16 ) ) );
			break;
		case 5:
			mNb.Set( realIdx, mNb.Get( realIdx ) & 0xFF0FFFFF );
			mNb.Set( realIdx, mNb.Get( realIdx ) | ( ( HexCharToInt( *riter ) << 20 ) ) );
			break;
		case 6:
			mNb.Set( realIdx, mNb.Get( realIdx ) & 0xF0FFFFFF );
			mNb.Set( realIdx, mNb.Get( realIdx ) | ( ( HexCharToInt( *riter ) << 24 ) ) );
			break;
		case 7:
			mNb.Set( realIdx, mNb.Get( realIdx ) & 0x0FFFFFFF );
			mNb.Set( realIdx, mNb.Get( realIdx ) | ( ( HexCharToInt( *riter ) << 28 ) ) );
			break;
		}
	}

	return true;
}


bool
BigNum::SetValue( uint intValue )
{
	if ( intValue < 0 )
	{
		mSign = Negative;
		intValue = - intValue;
	}
	else
		mSign = Positive;

	// Pokud je buffer alokovany, musime jej nejdrive uvolnit.
	if ( mNb.IsBufferAllocated() )
		mNb.Free();

	try
	{
		mNb.Allocate( 1 );
	}
	catch ( AllocationFailedException& e )
	{
		throw e;
	}

	mNb.Set( 0, intValue );

	return true;
}


int32_t
BigNum::ToInt32( void ) const
{
	if ( IsZero() )
		return 0;

	if ( mSign == Negative )
		return - mNb.Get( 0 );
	else
		return mNb.Get( 0 );
}


std::string
BigNum::ToDecString( void ) const
{
	std::string str_value;

	if ( IsZero() )
	{
		str_value = "0";
		return str_value;
	}

	BigNum bn = *this;


	do
	{
		str_value.insert( str_value.begin(),
		                  IntToHexChar( ( bn % 10 ).mNb.Get( 0 ) ) );
		bn /= 10;
	} while ( !bn.IsZero() );

	if ( mSign == Negative )
		str_value.insert( str_value.begin(), '-' );

	return str_value;
}


std::string
BigNum::ToHexString( void ) const
{
	std::string str_value;
	size_t      i = GetActiveSize() * 4;

	if ( IsZero() )
	{
		str_value = '0';
		return str_value;
	}

	if ( mSign == Negative )
		str_value += '-';
	
	uint8_t tmp;
	bool    begin = true;
	char    c;

	while ( i-- )
	{
		tmp = GetByte( i );
		c = IntToHexChar( ( tmp & 0xF0 ) >> 4 );
		if ( !begin || c != '0' )
		{
			str_value += c;
			begin = false;
		}
		
		c = IntToHexChar( tmp & 0x0F );
		if ( !begin || c != '0' )
		{
			str_value += c;
			begin = false;
		}
	}

	return str_value;
}


std::string
BigNum::ToHexString2( void ) const
{
	std::string str_value;
	size_t      i = GetByteSize() * 4;

	if ( IsZero() )
	{
		str_value = '0';
		return str_value;
	}

	if ( mSign == Negative )
		str_value += '-';
	
	uint8_t tmp;
	bool    begin = true;
	char    c;

	while ( i-- )
	{
		tmp = GetByte( i );
		c = IntToHexChar( ( tmp & 0xF0 ) >> 4 );
		if ( !begin || c != '0' || i == 0 )
		{
			str_value += c;
			begin = false;
		}
			
		c = IntToHexChar( tmp & 0x0F );
		if ( !begin || c != '0' || i == 0 )
		{
			str_value += c;
			begin = false;
		}

		if ( i != 0 )
			str_value += ':';
	}

	return str_value;
}


void
BigNum::DebugPrint() const
{
	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "Hex hodnota cisla: " << ToHexString() << std::endl;
	std::cout << "Dec hodnota cisla: " << ToDecString() << std::endl;
	std::cout << "Pocet bitu cisla:  " << GetBitCnt() << std::endl;
	std::cout << "NumberBuffer ptr:  " << ( uint32_t ) mNb.GetBufferPointer() << std::endl;
	std::cout << "NumberBuffer size: " << ( uint32_t ) mNb.GetSize() << std::endl;

	size_t i;
	for ( i = GetBitCnt() - 1 ; i != 0 ; i-- )
	{
		std::cout << ( int ) TestBit( i );
	}
	std::cout << ( int ) TestBit( i ) << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
}


bool
BigNum::IsProbablyPrime( unsigned int times ) const
{
	static uint16_t primes[] = {
		2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73,
		79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163,
		167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251,
		257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349,
		353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443,
		449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557,
		563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647,
		653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757,
		761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863,
		877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983,
		991, 997, 1009, 1013, 1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063,
		1069, 1087, 1091, 1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151, 1153, 1163,
		1171, 1181, 1187, 1193, 1201, 1213, 1217, 1223, 1229, 1231, 1237, 1249, 1259,
		1277, 1279, 1283, 1289, 1291, 1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361,
		1367, 1373, 1381, 1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453,
		1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511, 1523, 1531, 1543, 1549,
		1553, 1559, 1567, 1571, 1579, 1583, 1597, 1601, 1607, 1609, 1613, 1619, 1621,
		1627, 1637, 1657, 1663, 1667, 1669, 1693, 1697, 1699, 1709, 1721, 1723, 1733,
		1741, 1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811, 1823, 1831, 1847,
		1861, 1867, 1871, 1873, 1877, 1879, 1889, 1901, 1907, 1913, 1931, 1933, 1949,
		1951, 1973, 1979, 1987, 1993, 1997, 1999
	};

	// Pokud je cislo sude, neni to prvocislo
	if ( IsEven() )
		return false;

	unsigned int i;

	//
	// Nejdrive otestujeme zda-li cislo neni delitelne nekterym prvocislem
	// mensim nez 2000.
	for ( i = 0 ; i < sizeof( primes ) / sizeof( uint16_t ) ; i++ )
	{
		if ( ( *this % primes[ i ] ) == 0 )
			return false;
	}
	if ( RabinMillerPrimalityTest( times ) == false )
		return false;
	else
		return true;
	
#if 0
	BigNum p_minus_one = *this - 1;


	for ( i = 0 ; i < times ; i++ )
	{
		BigNum a = BigNum::GenerateRandomMax( p_minus_one );

		if ( BigNum::Gcd( a, *this ) != 1 )
			return false;

		BigNum b = BigNum::ModularExponentiation( a, p_minus_one, *this );

		if ( b != 1 )
		{
			return false;
		}
	}
	return true;
#endif // 0
}


BigNum::IntegerType
BigNum::RabinMillerPrimalityTest( uint accuracy ) const
{
	BigNum pMinusOne = *this - 1;
	BigNum t = pMinusOne;
	uint   i;
	uint   j;
	uint   s = 0;

	for ( i = 0 ; t.IsEven() ; i++ )
	{
		s++;
		t >>= 1;
	}

	IntegerType iType = Composite;

	for ( i = 0 ; i < accuracy ; i++ )
	{
		BigNum a = BigNum::GenerateRandomMax( pMinusOne );

		if ( BigNum::Gcd( a, *this ) != 1 )
			return Composite;

		BigNum b = BigNum::ModularExponentiation( a, t, *this );

		if ( b == 1 || b == pMinusOne )
			iType = ProbablyPrime;

		for ( j = 0 ; iType == Composite && j < s ; j++ )
		{
			if ( b == pMinusOne )
			{
				iType = ProbablyPrime;
				break;
			}
			
			b = ( b * b ) % *this;
		}

		if ( iType == Composite )
			return Composite;
	}

	return ProbablyPrime;
}


bool 
BigNum::IsZero( void ) const
{
	if ( !mNb.IsBufferAllocated() )
		return true;

	size_t i = mNb.GetSize();
	while ( i-- )
		if ( mNb.Get( i ) != 0 )
			return false;

	return true;
}


bool
BigNum::IsEven( void ) const
{
	// Pokud mame alokovany buffer muzeme otestovat, pokud
	// vsak buffer neni alokovany je hodnota cisla 0, a tudiz
	// cislo je sude => vracime true.
	if ( mNb.IsBufferAllocated() )
	{
		return !( mNb.Get( 0 ) & 1 );
	}
	else
		return true;
}


bool
BigNum::IsOdd( void ) const
{
	// Pokud mame alokovany buffer muzeme otestovat, pokud
	// vsak buffer neni alokovany je hodnota cisla 0, a tudiz
	// cislo neni liche => vracime false.
	if ( mNb.IsBufferAllocated() )
	{
		return mNb.Get( 0 ) & 1;
	}
	else
		return false;
}


size_t
BigNum::GetActiveBytes( void ) const
{
	size_t i = GetSize() * 4;

	while ( i-- )
		if ( GetByte( i ) != 0 )
			break;
	return i + 1;
}


size_t
BigNum::GetByteSize( void ) const
{
	return mNb.GetActiveSize() * 4;
}


size_t
BigNum::GetBitCnt( void ) const
{
	if ( !mNb.IsBufferAllocated() )
		return 0;

	size_t active_size = mNb.GetActiveSize();
	
	if ( active_size == 0 )
		return 0;

	size_t   bits;
	size_t   index;
	uint32_t value;

	bits  = 32;
	index = active_size - 1;
	value = mNb.Get( index ); 

	while ( ( ( value & 0x80000000 ) == 0 ) && ( bits > 0 ) )
	{
		bits--;
		value <<= 1;
	}

	bits += ( index * 32 );

	return bits;
}


size_t
BigNum::GetBitSize( void ) const
{
	return mNb.GetActiveSize() * 32;
}


uint8_t
BigNum::GetByte( size_t index ) const
{
	uint32_t tmp = mNb.Get( index / 4 );

	switch ( index % 4 )
	{
		case 0:
			return tmp & 0x000000FF;
		case 1:
			return ( tmp >> 8 ) & 0x000000FF;
		case 2:
			return ( tmp >> 16 ) & 0x000000FF;
		case 3:
			return ( tmp >> 24 ) & 0x000000FF;
	}
	return 0;
}


size_t
BigNum::GetByteCnt( void ) const
{
	return GetActiveSize() * 4;
}


bool
BigNum::SetByte( size_t index, uint8_t value )
{
	if ( index > GetSize() * 4 )
		return false;

	size_t realPosition = index / 4;

	uint32_t tmp = mNb.Get( realPosition );

	switch ( index % 4 )
	{
		case 0:
			mNb.Set( realPosition, ( tmp & 0xFFFFFF00 ) | static_cast< uint32_t >( value ) );
			break;
		case 1:
			mNb.Set( realPosition, ( tmp & 0xFFFF00FF ) | ( static_cast< uint32_t >( value ) << 8 ) );
			break;
		case 2:
			mNb.Set( realPosition, ( tmp & 0xFF00FFFF ) | ( static_cast< uint32_t >( value ) << 16 ) );
			break;
		case 3:
			mNb.Set( realPosition, ( tmp & 0x00FFFFFF ) | ( static_cast< uint32_t >( value ) << 24 ) );
			break;
	}

	return true;
}


uint32_t
BigNum::GetDoubleWord( size_t index ) const
{
	return mNb.Get( index );
}


bool
BigNum::SetDoubleWord( size_t index, uint32_t value )
{
	return mNb.Set( index, value );
}


int
BigNum::TestBit( size_t bit ) const
{
	size_t   index = bit / 32;
	uint32_t value = mNb.Get( index );

	value >>= bit % 32;

	return value & 1;
}


void
BigNum::SetBit( size_t bit )
{
	size_t   index = bit / 32;
	uint32_t value = mNb.Get( index );
	uint32_t mask = 1 << ( bit % 32 );

	mNb.Set( index, value | mask );
}


void
BigNum::ClearBit( size_t bit )
{
	size_t   index = bit / 32;
	uint32_t value = mNb.Get( index );
	uint32_t mask = ~( 1 << ( bit % 32 ) );

	mNb.Set( index, value & mask );
}


#if 0
bool
BigNum::ShortDivide( const BigNum& u, uint16_t v, BigNum& q, BigNum& r )
{
	//
	// Pokud je v nula, vyhodime vyjimku ...
	if ( v == 0 )
		throw DivisionByZeroException();

	//
	// Pokud je u rovno v, podil je jedna a zbytek nula. 
	if ( u == v )
	{
		q = 1;
		r = 0;
		return true;
	}

	//
	// Pokud je u rovno v, podil je jedna a zbytek nula. 
	if ( u == v )
	{
		q = 1;
		r = 0;
		return true;
	}

	// 
	// Pokud je u mensi nez v, podil je nula a zbytek roven v.
	if ( u < v )
	{
		q = 0;
		r = u;
		return true;
	}

	//
	// Pokud delime dvojkou, podil je roven u s bitovym posuvem 
	// doprava o 1 bit.
	if ( v == 2 )
	{
		q = u >> 1;
		r = u.mNb[ 0 ] & 1;
		return true;
	}

	// m = velikost cisla u ( ve wordech )
	// n = velikost cisla v ( ve wordech )
	size_t m = u.mNb.GetActiveSize();

	//
	// Alokujeme pamet pro podil a zbytek.
	try
	{
		q.mNb.Allocate( m );
		r.mNb.Allocate( 1 );
	}
	catch ( AllocationFailedException& e )
	{
		throw e;
	}

	//
	// Pokud ma delitel velikost = 1 ( 16 bitu )
	unsigned int b = WORD_VALUE;
	int          j;
	int          k;

	k = 0;
	for ( j = m - 1 ; j >= 0 ; j-- )
	{
		q.SetWord( j, ( k * b + u.GetWord( j ) ) / v );
		k = ( k * b + u.GetWord( j ) ) - ( q.GetWord( j ) * v );
	}
	r.SetWord( 0, k );

	return true;
}
#endif // 0


bool
BigNum::Divide( const BigNum& u, const BigNum& v, BigNum& q, BigNum& r )
{
	//
	// Algoritmus pro deleni je prevzat z knihy The Art of Computer Programming
	// od Donalda E. Knutha (viz [1] v sekci reference v dokumentaci).
	//
	// u - delenec           = vstup funkce
	// v - delitel           = vstup funkce
	// q - quotient, podil   = vystup funkce
	// r - remainder, zbytek = vystup funkce
	//
	//   q = u / v
	//   r = u mod v
	//
	// m - velikost(delka) u
	// n - velikost(delka) v
	//

	//
	// Pokud je v nula, vyhodime vyjimku ...
	if ( v.IsZero() )
		throw DivisionByZeroException();

	//
	// Pokud je u rovno v, podil je jedna a zbytek nula. 
	if ( u == v )
	{
		q = 1;
		r = 0;
		return true;
	}

	// 
	// Pokud je u mensi nez v, podil je nula a zbytek roven v.
	if ( u < v )
	{
		q = 0;
		r = u;
		return true;
	}

	//
	// Pokud delime dvojkou, podil je roven u s bitovym posuvem 
	// doprava o 1 bit.
	if ( v == 2 )
	{
		q = u >> 1;
		r = u.mNb[ 0 ] & 1;
		return true;
	}

	// m = velikost cisla u ( ve wordech )
	// n = velikost cisla v ( ve wordech )
	size_t m = u.mNb.GetActiveSize();
	size_t n = v.mNb.GetActiveSize();

	//
	// Alokujeme pamet pro podil a zbytek.
	try
	{
		q.mNb.Allocate( m );
		r.mNb.Allocate( n );
	}
	catch ( AllocationFailedException& e )
	{
		throw e;
	}

	//
	// Pokud ma delitel velikost = 1 ( 32 bitu )
	if ( n == 1 )
	{
		//uint64_t b = DWORD_VALUE;
		int64_t  j;
		uint64_t k;

		k = 0;
		for ( j = m - 1 ; j >= 0 ; j-- )
		{
			q.mNb.Set( j,
				( ( k << 32 ) + u.mNb.Get( j ) ) / v.mNb.Get( 0 ) );
			k = ( ( k << 32 ) + u.mNb.Get( j ) ) - ( q.mNb.Get( j ) * v.mNb.Get( 0 ) );
		}
		r.mNb.Set( 0, k );

		return true;
	}

	// Normalizovany delenec ( u )
	BigNum un( u, u.mNb.GetActiveSize() + 1 );
	// Normalizovany delitel ( v )
	BigNum vn( v );
	// Pocet bitu o ktere bude posunuto u a v
	int    s = 0;


	// Provedeme normalizaci...
	while ( ( vn.mNb[ n - 1 ] & 0x80000000 ) != 0x80000000 )
	{
		vn <<= 1;
		s++;
	}
	un <<= s;

	bool     still = false;
	uint64_t b = DWORD_VALUE;
	uint64_t qhat;
	uint64_t rhat;
	uint64_t p;
	int64_t  i;
	int64_t  j;
	int64_t  k;
	int64_t  t;

	for ( j = m - n ; j >= 0 ; j-- )
	{
		qhat = ( ( uint64_t( un.mNb[ j + n ] ) << 32 ) + uint64_t( un.mNb[ j + n - 1 ] ) ) / uint64_t( vn.mNb[ n - 1 ] );
		rhat = ( ( uint64_t( un.mNb[ j + n ] ) << 32 ) + uint64_t( un.mNb[ j + n - 1 ] ) ) - qhat * uint64_t( vn.mNb[ n - 1 ] );

		do
		{
			if ( ( qhat >= b ) || ( ( qhat * uint64_t( vn.mNb[ n - 2 ] ) ) > ( ( rhat << 32 ) + uint64_t( un.mNb[ j + n - 2 ] ) ) ) )
			{
				qhat = qhat - 1;
				rhat = rhat + uint64_t( vn.mNb[ n - 1 ] );

				if ( rhat < b )
					still = true;
				else
					still = false;
			}
			else
				still = false;
		} while ( still == true );

		k = 0;
		for ( i = 0 ; i < n ; i++ )
		{
			p = qhat * uint64_t( vn.mNb[ i ] );
			t = uint64_t( un.mNb[ i + j ] ) - k - uint64_t( p & 0xFFFFFFFF );
			un.mNb.Set( i + j, t );
			k = ( p >> 32 ) - ( t >> 32 );
		}
		t = un.mNb[ j + n ] - k;
		un.mNb.Set( j + n, t );

		q.mNb.Set( j, qhat );
		if ( t < 0 )
		{
			q.mNb.Set( j, q.mNb[ j ] - 1 );
			k = 0;

			for ( i = 0; i < n; i++ )
			{
				t = uint64_t( un.mNb[ i + j ] ) + uint64_t( vn.mNb[ i ] ) + k;
				un.mNb.Set( i + j, t );
				k = static_cast< uint64_t >( t ) >> 32;
			}
			un.mNb.Set( j + n, uint64_t( un.mNb[ j + n ] ) + k );
		}
	}

	for ( i = 0 ; i < n ; i++ )
		r.mNb.Set( i, ( uint64_t( un.mNb[ i ] ) >> s ) | ( uint64_t( un.mNb[ i + 1 ] ) << ( 32 - s ) ) );
	
	if ( u.mSign == Negative && v.mSign == Positive )
		q.mSign = Negative;
	else if ( u.mSign == Positive && v.mSign == Negative )
	{
		q.mSign = Negative;
		r.mSign = Negative;
	}
	else if ( u.mSign == Negative && v.mSign == Negative )
		r.mSign = Negative;

	return true;
}


BigNum
BigNum::Gcd( const BigNum& u, const BigNum& v )
{
	//
	// Algoritmus pro vypocet nejvetsiho spolecneho delitele je prevzat 
	// z Wikipedie (http://en.wikipedia.org/wiki/Binary_GCD_algorithm).
	//

	unsigned int k = 0;
	BigNum       _u = u;
	BigNum       _v = v;

	if ( u.IsZero() )
		return v;
	if ( v.IsZero() )
		return u;

	while ( _u.IsEven() &&  _v.IsEven() )
	{
		_u >>= 1;
		_v >>= 1;
		k++;
	}

	do {
		if ( _u.IsEven() )
			_u >>= 1;
		else if ( _v.IsEven() )
			_v >>= 1;
		else if ( _u >= _v )
			_u = ( _u - _v ) >> 1;
		else
			_v = ( _v - _u ) >> 1;
	} while ( _u > 0 );

	return _v << k;
}


BigNum
BigNum::Lcm( const BigNum& u, const BigNum& v )
{
	BigNum lcm = ( u / Gcd( u, v ) ) * v;
	return lcm;
}


BigNum
BigNum::ModularExponentiation( const BigNum& b, const BigNum& e, const BigNum& m )
{
	//
	// Algoritmus pro vypocet modularniho umocneni je prevzat z Wikipedie
	// (http://en.wikipedia.org/wiki/Modular_exponentiation).
	//

	BigNum a = 1;
	BigNum g = b % m;

	int i;
	for ( i = e.GetBitCnt() - 1 ; i >= 0 ; i-- )
	{
		a = ( a * a ) % m;
		if ( e.TestBit( i ) )
			a = ( a * g ) % m;
	}

	return a;
}


BigNum
BigNum::ModularInverse( const BigNum& u, const BigNum& v )
{
	BigNum u1, u3, v1, v3, t1, t3, q, w, inv;
	int    bIterations;

	u1 = 1;
	u3 = u;
	v1 = 0;
	v3 = v;

	bIterations = 1;
	while ( !v3.IsZero() )
	{
		q = u3 / v3;
		t3 = u3 % v3;

		w = q * v1;
		t1 = u1 + w;

		u1 = v1;
		v1 = t1;
		u3 = v3;
		v3 = t3;

		bIterations = -bIterations;
	}

	if ( bIterations < 0 )
		inv = v - u1;
	else
		inv = u1;

	if ( u3 == 1 )
		return inv;
	else
		return 0;
}


void
BigNum::SeedRandom( unsigned int seed )
{
	srand( seed );
}


BigNum
BigNum::GenerateRandom( size_t size )
{
	BigNum bn;

	if ( size == 0 )
		return bn;

	try
	{
		bn.mNb.Allocate( size );
	}
	catch ( AllocationFailedException& e )
	{
		throw e;
	}

	size_t i = 0;

	for ( i = 0 ; i < bn.mNb.GetSize() ; i++ )
	{
		uint64_t tmp = 1 + static_cast< uint64_t >(
			( ( float ) DWORD_VALUE * ( rand() / ( RAND_MAX + 1.0 ) ) )
		);
		bn.mNb.Set( i, tmp );
	}

	return bn;
}


BigNum
BigNum::GenerateRandomMax( const BigNum& max )
{
	if ( max.IsZero() )
		return max;

	BigNum random = BigNum::GenerateRandom( max.GetActiveSize() );

	if ( random > max )
	{
		random = random % max;
	}

	return random;
}


BigNum
BigNum::GenerateRandomBits( size_t bits )
{
	BigNum bn;

	if ( bits == 0 )
		return bn;

	size_t size = ( bits + 31 ) / 32;

	try
	{
		bn.mNb.Allocate( size );
	}
	catch ( AllocationFailedException& e )
	{
		throw e;
	}

	size_t  bytes = bits / 8;
	size_t  rest  = bits % 8;
	size_t  i;
	int     tmp;
	uint8_t mask = 0x1;

	for ( i = 0 ; i < bytes ; i++ )
	{
		tmp = rand();
		bn.SetByte( i, tmp & 0xFF );
	}


	tmp = rand();
	for ( i = bits - rest ; i < rest ; i++ )
	{
		if ( mask & tmp )
			bn.SetBit( i );
		mask <<= 1;
	}

	bn.SetBit( bits - 1 );

	return bn;
}


BigNum
BigNum::GeneratePrime( uint bits, uint topBits )
{
	BigNum bn;

	if ( bits < 2 || topBits >= bits )
		return bn;

	while ( true )
	{
		bn = GenerateRandomBits( bits );
		if ( topBits != 0 )
		{
			uint i;
			for ( i = 0 ; i < topBits ; i++ )
			{
				bn.SetBit( bn.GetBitCnt() - i - 1 );
			}
		}

		if ( bn.IsEven() )
			bn -= 1;

		while ( !bn.IsProbablyPrime( 30 ) )
		{
			bn += 2;
		}

		if ( bn.GetBitCnt() == bits )
			break;
	}

	return bn;
}

