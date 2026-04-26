#ifndef BOOL_ARGUMENT_H
#define BOOL_ARGUMENT_H

#include "command/argument/Argument.h"

class BoolArgument : public Argument
{
public:

    virtual Field parse(std::string token) override;
};

#endif