#include "KeyValuePairExtractorTest.h"

std::ostream & operator<<(std::ostream & ostr, const KeyValuePairExtractorTestCase & test_case)
{
    return ostr << test_case.input;
}