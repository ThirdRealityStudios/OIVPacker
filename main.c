#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <Windows.h>
#include <assert.h>
#include "subroutine.h"
#include "stringHelper.h"
#include "xmlHelper.h"
#include "fileHelper.h"
#include "list.h"
#include "test/stringHelperTest.h"
#include "test/listTest.h"
#include "test/directoryHelperTest.h"
#include "test/xmlHelperTest.h"
#include <winbase.h>
#include <Lmcons.h>
#include "internetHelper.h"
#include "directoryHelper.h"

// gcc ./main.c ./list.c ./subroutine.c ./stringHelper.c ./fileHelper.c ./xmlHelper.c ./directoryHelper.c ./reporter.c ./internetHelper.c ./test/stringHelperTest.c ./test/listTest.c ./test/directoryHelperTest.c ./test/xmlHelperTest.c -o main.exe -lws2_32

char *programTitle = "OIV Tools: Packer";
char *author = "https://discord.gg/tMkHnXVT";
char *modpackPath = NULL;
char *oivFilepath = NULL;

char *userName = NULL; // The current user name running this program.
char *appdataDirectoryPath = NULL; // The %appdata% folder, assumed on C: drive.
char *localDirectoryPath = NULL; // The directory path of %appdata%/Local/

char *temp = NULL, *temp2 = NULL; // for random, unrelated string operations.

