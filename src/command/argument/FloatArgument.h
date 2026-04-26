#ifndef FLOAT_ARGUMENT_H
#define FLOAT_ARGUMENT_H

#include "command/argument/Argument.h"

class FloatArgument : public Argument
{
public:

    virtual Field parse(std::string token) override;
};

#endif