#include <stdio.h>
#include <stdlib.h>

void *GutLoadBinaryStream(const char *filename, unsigned int *size)
{
	FILE *file = fopen(filename, "rb");
	if ( file==NULL )
		return NULL;

	unsigned int file_size = 0;

	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	unsigned char *buffer = new unsigned char[file_size];

	if ( buffer==NULL )
	{
		fclose(file);
		return NULL;
	}

	fread(buffer, file_size, 1, file);
	fclose(file);

	if ( size ) *size = file_size;

	return buffer;
}

void GutReleaseBinaryStream(const void *buffer)
{
	delete [] buffer;
}

void *GutLoadFileStream(const char *filename, unsigned int *size=NULL)
{
	return GutLoadBinaryStream(filename, size);
}

void GutReleaseFileStream(const void *buffer)
{
	GutReleaseBinaryStream(buffer); 
}
