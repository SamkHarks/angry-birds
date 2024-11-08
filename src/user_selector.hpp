#ifndef USER_SELECTOR_HPP
#define USER_SELECTOR_HPP

#include <SFML/Graphics.hpp>
#include "user_loader.hpp"

class UserSelector {
    public:
        enum class Screen {
            NEW_PLAYER,
            LOAD_PLAYER
        };
        enum class Item {
            ACCEPT,
            CANCEL,
            BACK,
            PREV,
            NEXT,
            PLAYER_NAME,
            UNDEFINED
        };
        struct IndexRange {
            int start = 0;
            int end = 0;
        };
        UserSelector();
        void draw(sf::RenderWindow& window) const;
        void handleMouseMove(const sf::Vector2f& mousePosition);
        bool handleMouseClick(const sf::Vector2f& mousePosition);
        void handleKeyPress(const sf::Keyboard::Key& code);
        void handleTextEntered(const sf::Uint32& unicode);
        void setPromptText(const std::string& text);
        void setPlayerText(const std::string& text);
        void setPlayer();
        void clearPlayer();
        const std::shared_ptr<Player>& getPlayer() const;
        std::string getPlayerText() const;
        void setPlayerAccepted(bool accepted);
        bool isPlayerAccepted() const;
        bool isPlayerSet() const;
        const int getSelectedItem() const;
        const Item convertIndexToItem() const;
        void setSelectedItem(int nextItem);
        int getItemAtPosition(const sf::Vector2f& mousePosition) const;
        Screen getScreen() const;
        void setScreen(Screen screen);
        bool isNewPlayer() const;
        void savePlayer();
        void loadPlayer();
        void handleResize();
        void initializeScreen();
        const std::vector<Player>& getPlayers() const;
        const IndexRange& getIndexRange() const;
        void setIndexRange(Item item);
    private:
        sf::Font font_;
        sf::Text promptText_; // Text prompt for player name
        sf::Text playerText_; // Text input for player name
        sf::Text loadHeaderText_; // Text header for load player screen
        std::vector<sf::Text> acceptText_; // Text for accepting player name
        std::vector<sf::Text> playerNames_; // List of player names for load player screen
        sf::RectangleShape caret_; // Caret for text input
        std::vector<sf::RectangleShape> buttons_; // Buttons for setting next players to visible
        
        mutable sf::Clock caretClock_;
        bool isPlayerAccepted_ = false;
        void updateItem(bool isSelected);
        int selectedItem_ = 0;
        int playerCount_ = 0;
        IndexRange range_;
        Screen screen_ = Screen::NEW_PLAYER;
        friend class UserLoader;
        UserLoader userLoader_;
        std::shared_ptr<Player> player_ = nullptr; // Solo owner of the player, other classes only have weak_ptr
        void drawNewPlayer(sf::RenderWindow& window) const;
        void drawLoadPlayer(sf::RenderWindow& window) const;
        void initializePlayerNames();
        const int getSelectedPlayerIndex() const;
};

#endif // USER_SELECTOR_HPP
