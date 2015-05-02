/*\
 * dir_utils.hxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#ifndef _DIR_UTILS_HXX_
#define _DIR_UTILS_HXX_
#include <string>
#include "utils.hxx" // vSTG

// add body
extern bool dir_create(std::string path, int mode);
extern std::string get_directory( std::string path, bool keep_sep = false );

#define dm_FILE 1
#define dm_DIRS 2
#define dm_DOTS 4

extern vSTG read_directory( std::string dir, std::string mask, int mode = 0, bool verb = false );
extern char *get_file_name(char *file);
extern int is_file_or_directory( char *file ); // 1=file 2=directory, else 0
extern size_t get_last_file_size(); // last file stat'ed above
extern bool check_file_path_exist( std::string file );
//////////////////////////////////////////////////////////////////////

extern std::string get_file_name_only(std::string in_file);
extern std::string get_file_name(std::string in_file);
extern void ensure_unix_seps( std::string &path );
extern void ensure_win_seps( std::string &path );
///////////////////////////////////////////////////////////////////////
// 20140513 - new services
extern bool get_directory_size( const char *dir, long long *psize, int depth = 0, bool recur = false, bool verb = false );

#endif // #ifndef _DIR_UTILS_HXX_
// eof - dir_utils.hxx
