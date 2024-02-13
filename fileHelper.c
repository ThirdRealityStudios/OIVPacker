#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "types.h"
#include "reporter.h"
#include <string.h>

char* readFile(char *filepath, List *exceptSymbols)
{
	if(filepath == NULL)
	{
		fprintf(stderr, "Error: Passed NULL for argument \"filepath\" in function readFile()");
		exit(EXIT_FAILURE);
	}
	
	if(exceptSymbols == NULL)
	{
		fprintf(stderr, "Error: Passed NULL for argument \"exceptSymbols\" in function readFile() - "
						"if you do not want to use \"exceptSymbols\", then please pass an empty list instead!");
		exit(EXIT_FAILURE);
	}
	
	FILE *file = fopen(filepath, "r");

	if(file == NULL)
	{
		fprintf(stderr, "Fatal Error: Got NULL from variable \"file\" while opening file in function readFile()");
		
		exit(EXIT_FAILURE);
	}

	char *fileContent = NULL;

	List *charsRead = createEmptyList();

	int charRead = 0;
	char *charReadNullTerminated = NULL;

	do
	{
		charRead = fgetc(file);
		
		charReadNullTerminated = calloc(sizeof(char), 2);
		memcpy(charReadNullTerminated, &charRead, sizeof(char));

		if(charRead != -1 && !containsValue(exceptSymbols, charReadNullTerminated))
			appendValue(charsRead, charReadNullTerminated);
		
		free(charReadNullTerminated);
	}
	while(charRead != -1);
	
	fclose(file);
	
	fileContent = concatenateValues(charsRead, "");
	
	clearList(charsRead);
	free(charsRead);
	
	return fileContent;
}

int sizeOfFile(char *filepath)
{
	struct stat *fileStats = calloc(sizeof(struct stat), 1);
	
	FILE *file = fopen(filepath, "rb");
	
	if(file == NULL)
		return -1;
	
	int success = stat(filepath, fileStats);
	
	if(success != 0)
		return -2;
	
	int fileSize = fileStats -> st_size;
	
	fclose(file);
	file = NULL;
	
	return fileSize;
}

char* readBinaryFile(char *filepath)
{
	FILE *binaryFile = fopen(filepath, "rb");

	reportIfNULL(binaryFile, "binaryFile", "readBinaryFile()", "fopen()");
	
	int fileSize = sizeOfFile(filepath);
	
	if(fileSize < 0)
		return NULL;
	
	char *buffer = calloc(sizeof(char), fileSize);
	
	reportIfNULL(buffer, "buffer", "readBinaryFile()", "calloc()");
	
	for(int i = 0; i < fileSize; i++)
	{		
		buffer[i] = fgetc(binaryFile);
	}

	fclose(binaryFile);
	binaryFile = NULL;
	
	return buffer;
}

int copyFileBinary(char *filepathDestination, char *filepathSource)
{
	char *sourceFileBinaryContent = readBinaryFile(filepathSource);
	FILE *destinationFile = fopen(filepathDestination, "wb");
	
	if(destinationFile == NULL)
	{
		return -1;
	}
	
	int destinationFileSize = sizeOfFile(filepathSource);
	
	if(destinationFileSize < 0)
		return -1;
	
	fwrite(sourceFileBinaryContent, 1, destinationFileSize, destinationFile);
	
	fclose(destinationFile);
	destinationFile = NULL;
	free(sourceFileBinaryContent);
	sourceFileBinaryContent = NULL;
	
	return 0;
}