#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>   
#include <string>
#include <optional>
#include <cstdlib>
#include <ctime>

using namespace std;

// =============================================================================
// STRUCTURES
// =============================================================================

struct Position 
{ 
    int r, c; 
};

struct Entity 
{ 
    Position pos; 
    sf::Color color; 
    int score; 
};

struct GameStateData 
{
    Entity player;
    Entity ghosts[2];
    int currentEaten;
    int totalPellets;
};

// =============================================================================
// CONSTANTS & GLOBALS
// =============================================================================

const int ROWS = 25;
const int COLS = 25;
const float TILE_SIZE = 24.0f; 
const unsigned int WIN_W = 1066;
const unsigned int WIN_H = 600;

const float OFFSET_X = (WIN_W - (COLS * TILE_SIZE)) / 2.0f;

enum State { START, PLAYING, GAMEOVER, WIN };

int maze[ROWS][COLS];

// =============================================================================
// FILE OPERATIONS
// =============================================================================

void saveHighScore(int score) 
{
    ofstream fout("highscore.txt");
    if (fout.is_open()) 
    { 
        fout << score; 
        fout.close(); 
    }
}

int loadHighScore() 
{
    int topScore = 0;
    ifstream fin("highscore.txt");
    if (fin.is_open()) 
    { 
        fin >> topScore; 
        fin.close(); 
    }
    return topScore;
}

// =============================================================================
// MAZE INITIALIZATION
// =============================================================================

void initMaze(int& total) 
{
    total = 0;
    for (int i = 0; i < ROWS; i++) 
    {
        for (int j = 0; j < COLS; j++) 
        {
            if (i == 0 || i == 24 || j == 0 || j == 24 || 
                (i == 3 && j >= 3 && j <= 8) || (i == 3 && j >= 16 && j <= 21) ||
                (i == 4 && j >= 3 && j <= 8) || (i == 4 && j >= 16 && j <= 21) ||
                (i >= 8 && i <= 16 && j == 4) || (i == 12 && j >= 5 && j <= 20) ||
                (i >= 8 && i <= 16 && j == 20) || (i >= 16 && i <= 21 && j == 8) ||
                (i == 21 && j >= 9 && j <= 15) || (i == 8 && j >= 8 && j <= 16) ||
                (i >= 9 && i <= 13 && j == 12) || (i == 20 && j >= 3 && j <= 6) ||
                (i == 21 && j >= 3 && j <= 6) || (i == 20 && j >= 18 && j <= 21) ||
                (i == 21 && j >= 18 && j <= 21)) 
            {
                maze[i][j] = 1; 
            } 
            else 
            {
                maze[i][j] = 0; 
                total++;
            }
        }
    }
}

// =============================================================================
// PIXEL TEXT RENDERING
// =============================================================================

