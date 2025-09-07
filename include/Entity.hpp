#pragma once
#include <string>
#include "Attributes.hpp"

namespace hx {
class Entity {
public:
    Entity(std::string name, Attributes attr);
    virtual ~Entity() = default;

    const std::string& name() const { return name_; }
    Attributes& attr() { return attr_; }
    const Attributes& attr() const { return attr_; }

protected:
    std::string name_;
    Attributes attr_{};
};
} // namespace hx
