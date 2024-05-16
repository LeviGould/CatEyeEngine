#include "CatEyeEngine.h"

CatEyeEngine::CatEyeEngine() {
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"GameWindowClass";
    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        0,
        L"GameWindowClass",
        L"CatEyeEngine",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        this
    );

    if (hwnd == NULL) {
        MessageBox(NULL, L"Window creation failed", L"Error", MB_ICONERROR);
        exit(1);
    }

    hdc = GetDC(hwnd);

    bufferDC = CreateCompatibleDC(hdc);
    bufferBitmap = CreateCompatibleBitmap(hdc, SCREEN_WIDTH, SCREEN_HEIGHT);
    SelectObject(bufferDC, bufferBitmap);

    memset(keys, false, sizeof(keys));
    leftMouseDown = false;
    rightMouseDown = false;
}

CatEyeEngine::~CatEyeEngine() {
    DeleteObject(bufferBitmap);
    DeleteDC(bufferDC);
    ReleaseDC(hwnd, hdc);
}

void CatEyeEngine::SetKeyDownCallback(std::function<void(int)> callback) {
    onKeyDown = callback;
}

void CatEyeEngine::SetKeyUpCallback(std::function<void(int)> callback) {
    onKeyUp = callback;
}

void CatEyeEngine::SetMouseMoveCallback(std::function<void(int, int)> callback) {
    onMouseMove = callback;
}

void CatEyeEngine::SetLeftMouseDownCallback(std::function<void()> callback) {
    onLeftMouseDown = callback;
}

void CatEyeEngine::SetLeftMouseUpCallback(std::function<void()> callback) {
    onLeftMouseUp = callback;
}

void CatEyeEngine::SetRightMouseDownCallback(std::function<void()> callback) {
    onRightMouseDown = callback;
}

void CatEyeEngine::SetRightMouseUpCallback(std::function<void()> callback) {
    onRightMouseUp = callback;
}

GameObject& CatEyeEngine::AddGameObject() {
    gameObjects.push_back(std::make_unique<GameObject>());
    return *gameObjects.back();
}

void CatEyeEngine::RemoveGameObject(const GameObject& gameObject) {
    for (auto it = gameObjects.begin(); it != gameObjects.end(); ++it) {
        if (**it == gameObject) {
            gameObjects.erase(it);
            break;
        }
    }
}

void CatEyeEngine::SortGameObjectsByZIndex() {
    bool swapped;
    int n = gameObjects.size();
    do {
        swapped = false;
        for (int i = 1; i < n; ++i) {
            if (gameObjects[i - 1]->zIndex > gameObjects[i]->zIndex) {
                std::swap(gameObjects[i - 1], gameObjects[i]);
                swapped = true;
            }
        }
        n--;
    } while (swapped);
}

void CatEyeEngine::Render() {
    RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    FillRect(bufferDC, &rect, (HBRUSH)(COLOR_WINDOW + 1));

    SortGameObjectsByZIndex();

    for (const auto& gameObjectPtr : gameObjects) {
        const auto& gameObject = *gameObjectPtr;

        HBRUSH hBrush = CreateSolidBrush(gameObject.color);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(bufferDC, hBrush);
        float centerX = gameObject.position.x + gameObject.width / 2.0f;
        float centerY = gameObject.position.y + gameObject.height / 2.0f;

        XFORM xForm;
        xForm.eM11 = cos(gameObject.rotation);
        xForm.eM12 = sin(gameObject.rotation);
        xForm.eM21 = -sin(gameObject.rotation);
        xForm.eM22 = cos(gameObject.rotation);
        xForm.eDx = centerX - centerX * xForm.eM11 - centerY * xForm.eM21;
        xForm.eDy = centerY - centerX * xForm.eM12 - centerY * xForm.eM22;
        SetGraphicsMode(bufferDC, GM_ADVANCED);
        SetWorldTransform(bufferDC, &xForm);

        Rectangle(bufferDC, gameObject.position.x , gameObject.position.y, gameObject.position.x + gameObject.width, gameObject.position.y + gameObject.height);

        ModifyWorldTransform(bufferDC, NULL, MWT_IDENTITY);

        SelectObject(bufferDC, hOldBrush);
        DeleteObject(hBrush);
    }

    BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bufferDC, 0, 0, SRCCOPY);
}

HWND CatEyeEngine::GetWindowHandle() const {
    return hwnd;
}


LRESULT CALLBACK CatEyeEngine::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    CatEyeEngine* engine = nullptr;
    if (uMsg == WM_CREATE) {
        engine = (CatEyeEngine*)((CREATESTRUCT*)lParam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)engine);
    }
    else {
        engine = (CatEyeEngine*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (engine) {
        switch (uMsg) {
        case WM_PAINT:
            engine->Render();
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_KEYDOWN:
            if (engine->onKeyDown) {
                int key = static_cast<int>(wParam);
                engine->onKeyDown(key);
            }
            break;
        case WM_KEYUP:
            if (engine->onKeyUp) {
                int key = static_cast<int>(wParam);
                engine->onKeyUp(key);
            }
            break;
        case WM_MOUSEMOVE:
            if (engine->onMouseMove) {
                int xPos = LOWORD(lParam);
                int yPos = HIWORD(lParam);
                engine->onMouseMove(xPos, yPos);
            }
            break;
        case WM_LBUTTONDOWN:
            engine->leftMouseDown = true;
            if (engine->onLeftMouseDown) {
                engine->onLeftMouseDown();
            }
            break;
        case WM_LBUTTONUP:
            engine->leftMouseDown = false;
            if (engine->onLeftMouseUp) {
                engine->onLeftMouseUp();
            }
            break;
        case WM_RBUTTONDOWN:
            engine->rightMouseDown = true;
            if (engine->onRightMouseDown) {
                engine->onRightMouseDown();
            }
            break;
        case WM_RBUTTONUP:
            engine->rightMouseDown = false;
            if (engine->onRightMouseUp) {
                engine->onRightMouseUp();
            }
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    else {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}
