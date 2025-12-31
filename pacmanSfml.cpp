/*
=============================================================================
PROGRAMMING FUNDAMENTALS - SEMESTER PROJECT
Project Title:  Advanced Pacman (SFML Edition)
Authors:         Muhammad Sohaib Zulfiqar-2501303
                 Muhammad Azeem-2501276
                 Hasnain Imtiaz-2501381
Requirements:   Nested Structures, Switch Statements, Arrays, File Handling,
                Sorting Algorithms, and GUI Implementation (SFML).
=============================================================================
*/

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>   
#include <string>
#include <optional>
#include <ctime>
#include <cstdlib> 

using namespace std;

// =============================================================================
// SECTION 1: DATA STRUCTURES (Requirements: Nested Structures)
// =============================================================================

// Basic coordinate structure for grid-based positioning
struct Position { 
    int r, c; 
};

// Gameplay statistics for the player
struct Stats { 
    int score; 
    int pelletsEaten; 
};

// Date structure used to demonstrate Advanced Nested Structures
struct RecordDate { 
    int day, month, year;
};

// Entity structure representing characters (Player/Ghosts)
struct Entity { 
    Position pos; 
    sf::Color color; 
    Stats gameplayStats; // Nested Structure Implementation
};

// High Score management structure
struct HighScoreRecord {
    char playerName[5];
    int score;
    RecordDate dateAchieved; // Nested Structure Implementation
};

// Container for the current game session data
struct GameStateData {
    Entity player;
    Entity ghosts[2];
    Position ghostDirs[2];
    int totalPellets;
};

// =============================================================================
// SECTION 2: GLOBAL CONSTANTS & STATE ENUMERATIONS
// =============================================================================

const int ROWS = 25;
const int COLS = 25;
const float TILE_SIZE = 24.0f; 
const unsigned int WIN_W = 1066;
const unsigned int WIN_H = 600;
const float OFFSET_X = (WIN_W - (COLS * TILE_SIZE)) / 2.0f;

// State Machine for game flow control
enum State { START, PLAYING, GAMEOVER, WIN };

int maze[ROWS][COLS];
HighScoreRecord highScores[20]; // Array requirement: 20 records

// =============================================================================
// SECTION 3: FILE HANDLING & SORTING (Requirements: Persistence & Algorithms)
// =============================================================================

// Loads leaderboard data from local text file
void loadData() {
    ifstream fin("records.txt");
    if (fin) {
        for(int i = 0; i < 20; i++) {
            if (!(fin >> highScores[i].playerName >> highScores[i].score)) {
                highScores[i].score = 0;
                snprintf(highScores[i].playerName, 5, "PAC");
            }
        }
        fin.close();
    } else {
        // Initialize with default values if file does not exist
        for(int i = 0; i < 20; i++) {
            highScores[i].score = 0;
            snprintf(highScores[i].playerName, 5, "PAC");
        }
    }
}

// Saves leaderboard data to local text file
void saveData() {
    ofstream fout("records.txt");
    if (fout) {
        for(int i = 0; i < 20; i++) {
            fout << highScores[i].playerName << " " << highScores[i].score << endl;
        }
        fout.close();
    }
}

// Processes new scores and sorts the leaderboard (Bubble Sort implementation)
void updateHighScores(int newScore) {
    if (newScore > highScores[19].score) {
        highScores[19].score = newScore;
        snprintf(highScores[19].playerName, 5, "YOU");
        
        // Sorting Algorithm: Bubble Sort (Ascending/Descending management)
        for (int i = 0; i < 20 - 1; i++) {
            for (int j = 0; j < 20 - i - 1; j++) {
                if (highScores[j].score < highScores[j+1].score) {
                    HighScoreRecord temp = highScores[j];
                    highScores[j] = highScores[j+1];
                    highScores[j+1] = temp;
                }
            }
        }
        saveData();
    }
}

// Searches the array for the current highest score
int findTopScore() {
    int top = 0;
    for(int i = 0; i < 20; i++) {
        if(highScores[i].score > top) top = highScores[i].score;
    }
    return top;
}

// =============================================================================
// SECTION 4: MAZE & CHARACTER INITIALIZATION
// =============================================================================

