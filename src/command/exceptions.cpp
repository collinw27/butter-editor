#include "command/exceptions.h"

ParseException::ParseException(const std::string& error_string) : ButterException{error_string} {}