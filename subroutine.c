#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <dirent.h>
#include <errno.h>
#include "list.h"
#include "stringHelper.h"
#include "directoryHelper.h"
#include "xmlHelper.h"
#include "reporter.h"

void assertCorrectMainArgs(int argc, char **argv)
{
	if(argc == 3)
	{
		argv++;
		
		char *oivFilepath = *(argv+1);
		int oivFilepathLength = strlen(oivFilepath);
		
		if(oivFilepathLength < 5)
		{
			fputs("OIV Filepath (2nd argument) is invalid!", stderr);
			exit(EXIT_FAILURE);
		}
		else if(oivFilepath[oivFilepathLength - 1] != 'v' ||
				oivFilepath[oivFilepathLength - 2] != 'i' ||
				oivFilepath[oivFilepathLength - 3] != 'o')
		{
			fputs("Invalid file extension for 2nd argument! (specify .oiv)", stderr);
			exit(EXIT_FAILURE);
		}
		
		puts("\nIf there are any errors,\n"
		     "please make sure you restart your computer and have elevated rights.\n");
	}
	else
	{
		if(argc <= 1)
			fprintf(stderr, "Please specifiy the modpack path and 2nd a filepath for the resulting .oiv archive!");
		else if(argc == 2)
			fprintf(stderr, "Please also specify 2nd a filepath for the resulting .oiv archive!");
		else
			fprintf(stderr, "Too many arguments!");
		
		exit(EXIT_FAILURE);
	}
}

void welcomeUser(char *programTitle, char *author)
{
	SetConsoleTitle(programTitle);
	
	printf("\n[%s]\n\n"
		   "Discord: %s\n\n",
	       programTitle, author);
}

