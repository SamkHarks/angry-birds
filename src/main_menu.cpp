#include "main_menu.hpp"
#include <vector>
#include "utils.hpp"
#include "resource_manager.hpp"


MainMenu::MainMenu() : Menu(Menu::Type::MAIN) {
    // Create menu items
    updateMenuItems();
    // Create title
    setTitle("AngryBirds", 680.f, 340);
    backgroundMusic_.play();
};

void MainMenu::handleResize() {
    Menu::handleResize();
    title_.clear();
    setTitle("AngryBirds", 680.f, 340);
};

void MainMenu::draw(sf::RenderWindow& window) const {
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
};

const std::vector<std::string>& MainMenu::getButtonNames() const {
    const static std::vector<std::string> buttonNames = { "Play", "Level Editor", "Settings", "Exit" };
    return buttonNames;
}
