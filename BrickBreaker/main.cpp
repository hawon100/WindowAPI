#include "BrickBreaker.h"

int main()
{
    BrickBreaker game;
    if (game.ConstructConsole(200, 100, 4, 4))
        game.Start();
    return 0;
}
