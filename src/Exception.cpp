#include <SockLib/Exception.hpp>

SockLib::Exception::Exception(const std::string &s) :
    std::runtime_error(s)
{
}
SockLib::Exception::Exception(const char *s) :
    std::runtime_error(s)
{
}