void drawPixelLetter(sf::RenderWindow& win, char c, float x, float y, float sz, sf::Color col) 
{
    sf::RectangleShape p(sf::Vector2f(sz - 1, sz - 1)); 
    p.setFillColor(col);
    auto d = [&](int i, int j) { p.setPosition(sf::Vector2f(x + i * sz, y + j * sz)); win.draw(p); };
    
    if (c == 'P') { for(int j=0; j<5; j++) d(0,j); d(1,0); d(1,2); d(2,1); }
    else if (c == 'A') { for(int j=1; j<5; j++) { d(0,j); d(2,j); } d(1,0); d(1,2); }
    else if (c == 'C') { for(int j=1; j<4; j++) d(0,j); for(int i=1; i<3; i++) { d(i,0); d(i,4); } }
    else if (c == 'M') { for(int j=0; j<5; j++) { d(0,j); d(4,j); } d(1,1); d(2,2); d(3,1); }
    else if (c == 'N') { for(int j=0; j<5; j++) { d(0,j); d(4,j); } d(1,1); d(2,2); d(3,3); }
    else if (c == 'S') { for(int i=0; i<3; i++) { d(i,0); d(i,2); d(i,4); } d(0,1); d(2,3); }
    else if (c == 'T') { for(int i=0; i<3; i++) d(i,0); for(int j=1; j<5; j++) d(1,j); }
    else if (c == 'R') { for(int j=0; j<5; j++) d(0,j); d(1,0); d(1,2); d(2,1); d(2,3); d(2,4); }
    else if (c == 'E') { for(int j=0; j<5; j++) d(0,j); for(int i=1; i<3; i++) { d(i,0); d(i,2); d(i,4); } }
    else if (c == 'F') { for(int j=0; j<5; j++) d(0,j); for(int i=1; i<3; i++) { d(i,0); d(i,2); } }
    else if (c == 'G') { for(int j=1; j<4; j++) d(0,j); for(int i=1; i<3; i++) { d(i,0); d(i,4); } d(2,2); d(2,3); }
    else if (c == 'O') { for(int j=1; j<4; j++) { d(0,j); d(2,j); } d(1,0); d(1,4); }
    else if (c == 'H') { for(int j=0; j<5; j++) { d(0,j); d(2,j); } d(1,2); }
    else if (c == 'I') { for(int i=0; i<3; i++) { d(i,0); d(i,4); } for(int j=1; j<4; j++) d(1,j); }
    else if (c == 'U') { for(int j=0; j<4; j++) { d(0,j); d(2,j); } d(1,4); }
    else if (c == 'V') { d(0,0); d(0,1); d(0,2); d(2,0); d(2,1); d(2,2); d(1,3); d(1,4); }
    else if (c == 'W') { for(int j=0; j<5; j++) { d(0,j); d(4,j); } d(1,4); d(2,3); d(3,4); }
    else if (c == 'Y') { d(0,0); d(0,1); d(2,0); d(2,1); d(1,2); d(1,3); d(1,4); }
    else if (c == 'L') { for(int j=0; j<5; j++) d(0,j); for(int i=1; i<3; i++) d(i,4); }
    else if (c == 'Q') { for(int j=1; j<4; j++) { d(0,j); d(2,j); } d(1,0); d(1,4); d(2,4); }
    else if (c == ':') { d(1,1); d(1,3); }
    else if (c == '>') { d(0,0); d(1,1); d(2,2); d(1,3); d(0,4); }
    else if (c >= '0' && c <= '9') 
    {
        int v = c - '0';
        if (v == 0) { for(int j=1; j<4; j++) { d(0,j); d(2,j); } d(1,0); d(1,4); }
        else if (v == 1) { for(int j=0; j<5; j++) d(1,j); d(0,1); }
        else if (v == 2) { for(int i=0; i<3; i++) { d(i,0); d(i,2); d(i,4); } d(2,1); d(0,3); }
        else if (v == 3) { for(int i=0; i<3; i++) { d(i,0); d(i,2); d(i,4); } d(2,1); d(2,3); }
        else if (v == 4) { for(int j=0; j<5; j++) d(2,j); d(0,0); d(0,1); d(0,2); d(1,2); }
        else if (v == 5) { for(int i=0; i<3; i++) { d(i,0); d(i,2); d(i,4); } d(0,1); d(2,3); }
        else if (v == 6) { for(int j=0; j<5; j++) d(0,j); for(int i=1; i<3; i++) { d(i,0); d(i,2); d(i,4); } d(2,3); }
        else if (v == 7) { for(int i=0; i<3; i++) d(i,0); d(2,1); d(2,2); d(2,3); d(2,4); }
        else if (v == 8) { for(int i=0; i<3; i++) { d(i,0); d(i,2); d(i,4); } d(0,1); d(0,3); d(2,1); d(2,3); }
        else if (v == 9) { for(int i=0; i<3; i++) { d(i,0); d(i,2); d(i,4); } d(0,1); d(2,1); d(2,3); }
    }
}

void drawPixelString(sf::RenderWindow& win, string s, float x, float y, float sz, sf::Color col, bool center = false) 
{
    float width = 0;
    for (char c : s) width += (c == 'M' || c == 'W' || c == 'N') ? sz * 6 : sz * 4;
    float curX = center ? (WIN_W - width) / 2.0f : x;

    for (char c : s) 
    { 
        if (c == ' ') { curX += sz * 4; continue; }
        drawPixelLetter(win, c, curX, y, sz, col); 
        curX += (c == 'M' || c == 'W' || c == 'N') ? sz * 6 : sz * 4; 
    }
}

// =============================================================================
// MAIN ENTRY POINT
// =============================================================================

