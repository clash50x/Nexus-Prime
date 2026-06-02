#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>

void flappyBirdLauncher() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode({1000u, 600u}), "Flappy Bird");
    window.setFramerateLimit(60);

    // Audio Setup
    sf::SoundBuffer flapBuffer;
    if (!flapBuffer.loadFromFile("data/audio/flap.wav"))
    {
        std::cerr << "Failed to load data/audio/flap.wav\n";
        return;
    }
    sf::Sound flapSound(flapBuffer);

    sf::Texture bgTex, birdTex, pipeTex, overTex;
    if (!bgTex.loadFromFile("GUI/flappy/background.png"))
    {
        std::cerr << "Failed to load GUI/flappy/background.png\n";
        return;
    }
    if (!birdTex.loadFromFile("GUI/flappy/flappy3.png"))
    {
        std::cerr << "Failed to load GUI/flappy/flappy3.png\n";
        return;
    }
    if (!pipeTex.loadFromFile("GUI/flappy/pipe.png"))
    {
        std::cerr << "Failed to load GUI/flappy/pipe.png\n";
        return;
    }
    if (!overTex.loadFromFile("GUI/flappy/gameover.png"))
    {
        std::cerr << "Failed to load GUI/flappy/gameover.png\n";
        return;
    }

    bgTex.setRepeated(true);

    sf::Sprite background(bgTex);
    sf::Sprite bird(birdTex);
    sf::Sprite topPipe(pipeTex);
    sf::Sprite bottomPipe(pipeTex);
    sf::Sprite gameOver(overTex);

    float bgScaleY = 600.f / static_cast<float>(bgTex.getSize().y);
    background.setScale(sf::Vector2f(bgScaleY, bgScaleY));
    background.setTextureRect(sf::IntRect({0, 0}, {1000, static_cast<int>(bgTex.getSize().y)}));

    bird.setScale(sf::Vector2f(2.5f, 2.5f));
    bird.setPosition(sf::Vector2f(200.f, 300.f));

    gameOver.setPosition(sf::Vector2f(350.f, 250.f));
    gameOver.setScale(sf::Vector2f(2.f, 2.f));

    topPipe.setScale(sf::Vector2f(2.5f, -2.5f));
    bottomPipe.setScale(sf::Vector2f(2.5f, 2.5f));

    float velocity = 0.f;
    int pipeX = 1000;
    int gapY = 300;
    bool dead = false;
    bool started = false;

    topPipe.setPosition(sf::Vector2f(static_cast<float>(pipeX), static_cast<float>(gapY - 100)));
    bottomPipe.setPosition(sf::Vector2f(static_cast<float>(pipeX), static_cast<float>(gapY + 100)));

    int score = 0;
    bool scoredThisPipe = false;

    std::cout << "Press SPACE to start the game...\n";

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::KeyPressed>())
            {
                auto key = event->getIf<sf::Event::KeyPressed>()->code;
                if (!started && key == sf::Keyboard::Key::Space)
                {
                    started = true;
                    std::cout << "Game started! Score: " << score << "\n";
                }
                else if (started && !dead && key == sf::Keyboard::Key::Space)
                {
                    velocity = -8.f;
                    flapSound.play();
                }
                else if (dead && key == sf::Keyboard::Key::R)
                {
                    bird.setPosition(sf::Vector2f(200.f, 300.f));
                    velocity = 0.f;
                    pipeX = 1000;
                    dead = false;
                    score = 0;
                    scoredThisPipe = false;
                    started = true;
                    system("cls");
                    std::cout << "Score: " << score << "\n";
                }
            }
        }

        if (started && !dead)
        {
            velocity += 0.4f;
            bird.move(sf::Vector2f(0.f, velocity));

            pipeX -= 4;

            if (pipeX < 200 && !scoredThisPipe)
            {
                score++;
                scoredThisPipe = true;
                system("cls");
                std::cout << "Score: " << score << "\n";
            }

            if (pipeX < -150)
            {
                pipeX = 1000;
                gapY = 150 + (std::rand() % 200);
                scoredThisPipe = false;
            }

            topPipe.setPosition(sf::Vector2f(static_cast<float>(pipeX), static_cast<float>(gapY - 100)));
            bottomPipe.setPosition(sf::Vector2f(static_cast<float>(pipeX), static_cast<float>(gapY + 100)));

            if (bird.getGlobalBounds().findIntersection(topPipe.getGlobalBounds()).has_value() ||
                bird.getGlobalBounds().findIntersection(bottomPipe.getGlobalBounds()).has_value() ||
                bird.getPosition().y < 0 || bird.getPosition().y > 600)
            {
                dead = true;
                std::cout << "Game Over! Final Score: " << score << "\nPressed 'R' to Restart.\n";
            }
        }

        window.clear();
        window.draw(background);
        window.draw(topPipe);
        window.draw(bottomPipe);
        window.draw(bird);

        if (dead)
            window.draw(gameOver);

        window.display();
    }
}
