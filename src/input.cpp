// ============================================================================
//  花至荼蘼 —— 跨平台按键输入实现
// ============================================================================
#include "input.h"

#include <iostream>

#ifdef _WIN32
// ----------------------------- Windows -------------------------------------
#include <conio.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace game::input {

namespace {
// 是否已为控制台输入句柄启用鼠标事件（init 时开启，restore 时关闭）
HANDLE g_hIn = INVALID_HANDLE_VALUE;
DWORD g_oldInputMode = 0;
bool g_mouseEnabled = false;
}  // namespace

void init() {
    g_hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (g_hIn == INVALID_HANDLE_VALUE || g_hIn == nullptr) return;
    if (!GetConsoleMode(g_hIn, &g_oldInputMode)) return;

    // 目标模式：开启鼠标输入，并关闭 QuickEdit（否则左键=框选文字，程序收不到点击）。
    // 官方要求：改 QuickEdit 时必须带 ENABLE_EXTENDED_FLAGS；
    // 「ENABLE_EXTENDED_FLAGS 不带 QUICK_EDIT 位」本身即表示禁用 QuickEdit。
    // 同时保留 ENABLE_PROCESSED_INPUT，让 Ctrl+C 等仍按标准处理。
    DWORD mode = ENABLE_EXTENDED_FLAGS |              // 使 QuickEdit/Insert 位被正确解释
                 ENABLE_MOUSE_INPUT |                 // 接收鼠标事件
                 (g_oldInputMode & ENABLE_PROCESSED_INPUT);  // 保留回车/退格处理

    if (SetConsoleMode(g_hIn, mode)) {
        g_mouseEnabled = true;
    } else {
        // 某些老 conhost 不接受整套标志：退一步只开鼠标输入（QuickEdit 可能仍在，
        // 但至少 ENABLE_MOUSE_INPUT 有几率生效）。
        DWORD mode2 = g_oldInputMode | ENABLE_MOUSE_INPUT;
        if (SetConsoleMode(g_hIn, mode2)) g_mouseEnabled = true;
    }
}

void restore() {
    if (g_hIn != INVALID_HANDLE_VALUE && g_hIn != nullptr) {
        SetConsoleMode(g_hIn, g_oldInputMode);
        g_hIn = INVALID_HANDLE_VALUE;
        g_mouseEnabled = false;
    }
}

bool kbhit() { return _kbhit() != 0; }

namespace {
// 把 Windows 控制台 KEY_EVENT_RECORD 转成统一的逻辑键码 / ASCII。
// 覆盖：方向键、回车、Esc、退格、Tab，以及普通可打印 ASCII 字符。
int keyEventToKey(const KEY_EVENT_RECORD& e) {
    // 优先按虚拟键码判断功能键
    switch (e.wVirtualKeyCode) {
        case VK_UP:    return KEY_UP;
        case VK_DOWN:  return KEY_DOWN;
        case VK_LEFT:  return KEY_LEFT;
        case VK_RIGHT: return KEY_RIGHT;
        case VK_RETURN:return KEY_ENTER;
        case VK_ESCAPE:return KEY_ESC;
        case VK_BACK:  return KEY_BACKSPACE;
        case VK_TAB:   return KEY_TAB;
        case VK_SPACE: return ' ';
        default: break;
    }
    // 普通字符：取 Unicode 字符的低字节（ASCII 范围直接可用）
    if (e.uChar.UnicodeChar != 0) {
        wchar_t wc = e.uChar.UnicodeChar;
        if (wc < 0x80) return static_cast<int>(wc);
        return KEY_NONE;   // 非 ASCII（如中文）映射为无
    }
    return KEY_NONE;
}
}  // namespace

