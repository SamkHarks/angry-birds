#include "menu.hpp"

Menu::Menu(Type& type) : type_(type) {}

const Menu::Type Menu::getType() const {
    return type_;
}