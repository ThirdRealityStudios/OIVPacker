#ifndef PROJECT_OIV_SUBROUTINE_H
#define PROJECT_OIV_SUBROUTINE_H

/// @brief Welcomes the user on the console and sets the window title.
/// @param programTitle Variable reference from main.c after calling init().
/// @param author Variable reference from main.c after calling init().
void welcomeUser(char *programTitle, char *author);

/// @brief The same as assertExistingDirectories(), but for the main arguments.
/// @param argc Exact argument from main() function (just pass from main() here without changes).
/// @param argv Exact argument from main() function (just pass from main() here without changes).
void assertCorrectMainArgs(int argc, char **argv);

/// @brief Private, internal function (use genAssemblyContent() instead)!
///        Reads all directories and contained files and will return a string,
/// 	   suitable for writing directly the file assembly.xml .
///        If a directory contains the ending ".img" or ".rpf",
///		   such directory will be interpreted as the content for a GTA image file (e.g. buildings, cars, whatever..).
///		   Also, it will prompt you for necessary information about the modpack, such as the title, author.
///	@param modpackPath The path of your folder which represents your modpack / GTA IV folder structure.
/// @param relativePath Relative path to the modpackPath, e.g. additional subdirectories can be added to this string.
/// @param resultFileContent Reference to the pointer which will later point to the resulting assembly content when done executing.
/// @param isArchive Prevents a recursive call to add folders to an archive because they are not allowed in GTA IV archives.
void genAssemblyContent_Internally(char *modpackPath, char *relativePath, char **resultFileContent, int isArchive);

/// @brief Reads all directories and contained files and will return a string,
/// 	   suitable for writing directly the file assembly.xml .
///        If a directory contains the ending ".img" or ".rpf",
///		   such directory will be interpreted as the content for a GTA image file (e.g. buildings, cars, whatever..).
///		   Also, it will prompt you for necessary information about the modpack, such as the title, author.
///	@param modpackPath The path of your folder which represents your modpack / GTA IV folder structure.
/// @param resultFileContent Reference to the pointer which will later point to the resulting assembly content when done executing.
void genAssemblyContent(char *modpackPath, char **resultFileContent);

#endif