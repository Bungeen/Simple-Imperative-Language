#include <stdexcept>

#include "value.h"
#include "overload.h"

bool Value::to_bool() const {
    auto operations = Overload{
        [](auto arg)->bool {
            throw std::logic_error("Object of this type can not be interpreted as bool");
        },
        [](std::int64_t val)->bool {return val != 0.0;},

        [](std::shared_ptr<std::string> val)->bool
          {return val->size() != 0;},

        [](std::shared_ptr<List> val)->bool
          {return val->size() != 0;},
    };
    return std::visit(operations, *this);
}