#include <stdio.h>
#include <string.h>

#include "qbc.cpp"

static const char *str = "\\cg_predictItems\\1\\cl_anonymous\\0\\cl_guid\\86D61B55F8F39E0596483BDD5D2B259E\\color1\\4\\color2\\5\\handicap\\100\\headmodel\\sarge\\model\\sarge\\name\\UnnamedPlayer\\rate\\25000\\sex\\male\\snaps\\40\\team_headmodel\\*james\\team_model\\james\\cg_scorePlums\\1\\cg_smoothClients\\0\\teamoverlay\\1\\protocol\\71\\qport\\45210\\challenge\\520506651\\client\\Q3 1.32e";

void main( void )
{
	unsigned char buf1[1024];
	unsigned char buf2[1024];
	qbc bc1;
	qbc bc2;
	int n;

	bc1.init( qbc::encode, buf1, sizeof( buf1 ) );
	bc1.encode_string( (unsigned char*) str );
	bc1.encode_flush();
	n = bc1.length();

	printf( "string encoded to %i bytes\n", n );

	if ( n >= 0 )
	{
		bc2.init( qbc::decode, buf1, sizeof( buf1 ) );
		bc2.decode_string( buf2, sizeof( buf2 ) );

		if ( strcmp( (char*)buf2, (char*)str ) == 0 )
		{
			printf( "string match!\n" );
		}
		else
		{
			printf( "string does not match!\n" );
		}
	}
}
