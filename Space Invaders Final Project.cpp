#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string> 

class Player {
public:
    sf::RectangleShape shape;
    sf::Texture texture;

    Player() {
        texture.loadFromFile("images/player.png");
        shape.setTexture(&texture);
        shape.setSize(sf::Vector2f(50.0f, 50.0f));
        shape.setPosition(375.0f, 550.0f);
    }
};

class Bullet {
public:
    sf::RectangleShape shape;
    float speed;

    Bullet() {
        shape.setSize(sf::Vector2f(5.0f, 20.0f));
        speed = 7.0f;
    }
};

class Enemy {
public:
    sf::RectangleShape shape;
    sf::Texture texture;
    float speed;
    bool movingRight;
    bool isDestroyed;

    Enemy(float x, float y) {
        texture.loadFromFile("images/Enemy.png");
        shape.setTexture(&texture);
        shape.setSize(sf::Vector2f(40.0f, 40.0f));
        shape.setPosition(x, y);
        speed = 2.0f;
        movingRight = true;
        isDestroyed = false;
    }

    void move() {
        if (!isDestroyed) {
            if (movingRight) {
                shape.move(speed, 0.0f);
            }
            else {
                shape.move(-speed, 0.0f);
            }
            if (shape.getPosition().x <= 0 || shape.getPosition().x + shape.getSize().x >= 800) {
                movingRight = !movingRight;

                // Move down
                shape.move(0.0f, shape.getSize().y);
            }
        }
    }

    bool checkCollision(const sf::RectangleShape& other) const {
        return shape.getGlobalBounds().intersects(other.getGlobalBounds());
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Space Invaders");
    window.setFramerateLimit(60);

    Player player;

    std::vector<Bullet> bullets;
    sf::Clock reloadTimer;
    sf::Time reloadTime = sf::seconds(0.5f); // Set reload time to 0.5 seconds

    std::vector<Enemy> enemies;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 5; ++j) {
            Enemy enemy(50.0f * i, 50.0f * j);
            enemies.push_back(enemy);
        }
    }

    int points = 0;
    sf::Font font;
    font.loadFromFile("arial.ttf");
    sf::Text pointsText;
    pointsText.setFont(font);
    pointsText.setCharacterSize(24);
    pointsText.setPosition(10.0f, 550.0f);

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("Game Over!");
    gameOverText.setPosition(250.0f, 250.0f);

    bool gameover = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (!gameover) {
            // Player controls
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && player.shape.getPosition().x > 0)
                player.shape.move(-5.0f, 0.0f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && player.shape.getPosition().x < 750)
                player.shape.move(5.0f, 0.0f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && reloadTimer.getElapsedTime() > reloadTime) {
                Bullet bullet;
                bullet.shape.setPosition(
                    player.shape.getPosition().x + player.shape.getSize().x / 2 - bullet.shape.getSize().x / 2,
                    player.shape.getPosition().y - bullet.shape.getSize().y
                );
                bullets.push_back(bullet);
                reloadTimer.restart();
            }

            for (size_t i = 0; i < bullets.size(); ++i) {
                bullets[i].shape.move(0.0f, -bullets[i].speed);

                if (bullets[i].shape.getPosition().y < 0) {
                    bullets.erase(bullets.begin() + i);
                    --i;
                }
            }

            for (auto& enemy : enemies) {
                enemy.move();

                if (enemy.checkCollision(player.shape)) {
                    gameover = true;
                }

                for (size_t i = 0; i < bullets.size(); ++i) {
                    if (!enemy.isDestroyed && enemy.checkCollision(bullets[i].shape)) {
                        enemy.isDestroyed = true;
                        bullets.erase(bullets.begin() + i);
                        --i;
                        points++;
                    }
                }
            }
        }

        window.clear();

        window.draw(player.shape);

        for (const auto& bullet : bullets)
            window.draw(bullet.shape);

        for (const auto& enemy : enemies)
            if (!enemy.isDestroyed)
                window.draw(enemy.shape);

        pointsText.setString("Points: " + std::to_string(points));
        window.draw(pointsText);

        if (gameover) {
            window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}

