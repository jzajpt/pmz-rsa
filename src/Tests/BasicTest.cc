#include <iostream>
#include <fstream>
#include <cstdlib>
#include <BigNum.h>


using namespace std;


bool
test_from_uint32_to_uint32( uint32_t from, uint32_t to, uint32_t step, bool break_on_error )
{
	size_t error_cnt     = 0;          // pocet chyb
	float  prev_progress = -1;
	float  progress      = 0;
	char   buf[ 100 ];
	size_t i;
	BigNum bn;

	for ( i = from ; i < ( to - step ) ; i += step )
	{
		progress = ( float ) ( ( ( i - from ) / ( double ) ( to - from ) ) * 100 );
		if ( progress != prev_progress )
		{
			snprintf( buf, 100, "\rPocet chyb: %d, prubeh %.4f %%", error_cnt, progress );
			cout << buf;
			cout << flush;
		}
		prev_progress = progress;

		// BigNum::operator = ( uint32_t )
		bn = i;

		if ( bn.ToInt32() != i )
		{
			if ( break_on_error )
			{
				cout << endl;
				return false;
			}	
			error_cnt++;
		}
	}

	cout << endl;

	if ( error_cnt )
		return false;
	else
		return true;
}


bool
test_from_uint32_to_string( uint32_t from, uint32_t to, uint32_t step, bool break_on_error )
{
	size_t error_cnt     = 0;          // pocet chyb
	float  prev_progress = -1;
	float  progress      = 0;
	char   buf[ 100 ];
	size_t i;
	BigNum bn;

	for ( i = from ; i < ( to - step ) ; i += step )
	{
		progress = ( float ) ( ( ( i - from ) / ( double ) ( to - from ) ) * 100 );
		if ( progress != prev_progress )
		{
			snprintf( buf, 100, "\rPocet chyb: %d, prubeh %.4f %%", error_cnt, progress );
			cout << buf;
			cout << flush;
		}
		prev_progress = progress;

		// BigNum::operator = ( uint32_t )
		bn = i;

		snprintf( buf, 100, "%x", i );

		if ( bn.ToHexString() != buf )
		{
			if ( break_on_error )
			{
				cout << endl;
				return false;
			}
			error_cnt++;
		}
	}

	cout << endl;

	if ( error_cnt )
		return false;
	else
		return true;
}


bool
test_from_string_to_uint32( uint32_t from, uint32_t to, uint32_t step, bool break_on_error )
{
	size_t error_cnt     = 0;          // pocet chyb
	float  prev_progress = -1;
	float  progress      = 0;
	char   buf[ 100 ];
	size_t i;
	BigNum bn;

	for ( i = from ; i < ( to - step ) ; i += step )
	{
		progress = ( float ) ( ( ( i - from ) / ( double ) ( to - from ) ) * 100 );
		if ( progress != prev_progress )
		{
			snprintf( buf, 100, "\rPocet chyb: %d, prubeh %.4f %%", error_cnt, progress );
			cout << buf;
			cout << flush;
		}
		prev_progress = progress;

		snprintf( buf, 100, "%x", i );
		// BigNum::operator = ( std::string )
		bn = buf;

		if ( bn.ToInt32() != i )
		{
			if ( break_on_error )
			{
				cout << endl;
				return false;
			}	
			error_cnt++;
		}
	}

	cout << endl;

	if ( error_cnt )
		return false;
	else
		return true;
}


bool
test_from_string_to_string( uint32_t from, uint32_t to, uint32_t step, bool break_on_error )
{
	size_t error_cnt     = 0;          // pocet chyb
	float  prev_progress = -1;
	float  progress      = 0;
	char   buf[ 100 ];
	size_t i;
	BigNum bn;

	for ( i = from ; i < ( to - step ) ; i += step )
	{
		progress = ( float ) ( ( ( i - from ) / ( double ) ( to - from ) ) * 100 );
		if ( progress != prev_progress )
		{
			snprintf( buf, 100, "\rPocet chyb: %d, prubeh %.4f %%", error_cnt, progress );
			cout << buf;
			cout << flush;
		}
		prev_progress = progress;

		snprintf( buf, 100, "%x", i );
		// BigNum::operator = ( std::string )
		bn = buf;

		if ( bn.ToHexString() != buf )
		{
			if ( break_on_error )
			{
				cout << endl;
				return false;
			}
			error_cnt++;
		}
	}

	cout << endl;

	if ( error_cnt )
		return false;
	else
		return true;
}


