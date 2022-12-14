/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "FsPath.h"

#include "Log.h"
#if DANDAN
#include "PathException.h"
#endif

#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#if DANDAN
#include <unistd.h>
#else
#include "GodotSDL.h"
#include "core/os/file_access.h"
#include "core/os/dir_access.h"
#endif

//-----------------------------------------------------------------
/**
 * Get native filename.
 * @param file posix filename
 */
    std::string
FsPath::getNative(const std::string &file)
{
    return file;
}
//-----------------------------------------------------------------
/**
 * Returns true when file or directory exists.
 * @param file posix filename
 */
    bool
FsPath::exists(const std::string &file)
{
    if (file.empty()) {
        return true;
    }

#if DANDAN
    struct stat buf;
    int error = stat(file.c_str(), &buf);
    return !error;
#else
    return FileAccess::exists(file.c_str()) || DirAccess::exists(file.c_str()) || ResourceLoader::exists(file.c_str());
#endif
}
//-----------------------------------------------------------------
/**
 * Joint two paths.
 * @param dir posix filename
 * @param file posix filename
 * @return "dir/file"
 */
    std::string
FsPath::join(const std::string &dir, const std::string &file)
{
    if (dir.empty()) {
        return file;
    }

    return dir + '/' + file;
}

//-----------------------------------------------------------------
/**
 * Return path without file basename.
 * @param file posix filename
 */
inline std::string
dirPath(const std::string &file)
{
    std::string dirs = "";
    std::string::size_type pos = file.rfind('/');
    if (pos == 0) {
        //NOTE: root
        dirs = "/";
    }
    else if (pos != std::string::npos) {
        dirs = file.substr(0, pos);
    }

    return dirs;
}
#if DANDAN
//-----------------------------------------------------------------
/**
 * Create dir.
 * @param dir posix filename
 * @throws NameException when error occurs
 */
inline void
createDir(const std::string &dir)
{
    if (dir.empty()) {
        return;
    }

#ifdef WIN32
    int error = mkdir(dir.c_str());
#else
    int error = mkdir(dir.c_str(), 0777);
#endif
    if (error) {
        throw PathException(ExInfo("cannot create dir")
            .addInfo("stderror", strerror(errno))
            .addInfo("dir", dir));
    }
}
#endif
//-----------------------------------------------------------------
/**
 * Create all directories in path (like "mkdir -p").
 * @param file posix filename
 */
void
FsPath::createPath(const std::string &file)
{
    std::string parent = dirPath(file);
    if (!FsPath::exists(parent)) {
#if DANDAN
        createPath(parent);
        createDir(parent);
#else
	DirAccess* da = DirAccess::create(DirAccess::ACCESS_USERDATA);
	Error err = da->make_dir_recursive(parent.c_str());
	memdelete(da);
#endif
    }
}

