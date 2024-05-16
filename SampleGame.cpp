// small test

#include "CatEyeEngine.h"

#include <iostream>
#include <string>

bool moveLeft = false;
bool moveRight = false;
bool moveUp = false;
bool moveDown = false;
bool lmouseDown = false;
int mouseX, mouseY = 0;

void HandleKeyDown(int key) {
    switch (key) {
    case 'A':
        moveLeft = true;
        break;
    case 'D':
        moveRight = true;
        break;
    case 'W':
        moveUp = true;
        break;
    case 'S':
        moveDown = true;
        break;
    default:
        break;
    }
}

void HandleKeyUp(int key) {
    switch (key) {
    case 'A':
        moveLeft = false;
        break;
    case 'D':
        moveRight = false;
        break;
    case 'W':
        moveUp = false;
        break;
    case 'S':
        moveDown = false;
        break;
    default:
        break;
    }
}

void HandleMouseMove(int x, int y) {
    mouseX = x;
    mouseY = y;
}
void HandleLeftMouseDown() {
    lmouseDown = true;
}

void HandleLeftMouseUp() {
    lmouseDown = false;
}

int main() {
    CatEyeEngine engine;
    ShowCursor(false);

    engine.SetKeyDownCallback(HandleKeyDown);
    engine.SetKeyUpCallback(HandleKeyUp);
    engine.SetMouseMoveCallback(HandleMouseMove);
    engine.SetLeftMouseDownCallback(HandleLeftMouseDown);
    engine.SetLeftMouseUpCallback(HandleLeftMouseUp);


    GameObject& player = engine.AddGameObject();
    player.position = { 100, 100 };
    player.width = 50;
    player.height = 50;
    player.color = RGB(255, 0, 255);
    player.zIndex = 1;

    GameObject& crosshair = engine.AddGameObject();
    crosshair.position = { 0, 0 };
    crosshair.width = 6;
    crosshair.height = 6;
    crosshair.color = RGB(255, 255, 255);
    crosshair.zIndex = INT_MAX;

    GameObject& arrow = engine.AddGameObject();
    arrow.position = { player.position.x, player.position.y};
    arrow.width = 12;
    arrow.height = 4;
    arrow.color = RGB(255, 0, 0);
    arrow.zIndex = 0;

    std::vector<GameObject*> bullets;


    ShowWindow(engine.GetWindowHandle(), SW_SHOW);
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    MSG msg;
    bool lastFrameLMDStatus = false;
    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) {
                return 0;
            }
        }

        if (moveLeft) {
            player.position.x -= 0.05f;
        }
        if (moveRight) {
            player.position.x += 0.05f;
        }
        if (moveUp) {
            player.position.y -= 0.05f;
        }
        if (moveDown) {
            player.position.y += 0.05f;
        }

        crosshair.position.x = mouseX;
        crosshair.position.y = mouseY;

        float dx = mouseX - (player.position.x + player.width / 2);
        float dy = mouseY - (player.position.y + player.height / 2);
        float angle = atan2(dy, dx);

        float radius = player.width;
        float arrowX = player.position.x + player.width / 2 + radius * cos(angle);
        float arrowY = player.position.y + player.height / 2 + radius * sin(angle);

        arrow.position = { arrowX - arrow.width / 2, arrowY - arrow.height / 2 };
        arrow.rotation = angle;

        if (lmouseDown && lastFrameLMDStatus == false) {
            GameObject& bullet = engine.AddGameObject();
            bullet.position.x = arrowX - bullet.width;
            bullet.position.y = arrowY - bullet.height / 2;
            bullet.width = 4;
            bullet.height = 4;
            bullet.color = RGB(255, 255, 0);
            bullet.zIndex = 2;
            bullet.rotation = arrow.rotation;

            bullets.push_back(&bullet);
        }
        
        float bulletSpeed = 0.1f;
        for (auto bulletPtr : bullets) {
            auto& bullet = *bulletPtr;
            bullet.position.x += bulletSpeed * cos(bullet.rotation);
            bullet.position.y += bulletSpeed * sin(bullet.rotation);
        }

        if (bullets.size() >= 10) {
            engine.RemoveGameObject(*bullets.front());
            bullets.erase(bullets.begin());
        }


        lastFrameLMDStatus = lmouseDown;

    }

    return 0;
}
