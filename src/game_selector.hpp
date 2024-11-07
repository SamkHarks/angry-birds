#ifndef GAME_SELECTOR_HPP
#define GAME_SELECTOR_HPP

#include "menu.hpp"
#include "level_selector.hpp"
#include "user_selector.hpp"

class GameSelector : public Menu {
    public:
        enum class Screen {
            GAME_SELECTOR,
            USER_SELECTOR,
            LEVEL_SELECTOR
        };
        enum class Item {
            CONTINUE,
            NEW_GAME,
            LOAD_GAME,
            BACK,
            UNDEFINED
        };
        GameSelector();
        void draw(sf::RenderWindow& window) const override;
        LevelSelector& getLevelSelector();
        UserSelector& getUserSelector();
        void setScreen(Screen screen);
        Screen getScreen() const;
        void handleMouseMove(const sf::Vector2f& mousePosition) override;
        bool handleMouseClick(const sf::Vector2f& mousePosition) override;
        void handleResize() override;
        Item getSelectedItem() const;
        void initializeLevelSelector();
    private:
        LevelSelector levelSelector_;
        UserSelector userSelector_;
        Screen screen_ = Screen::GAME_SELECTOR;
        void drawScreen(sf::RenderWindow& window) const;
        const std::vector<std::string>& getButtonNames() const override;

};


#endif // GAME_SELECTOR_HPP