#ifndef PROJECT_FILE_H
#define PROJECT_FILE_H

#include <string>

// This class is used to represent project data loaded directly
// from a file
// This is merely an intermediate step in project saving/loading,
// since projects internally store their data in a neater format than this

// The main purpose of splitting the work up like this, instead of
// just having the project handle saving/loading directly into its
// native format, is to keep file logic within the FileManager while
// also making file logic easier to read

struct ProjectFile
{
    std::string name;
};

#endif