void genAssemblyContent_Internally(char *modpackPath, char *relativePath, char **resultFileContent, int isArchive)
{
	if(modpackPath == NULL)
		return;
	
	char *temp0 = NULL;
	char *temp1 = NULL;
	
	// By default, there is no slash at the beginning, but it is required to gather all directories on the filesystem.
	temp0 = safeStrCat(modpackPath, "\\");
	char *filesystemRelativePath = safeStrCat(temp0, relativePath);
	free(temp0);
	temp0 = NULL;
	
	// Read all folders as a list, depending on the current path location (relativePath).
	List* dirFolders = getDirectoryContentList(filesystemRelativePath, -1);
	
	// Read all files as a list, depending on the current path location (relativePath).
	List* dirFiles = getDirectoryContentList(filesystemRelativePath, 1);
	
	free(filesystemRelativePath);
	filesystemRelativePath = NULL;

	// Add all files (but not of .img or .rpf archives) to the assembly file content.
	for(int i = 0; i < sizeOf(dirFiles); i++)
	{
		// This formatted string is built up here (exmaple):
		// <add source="df78abdd-fd7e-4bfc-9105-aa3007b6382e\bass.dll">bass.dll</add>

		char *fileName = getValue(dirFiles, i);

		temp0 = safeStrCat("<add source=\"", relativePath);
		
		if(relativePath[0] != '\0')
		{
			temp1 = safeStrCat(temp0, "\\");
		}
		else
		{
			temp1 = safeStrCat(temp0, "");
		}

		// Prepare source attribute
		free(temp0);
		temp0 = NULL;
		temp0 = safeStrCat(temp1, fileName);
		free(temp1);
		temp1 = NULL;
		temp1 = safeStrCat(temp0, "\">");
		free(temp0);
		temp0 = NULL;
		
		// Add formatted string the assembly.xml file content
		temp0 = safeStrCat(*resultFileContent, temp1);
		free(*resultFileContent); // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< HERE ALREADY DID FREE temp !!
								  // 										   CUZ ASSIGNED *resultFileContent = temp
		*resultFileContent = NULL;
		*resultFileContent = temp0; // Save progress.
		temp0 = NULL;

		if(relativePath[0] != '\0')
		{
			if(!isArchive)
			{
				temp1 = safeStrCat(relativePath, "\\");
			}
			else
			{
				temp1 = safeStrCat("", "\\");
			}
		}
		else
		{
			temp1 = safeStrCat("", "");
		}

		// Prepare tag content between the two ones opening and closing.
		temp0 = safeStrCat(temp1, fileName);
		free(temp1);
		temp1 = NULL;
		free(fileName);
		fileName = NULL;
		
		temp1 = safeStrCat(temp0, "</add>"); // TODO

		// Add formatted string the assembly.xml file content
		temp0 = safeStrCat(*resultFileContent, temp1);
		free(*resultFileContent);
		*resultFileContent = NULL;
		*resultFileContent = temp0; // Save progress.
		temp0 = NULL;
	}

	// Check if this is a recursive call and if this is an archive => do not go any deeper then into directories (ignore them).
	if(!isArchive)
	{
		// Make a recursive call to all folders in order to add their files in the end.
		for(int i = 0; i < sizeOf(dirFolders); i++)
		{
			// This formatted string is built up here (exmaple):
			// <archive path="pc\data\maps\east\bronx_e.img" createIfNotExist="True" type="RPF7"> ... </archive>

			char *folderName = getValue(dirFolders, i);
			char *recursiveCallRelativePath = NULL;
			
			if(relativePath[0] != '\0')
			{
				temp0 = safeStrCat(relativePath, "\\");
				recursiveCallRelativePath = safeStrCat(temp0, folderName);
				
				free(temp0);
				temp0 = NULL;
			}
			else
			{
				recursiveCallRelativePath = folderName;
			}
			
			// Does the folder end on ".img" ? If so, then value 1 here. Otherwise 0.
			int isImgArchive = lastIndexOf(folderName, ".img", 0) != -1;
			
			if(isImgArchive) // Add files, mentioning they need to be put into a specific archive.
			{
				temp0 = safeStrCat("<archive path=\"", recursiveCallRelativePath);
				temp1 = safeStrCat(temp0, "\" createIfNotExist=\"True\" type=\"RPF7\">");
				free(temp0);
				temp0 = NULL;
				
				// Append archive XML string to the assembly.xml file content
				temp0 = safeStrCat(*resultFileContent, temp1);
				free(*resultFileContent);
				*resultFileContent = NULL;
				*resultFileContent = temp0; // Save progress.
				temp0 = NULL;
				
				genAssemblyContent_Internally(modpackPath, recursiveCallRelativePath, resultFileContent, 1);
				isArchive = 0;
				
				// Append archive XML end tag string to the assembly.xml file content
				temp0 = safeStrCat(*resultFileContent, "</archive>");
				free(*resultFileContent);
				*resultFileContent = NULL;
				*resultFileContent = temp0; // Save progress.
				temp0 = NULL;
			}
			else // Only add files (not in a archive, but folder then in GTA IV).
			{
				genAssemblyContent_Internally(modpackPath, recursiveCallRelativePath, resultFileContent, 0);
			}
			
			free(recursiveCallRelativePath);
			recursiveCallRelativePath = NULL;
		}
	}
	
	clearList(dirFiles);
	free(dirFiles);
	dirFiles = NULL;
	
	clearList(dirFolders);
	free(dirFolders);
	dirFolders = NULL;
}

