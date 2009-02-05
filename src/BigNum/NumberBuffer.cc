/*
 * BigNum/NumberBuffer.cc
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



NumberBuffer::NumberBuffer()
	: mpBuffer( 0 ),
	mBufferSize( 0 )
{
}


NumberBuffer::NumberBuffer( const NumberBuffer& rFrom, size_t count )
	: mpBuffer( 0 ),
	mBufferSize( 0 )
{
	if ( count == 0 )
	{
		Allocate( rFrom.mBufferSize );
		memcpy( mpBuffer, rFrom.mpBuffer, mBufferSize * sizeof( uint32_t ) );
	}
	else
	{
		Allocate( count );
		if ( count > rFrom.mBufferSize )
			memcpy( mpBuffer, rFrom.mpBuffer, rFrom.mBufferSize * sizeof( uint32_t ) );
		else
			memcpy( mpBuffer, rFrom.mpBuffer, count * sizeof( uint32_t ) );
	}
}


NumberBuffer::NumberBuffer( const NumberBuffer& rFrom )
	: mpBuffer( 0 ),
	mBufferSize( 0 )
{
	Allocate( rFrom.mBufferSize );
	memcpy( mpBuffer, rFrom.mpBuffer, mBufferSize * sizeof( uint32_t ) );
}


NumberBuffer::NumberBuffer( size_t count )
	: mpBuffer( 0 ),
	mBufferSize( 0 )
{
	Allocate( count );
}


NumberBuffer::~NumberBuffer()
{
	Free();
}


NumberBuffer&
NumberBuffer::operator = ( const NumberBuffer& rFrom )
{
	// Pokud se objekt prirazuje sam sobe
	if ( this == &rFrom )
		return *this;

	// Pokud mame alokovany buffer musime jej nejdrive
	// uvolnit.
	if ( IsBufferAllocated() )
		Free();

	Allocate( rFrom.mBufferSize );
	memcpy( mpBuffer, rFrom.mpBuffer, mBufferSize * sizeof( uint32_t ) );
	return *this;
}


bool
NumberBuffer::Allocate( size_t count )
{
	// Pokud uz je buffer alokovany, s nicim nemanipulujeme
	// a vratime false.
	if ( IsBufferAllocated() )
	{
		std::cerr << "Buffer uz je alokovany!" << std::endl;
		return false;
	}

	// Alokovani bufferu velikosti nula = Free()
	if ( count == 0 )
	{
		Free();
		return true;
	}

	mpBuffer = new uint32_t[ count ];
	if ( !mpBuffer )
		throw AllocationFailedException();

	mBufferSize = count;
	// Nastavime nuly v celem bufferu.
	memset( mpBuffer, 0, mBufferSize * sizeof( uint32_t ) );

	return true;
}


bool
NumberBuffer::Reallocate( size_t newCount )
{
	if ( newCount == mBufferSize )
		return true;

	if ( !IsBufferAllocated() )
	{
		Allocate( newCount );
		return true;
	}
	
	if ( newCount == 0 )
	{
		Free();
		return true;
	}

	uint32_t* copy = new uint32_t[ mBufferSize ];
	size_t    old_count = mBufferSize;
	if ( !copy )
		throw AllocationFailedException();

	memcpy( copy, mpBuffer, old_count * sizeof( uint32_t ) );

	Free();
	mpBuffer = new uint32_t[ newCount ];
	if ( !mpBuffer )
		throw AllocationFailedException();

	mBufferSize = newCount;
	if ( newCount > old_count )
		memcpy( mpBuffer, copy, old_count * sizeof( uint32_t ) );
	else
		memcpy( mpBuffer, copy, newCount * sizeof( uint32_t ) );

	return true;
}


void
NumberBuffer::Free( void )
{
	if ( IsBufferAllocated() )
	delete[] mpBuffer;
	mBufferSize = 0;
	mpBuffer = 0;
}


uint32_t
NumberBuffer::operator [] ( size_t index ) const
{
	if ( index >= mBufferSize )
		return 0;
	return mpBuffer[ index ];
}


uint32_t
NumberBuffer::Get( size_t index ) const
{
	if ( index >= mBufferSize )
		return 0;
	return mpBuffer[ index ];
}


bool
NumberBuffer::Set( size_t index, uint32_t value )
{
	if ( index >= mBufferSize )
		return false;
	mpBuffer[ index ] = value;

	return true;
}


size_t
NumberBuffer::GetActiveSize( void ) const
{
	size_t i = mBufferSize;

	while ( i-- )
		if ( mpBuffer[ i ] != 0 )
			break;
	return i + 1;
}


