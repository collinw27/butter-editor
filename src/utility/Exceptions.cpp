#include "Exceptions.h"

ButterException::ButterException(const std::string& error_string) :
    error_string{error_string}
{}

void ButterException::print() const
{
    std::cout << "[ERROR] " << error_string << std::endl;
}