/// @brief Checks whether the hardcoded product key is still valid via internet.
/// @param localDirectoryPath Path to your Local folder in %appdata% (a program folder will be created to save the entered key).
void activateProduct(char *localDirectoryPath)
{
	char *oivtoolsDirectory = safeStrCat(localDirectoryPath, "OIVTools\\");
	
	_mkdir_safe(oivtoolsDirectory);
	
	char *oivtoolsKeyFilePath = safeStrCat(oivtoolsDirectory, "activation.dat");
	
	char productKeyHardcoded[] = "CD87T-HFP4C-V7X7H-8VY68-W7D7M";
	
	FILE *keyFile = fopen(oivtoolsKeyFilePath, "r");
	
	// Product is valid for 1 year.
	struct tm *productActivationLossDateTm = calloc(sizeof(struct tm), 1);
	productActivationLossDateTm -> tm_mday = 30;
	productActivationLossDateTm -> tm_mon = 1; // Feb
	productActivationLossDateTm -> tm_year = 2025 - 1900; // Always subtract 1900 from the actual year.
	productActivationLossDateTm -> tm_hour = 23;
	productActivationLossDateTm -> tm_min = 59;
	
	time_t productActivationLossDate = mktime(productActivationLossDateTm);
	
	List *ipv4List = createEmptyList();
	
	appendValue(ipv4List, "131.173.16.220");
	appendValue(ipv4List, "130.149.7.7");
	
	// rwth-aachen.de
	appendValue(ipv4List, "134.130.5.17");
	appendValue(ipv4List, "134.130.5.17");
	appendValue(ipv4List, "137.226.119.25");
	
	// NIST, Gaithersburg, Maryland
	appendValue(ipv4List, "129.6.15.27");
	appendValue(ipv4List, "129.6.15.28");
	appendValue(ipv4List, "129.6.15.29");
	appendValue(ipv4List, "129.6.15.30");
	
	// WWV, Fort Collins, Colorado
	appendValue(ipv4List, "132.163.97.1");
	appendValue(ipv4List, "132.163.97.2");
	appendValue(ipv4List, "132.163.97.3");
	appendValue(ipv4List, "132.163.97.4");
	
	// NIST, Boulder, Colorado
	appendValue(ipv4List, "132.163.96.1");
	appendValue(ipv4List, "132.163.96.2");
	appendValue(ipv4List, "132.163.96.3");
	appendValue(ipv4List, "132.163.96.4");

	time_t liveTime = -1;
	
	char *currentIPAddress = NULL;
	
	// Use all IPv4 addresses to aquire the current
	// live time via a NTP server.
	// This is because there can always be a server down,
	// or imagine this software is older yet and almost all servers are down.
	// Hence, it is important to use suitable amount of IP addresses.
	for(int i = 0; i < sizeOf(ipv4List) && liveTime == -1; i++)
	{
		currentIPAddress = getValue(ipv4List, i);
		
		liveTime = getLiveTime(currentIPAddress);
		
		if(liveTime == -1)
		{
			fputc('.', stdout); // illustrate some progress because
		}					    // could not connect to the NTP server directly..
		
		free(currentIPAddress);
		currentIPAddress = NULL;
	}
	
	if(liveTime == -1)
	{
		fputc('\n', stdout);
		fputc('\n', stdout);
	}
	
	clearList(ipv4List);
	free(ipv4List);
	ipv4List = NULL;
	
	if(liveTime == -1) // could not fetch the current internet live time (most likely user has no internet or NTP server is down?)
	{
		fputs("Activation or validation of your product license failed.\n"
			  "Please check your internet connection! (Hence, exiting now..)\n", stdout);

		exit(EXIT_FAILURE);
	}
	
	// See if the product key is still valid (no deep comparison here for the key itself, just checking for expiration which is also hardcoded).
	// If invalid, the customer needs to buy a new software with the key together.
	if(liveTime > productActivationLossDate)
	{
		fputs("Activation failed.\n"
			  "Your product key has expired (1 year is over).\n"
			  "Please get a new licensed software, e.g. write a simple message via Discord.\n"
			  "The Discord server is: https://discord.gg/tMkHnXVT", stdout);
	}
	
	if(keyFile == NULL) // Software was not activated yet.
	{
		char *productKeyEntered = calloc(sizeof(productKeyHardcoded), 1);
		
		do
		{
			fputs("Please enter your product key below for activation (internet required):\n> ", stdout);
			fgets(productKeyEntered, sizeof(productKeyHardcoded), stdin);
			fflush(stdin);
			
			fputs("\n", stdout);
			
			if(equals(productKeyEntered, productKeyHardcoded))
			{
				printf("Activation successful. Thank you for choosing %s!\n", programTitle);
				
				keyFile = fopen(oivtoolsKeyFilePath, "wt"); // Save the key to a file (uncritical as it serves only to remember the entered key => key manipulation in the file will cause corrupt software on startup => delete file in such a case).
				
				fputs(productKeyHardcoded, keyFile);
				
				fclose(keyFile);
				
				free(productKeyEntered);
				productKeyEntered = NULL;
				
				free(oivtoolsDirectory);
				oivtoolsDirectory = NULL;
				
				break;
			}
			
			fputs("Your product key entered was invalid.\n", stdout);
		}
		while(1);
	}
	else // Software was activated yet (does not mean the key in the file is valid! So check that here..).
	{
		char keyReadFromFile[sizeof(productKeyHardcoded)] = {'\0'};
		fscanf(keyFile, "%s", keyReadFromFile);
		fclose(keyFile);

		// Abort the program if the key was manipulated or for whatever reasons..
		if(!equals(keyReadFromFile, productKeyHardcoded))
		{
			// This issue might appear maybe because of an old installation of the OIVTools,
			// so delete "activation.dat" in %appdata%/Local/OIVTools/
			remove(oivtoolsKeyFilePath);

			fputs("Your product key is invalid.\n"
				  "If you received a new license and this error appears,\n"
				  "just restart the program (tried to delete files from old installations).\n"
				  "If it still doesn't work then anymore,\n"
				  "do not hesitate to write a message on Discord here in a support channel:\n"
				  "https://discord.gg/tMkHnXVT\n"
				  "In such a case we will most likely find a simple solution.\n"
				  "When writing to the support channel,\n"
				  "please make sure to provide sufficient screenshots that show the problem briefly.\n"
				  "Otherwise it might be necessary to start a screenshare in a separate Discord call if screenshots\n"
				  "do not describe the problem sufficiently (depends on if you want it then, but unnecessary mess can be avoided definitively).\n", stdout);

			exit(EXIT_FAILURE);
		}

		puts("Product status: activated and valid!");
	}
	
	free(oivtoolsKeyFilePath);
	oivtoolsKeyFilePath = NULL;
	
	char productActivationLossDateFormatted[100];
	
	strftime(productActivationLossDateFormatted, sizeof(productActivationLossDateFormatted), "%m/%d/%Y midnight (mm/dd/yyyy)", productActivationLossDateTm);
	
	fprintf(stdout, "Your product key and software is valid until: %s\n", productActivationLossDateFormatted);
	
	free(productActivationLossDateTm);
	productActivationLossDateTm = NULL;
}

