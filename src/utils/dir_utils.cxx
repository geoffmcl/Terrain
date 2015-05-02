/*\
 * dir_utils.cxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string>
#ifdef _MSC_VER
#include <Windows.h>
#else
#include <stdlib.h> // for malloc/free, ...
#include <string.h> // for strlen(), ...
#include <errno.h> // for errno, ...
#include <dirent.h> // for opendir(), ...
#include <unistd.h> // for getcwd(), ...
#endif
#include "sprtf.hxx"
#include "utils.hxx"
#include "dir_utils.hxx"

static const char *module = "dir_utils";

// implementation
////////////////////////////////////////////////////////////////////////////
char *get_file_name(char *file)
{
	char *name = file;
	size_t len = strlen(file);
	size_t ii;
	int c;
	for (ii = 0; ii < len; ii++) {
		c = file[ii];
		if ((c == '\\')||(c == '/')) {
			if ((ii + 1) < len)
				name = &file[ii+1];
		}
	}
	return name;
}

#ifdef _MSC_VER
#define M_IS_DIR _S_IFDIR
#else
#define M_IS_DIR S_IFDIR
#endif

static struct stat stat_buf;
// return: 0 = not file or directory, 1 = file, 2 = directory, 
int is_file_or_directory( char *file )
{
    int iret = 0;
    if (stat(file,&stat_buf) == 0) {
        if (stat_buf.st_mode &  M_IS_DIR) 
            iret = 2;
        else
            iret = 1;
    }
    return iret;
}

size_t get_last_file_size() { return stat_buf.st_size; }

void ensure_win_seps( std::string &path )
{
    size_t pos = path.find('/');
    while (pos != (size_t)-1) {
        path.replace(pos,1,"\\");
        pos = path.find('/');
    }
}

void ensure_unix_seps( std::string &path )
{
    size_t pos = path.find('\\');
    while (pos != (size_t)-1) {
        path.replace(pos,1,"/");
        pos = path.find('\\');
    }
}

std::string get_file_name(std::string in_file)
{
    std::string file(in_file);
#ifdef WIN32
    ensure_win_seps(file);
    int pos = (int)file.rfind('\\');
#else
    ensure_unix_seps(file);
    int pos = (int)file.rfind('/');
#endif
    if ((pos != -1) && ((pos+1) < (int)file.size())) {
        file = file.substr(pos+1);
    }
    return file;
}


std::string get_file_name_only(std::string in_file)
{
    std::string file = get_file_name(in_file);
    size_t pos = file.rfind('.');
    if (pos != (size_t)-1) {
        file = file.substr(0,pos);
    }
    return file;
}



#ifdef _WIN32
#  define mMkDir(d,m)       _mkdir(d)
#else
#  define mMkDir(d,m)       mkdir(d,m)
#endif

std::string get_directory( std::string path, bool keep_sep )
{
    std::string dir(path);
    int pos1 = (int)dir.rfind("\\");
    int pos2 = (int)dir.rfind("/");
    if ((pos1 >= 0)&&(pos2 >= 0)) {
        if (pos1 > pos2) {
            if (keep_sep)
                dir = dir.substr(0,pos1+1);
            else    // drop separator
                dir = dir.substr(0,pos1);
        } else {
            if (keep_sep)
                dir = dir.substr(0,pos2+1);
            else
                dir = dir.substr(0,pos2);
        }
    } else if (pos1 > 0) {
        if (keep_sep)
            dir = dir.substr(0,pos1+1);
        else
            dir = dir.substr(0,pos1);
    } else if (pos2 >= 0) {
        if (keep_sep)
            dir = dir.substr(0,pos2+1);
        else
            dir = dir.substr(0,pos2);
    } else {
        dir = "";
    }
    return dir;
}

bool dir_create(std::string path, int mode)
{
    // must create parents first, if they do NOT exist
    std::string tmp = get_directory(path);
    if ((tmp.size() > 0) && (is_file_or_directory((char *)tmp.c_str()) != 2)) {
        dir_create( tmp, mode );
    }
    int err = 0;
    if ((path.size() > 0) && (is_file_or_directory((char *)path.c_str()) != 2)) {
        err = mMkDir(path.c_str(), mode);
        if (err) {
            SPRTF("Warning: directory creation failed: %s - %s\n", path.c_str(), strerror(errno) );
        }
    }
    return (err == 0);
}

bool check_file_path_exist( std::string file )
{
    std::string tmp = get_directory(file);  // this ASSUMES the last entry IS THE FILE NAME
    return dir_create(tmp,0677);
}

std::string get_current_dir()
{
#ifdef _WIN32
    char* buf = _getcwd(NULL, 0);
#else
    char* buf = ::getcwd(NULL, 0);
#endif
    std::string path(buf);
    free(buf);
    return path;
}

vSTG read_directory( std::string dir, std::string mask, int mode, bool verb )
{
    vSTG items;
    if (mode == 0) mode = dm_FILE | dm_DIRS;
    std::string tmp;
#ifdef WIN32
    std::string search = dir + PATH_SEP;
    if (mask.size() > 0) {
        search += mask;
    } else {
        search += "*.*";
    }
    WIN32_FIND_DATA fd;
    HANDLE find = FindFirstFile(search.c_str(), &fd);
    if (find == INVALID_HANDLE_VALUE) {
        if (verb) SPRTF("%s: Failed to get handle for %s!\n", module, search.c_str());
        return items;
    }
    do
    {
        tmp = dir + PATH_SEP;
        tmp += fd.cFileName;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if ((strcmp(fd.cFileName,".") == 0) || (strcmp(fd.cFileName,"..") == 0)) {
                if (( mode & dm_DOTS )&&(mode & dm_DIRS)) {
                    items.push_back(tmp);
                }
            } else {
                if (mode & dm_DIRS) {
                    items.push_back(tmp);
                }
            }
        } else if (fd.dwFileAttributes & (FILE_ATTRIBUTE_DEVICE | FILE_ATTRIBUTE_REPARSE_POINT)) {
            // ignore these always
        } else if ((fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)||(fd.dwFileAttributes == FILE_ATTRIBUTE_NORMAL)) {
            if (mode & dm_FILE) {
                items.push_back(tmp);
            }
        }
    } while (FindNextFile(find, &fd) != 0);

    FindClose(find);

#else
    // TODO: Does NOT presently use the mask - FIX ME!!!
    DIR* dp = opendir(dir.c_str());
    if (!dp) {
        if (verb) SPRTF("%s: Failed to open directory %s!\n", module, dir.c_str());
        return items;
    }
    while (true) {
        struct dirent* entry = readdir(dp);
        if (!entry) {
            break; // done iteration
        }
        tmp = dir + PATH_SEP;
        tmp += entry->d_name;
        int res = is_file_or_directory((char *)tmp.c_str());
        if (res == 1) {
            if (mode & dm_FILE) {
                items.push_back(tmp);
            }
        } else if (res == 2) {
            if ( !(mode & dm_DOTS) ) {
                if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                    continue;
                }
            }
            if (mode & dm_DIRS) {
                items.push_back(tmp);
            }

        }
    }
    closedir(dp);
#endif
    if (verb) SPRTF("%s: Returning %d items\n", module, (int)items.size());
    return items;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// 20140513 - new services

bool get_directory_size( const char *dir, long long *psize, int depth, bool recur, bool verb )
{
    if (is_file_or_directory((char *)dir) != 2) {
        if (verb) SPRTF("%s: '%s' is NOT a valid directory!\n", module, dir );
        return false;
    }
    std::string tmp;
    vSTG dirs;
    long long total = 0;
    std::string search = dir;
#ifdef WIN32
    search += PATH_SEP;
    search += "*.*";
    WIN32_FIND_DATA fd;
    HANDLE find = FindFirstFile(search.c_str(), &fd);
    if (find == INVALID_HANDLE_VALUE) {
        if (verb) SPRTF("%s: Failed to get handle for %s!\n", module, search.c_str());
        return false;
    }
    do
    {
        tmp = dir;
        tmp += PATH_SEP;
        tmp += fd.cFileName;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
        {
            if ( !((strcmp(fd.cFileName,".") == 0) || (strcmp(fd.cFileName,"..") == 0)) ) {
                dirs.push_back(tmp);
            }
        } else if (fd.dwFileAttributes & (FILE_ATTRIBUTE_DEVICE | FILE_ATTRIBUTE_REPARSE_POINT)) 
        {
            // ignore these always
        } else if ((fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)||(fd.dwFileAttributes == FILE_ATTRIBUTE_NORMAL)) 
        {
            ULARGE_INTEGER ul;
            ul.HighPart = fd.nFileSizeHigh;
            ul.LowPart  = fd.nFileSizeLow;
            total += ul.QuadPart;
        }
    } while (FindNextFile(find, &fd) != 0);

    FindClose(find);
#else
    // TODO: Does NOT presently use the mask - FIX ME!!!
    DIR* dp = opendir(search.c_str());
    if (!dp) {
        if (verb) SPRTF("%s: Failed to open directory %s!\n", module, search.c_str());
        return false;
    }
    while (true) {
        struct dirent* entry = readdir(dp);
        if (!entry) {
            break; // done iteration
        }
        tmp = search;
        tmp += PATH_SEP;
        tmp += entry->d_name;
        int res = is_file_or_directory((char *)tmp.c_str());
        if (res == 1) {
            total += get_last_file_size();
        } else if (res == 2) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                continue;
            }
            dirs.push_back(tmp);
        }
    }
    closedir(dp);
#endif
    *psize += total;
    if (recur) {
        size_t ii, max = dirs.size();
        for (ii = 0; ii < max; ii++) {
            tmp = dirs[ii];
            if (!get_directory_size( tmp.c_str(), psize, (depth+1), recur, verb )) {
                return false;
            }
        }
    }
    if (depth == 0) {
        if (verb) SPRTF("%s: Returning total %s size\n", module,
            get_NiceNumberStg64(*psize));
    }
    return true;
}

// eof = dir_utils.cxx