int readKey() {
    // 开着鼠标输入时也必须用 INPUT_RECORD 读：否则 _getch() 会把鼠标事件读成
    // 0/0xE0 前缀，导致「鼠标一指，按键就丢」。这里转调 readEvent 并丢弃点击。
    if (g_mouseEnabled) {
        int x = 0, y = 0;
        while (true) {
            std::pair<int, bool> ev = readEvent(x, y);
            if (!ev.second) return ev.first;   // 只返回按键，鼠标点击忽略
        }
    }
    int c = _getch();
    if (c == 0 || c == 224) {  // 方向键前缀
        int c2 = _getch();
        switch (c2) {
            case 72: return KEY_UP;
            case 80: return KEY_DOWN;
            case 75: return KEY_LEFT;
            case 77: return KEY_RIGHT;
            default: return KEY_NONE;
        }
    }
    if (c == '\r') return KEY_ENTER;
    if (c == 3) return KEY_ESC;  // Ctrl+C 视为 Esc
    return c;
}

std::pair<int, bool> readEvent(int& outX, int& outY) {
    if (!g_mouseEnabled) {
        // 无法用鼠标时退化为纯按键读取
        int k = readKey();
        return std::make_pair(k, false);
    }
    while (true) {
        INPUT_RECORD rec;
        DWORD n = 0;
        if (!ReadConsoleInputW(g_hIn, &rec, 1, &n) || n == 0) {
            // 读取失败：退化
            int k = readKey();
            return std::make_pair(k, false);
        }
        if (rec.EventType == KEY_EVENT && rec.Event.KeyEvent.bKeyDown) {
            int k = keyEventToKey(rec.Event.KeyEvent);
            if (k == KEY_NONE) continue;   // 忽略未知/修饰键
            return std::make_pair(k, false);
        }
        if (rec.EventType == MOUSE_EVENT) {
            const MOUSE_EVENT_RECORD& m = rec.Event.MouseEvent;
            // 左键按下（单击）才算一次点击
            if (m.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                outX = m.dwMousePosition.X;
                outY = m.dwMousePosition.Y;
                return std::make_pair(KEY_NONE, true);
            }
        }
        // 其它事件（移动/释放/窗口）忽略，继续读
    }
}

std::string readLine() {
    std::string s;
    std::getline(std::cin, s);
    return s;
}

}  // namespace game::input

#else
// ----------------------------- POSIX ---------------------------------------
#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include <sys/select.h>

namespace game::input {

namespace {
struct termios g_old_termios;
bool g_inited = false;
}  // namespace

void init() {
    tcgetattr(STDIN_FILENO, &g_old_termios);
    struct termios raw = g_old_termios;
    raw.c_lflag &= ~(static_cast<tcflag_t>(ICANON) | static_cast<tcflag_t>(ECHO));
    raw.c_cc[VMIN] = 1;   // 阻塞读 1 字节
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    g_inited = true;
}

void restore() {
    if (g_inited) {
        tcsetattr(STDIN_FILENO, TCSANOW, &g_old_termios);
        g_inited = false;
    }
}

bool kbhit() {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    return select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0;
}

int readKey() {
    int c = fgetc(stdin);
    if (c == 27) {  // 可能是 ESC，也可能是方向键前缀
        if (!kbhit()) return KEY_ESC;
        int c2 = fgetc(stdin);
        if (c2 == '[') {
            int c3 = fgetc(stdin);
            switch (c3) {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
                default: return KEY_ESC;
            }
        }
        return KEY_ESC;
    }
    if (c == '\n' || c == '\r') return KEY_ENTER;
    if (c == 3) return KEY_ESC;  // Ctrl+C
    return c;
}

std::pair<int, bool> readEvent(int& /*outX*/, int& /*outY*/) {
    // POSIX 无鼠标支持：退化为纯按键
    int k = readKey();
    return std::make_pair(k, false);
}

std::string readLine() {
    restore();  // 临时恢复行编辑
    std::string s;
    std::getline(std::cin, s);
    init();     // 重新进入逐键模式
    return s;
}

}  // namespace game::input

#endif
