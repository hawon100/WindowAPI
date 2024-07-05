#pragma once
#include "ConsoleGameEngine.h"
#include <vector>

class BrickBreaker : public ConsoleGameEngine
{
public:
    BrickBreaker()
    {
        m_sAppName = L"Brick Breaker";
    }

private:
    int m_nPaddleWidth = 20; // Adjusted for the larger screen width
    float m_fBallPosX = 100.0f;
    float m_fBallPosY = 90.0f;
    float m_fBallVelX = 0.0f;
    float m_fBallVelY = 0.0f;
    float m_nPaddlePos = 100.0f; // Changed to float for smooth movement
    bool m_bBallReleased = false;
    std::vector<int> m_vecBricks;

protected:
    virtual bool OnUserCreate() override
    {
        // Initialize ball position and velocity
        m_fBallPosX = 100.0f;
        m_fBallPosY = 90.0f;
        m_fBallVelX = 0.0f;
        m_fBallVelY = 0.0f;
        m_bBallReleased = false;

        // Create bricks (1 = exists, 0 = doesn't exist)
        m_vecBricks.resize(200);
        for (int i = 0; i < 200; i++)
        {
            m_vecBricks[i] = 1;
        }

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) override
    {
        // Clear Screen
        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

        // Control paddle
        if (GetKey(VK_LEFT).bHeld && m_nPaddlePos > 0)
            m_nPaddlePos -= 60.0f * fElapsedTime; // Adjust speed for larger screen
        if (GetKey(VK_RIGHT).bHeld && m_nPaddlePos < ScreenWidth() - m_nPaddleWidth)
            m_nPaddlePos += 60.0f * fElapsedTime; // Adjust speed for larger screen

        // Draw paddle
        for (int i = 0; i < m_nPaddleWidth; i++)
            Draw(m_nPaddlePos + i, ScreenHeight() - 1, PIXEL_SOLID, FG_WHITE);

        // Ball logic
        if (m_bBallReleased)
        {
            m_fBallPosX += m_fBallVelX * fElapsedTime;
            m_fBallPosY += m_fBallVelY * fElapsedTime;

            // Ball collision with walls
            if (m_fBallPosX < 0) m_fBallPosX = 0, m_fBallVelX *= -1.0f;
            if (m_fBallPosX >= ScreenWidth()) m_fBallPosX = ScreenWidth() - 1, m_fBallVelX *= -1.0f;
            if (m_fBallPosY < 0) m_fBallPosY = 0, m_fBallVelY *= -1.0f;

            // Ball collision with paddle
            if (m_fBallPosY >= ScreenHeight() - 1)
            {
                if (m_fBallPosX >= m_nPaddlePos && m_fBallPosX < m_nPaddlePos + m_nPaddleWidth)
                    m_fBallVelY *= -1.0f;
                else
                    m_bBallReleased = false; // Ball is lost
            }

            // Ball collision with bricks
            int nBrickX = (int)m_fBallPosX / 10;
            int nBrickY = (int)m_fBallPosY / 5;
            if (nBrickY >= 0 && nBrickY < 10 && nBrickX >= 0 && nBrickX < 20)
            {
                if (m_vecBricks[nBrickY * 20 + nBrickX] == 1)
                {
                    m_vecBricks[nBrickY * 20 + nBrickX] = 0;
                    m_fBallVelY *= -1.0f;
                }
            }
        }
        else
        {
            // Ball is on the paddle
            m_fBallPosX = m_nPaddlePos + m_nPaddleWidth / 2;
            m_fBallPosY = ScreenHeight() - 2;

            // Launch the ball
            if (GetKey(VK_SPACE).bPressed)
            {
                m_bBallReleased = true;
                m_fBallVelX = 30.0f * cosf(-3.14159f / 4.0f);
                m_fBallVelY = 30.0f * sinf(-3.14159f / 4.0f);
            }
        }

        // Draw ball
        Draw((int)m_fBallPosX, (int)m_fBallPosY, PIXEL_SOLID, FG_WHITE);

        // Draw bricks
        for (int x = 0; x < 20; x++)
        {
            for (int y = 0; y < 10; y++)
            {
                if (m_vecBricks[y * 20 + x] == 1)
                {
                    Fill(x * 10, y * 5, x * 10 + 9, y * 5 + 4, PIXEL_SOLID, FG_GREEN);
                }
            }
        }

        return true;
    }
};
