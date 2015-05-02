/*\
 * utils.h
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef _MSC_VER
#define M_IS_DIR _S_IFDIR
#else // !_MSC_VER
#define M_IS_DIR S_IFDIR
#endif

enum DiskType {
    MDT_NONE,
    MDT_FILE,
    MDT_DIR
};

extern DiskType is_file_or_directory32 ( const char * path );
extern size_t get_last_file_size32();
#ifdef _MSC_VER
extern DiskType is_file_or_directory64 ( const char * path );
extern __int64 get_last_file_size64();
#define is_file_or_directory is_file_or_directory64
#define get_last_file_size get_last_file_size64
#else
#define is_file_or_directory is_file_or_directory32
#define get_last_file_size get_last_file_size32
#endif



#endif // #ifndef _UTILS_H_
// eof - utils.h
