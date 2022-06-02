#ifndef _QBC_C_
#define _QBC_C_

static const unsigned char bc_pmod[2][256] = {
{
	  1,  1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	 14, 15, 16, 17, 18, 19, 20, 21, 21, 22, 23, 24, 25, 26, 27, 27,
	 27, 28, 29, 30, 31, 32, 33, 34, 34, 35, 36, 37, 38, 39, 40, 41,
	 41, 42, 43, 44, 45, 46, 47, 48, 48, 49, 50, 51, 52, 53, 54, 54,
	 54, 55, 56, 57, 58, 59, 60, 61, 61, 62, 63, 64, 65, 66, 67, 68,
	 68, 69, 70, 71, 72, 73, 74, 75, 75, 76, 77, 78, 79, 80, 81, 81,
	 81, 82, 83, 84, 85, 86, 87, 88, 88, 89, 90, 91, 92, 93, 94, 95,
	 95, 96, 97, 98, 99,100,101,102,102,103,104,105,106,107,108,108,
	108,109,110,111,112,113,114,115,115,116,117,118,119,120,121,122,
	122,123,124,125,126,127,128,129,129,130,131,132,133,134,135,135,
	135,136,137,138,139,140,141,142,142,143,144,145,146,147,148,149,
	149,150,151,152,153,154,155,156,156,157,158,159,160,161,162,162,
	162,163,164,165,166,167,168,169,169,170,171,172,173,174,175,176,
	176,177,178,179,180,181,182,183,183,184,185,186,187,188,189,189,
	189,190,191,192,193,194,195,196,196,197,198,199,200,201,202,203,
	203,204,205,206,207,208,209,210,210,211,212,213,214,215,216,216
}, {
	 40, 40, 41, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51, 52, 53,
	 53, 54, 55, 56, 57, 58, 59, 60, 60, 61, 62, 63, 64, 65, 66, 67,
	 67, 67, 68, 69, 70, 71, 72, 73, 73, 74, 75, 76, 77, 78, 79, 80,
	 80, 81, 82, 83, 84, 85, 86, 87, 87, 88, 89, 90, 91, 92, 93, 94,
	 94, 94, 95, 96, 97, 98, 99,100,100,101,102,103,104,105,106,107,
	107,108,109,110,111,112,113,114,114,115,116,117,118,119,120,121,
	121,121,122,123,124,125,126,127,127,128,129,130,131,132,133,134,
	134,135,136,137,138,139,140,141,141,142,143,144,145,146,147,148,
	148,148,149,150,151,152,153,154,154,155,156,157,158,159,160,161,
	161,162,163,164,165,166,167,168,168,169,170,171,172,173,174,175,
	175,175,176,177,178,179,180,181,181,182,183,184,185,186,187,188,
	188,189,190,191,192,193,194,195,195,196,197,198,199,200,201,202,
	202,202,203,204,205,206,207,208,208,209,210,211,212,213,214,215,
	215,216,217,218,219,220,221,222,222,223,224,225,226,227,228,229,
	229,229,230,231,232,233,234,235,235,236,237,238,239,240,241,242,
	242,243,244,245,246,247,248,249,250,251,252,253,254,255,255,255
} };

// probability range, hardcoded
#define PR_BITS 8

// default probability
#define DEF_PROB ((1<<PR_BITS)/2) 

//renormalization thershold
#define RENORM_THRES  (1<<((32-PR_BITS)/2))

// range split function
#define MID_FUNC(LO,HI,PR) ( (LO) + ( ( ((HI)-(LO)) * ((PR)) ) >> (PR_BITS) ) )


typedef struct bitcodec_s {
	unsigned int range[2]; // [low,high]
	unsigned int code;
	unsigned char prob[ 1 << PR_BITS ];
	unsigned char *bufPtr;
	unsigned char *bufBase;
	int bufLen;
} bitcodec_t;


static void bc_init_probs( bitcodec_t *bc ) {
	memset( bc->prob, DEF_PROB, sizeof( bc->prob ) );
	bc->code = 0x0;
	bc->range[0] = 0x0;
	bc->range[1] = 0xFFFFFFFF / ((1<<PR_BITS)-1);
}


