#ifndef USER_SELECTOR_HPP
#define USER_SELECTOR_HPP

#include <SFML/Graphics.hpp>
#include "user_loader.hpp"

class UserSelector {
    public:
        enum class Screen {
            MAIN,
            LOAD_PLAYER,
            NEW_PLAYER
        };
        UserSelector();
        void draw(sf::RenderWindow& window) const;
        void handleMouseMove(sf::Vector2f mousePosition);
        bool handleMouseClick(sf::Vector2f mousePosition);
        void handleTextEntered(sf::Uint32 unicode);
        void setPromptText(const std::string& text);
        void setPlayerText(const std::string& text);
        void setPlayer();
        void clearPlayer();
        const Player& getPlayer() const;
        std::string getPlayerText() const;
        void setPlayerAccepted(bool accepted);
        bool isPlayerAccepted() const;
        bool isPlayerSet() const;
        const int getSelectedItem() const;
        void setSelectedItem(int nextItem);
        int getItemAtPosition(sf::Vector2f mousePosition) const;
        Screen getScreen() const;
        void setScreen(Screen screen);
        bool isNewPlayer() const;
        void savePlayer(bool isNewPlayer);
    private:
        sf::Font font_;
        sf::Text promptText_;
        sf::Text playerText_;
        std::vector<sf::Text> acceptText_;
        sf::RectangleShape caret_;
        mutable sf::Clock caretClock_;
        bool isPlayerAccepted_ = false;
        void updateItem(bool isSelected);
        int selectedItem_ = 0;
        Screen screen_ = Screen::MAIN;
        friend class UserLoader;
        UserLoader userLoader_;
        Player player_;
};

#endif // USER_SELECTOR_HPP