void initMaze(int& total) {
    total = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            // Hardcoded wall layout logic
            if (i == 0 || i == 24 || j == 0 || j == 24 || 
                (i == 3 && j >= 3 && j <= 8) || (i == 3 && j >= 16 && j <= 21) ||
                (i == 4 && j >= 3 && j <= 8) || (i == 4 && j >= 16 && j <= 21) ||
                (i >= 8 && i <= 16 && j == 4) || (i == 12 && j >= 5 && j <= 20) ||
                (i >= 8 && i <= 16 && j == 20) || (i >= 16 && i <= 21 && j == 8) ||
                (i == 21 && j >= 9 && j <= 15) || (i == 8 && j >= 8 && j <= 16) ||
                (i >= 9 && i <= 13 && j == 12) || (i == 20 && j >= 3 && j <= 6) ||
                (i == 21 && j >= 3 && j <= 6) || (i == 20 && j >= 18 && j <= 21) ||
                (i == 21 && j >= 18 && j <= 21)) 
            { maze[i][j] = 1; } // Wall
            else { maze[i][j] = 0; total++; } // Pellet
        }
    }
}

// =============================================================================
// SECTION 5: CUSTOM PIXEL-ART FONT SYSTEM (Manual Vertex Drawing)
// =============================================================================

void drawPixelLetter(sf::RenderWindow& win, char c, float x, float y, float sz, sf::Color col) {
    sf::RectangleShape p(sf::Vector2f(sz - 1, sz - 1)); 
    p.setFillColor(col);
    auto d = [&](int i, int j) { p.setPosition(sf::Vector2f(x + i * sz, y + j * sz)); win.draw(p); };
    
    // Character Map (Draws letters pixel by pixel using individual rectangles)
    if (c == 'P') { for(int j=0; j<5; j++) d(0,j); d(1,0); d(1,2); d(2,1); }
    else if (c == 'A') { for(int j=1; j<5; j++) { d(0,j); d(2,j); } d(1,0); d(1,2); }
    else if (c == 'C') { for(int j=1; j<4; j++) d(0,j); for(int i=1; i<3; i++) { d(i,0); d(i,4); } }
    else if (c == 'M') { for(int j=0; j<5; j++) { d(0,j); d(4,j); } d(1,1); d(2,2); d(3,1); }
    else if (c == 'N') { for(int j=0; j<5; j++) { d(0,j); d(4,j); } d(1,1); d(2,2); d(3,3); }
    else if (c == 'S') { for(int i=0; i<3; i++) { d(i,0); d(i,2); d(i,4); } d(0,1); d(2,3); }
    else if (c == 'T') { for(int i=0; i<3; i++) d(i,0); for(int j=1; j<5; j++) d(1,j); }
    else if (c == 'R') { for(int j=0; j<5; j++) d(0,j); d(1,0); d(1,2); d(2,1); d(2,3); d(2,4); }
    else if (c == 'E') { for(int j=0; j<5; j++) d(0,j); for(int i=1; i<3; i++) { d(i,0); d(i,2); d(i,4); } }
    else if (c == 'F') { for(int j=0; j<5; j++) d(0,j); d(1,0); d(2,0); d(1,2); }
    else if (c == 'L') { for(int j=0; j<5; j++) d(0,j); d(1,4); d(2,4); }
    else if (c == 'G') { for(int j=1; j<4; j++) d(0,j); for(int i=1; i<3; i++) { d(i,0); d(i,4); } d(2,2); d(2,3); }
    else if (c == 'O') { for(int j=1; j<4; j++) { d(0,j); d(2,j); } d(1,0); d(1,4); }
    else if (c == 'V') { d(0,0); d(0,1); d(0,2); d(0,3); d(2,0); d(2,1); d(2,2); d(2,3); d(1,4); }
    else if (c == 'Q') { for(int j=1; j<4; j++) { d(0,j); d(2,j); } d(1,0); d(1,4); d(2,4); d(2,3); }
    else if (c == 'H') { for(int j=0; j<5; j++) { d(0,j); d(2,j); } d(1,2); }
    else if (c == 'I') { for(int i=0; i<3; i++) { d(i,0); d(i,4); } for(int j=1; j<4; j++) d(1,j); }
    else if (c == 'U') { for(int j=0; j<4; j++) { d(0,j); d(2,j); } d(1,4); }
    else if (c == 'Y') { d(0,0); d(0,1); d(2,0); d(2,1); d(1,2); d(1,3); d(1,4); }
    else if (c == ':') { d(1,1); d(1,3); }
    else if (c == '>') { d(0,0); d(1,1); d(2,2); d(1,3); d(0,4); }
    else if (c >= '0' && c <= '9') {
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

void drawPixelString(sf::RenderWindow& win, string s, float x, float y, float sz, sf::Color col, bool center = false) {
    float width = 0;
    for (char c : s) width += (c == 'M' || c == 'W' || c == 'N') ? sz * 6 : sz * 4;
    float curX = center ? (WIN_W - width) / 2.0f : x;
    for (char c : s) { 
        if (c == ' ') { curX += sz * 4; continue; }
        drawPixelLetter(win, c, curX, y, sz, col); 
        curX += (c == 'M' || c == 'W' || c == 'N') ? sz * 6 : sz * 4; 
    }
}

// =============================================================================
// SECTION 6: MAIN GAME ENGINE
// =============================================================================

int main() {
    // Initialization of Random Seed and Window
    srand(static_cast<unsigned int>(time(0)));
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WIN_W, WIN_H)), "Pacman PF Project");
    window.setFramerateLimit(12);

    loadData();

    // Audio Asset Management
    sf::Music music;
    bool musicOk = music.openFromFile("playing-pac-man-6783.mp3");
    if (musicOk) music.setLooping(true);

    sf::SoundBuffer deathBuffer;
    std::optional<sf::Sound> deathSound;
    if (deathBuffer.loadFromFile("pacman_death.wav")) deathSound.emplace(deathBuffer);

    GameStateData game;
    State state = START;
    int menuSelect = 0;

    // Reset Function (Lambda used for encapsulation)
    auto reset = [&]() {
        initMaze(game.totalPellets);
        game.player = {{1, 1}, sf::Color::Yellow, {0, 0}};
        game.ghosts[0] = {{23, 23}, sf::Color::Red, {0, 0}};
        game.ghosts[1] = {{23, 1}, sf::Color::Cyan, {0, 0}};
        game.ghostDirs[0] = {-1, 0}; 
        game.ghostDirs[1] = {0, 1};
        state = PLAYING;
        if (musicOk) { music.stop(); music.play(); }
    };

    // Main Game Loop (Requirement: While Loop)
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            
            // Menu Navigation & Input Validation
            if (state != PLAYING && event->is<sf::Event::KeyPressed>()) {
                const auto* k = event->getIf<sf::Event::KeyPressed>();
                if (k->code == sf::Keyboard::Key::W || k->code == sf::Keyboard::Key::Up) menuSelect = 0;
                else if (k->code == sf::Keyboard::Key::S || k->code == sf::Keyboard::Key::Down) menuSelect = 1;
                else if (k->code == sf::Keyboard::Key::Enter) {
                    if (state == START || menuSelect == 0) reset();
                    else window.close();
                }
            }
        }

        // Gameplay Logic Branch
        if (state == PLAYING) {
            // Player Movement Control
            int dr = 0, dc = 0;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) dr = -1;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) dr = 1;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) dc = -1;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) dc = 1;

            // Collision Detection with Walls
            if (maze[game.player.pos.r + dr][game.player.pos.c + dc] != 1) {
                game.player.pos.r += dr; 
                game.player.pos.c += dc;
                // Pellet Eating Logic
                if (maze[game.player.pos.r][game.player.pos.c] == 0) {
                    maze[game.player.pos.r][game.player.pos.c] = 2;
                    game.player.gameplayStats.score += 10; 
                    game.player.gameplayStats.pelletsEaten++;
                    if (game.player.gameplayStats.pelletsEaten >= game.totalPellets) state = WIN;
                }
            }

            // Ghost AI Logic (Pathfinding & Intersections)
            for (int i = 0; i < 2; i++) {
                if (game.ghosts[i].pos.r == game.player.pos.r) {
                    game.ghostDirs[i].r = 0;
                    game.ghostDirs[i].c = (game.player.pos.c > game.ghosts[i].pos.c) ? 1 : -1;
                } else if (game.ghosts[i].pos.c == game.player.pos.c) {
                    game.ghostDirs[i].r = (game.player.pos.r > game.ghosts[i].pos.r) ? 1 : -1;
                    game.ghostDirs[i].c = 0;
                }

                // Handling Ghost Wall Collisions
                if (maze[game.ghosts[i].pos.r + game.ghostDirs[i].r][game.ghosts[i].pos.c + game.ghostDirs[i].c] == 1) {
                    bool found = false;
                    while (!found) {
                        int d = rand() % 4;
                        int nr = 0, nc = 0;
                        // Requirement: Switch Statement usage
                        switch(d) {
                            case 0: nr = -1; break;
                            case 1: nr = 1; break;
                            case 2: nc = -1; break;
                            case 3: nc = 1; break;
                        }
                        if (maze[game.ghosts[i].pos.r + nr][game.ghosts[i].pos.c + nc] != 1) {
                            game.ghostDirs[i].r = nr; game.ghostDirs[i].c = nc;
                            found = true;
                        }
                    }
                }
                game.ghosts[i].pos.r += game.ghostDirs[i].r;
                game.ghosts[i].pos.c += game.ghostDirs[i].c;

                // Game Over Condition: Collision with Enemy
                if (game.ghosts[i].pos.r == game.player.pos.r && game.ghosts[i].pos.c == game.player.pos.c) {
                    state = GAMEOVER;
                    music.stop();
                    if (deathSound) deathSound->play();
                    updateHighScores(game.player.gameplayStats.score);
                }
            }
        }

        // =============================================================================
        // SECTION 7: RENDERING PIPELINE (Requirement: Switch Statement & Graphics)
        // =============================================================================
        window.clear();
        
        switch (state) {
            case START: {
                drawPixelString(window, "PACMAN", 0, 150, 12, sf::Color::Yellow, true);
                drawPixelString(window, "HIGH SCORE: " + to_string(findTopScore()), 0, 300, 6, sf::Color::White, true);
                drawPixelString(window, "PRESS ENTER", 0, 420, 6, sf::Color::Cyan, true);
                break;
            }

            case PLAYING: {
                // Render Maze Walls and Pellets
                for (int r = 0; r < ROWS; r++) {
                    for (int c = 0; c < COLS; c++) {
                        float px = OFFSET_X + (c * TILE_SIZE);
                        float py = r * TILE_SIZE;
                        if (maze[r][c] == 1) {
                            sf::RectangleShape w(sf::Vector2f(TILE_SIZE - 1, TILE_SIZE - 1)); 
                            w.setFillColor(sf::Color(30, 30, 150)); w.setPosition(sf::Vector2f(px, py)); window.draw(w);
                        } else if (maze[r][c] == 0) {
                            sf::CircleShape p(3.0f); p.setFillColor(sf::Color::White);
                            p.setPosition(sf::Vector2f(px + TILE_SIZE/2-3, py + TILE_SIZE/2-3)); window.draw(p);
                        }
                    }
                }

                // Render Entities (Player/Ghosts)
                auto drawE = [&](Entity& e, bool isPlayer) {
                    float px = OFFSET_X + (e.pos.c * TILE_SIZE);
                    float py = e.pos.r * TILE_SIZE;
                    sf::CircleShape body(TILE_SIZE / 2 - 2); 
                    body.setFillColor(e.color); body.setPosition(sf::Vector2f(px, py)); window.draw(body);

                    if (isPlayer) { // Render Pacman Eyes/Mouth
                        sf::CircleShape eyeBody(4.0f); eyeBody.setFillColor(sf::Color::White);
                        eyeBody.setPosition(sf::Vector2f(px + 10, py + 3)); window.draw(eyeBody);
                        sf::CircleShape pupil(1.5f); pupil.setFillColor(sf::Color::Black);
                        pupil.setPosition(sf::Vector2f(px + 13, py + 5)); window.draw(pupil);
                        sf::ConvexShape mouth; mouth.setPointCount(3);
                        mouth.setPoint(0, sf::Vector2f(px + 12, py + 12));
                        mouth.setPoint(1, sf::Vector2f(px + 24, py + 4));
                        mouth.setPoint(2, sf::Vector2f(px + 24, py + 20));
                        mouth.setFillColor(sf::Color::Black); window.draw(mouth);
                    } else { // Render Ghost Eyes
                        sf::CircleShape eye(3.5f); eye.setFillColor(sf::Color::White);
                        sf::CircleShape pupil(1.5f); pupil.setFillColor(sf::Color::Black);
                        eye.setPosition(sf::Vector2f(px + 4, py + 5)); window.draw(eye);
                        pupil.setPosition(sf::Vector2f(px + 5, py + 6)); window.draw(pupil);
                        eye.setPosition(sf::Vector2f(px + 11, py + 5)); window.draw(eye);
                        pupil.setPosition(sf::Vector2f(px + 12, py + 6)); window.draw(pupil);
                    }
                };
                drawE(game.player, true); drawE(game.ghosts[0], false); drawE(game.ghosts[1], false);
                drawPixelString(window, "SCORE: " + to_string(game.player.gameplayStats.score), 30, 30, 5, sf::Color::Yellow);
                break;
            }

            case GAMEOVER:
            case WIN: {
                string msg = (state == WIN) ? "YOU WIN" : "GAME OVER";
                drawPixelString(window, msg, 0, 100, 12, (state == WIN ? sf::Color::Green : sf::Color::Red), true);
                drawPixelString(window, "FINAL SCORE: " + to_string(game.player.gameplayStats.score), 0, 230, 6, sf::Color::White, true);
                drawPixelString(window, "HIGH SCORE: " + to_string(findTopScore()), 0, 280, 6, sf::Color::Yellow, true);
                drawPixelString(window, (menuSelect == 0 ? "> RESTART" : "  RESTART"), 0, 380, 8, (menuSelect == 0 ? sf::Color::Cyan : sf::Color::White), true);
                drawPixelString(window, (menuSelect == 1 ? "> QUIT" : "  QUIT"), 0, 450, 8, (menuSelect == 1 ? sf::Color::Red : sf::Color::White), true);
                break;
            }
        }
        window.display();
    }
    return 0;
}