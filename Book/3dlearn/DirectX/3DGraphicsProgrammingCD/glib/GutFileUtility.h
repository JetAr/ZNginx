
void *GutLoadBinaryStream(const char *filename, unsigned int *size = NULL);
void GutReleaseBinaryStream(const void *buffer);

void *GutLoadFileStream(const char *filename, unsigned int *size=NULL);
void GutReleaseFileStream(const void *buffer);
