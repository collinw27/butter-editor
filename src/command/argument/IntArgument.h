#ifndef INT_ARGUMENT_H
#define INT_ARGUMENT_H

#include "command/argument/Argument.h"

class IntArgument : public Argument
{
public:

    virtual Field parse(std::string token) override;
};

#endif