void preInit()
{
	DWORD maxUserNameLength = UNLEN+1; // inclusive NULL-byte.
	
	userName = calloc(sizeof(char), maxUserNameLength);
	
	int userNameRetrievalSuccess = GetUserNameA(userName, &maxUserNameLength) != 0;
	
	// The user name is important for the runtime.
	// Hence, without, the program will exit.
	if(!userNameRetrievalSuccess)
	{
		fputs("Could not retrieve local user name (Hence, exiting now..)\n", stdout);
		
		exit(EXIT_FAILURE);
	}
	
	// Only used for temporary string calculations to determine the %appdata% directory path..
	temp = safeStrCat("C:\\Users\\", userName);
	
	// This is the usual %appdata% directory path now (on 99.99% of all Windows systems).
	appdataDirectoryPath = safeStrCat(temp, "\\AppData\\");
	
	localDirectoryPath = safeStrCat(appdataDirectoryPath, "Local\\");
	
	free(temp);
	temp = NULL;
}

void init(int argc, char **argv)
{
	argv++;

	modpackPath = *argv++;
	oivFilepath = *argv;

	modpackPath = _fullpath(NULL, modpackPath, 0);
	oivFilepath = _fullpath(NULL, oivFilepath, 0);
}

void testModules()
{
	testStringHelper();
	puts("");
	testList();
	puts("");
	testDirectoryHelper();
	puts("");
	testXmlHelper();
}

