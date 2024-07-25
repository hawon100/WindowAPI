#define UNICODE
#define _UNICODE

#include "TextRPG.h"

int main() {
    TextRPG game;
    if (game.ConstructConsole(160, 100, 8, 8)) {
        game.Start();
    }
    return 0;
}
