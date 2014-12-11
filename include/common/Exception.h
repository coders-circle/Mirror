#ifndef __EXCEPTION__
#define __EXCEPTION__
class Exception : public std::runtime_error
{
public:
    Exception(const std::string& errorString)
        : std::runtime_error(errorString)
    {}
};
#endif