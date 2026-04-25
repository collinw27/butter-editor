#include "utility/core.h"

#include <iostream>

ButterException::ButterException(const std::string& error_string) :
    error_string{error_string}
{}

std::string ButterException::message() const
{
    return error_string;
}

void ButterException::print() const
{
    std::cout << "[ERROR] " << error_string << std::endl;
}