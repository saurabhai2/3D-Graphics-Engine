#include "ConsoleEngine.h"

class Game : public ConsoleEngine
{
public:
  
    int x = 10;

    bool OnUserCreate() override
    {
        return true;
    }

    bool OnUserUpdate(float dt) override
    {
        Clear();

        Draw(x, 10, L'@');

        if (keyPressed('a'))
            x--;

        if (keyPressed('d'))
            x++;

        return true;
    }
};

int main()
{
    Game game;

    game.ConstructConsole(120, 40);

    game.Start();

    return 0;
}