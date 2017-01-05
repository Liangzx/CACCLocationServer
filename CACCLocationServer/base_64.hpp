#ifndef _BASE_64_HPP_
#define _BASE_64_HPP_

typedef unsigned char BYTE;

#include <cstddef>

class base_64 {
public:
	size_t decode(char *pDest, const char *pSrc, size_t srclen);
	size_t encode(char *pDest, const char *pSrc, size_t srclen);
private:
	BYTE decode_getbyte(char c);
	char encode_getchar(BYTE num);
};
size_t base_64::decode(char * pDest, const char * pSrc, size_t srclen)
{
	BYTE input[4];
	size_t i, index = 0;
	for (i = 0; i < srclen; i += 4)
	{
		//byte[0]
		input[0] = decode_getbyte(pSrc[i]);
		input[1] = decode_getbyte(pSrc[i + 1]);
		pDest[index++] = (input[0] << 2) + (input[1] >> 4);

		//byte[1]
		if (pSrc[i + 2] != '=')
		{
			input[2] = decode_getbyte(pSrc[i + 2]);
			pDest[index++] = ((input[1] & 0x0f) << 4) + (input[2] >> 2);
		}

		//byte[2]
		if (pSrc[i + 3] != '=')
		{
			input[3] = decode_getbyte(pSrc[i + 3]);
			pDest[index++] = ((input[2] & 0x03) << 6) + (input[3]);
		}
	}

	//null-terminator
	pDest[index] = 0;
	return index;
}

size_t base_64::encode(char * pDest, const char * pSrc, size_t srclen)
{
	BYTE input[3], output[4];
	size_t i, index_src = 0, index_dest = 0;
	for (i = 0; i < srclen; i += 3)
	{
		//char [0]
		input[0] = pSrc[index_src++];
		output[0] = (BYTE)(input[0] >> 2);
		pDest[index_dest++] = encode_getchar(output[0]);

		//char [1]
		if (index_src < srclen)
		{
			input[1] = pSrc[index_src++];
			output[1] = (BYTE)(((input[0] & 0x03) << 4) + (input[1] >> 4));
			pDest[index_dest++] = encode_getchar(output[1]);
		}
		else
		{
			output[1] = (BYTE)((input[0] & 0x03) << 4);
			pDest[index_dest++] = encode_getchar(output[1]);
			pDest[index_dest++] = '=';
			pDest[index_dest++] = '=';
			break;
		}

		//char [2]
		if (index_src < srclen)
		{
			input[2] = pSrc[index_src++];
			output[2] = (BYTE)(((input[1] & 0x0f) << 2) + (input[2] >> 6));
			pDest[index_dest++] = encode_getchar(output[2]);
		}
		else
		{
			output[2] = (BYTE)((input[1] & 0x0f) << 2);
			pDest[index_dest++] = encode_getchar(output[2]);
			pDest[index_dest++] = '=';
			break;
		}

		//char [3]
		output[3] = (BYTE)(input[2] & 0x3f);
		pDest[index_dest++] = encode_getchar(output[3]);
	}
	//null-terminator
	pDest[index_dest] = 0;
	return index_dest;
}

BYTE base_64::decode_getbyte(char c)
{
	if (c == '+')
		return 62;
	else if (c == '/')
		return 63;
	else if (c <= '9')
		return (BYTE)(c - '0' + 52);
	else if (c == '=')
		return 64;
	else if (c <= 'Z')
		return (BYTE)(c - 'A');
	else if (c <= 'z')
		return (BYTE)(c - 'a' + 26);
	return 64;
}

char base_64::encode_getchar(BYTE num)
{
	return
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789"
		"+/="[num];
}
#endif // !_BASE_64_HPP_



