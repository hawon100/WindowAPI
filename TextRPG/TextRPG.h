#pragma once
#include "ConsoleGameEngine.h"

// 플레이어, 적, 아이템 등 게임 요소 정의
struct Character {
    std::wstring name;
    int health;
    int attack;
    int defense;
};

class TextRPG : public ConsoleGameEngine {
public:
    TextRPG() {
        m_sAppName = L"Text RPG";
    }

private:
    Character player;
    std::vector<Character> enemies;
    std::vector<std::wstring> items;
    int currentLevel;
    bool inBattle;
    bool inExplore;
    bool gameOver;

    void InitializeGame() {
        // 플레이어 초기화
        player.name = L"Hero";
        player.health = 100;
        player.attack = 10;
        player.defense = 5;

        // 적 초기화
        enemies.push_back({ L"Goblin", 30, 5, 2 });
        enemies.push_back({ L"Orc", 50, 8, 4 });
        enemies.push_back({ L"Dragon", 100, 15, 10 });

        // 아이템 초기화
        items.push_back(L"Health Potion");
        items.push_back(L"Attack Boost");
        items.push_back(L"Defense Boost");

        // 시작 레벨
        currentLevel = 0;
        inBattle = false;
        inExplore = true;
        gameOver = false;
    }

    void DisplayStatus() {
        DrawString(2, 2, L"Player: " + player.name);
        DrawString(2, 3, L"Health: " + std::to_wstring(player.health));
        DrawString(2, 4, L"Attack: " + std::to_wstring(player.attack));
        DrawString(2, 5, L"Defense: " + std::to_wstring(player.defense));
    }

    void Battle(Character& enemy) {
        inBattle = true;
        while (player.health > 0 && enemy.health > 0) {
            int playerDamage = player.attack - enemy.defense;
            if (playerDamage < 0) playerDamage = 0;

            int enemyDamage = enemy.attack - player.defense;
            if (enemyDamage < 0) enemyDamage = 0;

            enemy.health -= playerDamage;
            player.health -= enemyDamage;

            // 전투 상황 출력
            Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
            DrawString(2, 2, L"Fighting " + enemy.name);
            DrawString(2, 3, L"Player Health: " + std::to_wstring(player.health));
            DrawString(2, 4, L"Enemy Health: " + std::to_wstring(enemy.health));
            DrawString(2, 6, L"Press any key to continue...");
            return;
        }
        if (player.health > 0) {
            currentLevel++;
            inBattle = false;
            inExplore = true;
        }
        else {
            gameOver = true;
        }
    }

    void Explore() {
        inExplore = true;
        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
        DrawString(2, 2, L"Exploring level " + std::to_wstring(currentLevel + 1));
        DrawString(2, 4, L"Press space to encounter an enemy...");
    }

public:
    bool OnUserCreate() override {
        InitializeGame();
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        if (gameOver) {
            Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
            DrawString(2, 2, L"Game Over! You died.");
            return false;
        }
        if (currentLevel >= enemies.size()) {
            Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
            DrawString(2, 2, L"Congratulations! You defeated all enemies.");
            return false;
        }

        if (inExplore && m_keys[VK_SPACE].bPressed) {
            inExplore = false;
            Battle(enemies[currentLevel]);
        }
        else if (inBattle && m_keys[VK_SPACE].bPressed) {
            Battle(enemies[currentLevel]);
        }
        else if (!inBattle && !inExplore) {
            Explore();
        }

        return true;
    }
};
