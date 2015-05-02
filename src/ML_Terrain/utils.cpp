/*\
 * utils.cpp
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
// other includes
#include "utils.h"

static const char *module = "utils";

// implementation
static struct stat buf;
DiskType is_file_or_directory32 ( const char * path )
{
    if (!path)
        return MDT_NONE;
	if (stat(path,&buf) == 0)
	{
		if (buf.st_mode & M_IS_DIR)
			return MDT_DIR;
		else
			return MDT_FILE;
	}
	return MDT_NONE;
}

size_t get_last_file_size32() { return buf.st_size; }
#ifdef _MSC_VER
static struct _stat64 buf64;
DiskType is_file_or_directory64 ( const char * path )
{
    if (!path)
        return MDT_NONE;
	if (_stat64(path,&buf64) == 0)
	{
		if (buf64.st_mode & M_IS_DIR)
			return MDT_DIR;
		else
			return MDT_FILE;
	}
	return MDT_NONE;
}

__int64 get_last_file_size64() { return buf64.st_size; }

#endif

// eof = utils.cpp
