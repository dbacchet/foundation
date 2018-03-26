#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/// normalize the path format, converting all '\' to '/' and removing the trailing '/'
void normalize_path( char *path );

/// check if a file/dir exists
int exists( const char *path );

/// check if the given path is a file
int is_file( const char *path );

/// check if the given path is a directory
int is_dir( const char *path );


enum CreateDirResult {
	MKDIR_ERROR = 0,
	MKDIR_SUCCESS,
	MKDIR_EXISTS
};

/// create a directory given its path. Returns a CreateDirResult value
int create_dir( const char *path );
/// remove a directory given its path
int remove_dir( const char *path );


/// open file using a path; it creates all the intermediate directories if needed;
FILE* fopen_with_path( const char *path, const char *mode );



#ifdef __cplusplus
}
#endif
