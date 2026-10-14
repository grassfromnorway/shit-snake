#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>
#include <bits/stdc++.h>

class GameState //w monolith
{
public:
    uint initialX; //for savestate
    uint initialY;
    char initialBg1;
    char initialBg2;
    uint initialScaleFactor;
    uint initialFps;

    explicit GameState(
        uint X = 0,
        uint Y = 0,
        char Bg1 = '#',
        char Bg2 = '@',
        uint scaleFactor = 1,
        uint tFps = 30
        ) :
            initialX(X),
            initialY(Y),
            initialBg1(Bg1),
            initialBg2(Bg2),
            initialScaleFactor(scaleFactor),
            initialFps(tFps),
            mapRow(Y),
            mapCol(X),
            mapBg1(Bg1),
            mapBg2(Bg2),
            mapScaleFactor(scaleFactor),
            map(Y, std::vector<char>(X, 0)),
            appleActive(false),
            targetFps(tFps),
            snakePrevX(0),
            snakePrevY(0)
    {
        reset();
    }

    void reset()
    {
        mapRow = initialY;
        mapCol = initialX;
        mapBg1 = initialBg1;
        mapBg2 = initialBg2;
        mapScaleFactor = initialScaleFactor;
        map.erase(map.begin(),map.end());
        map.assign(initialY, std::vector<char>(initialX, 0));
        appleActive = false;
        targetFps = initialFps;
        snakePrevX = 0;
        snakePrevY = 0;
        snake.erase(snake.begin(), snake.end());
        InitGame();
    }

    void InitGame()
    {
        map.reserve(mapRow);
        for (int i = 0; i < mapRow; i++)
        {
            map.emplace_back();
            map.back().reserve(mapCol);
        }
        fillMapVector();
        seedSnake();
        srand(time(nullptr));
    }

    void drawAll()
    {
        fillMapVector();
        if (appleActive)
        {
            map[applePos.first][applePos.second] = '*'; //draw appel
        }
        for (auto &p : snake) //drawing snake
        {
            map[p.first][p.second] = '$';
        }
    }

    void drawBuffer()
    {
        for (int i = 0; i < mapRow; i++)
        {
            for (int j = 0; j < mapCol; j++)
            {
                std::cout << map[i][j];
            }
            std::cout << "\n";
        }
    }

    static void clearScreen()
    {
        std::cout << "\033[2J\033[1;1H";
    }

    bool stop = false;

    void updateGameLogic()
    {
        moveSnake();
        checkAppleCollision();
        updateSnakeLength();
        collisionCheck();
        if (!appleActive){addApple();} //should be run after updating snake
    };

    void addApple()
    {
        ushort randX = (rand() % mapCol);
        ushort randY = (rand() % mapRow);
        for (auto p : snake)
        {
            if (p.first == randX && p.second == randY){addApple(); return;}
        }
        applePos.first = randX;
        applePos.second = randY;
        appleActive = true;
    }

    void processUserInput()
    {
        std::cin >> requestedDir;

        switch (currentDir){ //this fucking sucks
        case 'W':
            if (requestedDir == 'S'){break;}
            currentDir = requestedDir;
            break;
        case 'A':
            if (requestedDir == 'D'){break;}
            currentDir = requestedDir;
            break;
        case 'S':
            if (requestedDir == 'W'){break;}
            currentDir = requestedDir;
            break;
        case 'D':
            if (requestedDir == 'A'){break;}
            currentDir = requestedDir;
            break;
        default:
            break;
            requestedDir = currentDir;
        }
    }

    void moveSnake()
    {
        //handle bounds wrapping, fucked because of my direction implementation
        if (snake[0].first == 0 && currentDir == 'W'){snake[0].first = mapCol;} //left
        if (snake[0].first == mapCol-1 && currentDir == 'S'){snake[0].first = -1;} //right | weird wrap offset fix
        if (snake[0].second == 0 && currentDir == 'A'){snake[0].second = mapRow;} //top
        if (snake[0].second == mapRow-1 && currentDir == 'D'){snake[0].second = -1;} //down | weird wrap offset fix

        snakePrevX = snake[0].first;
        snakePrevY = snake[0].second;
        switch (currentDir)
        {
        case 'A': //again this is shit could of just calculated the difference
            snake[0].second--;
            break;
        case 'W':
            snake[0].first--;
            break;
        case 'D':
            snake[0].second++;
            break;
        case 'S':
            snake[0].first++;
            break;
        default:
            break;
        }
    }

    void updateSnakeLength()
    {
        for (int i = snake.size() - 1; i > 0; i--) {
            snake[i] = snake[i - 1];
        }
    }

    void checkAppleCollision()
    {
        if (snake[0].first == applePos.first && snake[0].second == applePos.second)
        {
            snake.emplace_back(snakePrevX,snakePrevY);
            appleActive = false;
        }
    }


private:
    char currentDir = 'W';
    char requestedDir;

    uint snakePrevX;
    uint snakePrevY;

    uint mapRow;
    uint mapCol;

    char mapBg1;
    char mapBg2;

    uint mapScaleFactor;

    std::vector<std::vector<char>> map;

    std::vector<std::pair<uint, uint>> snake;

    //note: map is indexed raylib style so topleft is starting point, anything down is +y and right +x

    uint targetFps; //probably dont modify

    bool appleActive;
    std::pair<uint, uint> applePos;

    void collisionCheck()
    {
        for (int i=2; i < snake.size(); i++)
        {
            if (snake[i].first == snake[0].first && snake[i].second == snake[0].second)
            {
                clearScreen();
                reset();
            }
        }
    };

    void fillMapVector()
    {
        for (int i = 0; i < mapRow; i++)
        {
            for (int j = 0; j < mapCol; j++)
            {
                if (i % 2 == 0) //checkered pattern
                {
                    if (j % 2 == 0){map[i][j] = mapBg1;}
                    else {map[i][j] = mapBg2;}
                }
                else
                {
                    if (j % 2 == 0){map[i][j] = mapBg2;}
                    else {map[i][j] = mapBg1;}
                }
            }
        }
        
    };
    void seedSnake()
    {
        uint snakeX = std::floor(mapCol / 2);
        uint snakeY = std::floor(mapRow / 2);
        snake.emplace(snake.begin(), snakeX, snakeY);
    }
};


int main()
{
    GameState game_state(5,5,'#','#',1);

    game_state.InitGame();
    while (!game_state.stop)
    {
        //input
        game_state.processUserInput();

        //logic
        game_state.updateGameLogic();

        //draw
        GameState::clearScreen();

        game_state.drawAll();
        game_state.drawBuffer();
    }

    return 0;
}