void genAssemblyContent(char *modpackPath, char **resultFileContent)
{
	char *temp = NULL, *temp2 = NULL;
	
	// Init target string (just an empty string).
	*resultFileContent = calloc(sizeof(char), 1);
	
	// Ask user what name the modpack has and which version:
	char name[32];
	
	printf("Name of your modpack (max. 32 symbols): ");
	fgets(name, sizeof(name) * sizeof(char), stdin);
	fflush(stdin);
	puts("");
	
	// fscanf(FILE *stream, const char *format, ...)
	float version = 0.0f;
	int inputs = 0;
	
	do
	{
		printf("Version of your modpack: ");
		
		inputs = fscanf(stdin, "%f", &version);
		fflush(stdin);
		
		if(inputs != 1)
		{
			puts("Incorrect version format! Enter like this: 1.2, 2.1, 1.1 or 0.1 and so on:");
		}
		else if(version < 0 || version >= 10)
		{
			puts("Please enter a version between 0 (including) and 10 (excluded)");
		}
	}
	while(inputs != 1 || version < 0 || version >= 10);
	
	int bugCounter = 0;
	
	char *versionFormatted = calloc(sizeof(char), 4);
	
	sprintf(versionFormatted, "%.1f", version);
	
	char *majorVersion = calloc(sizeof(char), 2);
	*majorVersion = *versionFormatted; // So first char is the major version number, exact one digit.
	char *minorVersion = calloc(sizeof(char), 2);
	*minorVersion = versionFormatted[2]; // So the char after the dot (minor version number, exact one digit).
	
	// <?xml version="1.0" encoding="UTF-8"?> <package version="
	puts("");

	// OpenIV version is here 2.0 in <package version=...
	temp = safeStrCat("<?xml version=\"1.0\" encoding=\"UTF-8\"?><package version=\"2.0\" id=\"{8DC63569-721C-4552-8D71-470770B77E95}\" target=\"IV\"><metadata><name>", name);

	temp2 = safeStrCat(temp, "</name><version><major>");
	free(temp);
	temp = NULL;

	temp = safeStrCat(temp2, majorVersion);
	free(temp2);
	temp2 = NULL;

	temp2 = safeStrCat(temp, "</major><minor>");
	free(temp);
	temp = NULL;

	temp = safeStrCat(temp2, minorVersion);
	free(temp2);
	temp2 = NULL;

	temp2 = safeStrCat(temp, "</minor><tag>Stable</tag></version><author><displayName>");
	free(temp);
	temp = NULL;

	char *author = calloc(sizeof(char), 32);
	
	printf("Please now enter author of modpack (max. 32 symbols): ");
	fgets(author, sizeof(char) * 32, stdin);
	
	temp = safeStrCat(temp2, author);
	free(temp2);
	temp2 = NULL;

	// </displayName></author><description><![CDATA[
	
	temp2 = safeStrCat(temp, "</displayName></author><description><![CDATA[");
	free(temp);
	temp = NULL;

	char *description = calloc(sizeof(char), 32);
	
	printf("Please now enter a short description of modpack (max. 32 symbols): ");
	fgets(description, sizeof(char) * 32, stdin);
	
	temp = safeStrCat(temp2, description);
	free(temp2);
	temp2 = NULL;

	temp2 = safeStrCat(temp, "]]></description></metadata><colors><headerBackground useBlackTextColor=\"False\">$FF000000</headerBackground><iconBackground>$FF000000</iconBackground></colors><content>");
	free(temp);
	temp = NULL;

	temp = safeStrCat(*resultFileContent, temp2);
	free(temp2);
	temp2 = NULL;

	free(*resultFileContent);
	*resultFileContent = NULL;
	
	// Update file content:
	*resultFileContent = temp; // Work here again with latest updated version to build up the file content.
	
	// </content></package>

	genAssemblyContent_Internally(modpackPath, "", resultFileContent, 0);

	temp2 = safeStrCat(*resultFileContent, "</content></package>");

	free(temp); // Uncommented in order to remember NOT to call free after calling genAssemblyContent_Internally()
	// Why? Because temp was freed yet by the function. Otherwise you would free a dangling pointer => crash! (so bugfix here).
	temp = NULL;
	
	*resultFileContent = temp2;

	free(description);
	description = NULL;
	
	free(author);
	author = NULL;
	
	free(majorVersion);
	majorVersion = NULL;
	
	free(minorVersion);
	minorVersion = NULL;
	
	free(versionFormatted);
	versionFormatted = NULL;
}