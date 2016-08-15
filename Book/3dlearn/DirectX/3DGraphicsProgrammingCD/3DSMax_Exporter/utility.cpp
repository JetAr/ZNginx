#include <windows.h>

#define MOD_ADLER 65521

// checksum generator
// copied from wikipedia
// http://en.wikipedia.org/wiki/Adler-32

UINT32 adler(BYTE *data, int len) 
{
	UINT32 a = 1, b = 0;

	while (len) 
	{
		size_t tlen = len > 5550 ? 5550 : len;
		len -= tlen;

		do 
		{
			a += *data++;
			b += a;
		} while (--tlen);

		a = (a & 0xffff) + (a >> 16) * (65536-MOD_ADLER);
		b = (b & 0xffff) + (b >> 16) * (65536-MOD_ADLER);
	}

	/* It can be shown that a <= 0x1013a here, so a single subtract will do. */
	if (a >= MOD_ADLER)
		a -= MOD_ADLER;

	/* It can be shown that b can reach 0xfff87 here. */
	b = (b & 0xffff) + (b >> 16) * (65536-MOD_ADLER);

	if (b >= MOD_ADLER)
		b -= MOD_ADLER;

	return (b << 16) | a;
}

UINT32 GenerateChecksum(void *data, int len)
{
	return adler((BYTE*)data, len);
}

UINT32 GenerateChecksum(const char *str)
{
	size_t len = strlen(str);
	return adler((BYTE*)str, len);
}

unsigned short GenerateHash_16bits(void *data, int len)
{
	unsigned char *key = (unsigned char *) data;
	unsigned int hash = 0;
	size_t i;

	for (i = 0; i < len; i++) {
		hash += key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	unsigned short hash_16bits = hash & 0xffff;

	return hash_16bits;
}