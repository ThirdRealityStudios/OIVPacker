#ifndef PROJECT_OIV_FILEHELPER_H
#define PROJECT_OIV_FILEHELPER_H

#include "types.h"

/// @brief Reads all bytes of all file and returns it as a complete string.
///		   (simply like opening notepad.exe and seeing what's inside the file).
///	@param filepath The relative (to the .exe) path or absolute path to the file.
/// @param exceptSymbols A list of symbols to ignore
///						 (not to include in the returned string)
/// 					 while reading the content of the file.
///        			     If you do not want "exceptSymbols", then pass an empty list here.
/// @return The string, contained by the given file (call free() later).
char* readFile(char *filepath, List *exceptSymbols);

/// @brief Tells you the size of a file.
/// @param filepath The relative (to the .exe) path or absolute path to the file.
/// @return Size of the file.
int sizeOfFile(char *filepath);

/// @brief Reads a file binary.
/// @param filepath The relative (to the .exe) path or absolute path to the file.
/// @return All bytes read from the file (call free() later).
char* readBinaryFile(char *filepath);

/// @brief Copies a file in binary mode, which is especially useful and reliable for non-text-documents.
/// 	   E.g. .png, .img, .zip and so on..
/// @param filepathDestination The target filepath
/// @param filepathSource The source filepath
/// @return 
int copyFileBinary(char *filepathDestination, char *filepathSource);

#endif