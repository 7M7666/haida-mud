#include "Entity.hpp"

namespace hx {
Entity::Entity(std::string name, Attributes attr) : name_(std::move(name)), attr_(attr) {}
} // namespace hx
