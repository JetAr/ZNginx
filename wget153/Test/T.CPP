#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define HEXD2ASC(x) (((x) < 10) ? ((x) + '0') : ((x) - 10 + 'A'))

typedef union hdr
{
	char align[8];	/* To force sizeof(header).  */
	
	struct
    {
		union hdr *next;		/* For chaining headers.  */
		char *deep;		/* For stack depth measure.  */
    } h;
} header;

char *
encode_string (const char *s)
{
	const char *b;
	char *p, *res;
	int i;

	b = s;
	for (i = 0; *s; s++, i++)
	{
		if (strchr (" <>\"%{}|\\^[]`\033", *s))
			i += 2; /* Two more characters (hex digits) */
	}

	res = (char *)malloc (i + 1);
	s = b;

	for (p = res; *s; s++)
	{
		if (strchr (" <>\"%{}|\\^[]`\033", *s))
		{
			const unsigned char c = *s;

			*p++ = '%';

			*p++ = HEXD2ASC (c >> 4);
			*p++ = HEXD2ASC (c & 0xf);
		}
		else
		{
			*p++ = *s;
		}
	}

	*p = '\0';

	return res;
}

void test1()
{
	//2
	printf("%d\n",sizeof(short));
	//4
	printf("%d\n",sizeof(int));
	//4
	printf("%d\n",sizeof(long));
	//8
	printf("%d\n",sizeof(double));
	printf("%d\n",sizeof(header));
}

int main()
{
	// <>\"%{}|\\^[]`\033
	//printf("%s",encode_string("http://www.edu.cn/<>%\t[]"));
	printf("%s",encode_string("http://www.edu.cn/<>%\t[]"));

	return 0;
}
