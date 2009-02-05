/*
 * BigNum/NumberBuffer.h
 *
 * Prakticka maturitni zkouska, 2005 - 2006
 * Implementace sifrovaci algoritmu RSA.
 *
 * Copyright (c) 2005-2006 Jiri Zajpt, <jzajpt@blueberry.cz>
 */

#ifndef _BIGNUM_NUMBERBUFFER__H
#define _BIGNUM_NUMBERBUFFER__H


#include "../Common/Exceptions.h"
#include "../Common/Exceptions.h"


/**
 * \brief Trida NumberBuffer slouzi k ulozeni nekolika 32-bitovych cisel.
 *
 * \author Jiri Zajpt
 */
class NumberBuffer
{
public:
	/**
	 * \brief Implicitni konstruktor.
	 *
	 * Nealokuje zadnou pamet, pouze nastavi pocet prvku na nula
	 * a pointer na bufffer na NULL.
	 */
	NumberBuffer();


	/**
	 * \brief Kopirovaci konstruktor.
	 *
	 * Zkopiruje objekt nb. Pokud je specifikovan parametr count,
	 * je alokovan buffer pro pocet prvku count a do nej zkopirovan
	 * objekt nb. Parametr count muze byt vetsi i mensi nez je 
	 * puvodni pocet prvku v objektu nb.
	 *
	 * \param rFrom Objekt pro zkopirovani
	 * \param count Velikost noveho bufferu.
	 */
	NumberBuffer( const NumberBuffer& rFrom, size_t count );
	NumberBuffer( const NumberBuffer& rFrom );


	/**
	 * \brief Konstruktor, ktery alokuje buffer pro dany pocet cisel.
	 *
	 * \param count Velikost noveho bufferu.
	 */
	NumberBuffer( size_t count );


	/**
	 * \brief Destruktor.
	 */
	~NumberBuffer();


	/**
	 * \brief Operator prirazeni.
	 *
	 * \param rFrom Zdroj prirazeni.
	 *
	 * \return Reference na nove prirazeny objekt.
	 */
	NumberBuffer& operator = ( const NumberBuffer& rFrom );


	/**
	 * \brief Operator pro pristup do bufferu.
	 *
	 * Pokud je index vetsi nez velikost bufferu, funkce vraci nulu.
	 *
	 * \param index Index v bufferu. Pocita se od nuly.
	 *
	 * \return Hodnota bufferu na dane pozici.
	 */
	uint32_t operator [] ( size_t index ) const;


	/**
	 * \brief Vraci hodnotu na dane pozici.
	 *
	 * \param index Index v bufferu. Pocita se od nuly.
	 *
	 * \return Hodnota na dane pozici.
	 */
	uint32_t Get( size_t index ) const;

	
	/**
	 * \brief Nastavi hodnotu na dane pozici.
	 *
	 * \param index Index v bufferu. Pocita se od nuly.
	 * \param value Hodnota ktera se priradi.
	 *
	 * \return True pokud se podarilo nastavit prvek, jinak false.
	 */
	bool Set( size_t index, uint32_t value );

	
	/**
	 * \brief Alokuje buffer pro dany pocet prvku.
	 *
	 * Po alokaci funkce nastavi vsechny prvky na nulu pomoci
	 * fce memset().
	 *
	 * \param count Pocet prvku.
	 *
	 * \return True pokud alokace probehla uspesne, jinak false.
	 */
	bool Allocate( size_t count );

	
	/**
	 * \brief Realokuje buffer na novou velikost, pri zachovani obsahu.
	 *
	 * \param newCount Novy pocet prvku bufferu.
	 *
	 * \return True pokud re-alokace probehla uspesne, jinak false.
	 */
	bool Reallocate( size_t newCount );

	
	/**
	 * \brief Uvolni buffer.
	 */
	void Free( void );

	
	/**
	 * \brief Vraci pocet aktivnich prvku v cisle. 
	 *
	 * Aktivni prvek je takovy prvek ktera nese uzitecnou
	 * hodnotu, tj. vsechny cislice krome tzv. leading zero.
	 *
	 * \return Pocet aktivnich prvku v cisle.
	 */
	size_t GetActiveSize( void ) const;


	/**
	 * \brief Vraci pocet prvku ktere muze prave alokovany buffer uchovat.
	 *
	 * \return Pocet prvku ktere muze buffer uchovat.
	 */
	size_t GetSize( void ) const { return mBufferSize; };


	/**
	 * \brief Vraci ukazatel na buffer.
	 *
	 * \return Ukazatel na buffer.
	 */
	uint32_t* GetBufferPointer( void ) const { return mpBuffer; };


	/**
	 * \brief Testuje zda-li je buffer alokovany.
	 *
	 * \return True je buffer alokovany, pokud neni false.
	 */
	bool IsBufferAllocated( void ) const { return mpBuffer && mBufferSize; };


private:
	/**
	 * \brief Ukazatel na buffer ve kterem budou ulozeny cislice.
	 *
	 * Cislo je ukladano ve stylu little-endian po 32 bitech.
	 * 
	 * \see http://en.wikipedia.org/wiki/Little_endian
	 */
	uint32_t* mpBuffer;


	/**
	 * \brief Pocet prvku ktere muze prave alokovany buffer uchovat.
	 */
	size_t mBufferSize;
};


#endif // _BIGNUM_NUMBERBUFFER__H
