#include "ConsoleGameEngine.h"

class Tetris : public ConsoleGameEngine
{
public:
    Tetris()
    {
        m_sAppName = L"Tetris";
    }

private:
    int m_nFieldWidth = 12;
    int m_nFieldHeight = 18;
    unsigned char* m_pField = nullptr;

    std::wstring tetromino[7];

    int Rotate(int px, int py, int r)
    {
        switch (r % 4)
        {
        case 0: return py * 4 + px; // 0 degrees
        case 1: return 12 + py - (px * 4); // 90 degrees
        case 2: return 15 - (py * 4) - px; // 180 degrees
        case 3: return 3 - py + (px * 4); // 270 degrees
        }
        return 0;
    }

    bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
    {
        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
            {
                int pi = Rotate(px, py, nRotation);
                int fi = (nPosY + py) * m_nFieldWidth + (nPosX + px);

                if (nPosX + px >= 0 && nPosX + px < m_nFieldWidth)
                {
                    if (nPosY + py >= 0 && nPosY + py < m_nFieldHeight)
                    {
                        if (tetromino[nTetromino][pi] != L'.' && m_pField[fi] != 0)
                            return false;
                    }
                }
            }
        return true;
    }

protected:
    virtual bool OnUserCreate()
    {
        m_pField = new unsigned char[m_nFieldWidth * m_nFieldHeight];
        for (int x = 0; x < m_nFieldWidth; x++)
            for (int y = 0; y < m_nFieldHeight; y++)
                m_pField[y * m_nFieldWidth + x] = (x == 0 || x == m_nFieldWidth - 1 || y == m_nFieldHeight - 1) ? 9 : 0;

        tetromino[0].append(L"..X...X...X...X.");
        tetromino[1].append(L"..X..XX...X.....");
        tetromino[2].append(L".....XX..XX.....");
        tetromino[3].append(L"..X..XX..X......");
        tetromino[4].append(L".X...XX...X.....");
        tetromino[5].append(L".X...X...XX.....");
        tetromino[6].append(L"..X...X..XX.....");

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime)
    {
        // Game logic
        static int nCurrentPiece = 0;
        static int nCurrentRotation = 0;
        static int nCurrentX = m_nFieldWidth / 2;
        static int nCurrentY = 0;
        static int nSpeed = 30; // Increase the speed value to slow down the piece falling speed
        static int nSpeedCount = 0;
        static bool bForceDown = false;
        static int nPieceCount = 0;
        static int nScore = 0;
        static bool bKey[4];

        for (int k = 0; k < 4; k++)
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x27\x28Z"[k]))) != 0;

        nSpeedCount++;
        bForceDown = (nSpeedCount == nSpeed);

        // Game logic
        if (bKey[0])
        {
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))
                nCurrentX = nCurrentX - 1;
        }

        if (bKey[1])
        {
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY))
                nCurrentX = nCurrentX + 1;
        }

        if (bKey[2])
        {
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
                nCurrentY = nCurrentY + 1;
        }

        if (bKey[3])
        {
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY))
                nCurrentRotation = nCurrentRotation + 1;
        }

        if (bForceDown)
        {
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
                nCurrentY++;
            else
            {
                for (int px = 0; px < 4; px++)
                    for (int py = 0; py < 4; py++)
                    {
                        if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
                            m_pField[(nCurrentY + py) * m_nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
                    }

                nPieceCount++;
                if (nPieceCount % 10 == 0)
                    if (nSpeed >= 10) nSpeed--;

                for (int py = 0; py < 4; py++)
                    if (nCurrentY + py < m_nFieldHeight - 1)
                    {
                        bool bLine = true;
                        for (int px = 1; px < m_nFieldWidth - 1; px++)
                            bLine &= (m_pField[(nCurrentY + py) * m_nFieldWidth + px]) != 0;

                        if (bLine)
                        {
                            for (int px = 1; px < m_nFieldWidth - 1; px++)
                                m_pField[(nCurrentY + py) * m_nFieldWidth + px] = 8;
                        }
                    }

                nCurrentX = m_nFieldWidth / 2;
                nCurrentY = 0;
                nCurrentRotation = 0;
                nCurrentPiece = rand() % 7;

                if (!DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY))
                    return false;
            }

            nSpeedCount = 0;
        }

        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ', 0);
        for (int x = 0; x < m_nFieldWidth; x++)
            for (int y = 0; y < m_nFieldHeight; y++)
                Draw(x + 2, y + 2, L" ABCDEFG=#"[m_pField[y * m_nFieldWidth + x]], m_pField[y * m_nFieldWidth + x] == 0 ? 0 : m_pField[y * m_nFieldWidth + x] == 9 ? FG_WHITE : FG_GREEN);

        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
                if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
                    Draw(nCurrentX + px + 2, nCurrentY + py + 2, nCurrentPiece + 65, FG_RED);

        return true;
    }
};

int main()
{
    Tetris game;
    game.ConstructConsole(80, 30, 16, 16);
    game.Start();
    return 0;
}
