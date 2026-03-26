#include <iostream>

class ButterException
{
    std::string error_string;

public:

    ButterException(const std::string& error_string);
    void print() const;
};