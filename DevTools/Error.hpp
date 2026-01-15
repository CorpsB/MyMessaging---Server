#include "../Include.hpp"

class Error : public std::exception {
    public:
        Error(const std::string &where, const std::string &what)
            : _where(where), _what(what)
        {

        }

        const char *what() const noexcept override
        {
            return _where.c_str();
        }

        const char *where() const noexcept
        {
            return _what.c_str();
        }

    private:
        const std::string _where;
        const std::string _what;
};