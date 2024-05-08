#include <SFML/Graphics.hpp>

int main()
{
    auto window = sf::RenderWindow{ { 1024u, 720u }, "Angry Birds Game" };
    window.setFramerateLimit(144);

    while (window.isOpen())
    {
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear(sf::Color::Blue);
        window.display();
    }
}