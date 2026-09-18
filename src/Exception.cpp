#include <SockLib/Exception.hpp>

SockLib::Exception::Exception(const char *s) :
    std::runtime_error(s)
{
}