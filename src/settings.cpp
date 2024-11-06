#include "settings.hpp"
#include "utils.hpp"

Settings::Settings() : Menu(Menu::Type::SETTINGS) {
    // Create menu items
    updateMenuItems();
    // Create title
    setTitle("Settings", 500.f, 160);
};

void Settings::handleResize() {
    Menu::handleResize();
    title_.clear();
    setTitle("Settings", 500.f, 160);
}

void Settings::draw(sf::RenderWindow& window) const {
    window.draw(background_);
    window.draw(woodenSign_);
    // Draw the title characters
    for (const auto& character : title_) {
        window.draw(character);
    }
    // Draw menu items
    for (auto menuItem : menuItems_) {
        window.draw(menuItem);
    }
}

const std::vector<std::string>& Settings::getButtonNames() const {
    const static std::vector<std::string> buttonNames = { "Back" };
    return buttonNames;
}