bool
test_get_size()
{
	size_t error_cnt = 0;
	BigNum bn;

	if ( bn.GetSize() != 0 )
	{
		error_cnt++;
	}
	if ( bn.GetActiveSize() != 0 )
	{
		error_cnt++;
	}

	bn = 0xFFFF;
	if ( bn.GetSize() != 1 )
	{
		error_cnt++;
	}
	if ( bn.GetActiveSize() != 1 )
	{
		error_cnt++;
	}

	bn = 0xFFFFFFFF;
	if ( bn.GetSize() != 2 )
	{
		error_cnt++;
	}
	if ( bn.GetActiveSize() != 2 )
	{
		error_cnt++;
	}
	cout << "Test na GetSize() dokoncen, pocet chyb: " << error_cnt << endl;

	if ( error_cnt )
		return false;
	return true;
}


bool
test_division(
	uint32_t u_from, uint32_t u_to, uint32_t u_step,
	uint32_t v_from, uint32_t v_to, uint32_t v_step,
	bool break_on_error)
{
	size_t   error_cnt = 0;
	float    prev_progress = -1;
	float    progress      = 0;
	char     buf[ 100 ];
	BigNum   bn_u;
	BigNum   bn_v;
	BigNum   bn_res;
	uint32_t u;
	uint32_t v;
	uint32_t res;

	for ( u = u_from ; u < ( u_from - u_step ) ; u += u_step )
	{
		progress = ( float ) ( ( ( u - u_from ) / ( double ) ( u_to - u_from ) ) * 100 );
		if ( progress != prev_progress )
		{
			snprintf( buf, 100, "\rPocet chyb: %d, prubeh %.4f %%", error_cnt, progress );
			cout << buf;
			flush( cout );
		}
		prev_progress = progress;

		bn_u = u;

		for ( v = v_from ; v < ( v_from - v_step ) ; v += v_step )
		{
			bn_v = v;

			// BigNum::operator / ( const BigNum& bn )
			bn_res = bn_u / bn_v;

			res = u / v;

			if ( bn_res.ToInt32() != res )
			{
				error_cnt++;
				if ( break_on_error )
				{
					cout << endl;
					return false;
				}
			}
		}
	}

	cout << endl;

	if ( error_cnt )
		return false;
	return true;
}


bool
test_modulo(
	uint32_t u_from, uint32_t u_to, uint32_t u_step,
	uint32_t v_from, uint32_t v_to, uint32_t v_step,
	bool break_on_error )
{
	size_t   error_cnt = 0;
	float    prev_progress = -1;
	float    progress      = 0;
	char     buf[ 100 ];
	BigNum   bn_u;
	BigNum   bn_v;
	BigNum   bn_res;
	uint32_t u;
	uint32_t v;
	uint32_t res;

	for ( u = u_from ; u < ( u_from - u_step ) ; u += u_step )
	{
		progress = ( float ) ( ( ( u - u_from ) / ( double ) ( u_to - u_from ) ) * 100 );
		if ( progress != prev_progress )
		{
			snprintf( buf, 100, "\rPocet chyb: %d, prubeh %.4f %%", error_cnt, progress );
			cout << buf;
			cout << flush;
		}
		prev_progress = progress;

		bn_u = u;

		for ( v = v_from ; v < ( v_from - v_step ) ; v += v_step )
		{
			bn_v = v;

			// BigNum::operator % ( const BigNum& bn )
			bn_res = bn_u % bn_v;

			res = u % v;

			if ( bn_res.ToInt32() != res )
			{
				error_cnt++;
				if ( break_on_error )
				{
					cout << endl;
					return false;
				}
			}
		}
	}

	cout << endl;

	if ( error_cnt )
		return false;
	return true;
}


