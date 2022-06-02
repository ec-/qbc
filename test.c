#include <stdio.h>
#include <string.h>

#include "qbc.c"

static const char *str = "\\cg_predictItems\\1\\cl_anonymous\\0\\cl_guid\\86D61B55F8F39E0596483BDD5D2B259E\\color1\\4\\color2\\5\\handicap\\100\\headmodel\\sarge\\model\\sarge\\name\\UnnamedPlayer\\rate\\25000\\sex\\male\\snaps\\40\\team_headmodel\\*james\\team_model\\james\\cg_scorePlums\\1\\cg_smoothClients\\0\\teamoverlay\\1\\protocol\\71\\qport\\45210\\challenge\\520506651\\client\\Q3 1.32e";

void main( void )
{
	unsigned char buf1[1024];
	unsigned char buf2[1024];
	bitcodec_t bc1;
	bitcodec_t bc2;
	int n;

	bc_init_encode( &bc1, buf1, sizeof( buf1 ) );
	bc_encode_string( &bc1, (unsigned char*) str );
	bc_encode_flush( &bc1 );
	n = bc_length( &bc1 );

	printf( "string encoded from %i to %i bytes\n", strlen( str ) + 1, n );

	if ( n >= 0 )
	{
		bc_init_decode( &bc2, buf1, sizeof( buf1 ) );
		bc_decode_string( &bc2, buf2, sizeof( buf2 ) );

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
