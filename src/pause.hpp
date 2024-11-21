#ifndef PAUSE_HPP
#define PAUSE_HPP

#include "menu.hpp"

class Pause : public Menu {
    public:
        enum class PausedState {
            RUNNING,
            LEVEL_EDITOR
        };
        Pause();
        void draw(sf::RenderWindow& window) const override;
        void handleResize(const sf::RenderWindow&) override;
        void handleResize() override;
        void updatePosition(const sf::RenderWindow& window);
        PausedState getPausedState() const;
        void setPausedState(PausedState state);
        bool isPausedAtRunning() const;
    private:
        const std::vector<std::string>& getButtonNames() const override;
        void updateTitle(const sf::RenderWindow& window);
        PausedState pausedState_ = PausedState::RUNNING;
};

#endif // PAUSE_HPP