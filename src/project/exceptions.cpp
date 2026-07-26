#include "project/exceptions.h"

ProjectLoadException::ProjectLoadException(const std::string& error_string) : ButterException{error_string} {}