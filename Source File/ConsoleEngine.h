#pragma once

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

class ConsoleEngine
{
protected:
    int screenWidth = 120;
    int screenHeight = 40;

    std::vector<char> screen;

    termios originalTermios;

    bool running = true;

public:

    ConsoleEngine()
    {
        enableRawMode();

        std::cout << "\033[2J";
        std::cout << "\033[?25l";
    }

    virtual ~ConsoleEngine()
    {
        disableRawMode();

        std::cout << "\033[?25h";
    }

public:

    bool ConstructConsole(int w, int h)
    {
        screenWidth = w;
        screenHeight = h;

        screen.resize(screenWidth * screenHeight, ' ');

        return true;
    }

    virtual bool OnUserCreate() = 0;

    virtual bool OnUserUpdate(float fElapsedTime) = 0;

public:

    void Start()
    {
        if (!OnUserCreate())
            return;

        auto tp1 = std::chrono::high_resolution_clock::now();

        while (running)
        {
            auto tp2 = std::chrono::high_resolution_clock::now();

            std::chrono::duration<float> elapsed = tp2 - tp1;

            tp1 = tp2;

            float dt = elapsed.count();

            if (!OnUserUpdate(dt))
                running = false;

            Render();

            std::this_thread::sleep_for(
                std::chrono::milliseconds(16));
        }
    }

public:

    void Clear()
    {
        std::fill(screen.begin(), screen.end(), ' ');
    }

    void Draw(int x, int y, char c)
    {
        if (x >= 0 && x < screenWidth &&
            y >= 0 && y < screenHeight)
        {
            screen[y * screenWidth + x] = c;
        }
    }

    bool keyPressed(char key)
    {
        char ch;

        if (read(STDIN_FILENO, &ch, 1) > 0)
        {
            return ch == key;
        }

        return false;
    }

private:

    void Render()
    {
        std::cout << "\033[H";

        for (int y = 0; y < screenHeight; y++)
        {
            for (int x = 0; x < screenWidth; x++)
            {
                std::cout << screen[y * screenWidth + x];
            }

            std::cout << '\n';
        }

        std::cout.flush();
    }

    void enableRawMode()
    {
        tcgetattr(STDIN_FILENO, &originalTermios);

        termios raw = originalTermios;

        raw.c_lflag &= ~(ECHO | ICANON);

        tcsetattr(STDIN_FILENO,
                  TCSAFLUSH,
                  &raw);

        fcntl(STDIN_FILENO,
              F_SETFL,
              O_NONBLOCK);
    }

    void disableRawMode()
    {
        tcsetattr(STDIN_FILENO,
                  TCSAFLUSH,
                  &originalTermios);
    }
};