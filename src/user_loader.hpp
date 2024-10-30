#ifndef USER_LOADER_HPP
#define USER_LOADER_HPP

#include <nlohmann/json.hpp>
#include <fstream>
#include "utils.hpp"

class UserSelector;

using json = nlohmann::json;

/**
 * @brief Player struct to store current player data
 * vector<int> stars: stars collected by the player
 * vector<int> highScores: high scores achieved by the player
 * index of the vectors corresponds to the level number such that stars[0] and highScores[0] are for level 1
 */
struct Player {
    std::string name;
    std::vector<int> stars;
    std::vector<int> highScores;
};

class UserLoader {
    public:
        UserLoader(UserSelector& userSelector);
        void loadPlayers();
        void loadPlayer(const std::string& playerName);
        void savePlayer(bool isNewPlayer);
        bool isPlayerNameAvailable(const std::string& playerName) const;
    private:
        std::vector<Player> players_;
        UserSelector& userSelector_;
};

#endif // USER_LOADER_HPP