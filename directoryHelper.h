#ifndef PROJECT_OIV_DIRECTORYHELPER_H
#define PROJECT_OIV_DIRECTORYHELPER_H

#include "list.h"

/// @brief Returns all elements of a given directory as a list.
/// 	   The returned list will contain files and folders,
///        depending on the mode specified.
/// @param directoryPath Self-explaining.
/// @param mode Specify if files only (any value > 0), folders only (any value < 0) or both (0) should be contained.
/// @return List of directory elements, relative to your specified path "directoryPath".
List* getDirectoryContentList(char *directoryPath, int mode);

/// @brief Copies the content of a directory into another directory.
///		   If the target directory does not exist or other directories along the target path,
///		   then those will be created automatically.
/// @param targetDir Target directory
/// @param sourceDir Source directory
void copyDir(char *targetDir, char *sourceDir);

/// @brief Gets you the shortened directory path for the given level.
///		   E.g. you have "C:\Users\myAccount\" for argument "directoryPath"
///        and 0 for argument "level", which returns "C:\" only.
///		   If you have "level" = 1, it will return "C:\Users".
///		   If you have "level" = 2, it will return "C:\Users\myAccount\",
///		   so the full path.
/// 	   If you set "level" = 3, it will still return "C:\Users\myAccount\"
///		   and therefore there will be no change anymore by increasing "level"
///		   in such a case.
/// @param directoryPath Directory path
/// @param level Desired deepness or level of your directory path string
/// @return Directory path with the desired deepness / level (remember to call free() later for the returned string!).
char* getSubdirectoryPath(char *directoryPath, int level);

/// @brief Creates a directory for the given path.
///		   What makes this function safe and easy to use
/// 	   is the fact that _mkdir_safe() will create all
///        folders missing along the path.
///		   This is the main difference against default _mkdir().
/// @param Directory path
/// @return errno code for the last directory created
///         (the deepest in the path which is usually also the most important one to know the errno of).
int _mkdir_safe(char *directoryPath);

#endif