int main() 
{
    srand(static_cast<unsigned int>(time(0)));
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WIN_W, WIN_H)), "Pacman Project");
    window.setFramerateLimit(12);

    sf::Music music;
    bool musicOk = music.openFromFile("playing-pac-man-6783.mp3");
    if (musicOk) music.setLooping(true);

    sf::SoundBuffer deathBuffer;
    std::optional<sf::Sound> deathSound;
    if (deathBuffer.loadFromFile("pacman_death.wav")) deathSound.emplace(deathBuffer);

    GameStateData game;
    State state = START;
    int menuSelect = 0;
    int highScore = loadHighScore();

    // Store velocities for ghost straight-line movement
    Position ghostDirs[2];

    auto reset = [&]() 
    {
        initMaze(game.totalPellets);
        game.player = {{1, 1}, sf::Color::Yellow, 0};
        game.ghosts[0] = {{23, 23}, sf::Color::Red, 0};
        game.ghosts[1] = {{23, 1}, sf::Color::Cyan, 0};
        
        // Initial random directions
        ghostDirs[0] = {-1, 0}; 
        ghostDirs[1] = {0, 1};
        
        game.currentEaten = 0;
        state = PLAYING;
        if (musicOk) { music.stop(); music.play(); }
    };

    while (window.isOpen()) 
    {
        while (const std::optional event = window.pollEvent()) 
        {
            if (event->is<sf::Event::Closed>()) window.close();
            if (state != PLAYING && event->is<sf::Event::KeyPressed>()) 
            {
                const auto* k = event->getIf<sf::Event::KeyPressed>();
                if (k->code == sf::Keyboard::Key::W || k->code == sf::Keyboard::Key::Up) menuSelect = 0;
                if (k->code == sf::Keyboard::Key::S || k->code == sf::Keyboard::Key::Down) menuSelect = 1;
                if (k->code == sf::Keyboard::Key::Enter) 
                {
                    if (state == START || menuSelect == 0) reset();
                    else window.close();
                }
            }
        }

        if (state == PLAYING) 
        {
            // --- PLAYER MOVEMENT ---
            int dr = 0, dc = 0;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) dr = -1;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) dr = 1;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) dc = -1;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) dc = 1;

            if (maze[game.player.pos.r + dr][game.player.pos.c + dc] != 1) 
            {
                game.player.pos.r += dr; 
                game.player.pos.c += dc;
                if (maze[game.player.pos.r][game.player.pos.c] == 0) 
                {
                    maze[game.player.pos.r][game.player.pos.c] = 2;
                    game.player.score += 10; 
                    game.currentEaten++;
                    if (game.currentEaten >= game.totalPellets) state = WIN;
                }
            }

            // --- GHOST MOVEMENT ---
            for (int i = 0; i < 2; i++) 
            {
                bool seePacman = false;

                // Check Row/Col alignment for Chasing
                if (game.ghosts[i].pos.r == game.player.pos.r) {
                    ghostDirs[i].r = 0;
                    ghostDirs[i].c = (game.player.pos.c > game.ghosts[i].pos.c) ? 1 : -1;
                    seePacman = true;
                }
                else if (game.ghosts[i].pos.c == game.player.pos.c) {
                    ghostDirs[i].r = (game.player.pos.r > game.ghosts[i].pos.r) ? 1 : -1;
                    ghostDirs[i].c = 0;
                    seePacman = true;
                }

                // If chasing or already moving, check for wall collision
                int nextR = game.ghosts[i].pos.r + ghostDirs[i].r;
                int nextC = game.ghosts[i].pos.c + ghostDirs[i].c;

                if (maze[nextR][nextC] == 1) 
                {
                    // If they hit a wall while chasing OR patrolling, pick a new random path
                    bool found = false;
                    while (!found) 
                    {
                        int d = rand() % 4;
                        int nr = 0, nc = 0;
                        if(d == 0) nr = -1; else if(d == 1) nr = 1; else if(d == 2) nc = -1; else nc = 1;
                        if (maze[game.ghosts[i].pos.r + nr][game.ghosts[i].pos.c + nc] != 1) 
                        {
                            ghostDirs[i].r = nr;
                            ghostDirs[i].c = nc;
                            found = true;
                        }
                    }
                }

                // Move the ghost
                game.ghosts[i].pos.r += ghostDirs[i].r;
                game.ghosts[i].pos.c += ghostDirs[i].c;

                // Check collision with player
                if (game.ghosts[i].pos.r == game.player.pos.r && game.ghosts[i].pos.c == game.player.pos.c) 
                {
                    state = GAMEOVER;
                    music.stop();
                    if (deathSound) deathSound->play();
                    if (game.player.score > highScore) { highScore = game.player.score; saveHighScore(highScore); }
                }
            }
        }

        // --- DRAWING ---
        window.clear();
        if (state == START) 
        {
            drawPixelString(window, "PACMAN", 0, 150, 12, sf::Color::Yellow, true);
            drawPixelString(window, "HI SCORE: " + to_string(highScore), 0, 300, 6, sf::Color::White, true);
            drawPixelString(window, "PRESS ENTER", 0, 420, 6, sf::Color::Cyan, true);
        } 
        else if (state == PLAYING) 
        {
            for (int r = 0; r < ROWS; r++) 
            {
                for (int c = 0; c < COLS; c++) 
                {
                    float px = OFFSET_X + (c * TILE_SIZE);
                    float py = r * TILE_SIZE;
                    if (maze[r][c] == 1) 
                    {
                        sf::RectangleShape w(sf::Vector2f(TILE_SIZE - 1, TILE_SIZE - 1)); 
                        w.setFillColor(sf::Color(30, 30, 150));
                        w.setPosition(sf::Vector2f(px, py)); window.draw(w);
                    } 
                    else if (maze[r][c] == 0) 
                    {
                        sf::CircleShape p(3.0f); p.setFillColor(sf::Color::White);
                        p.setPosition(sf::Vector2f(px + TILE_SIZE / 2 - 3, py + TILE_SIZE / 2 - 3)); window.draw(p);
                    }
                }
            }

            auto drawE = [&](Entity& e, bool isPlayer) 
            {
                float px = OFFSET_X + (e.pos.c * TILE_SIZE);
                float py = e.pos.r * TILE_SIZE;
                sf::CircleShape b(TILE_SIZE / 2 - 2); 
                b.setFillColor(e.color); 
                b.setPosition(sf::Vector2f(px, py)); 
                window.draw(b);

                if (isPlayer) 
                {
                    sf::RectangleShape eye(sf::Vector2f(6, 6)); 
                    eye.setFillColor(sf::Color::White);
                    sf::RectangleShape pupil(sf::Vector2f(3, 3)); 
                    pupil.setFillColor(sf::Color::Black);
                    eye.setPosition(sf::Vector2f(px + 10, py + 4)); 
                    window.draw(eye);
                    pupil.setPosition(sf::Vector2f(px + 12, py + 5)); 
                    window.draw(pupil);
                    sf::ConvexShape mouth;
                    mouth.setPointCount(3);
                    mouth.setPoint(0, sf::Vector2f(px + TILE_SIZE/2, py + TILE_SIZE/2));
                    mouth.setPoint(1, sf::Vector2f(px + TILE_SIZE, py + 4));
                    mouth.setPoint(2, sf::Vector2f(px + TILE_SIZE, py + TILE_SIZE - 4));
                    mouth.setFillColor(sf::Color::Black);
                    window.draw(mouth);
                }
                else 
                {
                    sf::RectangleShape eye(sf::Vector2f(4, 4)); 
                    eye.setFillColor(sf::Color::White);
                    sf::RectangleShape pupil(sf::Vector2f(2, 2)); 
                    pupil.setFillColor(sf::Color::Black);
                    eye.setPosition(sf::Vector2f(px + 5, py + 6)); 
                    window.draw(eye);
                    pupil.setPosition(sf::Vector2f(px + 6, py + 7)); 
                    window.draw(pupil);
                    eye.setPosition(sf::Vector2f(px + 13, py + 6)); 
                    window.draw(eye);
                    pupil.setPosition(sf::Vector2f(px + 14, py + 7)); 
                    window.draw(pupil);
                }
            };

            drawE(game.player, true); 
            drawE(game.ghosts[0], false); 
            drawE(game.ghosts[1], false);
            drawPixelString(window, "SCORE: " + to_string(game.player.score), 30, 30, 5, sf::Color::Yellow);
        } 
        else 
        {
            string msg = (state == WIN) ? "YOU WIN" : "GAME OVER";
            drawPixelString(window, msg, 0, 100, 12, (state == WIN ? sf::Color::Green : sf::Color::Red), true);
            drawPixelString(window, "FINAL SCORE: " + to_string(game.player.score), 0, 220, 6, sf::Color::White, true);
            drawPixelString(window, "HI SCORE: " + to_string(highScore), 0, 270, 6, sf::Color::Yellow, true);
            drawPixelString(window, (menuSelect == 0 ? "> RESTART" : "  RESTART"), 0, 380, 8, (menuSelect == 0 ? sf::Color::Cyan : sf::Color::White), true);
            drawPixelString(window, (menuSelect == 1 ? "> QUIT" : "  QUIT"), 0, 450, 8, (menuSelect == 1 ? sf::Color::Red : sf::Color::White), true);
        }
        window.display();
    }
    return 0;
}
