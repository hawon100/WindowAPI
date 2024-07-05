#include "ConsoleGameEngine.h"
#include <random>

class Minesweeper : public ConsoleGameEngine
{
public:
    Minesweeper()
    {
        m_sAppName = L"Minesweeper";
    }

private:
    int nFieldWidth;
    int nFieldHeight;
    int nMines;
    int* pField;
    bool* pRevealed;
    bool bGameOver;
    bool bGameWon;

protected:
    virtual bool OnUserCreate() override
    {
        nFieldWidth = 16;
        nFieldHeight = 16;
        nMines = 40;
        pField = new int[nFieldWidth * nFieldHeight];
        pRevealed = new bool[nFieldWidth * nFieldHeight];
        bGameOver = false;
        bGameWon = false;

        // Initialize field and place mines
        std::fill(pField, pField + nFieldWidth * nFieldHeight, 0);
        std::fill(pRevealed, pRevealed + nFieldWidth * nFieldHeight, false);

        // Place mines randomly
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> dist(0, nFieldWidth * nFieldHeight - 1);

        for (int i = 0; i < nMines; ++i)
        {
            int idx;
            do {
                idx = dist(rng);
            } while (pField[idx] == -1);
            pField[idx] = -1;

            // Increment adjacent cells
            int x = idx % nFieldWidth;
            int y = idx / nFieldWidth;
            for (int dx = -1; dx <= 1; ++dx)
            {
                for (int dy = -1; dy <= 1; ++dy)
                {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < nFieldWidth && ny >= 0 && ny < nFieldHeight && pField[ny * nFieldWidth + nx] != -1)
                    {
                        pField[ny * nFieldWidth + nx]++;
                    }
                }
            }
        }
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        int nMouseX = GetMouseX();
        int nMouseY = GetMouseY();

        if (bGameOver || bGameWon)
        {
            if (GetKey(VK_RETURN).bPressed)
            {
                OnUserCreate(); // Reset game
                return true;
            }
        }
        else
        {
            if (GetMouse(0).bPressed) // Left click
            {
                int x = nMouseX / 2;
                int y = nMouseY;
                if (x >= 0 && x < nFieldWidth && y >= 0 && y < nFieldHeight)
                {
                    if (pField[y * nFieldWidth + x] == -1)
                    {
                        bGameOver = true;
                    }
                    else
                    {
                        Reveal(x, y);
                    }

                    // Check for victory
                    bGameWon = true;
                    for (int i = 0; i < nFieldWidth * nFieldHeight; ++i)
                    {
                        if (!pRevealed[i] && pField[i] != -1)
                        {
                            bGameWon = false;
                            break;
                        }
                    }
                }
            }

            if (GetMouse(1).bPressed) // Right click
            {
                // Implement flagging functionality if desired
            }
        }

        // Render the field
        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
        for (int x = 0; x < nFieldWidth; ++x)
        {
            for (int y = 0; y < nFieldHeight; ++y)
            {
                if (pRevealed[y * nFieldWidth + x])
                {
                    if (pField[y * nFieldWidth + x] == -1)
                    {
                        Draw(x * 2, y, L'X', FG_RED);
                    }
                    else
                    {
                        Draw(x * 2, y, L'0' + pField[y * nFieldWidth + x], FG_WHITE);
                    }
                }
                else
                {
                    Draw(x * 2, y, L'#', FG_GREY);
                }
            }
        }

        if (bGameOver)
        {
            DrawString(2, ScreenHeight() / 2 - 1, L"GAME OVER! Press ENTER to restart.", FG_RED);
        }
        else if (bGameWon)
        {
            DrawString(2, ScreenHeight() / 2 - 1, L"YOU WIN! Press ENTER to restart.", FG_GREEN);
        }

        return true;
    }

    void Reveal(int x, int y)
    {
        if (x < 0 || x >= nFieldWidth || y < 0 || y >= nFieldHeight || pRevealed[y * nFieldWidth + x])
        {
            return;
        }

        pRevealed[y * nFieldWidth + x] = true;

        if (pField[y * nFieldWidth + x] == 0)
        {
            for (int dx = -1; dx <= 1; ++dx)
            {
                for (int dy = -1; ++dy <= 1; ++dy)
                {
                    Reveal(x + dx, y + dy);
                }
            }
        }
    }
};

int main()
{
    Minesweeper game;
    game.ConstructConsole(32, 16, 16, 16);
    game.Start();
    return 0;
}