/// @brief The entry point of this program.
///		   The program assumes the user account is located on the C: drive.
///		   If not, the program will not work.
int main(int argc, char **argv) // TODO Save activation file in %appdata%/Local/ThirdRealityStudios/OIVPacker
{
	preInit();
	
	// Example usage: ./main.exe "./Setup 1.oiv" "./myModpack/"
	
	// testModules(); // Only uncomment if there are errors.
	
	welcomeUser(programTitle, author);
	
	activateProduct(localDirectoryPath); // Activation check before use.
	
	assertCorrectMainArgs(argc, argv);
	// From here, it is safe to use main args now as intended.
	
	init(argc, argv); // do NOT change order! Leave it here after assertCorrectMainArgs(argc, argv) check..

	// Ignore comment, only relevant for dev
	// ./7z.exe x -y ./trash/test.oiv -o"C:\Users\Konto\Desktop\OIVReader\trash\"
	
	char tempWorkingDir[] = "0b84f2dc-3d44-4fae-b174-da372fa341d2";
	_mkdir_safe(tempWorkingDir);
	
	char *tempWorkingDirOpened = safeStrCat(tempWorkingDir, "\\");
	char *assemblyXmlFilepath = safeStrCat(tempWorkingDirOpened, "assembly.xml");
	
	FILE *assemblyXmlFile = fopen(assemblyXmlFilepath, "wt");
	
	// Generate suitable assembly.xml file content for the specified folder (first program argument)
	char *assemblyXmlFileContent = NULL;
	genAssemblyContent(modpackPath, &assemblyXmlFileContent);
	
	// Write resulted XML string into the file called "assembly.xml".
	fputs(assemblyXmlFileContent, assemblyXmlFile);
	fflush(assemblyXmlFile);
	fclose(assemblyXmlFile);
	assemblyXmlFile = NULL;
	
	char *contentFolderPath = safeStrCat(tempWorkingDirOpened, "content");
	
	// Create "content" folder:
	_mkdir_safe(contentFolderPath);
	
	char *contentDirOpened = safeStrCat(contentFolderPath, "\\");
	char *placeholderFilepath = safeStrCat(contentDirOpened, "PLACEHOLDER");
	
	// Create file "PLACEHOLDER" (without file extension) in folder "content":
	FILE *placeholderFile = fopen(placeholderFilepath, "wt");
	fputs("This file is required. DO NOT REMOVE!", placeholderFile);
	fflush(placeholderFile);
	fclose(placeholderFile);
	placeholderFile = NULL;
	
	// Copy icon.png to the temp working dir
	char pngFilename[] = "icon.png";
	char *pngFileCopyFilepath = safeStrCat(tempWorkingDirOpened, "icon.png");
	copyFileBinary(pngFileCopyFilepath, pngFilename);
	
	// Copy all modpack files into folder "content":
	copyDir(contentFolderPath, modpackPath);
	
	// Zip files into .zip archive
	char *zipFilepath = safeStrCat("", oivFilepath);
	int zipFilepathLength = strlen(zipFilepath);
	zipFilepath[zipFilepathLength - 1] = 'p';
	zipFilepath[zipFilepathLength - 2] = 'i';
	zipFilepath[zipFilepathLength - 3] = 'z';
	
	temp = safeStrCat(".\\7z a -m0=Deflate \"", zipFilepath);
	temp2 = safeStrCat(temp, "\" \".\\");
	free(temp);
	temp = NULL;
	
	temp = safeStrCat(temp2, tempWorkingDir);
	free(temp2);
	temp2 = NULL;
	
	temp2 = safeStrCat(temp, "\"");
	
	// A call to 7z.exe is being made via console, e.g. like this one here:
	// .\7z a -m0=Deflate "archive.zip" .\0b84f2dc-3d44-4fae-b174-da372fa341d2\*
	char *_7zipArchiveCommand = safeStrCat(temp2, "\\*");
	
	free(temp2);
	temp2 = NULL;
	
	printf("Creating archive: %s\n", _7zipArchiveCommand);
	system(_7zipArchiveCommand);
	
	free(_7zipArchiveCommand);
	_7zipArchiveCommand = NULL;
	
	// Rename .zip to .oiv
	// Reminder: I needed to create a zip archive first,
	// because otherwise 7zip would not be to able to create one.
	// Hence, I only change the file extension here to .oiv in order for OpenIV Package Installer
	// as it is the preferred file extension (anyway, it is still of course a .zip archive technically).
	if(rename(zipFilepath, oivFilepath) != 0)
	{
		fputs("Error: could not rename .zip archive to .oiv! Please run as admin!", stderr);
	}
	
	// TODO Here: Remove files and folder after creating .oiv archive
	
	free(pngFileCopyFilepath);
	pngFileCopyFilepath = NULL;
	
	free(zipFilepath);
	zipFilepath = NULL;
	
	free(placeholderFilepath);
	placeholderFilepath = NULL;
	
	free(contentDirOpened);
	contentDirOpened = NULL;
	
	free(tempWorkingDirOpened);
	tempWorkingDirOpened = NULL;
	
	free(contentFolderPath);
	contentFolderPath = NULL;
	
	free(assemblyXmlFilepath);
	assemblyXmlFilepath = NULL;
	
	free(assemblyXmlFileContent);
	assemblyXmlFileContent = NULL;

	free(localDirectoryPath);
	localDirectoryPath = NULL;
	
	free(appdataDirectoryPath);
	appdataDirectoryPath = NULL;
	
	free(userName);
	userName = NULL;
	
	free(modpackPath);
	modpackPath = NULL;

	fputs("Exiting program (done, everything successful, no errors)\n", stdout);

	return EXIT_SUCCESS;
}