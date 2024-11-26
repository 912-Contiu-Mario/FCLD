#include <stdexcept>
#include <string>

class LexicalException : public std::runtime_error {
public:
    explicit LexicalException(const std::string& message)
        : std::runtime_error(message) {}
};