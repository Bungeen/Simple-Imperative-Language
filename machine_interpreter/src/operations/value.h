#pragma once

#include <cstdint>
#include <variant>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

template<class... Ts> struct Overload : Ts... { using Ts::operator()...; };
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

class Value;
using List = std::vector<Value>;

template <typename T>
struct SexpImpl {
    std::string tag;
    std::vector<T> values;
};

using Sexp = SexpImpl<Value>;

template <typename T>
using ValueImpl = typename std::variant<std::int64_t, 
                                        std::shared_ptr<std::string>, 
                                        std::shared_ptr<std::vector<T>>,
                                        std::shared_ptr<SexpImpl<T>>>;

class Value : public ValueImpl<Value> {
public:
    using ValueImpl<Value>::ValueImpl;

    bool to_bool() const {
        auto operations = Overload{
            [](auto arg) -> bool {
                throw std::logic_error("Object of this type cannot be interpreted as bool");
            },
            [](std::int64_t val) -> bool { return val != 0; },
            [](const std::shared_ptr<std::string>& val) -> bool { return !val->empty(); },
            [](const std::shared_ptr<List>& val) -> bool { return !val->empty(); },
            [](const std::shared_ptr<Sexp>& val) -> bool { return !val->values.empty(); },
        };
        return std::visit(operations, *this);
    }

    explicit operator bool() const {
        return to_bool();
    }
};
