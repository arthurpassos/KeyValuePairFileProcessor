#pragma once

#include <string>
#include <ReadBuffer.h>

class EscapeSequenceParser
{
public:
    static bool parseComplex(std::string & s, ReadBuffer & buf);
};
