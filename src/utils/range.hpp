#ifndef FALCONDB_UTILS_RANGE_HPP
#define FALCONDB_UTILS_RANGE_HPP

#include <string>

namespace falcondb {

// utils hould be top-leve, no need to nested namespace for general-pourpose classes

/// Describes, but dosn't own, a binary data range
class range
{
public:
    typedef char value_type;

    range(const std::string& s)
    : _begin(nullptr), _end(nullptr)
    {
        if (!s.empty())
        {
            _begin = reinterpret_cast<const value_type*>(&s[0]);
            _end = _begin + s.length();
        }
    }

    const value_type* begin() const { return _begin; }
    const value_type* end() const { return _end; }
    std::size_t size() const { return _end - _begin; }
    bool empty() const { return _begin == _end; }

private:

    const value_type* _begin;
    const value_type* _end;
};



}

#endif
