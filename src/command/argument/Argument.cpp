#include "Argument.h"

Argument::Argument(std::string name) : name{name} {}

std::string Argument::get_name()
{
    return name;
}