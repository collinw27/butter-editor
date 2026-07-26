#ifndef PROJECT_EXCEPTIONS_H
#define PROJECT_EXCEPTIONS_H

#include "utility/core.h"

class ProjectLoadException : public ButterException
{
    std::string error_string;

public:

    ProjectLoadException(const std::string& error_string);
};

#endif