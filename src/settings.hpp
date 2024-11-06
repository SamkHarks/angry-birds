#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "menu.hpp"

class Settings : public Menu {
    public:
        Settings();
        void draw(sf::RenderWindow& window) const override;
        void handleResize() override;
    private:
        const std::vector<std::string>& getButtonNames() const override;
};

#endif // SETTINGS_HPP