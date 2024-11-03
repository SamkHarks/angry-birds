#include "user_loader.hpp"
#include "user_selector.hpp"


void from_json(const json& j, Player& player) {
    j.at("name").get_to(player.name);
    j.at("stars").get_to(player.stars);
    j.at("highScores").get_to(player.highScores);
}

void to_json(json& j, const Player& player) {
    j = json{
        {"name", player.name},
        {"stars", player.stars},
        {"highScores", player.highScores}
    };
}

UserLoader::UserLoader(UserSelector& userSelector) : userSelector_(userSelector) {}

void UserLoader::loadPlayers() {
    std::string path = utils::getExecutablePath() + "/assets/data/players.json";
    std::ifstream inFile(path);
    if(!inFile.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    json playersJson;
    inFile >> playersJson;
    inFile.close();
    for (const auto& player : playersJson["players"]) {
        Player newPlayer = player.template get<Player>();
        players_.push_back(newPlayer);
    }
}

bool UserLoader::isPlayerNameAvailable(const std::string& playerName) const {
    for (const auto& player : players_) {
        if (player.name == playerName) {
            return false;
        }
    }
    return true;
}

void UserLoader::loadPlayer(const std::string& playerName) {
    for (const auto& player : players_) {
        if (player.name == playerName) {
            userSelector_.player_ = std::make_shared<Player>(player);
            return;
        }
    }
}

void UserLoader::savePlayer() {
    if (!userSelector_.player_) {
        throw std::runtime_error("No player loaded to save.");
    }
    std::string path = utils::getExecutablePath() + "/assets/data/players.json";
    std::ifstream inFile(path);
    if(!inFile.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    json playersJson;
    inFile >> playersJson;
    inFile.close();

    const std::shared_ptr<Player>& player = userSelector_.player_;

    // Add or update the player in the players_ vector
    auto it = std::find_if(players_.begin(), players_.end(), [&player](const Player& p){
        return p.name == player->name;
    });
    if (it == players_.end()) {
        players_.push_back(*player);
    } else {
        *it = *player;
    }

    // Create a JSON array to store player data
    json playersArray = json::array();
    for (const auto& player : players_) {
        json playerJson = player;
        playersArray.push_back(playerJson);
    }
    // Update the playersJson object with the new players array
    playersJson["players"] = playersArray;

    // Write the updated JSON back to the file
    std::ofstream outFile(path);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + path);
    }
    outFile << playersJson.dump(4); // Pretty print with 4 spaces
    outFile.close();
}