//
// Test, ktery ziskava vstupni hodnoty ze souboru ve formatu:
//  a, b, c
//  a, b, c
//  .
//  .
//  .
//
//  Z cehoz vyplyva ze: a * b = c
//
//  Preji hezky den :)
//  
bool
test_multiplication_from_file( const string& filename, bool break_on_error )
{
	ifstream in_file( filename.c_str() );

	// Pokud se nepodarilo otevrit vstupni soubor
	if ( !in_file )
	{
		return false;
	}

	// Alokujeme buffer pro nacitani ze souboru
	char* in_cbuf = ( char* ) malloc( sizeof( char ) * 10240 );

	if ( !in_cbuf )
	{
		cerr << "Nepodarilo se otevrit soubor!" << endl;
		return false;
	}

	string buf;
	int    pos1, pos2;
	string str_a;
	string str_b;
	string str_c;
	BigNum a;
	BigNum b;
	BigNum c;

	while ( in_file.getline( in_cbuf, 10240 ) )
	{
		buf = in_cbuf;
		pos1 = buf.find( "," );
		pos2 = buf.find( ",", pos1 + 1 );
		
		str_a = buf.substr( 0, pos1 );
		str_b = buf.substr( pos1 + 2, pos2 - ( pos1 + 2 ) );
		str_c = buf.substr( pos2 + 2 );

		a = str_a;
		b = str_b;
		c = a * b;

		if ( c.ToHexString() != str_c )
		{
			cout << c.ToHexString() << " != " << str_c << endl;
		}
	}

	return true;
}


bool
test_modular_exponentiation_from_file( const string& filename, bool break_on_error )
{
	ifstream in_file( filename.c_str() );

	// Pokud se nepodarilo otevrit vstupni soubor
	if ( !in_file )
	{
		cerr << "Nepodarilo se otevrit soubor!" << endl;
		return false;
	}

	// Alokujeme buffer pro nacitani ze souboru
	char* in_cbuf = ( char* ) malloc( sizeof( char ) * 10240 );

	if ( !in_cbuf )
	{
		return false;
	}

	string buf;
	int    pos1, pos2, pos3;
	string str_b;     // base
	string str_e;     // exponent
	string str_m;     // modulo
	string str_r;     // result
	BigNum b;
	BigNum e;
	BigNum m;
	BigNum r;

	while ( in_file.getline( in_cbuf, 10240 ) )
	{
		buf = in_cbuf;
		pos1 = buf.find( "," );
		pos2 = buf.find( ",", pos1 + 1 );
		pos3 = buf.find( ",", pos2 + 1 );
		
		str_b = buf.substr( 0, pos1 );
		str_e = buf.substr( pos1 + 2, pos2 - ( pos1 + 2 ) );
		str_m = buf.substr( pos2 + 2, pos3 - ( pos2 + 2 ) );
		str_r = buf.substr( pos3 + 2 );

		b = str_b;
		e = str_e;
		m = str_m;
		r = str_r;
		r = BigNum::ModularExponentiation( b, e, m );

		if ( r.ToHexString() != str_r )
		{
			cout << r.ToHexString() << " != " << str_r << endl;
		}
	}

	return true;
}


int
main( int argc, char** argv )
{
	bool failed;
	int  ret = 0;
	test_get_size();

	//test_modular_exponentiation_from_file( "ModularExponentiationTest_512bits", false );
	test_multiplication_from_file( "MultiplyTest_2048bits", false );

	cout << endl << "- test_modulo() -----------------------------------" << endl;
	failed = test_modulo(
		0x0,   0xFFFFFFFF, 0xE,
		0x01, 0xFF, 0x1,
		false );
	if ( failed )
		ret = 1;
	cout << "=====================================================" << endl << endl;
	
	cout << "- test_division() -----------------------------------" << endl;
	failed = test_division(
		0x0,   0xFFFFFFFF, 0xE,
		0x01, 0xFF, 0x1,
		false );
	if ( failed )
		ret = 1;
	cout << "=====================================================" << endl << endl;
	
	cout << "- test_from_string_to_string() ----------------------" << endl;
	failed = test_from_string_to_string( 0x0, 0xFFFFFFFF, 0x1, false );
	if ( failed )
		ret = 1;
	cout << "=====================================================" << endl << endl;

	cout << "- test_from_string_to_uint32() ----------------------" << endl;
	failed = test_from_string_to_uint32( 0x0, 0xFFFFFFFF, 0x1, false );
	if ( failed )
		ret = 1;
	cout << "=====================================================" << endl << endl;

	cout << "- test_from_uint32_to_string() ----------------------" << endl;
	failed = test_from_uint32_to_string( 0x0, 0xFFFFFFFF, 0x1, false );
	if ( failed )
		ret = 1;
	cout << "=====================================================" << endl << endl;

	cout << "- test_from_uint32_to_uint32() ----------------------" << endl;
	failed = test_from_uint32_to_uint32( 0x0, 0xFFFFFFFF, 0x1, false );
	if ( failed )
		ret = 1;
	cout << "=====================================================" << endl << endl;

	return 0;
}
