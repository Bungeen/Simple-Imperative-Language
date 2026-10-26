#pragma once

#include <cstdint>
#include <variant>
#include <string>
#include <vector>
#include <memory>

template <typename T>
struct SexpImpl {
    std::string tag;
    std::vector<T> values;
};

template <typename T>
using ValueImpl = typename std::variant<std::int64_t, 
                                        std::shared_ptr<std::string>, 
                                        std::shared_ptr<std::vector<T>>,
                                        std::shared_ptr<SexpImpl<T>>>;

class Value : public ValueImpl<Value> {
public:
    using ValueImpl<Value>::ValueImpl;

    bool to_bool() const;
};

using List = std::vector<Value>;
using Sexp = SexpImpl<Value>;