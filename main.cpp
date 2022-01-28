#include <SFML/Graphics.hpp>

#include <chrono>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

// file stream stuff
#include <fstream>

// working directory stuff
#include <direct.h>

#include "pad.hpp"
#include "ball.hpp"
#include "collision.hpp"

const float SCREEN_WIDTH = 1280, 
            SCREEN_HEIGHT = 720;

std::chrono::high_resolution_clock::time_point lastFrameMs = std::chrono::high_resolution_clock::now();;

using namespace std;

/*
    TODO
    - print FPS in screen - no font loading method works
    - show scoring (blocked by print FPS)
    - smooth entities movement based on framerate
    - why do i have to rebuild project for changes to count?
    - integrate render position with entity position
    - consider ball size when checking for collision with lower wall
    - prevent pad from going lower than the lower wall when following ball (wrap in setY())?
    - angle of ball reflection on pad depending on point of contact

    general improvements
    - player B (actual) IA
    - timer before launching ball
    - ball speed ceiling?
    - use int instead of float for screen position stuff
    - screen width/height as global var instead of passing as param
    - sound/menu/general polish
    - module for rendering stuff (rethink architecture)
*/

typedef struct score_t {
    int playerA;
    int playerB;
} Score;

// used to debug assets loading (like fonts)
std::string getWorkingDir() {
    char buff[FILENAME_MAX]; //create string buffer to hold path
    _getcwd(buff, FILENAME_MAX);
    string current_working_dir(buff);
    return current_working_dir;
}

// loads the whole font, located in the working directory, to a memory buffer
std::vector<char> loadFontToBuffer(std::string fontName) {
    std::ifstream fontStream{ fontName };
    std::vector<char> buffer;
    if (fontStream) {
        // get length of file:
        fontStream.seekg(0, fontStream.end);
        const auto length = fontStream.tellg();
        if (!length) {
            std::cerr << "Cannot load zero byte file" << std::endl;
            exit(EXIT_FAILURE);
        }
        buffer.resize(length); // reserve space

        fontStream.seekg(0, fontStream.beg);

        auto start = &*buffer.begin();
        fontStream.read(start, length);
        fontStream.close();
    }
    else {
        std::cerr << "Could not open texture file" << std::endl;
        exit(EXIT_FAILURE);
    }
    return buffer;
}

// draws a line from the center of the screen
sf::RectangleShape makeScreenDiagonal(float angle) {
    sf::RectangleShape line(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
    line.setPosition(sf::Vector2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));
    line.setSize(sf::Vector2f(3000, 1)); // why 3000? no idea
    line.rotate(angle * (180.f / (float) M_PI)); // convert radian to degrees
    line.setFillColor(sf::Color::White);
    return line;
}

sf::RectangleShape makeBallTrail(float angle) {
    sf::RectangleShape line = makeScreenDiagonal(angle);
    line.setFillColor(sf::Color::Yellow);
    return line;
}

sf::Text makeFpsText(sf::Font font, std::string content) {
    sf::Text text;
    text.setString(content);
    text.setFont(font);
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::Yellow);
    text.setPosition(SCREEN_WIDTH - 100, 30);
    return text;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode((unsigned int) SCREEN_WIDTH, (unsigned) SCREEN_HEIGHT), "Pong");

    // entities
    Pad padA(SCREEN_WIDTH, SCREEN_HEIGHT, true), padB(SCREEN_WIDTH, SCREEN_HEIGHT, false);
    Ball ball(SCREEN_WIDTH, SCREEN_HEIGHT);

    bool isDebugActive = false;

    // aparently using booleans is smoother than directly updating positions
    // we need to investigate if this is true, and maybe look for better approaches
    bool isMovingUp = false, isMovingDown = false;

    cout << "\n" << getWorkingDir() << endl; // print working directory

    sf::Font font;
    std::vector<char> fontBuffer = loadFontToBuffer("arial.ttf");
    // if (!font.loadFromMemory(&fontBuffer[0], fontBuffer.size())) // !! why is neither load font method working???
    {
        // exit(EXIT_FAILURE);
    }

    Score score = { 0, 0 };

    auto resetGame = [&ball, &padA, &padB, &score]() -> void {
        cout << "\np1: " << score.playerA << " p2: " << score.playerB;
        ball.reset();
        padA.reset();
        padB.reset();
    };

    while (window.isOpen())
    {
        // delta time stuff
        auto nowMs = std::chrono::high_resolution_clock::now();
        double elapsedMs = std::chrono::duration<double, std::milli>(nowMs - lastFrameMs).count();
        double fps = 1000 / elapsedMs; // 1000 to change from ms to s
        // cout << "\nelapsed: " << elapsedMs;
        // cout << "\nfps: " << fps;
        lastFrameMs = nowMs;

        // read events
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Up) {
                        isMovingUp = true;
                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        isMovingDown = true;
                    }
                    else if (event.key.code == sf::Keyboard::B) {
                        ball.reset();
                    }
                    else if (event.key.code == sf::Keyboard::R) {
                        score = { 0, 0 };
                        cout << "\n< game reset >";
                        resetGame();
                    }
                    else if (event.key.code == sf::Keyboard::F1) {
                        isDebugActive = !isDebugActive;
                    }
                    break;

                case sf::Event::KeyReleased:
                    if (event.key.code == sf::Keyboard::Up) {
                        isMovingUp = false;
                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        isMovingDown = false;
                    }
                    break;
            }
        }

        if (isMovingUp) {
            padA.goUp();
        }

        if (isMovingDown) {
            padA.goDown();
        }

        ball.move();
        padB.followBall(ball);

        if (collision::check(&ball, 0.f) || collision::check(&ball, SCREEN_HEIGHT)) {
            ball.wallCollision();
        }

        if (collision::check(&ball, &padA) || collision::check(&ball, &padB)) {
            ball.padCollision();
        } 
        else if (ball.x < padA.x) {
            score.playerB++;
            resetGame();
        }
        else if (ball.x > padB.x) {
            score.playerA++;
            resetGame();
        }

        // render
        window.clear();

        if (isDebugActive) {
            // ball boundaries
            window.draw(makeScreenDiagonal(ball.LAUNCH_RIGHT_UPPER_BOUND));
            window.draw(makeScreenDiagonal(ball.LAUNCH_LEFT_UPPER_BOUND));
            window.draw(makeScreenDiagonal(ball.LAUNCH_LEFT_LOWER_BOUND));
            window.draw(makeScreenDiagonal(ball.LAUNCH_RIGHT_LOWER_BOUND));

            window.draw(makeBallTrail(ball.angle));
            // window.draw(makeFpsText(font, "FPS: " + std::to_string(fps)));
        }

        window.draw(padA.renderObj);
        window.draw(padB.renderObj);
        window.draw(ball.renderObj);
        window.display();
    }

    return 0;
}
