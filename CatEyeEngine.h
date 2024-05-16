#ifndef CATEYEENGINE_H
#define CATEYEENGINE_H

#include <Windows.h>
#include <vector>
#include <functional>
#include <string>
#include <memory>

struct Vector2D {
    float x, y;
};

struct GameObject {
    Vector2D position;
    int width, height;
    COLORREF color;
    float rotation;
    int zIndex;

    bool operator==(const GameObject& other) const {
        return position.x == other.position.x &&
            position.y == other.position.y &&
            width == other.width &&
            height == other.height &&
            color == other.color &&
            zIndex == other.zIndex;
    }
};

class CatEyeEngine {
private:
    static const int SCREEN_WIDTH = 800;
    static const int SCREEN_HEIGHT = 600;

    HWND hwnd;
    HDC hdc;
    HDC bufferDC;
    HBITMAP bufferBitmap;
    std::vector<std::unique_ptr<GameObject>> gameObjects;

    std::function<void(int key)> onKeyDown;
    std::function<void(int key)> onKeyUp;
    std::function<void(int x, int y)> onMouseMove;
    std::function<void()> onLeftMouseDown;
    std::function<void()> onLeftMouseUp;
    std::function<void()> onRightMouseDown;
    std::function<void()> onRightMouseUp;

    void SortGameObjectsByZIndex();

    bool keys[256];
    bool leftMouseDown;
    bool rightMouseDown;

public:
    CatEyeEngine();
    ~CatEyeEngine();

    void SetKeyDownCallback(std::function<void(int)> callback);
    void SetKeyUpCallback(std::function<void(int)> callback);
    void SetMouseMoveCallback(std::function<void(int, int)> callback);
    void SetLeftMouseDownCallback(std::function<void()> callback);
    void SetLeftMouseUpCallback(std::function<void()> callback);
    void SetRightMouseDownCallback(std::function<void()> callback);
    void SetRightMouseUpCallback(std::function<void()> callback);

    GameObject& AddGameObject();
    void RemoveGameObject(const GameObject& gameObject);
    const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const {
        return gameObjects;
    }

    void Render();
    HWND GetWindowHandle() const;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif
