#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>

#include <vector>
#include <string>

#ifdef _WIN32
	#include <direct.h>
#else
	#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include "fs_utils.h"
#include "string_utils.h"

// local functions
namespace 
{

}

// normalize the path format, converting all '\' to '/' and removing the trailing '/'
extern "C" void normalize_path( char *path )
{
	int len = strlen(path);
	for (int i=0; i<len; i++)
		if (path[i]=='\\')	path[i] = '/';
	len--;
	while (len>=0)
	{
		if (path[len]=='/') path[len] = '\0';
		else break;
		len--;
	}
	
	return;
}

// check if a file/dir exists
extern "C" int exists( const char *path )
{
	/// \todo replace this using the posix "access" function: fopen can fail if the fail exists but has no read permissions
	FILE *fp = fopen(path,"r");
	if (fp)
	{
		fclose(fp);
		return 1;
	}
	return 0;
}

// check if the given path is a file
extern "C" int is_file( const char *path )
{
#ifdef _WIN32
	struct _stat s;
	if( _stat(path,&s)==0 )
		if( s.st_mode & _S_IFREG )
			return 1;
#else
	struct stat s;
	if( stat(path,&s)==0 )
		if( s.st_mode & S_IFREG )
			return 1;
#endif
	return 0;
}

// check if the given path is a directory
extern "C" int is_dir( const char *path )
{
#ifdef _WIN32
	struct _stat s;
	if( _stat(path,&s)==0 )
		if( s.st_mode & _S_IFDIR )
			return 1;
#else
	struct stat s;
	if( stat(path,&s)==0 )
		if( s.st_mode & S_IFDIR )
			return 1;
#endif
	return 0;
}

// create a directory given its path
extern "C" int create_dir( const char *path )
{
	int ret = 0;
	#ifdef _WIN32
		ret = _mkdir(path);
	#else
		ret = mkdir(path,0777);
	#endif
	if (ret==0) return MKDIR_SUCCESS;
	else if (errno==EEXIST) return MKDIR_EXISTS;
	return MKDIR_ERROR;
}

// create a directory given its path
extern "C" int remove_dir( const char *path )
{
	int ret = 0;
	#ifdef WIN32
		ret = _rmdir(path);
	#else
		ret = rmdir(path);
	#endif
	if (ret==0) return 1;
	return 0;
}


// open file using a path; it creates all the intermediate directories if needed;
extern "C" FILE* fopen_with_path( const char *path, const char *mode )
{
	FILE *fp=NULL;
	bool readMode = NULL!=strchr(mode, 'r');
	char normpath[512];
	strcpy(normpath,path);
	normalize_path(normpath);
	// if 'read' just check if the file exist, don't create any folder
	if (readMode)
	{
		fp = fopen(normpath,mode);
	}
	// in case of 'write' mode also create all the intermediate folders
	else
	{

		std::vector<std::string> tokens = tokenize( normpath, '/' );
		// create all intermediate folders
		std::string currpath = "";
		for (unsigned int i=0; i<tokens.size()-1; i++)
		{
			currpath = tokens[0];
			for (unsigned int j=1; j<=i; j++)
				currpath += std::string("/") + tokens[j];
			if (create_dir(currpath.c_str())==MKDIR_ERROR)
				return NULL;
		}
		fp = fopen(normpath,mode);
	}

	return fp;
}

