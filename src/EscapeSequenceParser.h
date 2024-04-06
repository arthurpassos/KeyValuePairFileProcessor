#pragma once

#include <string>
#include <util/ReadBuffer.h>

class EscapeSequenceParser
{
public:
    static bool parseComplex(std::string & s, ReadBuffer & buf);
};