void bc_init_encode( bitcodec_t *bc, unsigned char *buf, int len ) {

	bc_init_probs( bc );

	bc->bufBase = buf;
	bc->bufPtr = buf;
	bc->bufLen = len;
}


void bc_init_decode( bitcodec_t *bc, unsigned char *buf, int len ) {
	int i;

	bc_init_probs( bc );

	bc->bufBase = buf;
	bc->bufPtr = buf;
	bc->bufLen = len;

	for ( i = 0; i < 4 && bc->bufLen > 0; ++i, --bc->bufLen ) {
		bc->code = (bc->code << 8) | *bc->bufPtr++;
	}
}


int bc_encode_byte( bitcodec_t *bc, unsigned char value ) {
	int i, offset = 1; // initial context offset
	unsigned int code, range[2];
	unsigned char *prob;

	prob = bc->prob;
	code = bc->code;
	range[0] = bc->range[0];
	range[1] = bc->range[1];

	for ( i = 8-1; i >= 0; --i ) {
		const unsigned int split = MID_FUNC( range[0], range[1], prob[ offset ] );
		const int bit = (value >> i) & 1;

		range[bit] = split + (bit^1);

		 // entropy prediction
		prob[offset] = bc_pmod[bit][prob[offset]];

		// renormalization
		if ( ( range[0] ^ range[1] ) < RENORM_THRES ) {
			if ( bc->bufLen > 0 ) {
				*bc->bufPtr++ = range[1] >> 24; --bc->bufLen;
			} else {
				//return -1; // encode stream overrun
				value = -1; break; // encode stream overrun
			}
			range[1] = ( range[1] << 8 ) | 255;
			range[0] <<= 8;
		}
		// offset context
		offset += offset + bit;
	}

	bc->code = code;
	bc->range[0] = range[0];
	bc->range[1] = range[1];

	return value;
}


int bc_encode_string( bitcodec_t *bc, const unsigned char *str ) {
	while ( 1 ) {
		const int c = bc_encode_byte( bc, *str );
		if ( c <= 0 ) {
			return c;
		}
		str++;
	}
}


int bc_encode_flush( bitcodec_t *bc ) {
	int i;

	for ( i = 0; i < 4 && bc->bufLen > 0; ++i ) {
		if ( bc->bufLen > 0 ) {
			*bc->bufPtr++ = bc->range[0] >> 24; --bc->bufLen;
		} else {
			return -1; // encode stream overrun
		}
		bc->range[0] <<= 8;
	}
	bc->bufLen = 0; // so we can't flush it again
	return 0; //length();
}


int bc_length( bitcodec_t *bc ) {
	return (int) (bc->bufPtr - bc->bufBase);
}


int decode_byte( bitcodec_t *bc ) {
	int i, value = 0;
	int offset = 1; // initial context offset
	unsigned int code, range[2];
	unsigned char *prob;

	prob = bc->prob;
	code = bc->code;
	range[0] = bc->range[0];
	range[1] = bc->range[1];

	for ( i = 8-1; i >= 0; --i ) {
		const unsigned int split = MID_FUNC( range[0], range[1], prob[offset] );
		const int bit = ( code <= split ? 1 : 0 );

		range[bit] = split + (bit^1);

		// entropy prediction
 		prob[offset] = bc_pmod[bit][prob[offset]];

		// renormalization
		if ( ( range[0] ^ range[1] ) < RENORM_THRES ) {
			if ( bc->bufLen > 0 ) {
				code = (code << 8) | *bc->bufPtr++; --bc->bufLen;
			} else {
				value = -1; break; // decode stream overrun
				//return -1; // decode stream overrun
			}
			range[1] = ( range[1] << 8 ) | 255;
			range[0] <<= 8;
		}

		value = (value << 1) | bit;

		// offset context
		offset += offset + bit;
	}

	bc->code = code;
	bc->range[0] = range[0];
	bc->range[1] = range[1];

	return value;
}


// returns length of decoded string
int bc_decode_string( bitcodec_t *bc, unsigned char *str, int len ) {
	const unsigned char *base = str;

	while ( len > 0 ) {
		const int c = decode_byte( bc );
		if ( c <= 0 ) {
			break;
		}
		*str++ = c;
		len--;
	}

	*str = '\0';
	return (int) (str - base);
}

#endif // _QBC_C_