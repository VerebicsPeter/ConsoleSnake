#include <iostream>
#include <ctime>
#include <conio.h>
#include <windows.h>
// for sleep delay
#include <chrono>
#include <thread>

using namespace std;

const int width = 32;
const int height = 24;
int highscore = 1;

class Point {
public: int x;
public: int y;
};

class Snake {
public: char headChar;
public: char tailChar;
public: int length;
public: Point head;
public: Point tailpoint[1000];
};

enum class DIR { UP, DOWN, LEFT, RIGHT, NONE };
DIR direction;
Snake snake;
Point fruit;

char snakemap[height][width];
bool gameover;
bool walls;

void clearScreen()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD Position;
    Position.X = 0;
    Position.Y = 0;
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorPosition(hOut, Position);
    SetConsoleCursorInfo(hOut, &info);
}

bool inTail(int x, int y)
{
    for (int i = 1; i < snake.length; i++) if (x == snake.tailpoint[i].x && y == snake.tailpoint[i].y) return true;
    return false;
}

void resetMap()
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j == 0 || j == width - 1 || i == 0 || i == height - 1) snakemap[i][j] = '#';
            else snakemap[i][j] = ' ';
        }
    }
    for (int i = 0; i < 1000; i++)
    {
        snake.tailpoint[i].x = -1;
        snake.tailpoint[i].y = -1;
    }
}

void setFruitPos()
{
    do {
        fruit.x = rand() % height;
        fruit.y = rand() % width;
    } while (snakemap[fruit.x][fruit.y] == '#' || inTail(fruit.x, fruit.y));
}

void reset()
{
    gameover = false;
    resetMap();
    srand(time(NULL));
    setFruitPos();
    snake.headChar = '#';
    snake.tailChar = '#';
    snake.length = 1;
    snake.head.x = height / 2;
    snake.head.y = width / 2;
    direction = DIR::NONE;
    cout << endl;
    cout << " Walls?" << endl;
    cout << " 1 - no" << endl;
    cout << " 2 - yes " << endl;

    char mode;
    do {
        mode = _getch();
    } while (mode != '1' && mode != '2');
    if (mode == '2') walls = true;
    else walls = false;
}

void input()
{
    if (_kbhit())
    {
        char key = _getch();
        if ((key == 'w' || key == 'W') && direction != DIR::DOWN) direction = DIR::UP;
        else if ((key == 's' || key == 'S') && direction != DIR::UP) direction = DIR::DOWN;
        else if ((key == 'a' || key == 'A') && direction != DIR::RIGHT) direction = DIR::LEFT;
        else if ((key == 'd' || key == 'D') && direction != DIR::LEFT) direction = DIR::RIGHT;
        else if (key == 'p' || key == 'P')
        {
            cout << " paused" << endl;
            char p;
            do {
                p = _getch();
            } while (p != 'p' && p != 'P');
            system("cls");
        }
    }
}

void moveFirst() // moves first tail segment to head's position
{
    snake.tailpoint[0].x = snake.head.x;
    snake.tailpoint[0].y = snake.head.y;
}

void moveSnake() // moves head
{
    if (direction != DIR::NONE)
    {
        moveFirst();
        for (int i = snake.length; i > 1; i--)
        {
            snake.tailpoint[i - 1].x = snake.tailpoint[i - 2].x;
            snake.tailpoint[i - 1].y = snake.tailpoint[i - 2].y;
        }
        if (direction == DIR::DOWN)
        {
            snake.head.x++;
        }
        else if (direction == DIR::UP)
        {
            snake.head.x--;
        }
        else if (direction == DIR::LEFT)
        {
            snake.head.y--;
        }
        else if (direction == DIR::RIGHT)
        {
            snake.head.y++;
        }
    }
}

void collision()
{
    int x = snake.head.x;
    int y = snake.head.y;
    if (walls)
    {
        if (x == 0 || x == height - 1 || y == 0 || y == width - 1) gameover = true;
    }
    else
    {
        if (x == 0)
        {
            snake.head.x = height - 2;
            if (snake.head.x == fruit.x && snake.head.y == fruit.y)
            {
                snake.length++;
                setFruitPos();
            }
        }
        else if (x == height - 1)
        {
            snake.head.x = 1;
            if (snake.head.x == fruit.x && snake.head.y == fruit.y)
            {
                snake.length++;
                setFruitPos();
            }
        }
        else if (y == 0)
        {
            {
                snake.head.y = width - 2;
                if (snake.head.x == fruit.x && snake.head.y == fruit.y)
                {
                    snake.length++;
                    setFruitPos();
                }
            }
        }
        else if (y == width - 1)
        {
            snake.head.y = 1;
            if (snake.head.x == fruit.x && snake.head.y == fruit.y)
            {
                snake.length++;
                setFruitPos();
            }
        }
    }
    if (inTail(x, y)) gameover = true;
    else if (x == fruit.x && y == fruit.y)
    {
        snake.length++;
        setFruitPos();
    }
}

void draw()
{
    clearScreen();
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    cout << "  score: " << snake.length << "\t highscore: " << highscore << endl;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (i == snake.head.x && j == snake.head.y)
            {
                SetConsoleTextAttribute(hOut, 2);
                cout << snake.headChar;
            }
            else if (inTail(i, j))
            {
                SetConsoleTextAttribute(hOut, 2);
                cout << snake.tailChar;
            }
            else if (i == fruit.x && j == fruit.y)
            {
                SetConsoleTextAttribute(hOut, 4);
                cout << 'O';
            }
            else cout << snakemap[i][j];
            SetConsoleTextAttribute(hOut, 7);
        }
        cout << endl;
    }
}

int main()
{
    while (true)
    {
        reset();
        system("cls");
        while (!gameover)
        {
            draw();
            input();
            moveSnake();
            collision();
            this_thread::sleep_for(20ms); // 16ms = 60fps 32ms = 30fps 64ms = 15fps 48ms = 20fps 
            if (snake.length > highscore) highscore = snake.length;
        }
        PlaySound(TEXT("bruh.wav"), NULL, SND_SYNC);
        cout << " Game Over\n Press a key to restart!" << endl;

        char ch = _getch();
        if (ch == 'q' || ch == 'Q') break;
    }
    return 0;
}
