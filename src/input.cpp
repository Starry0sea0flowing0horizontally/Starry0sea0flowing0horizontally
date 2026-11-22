// ============================================================================
//  花至荼蘼 —— 跨平台按键输入实现
//
//  POSIX 这一支（Linux / WSL / macOS）以前有个很坑的写法：用 fgetc(stdin)
//  按键、却用 select(fd 0) 判断「有没有键」。stdio 会偷偷多读一截进自己的
//  缓冲区，select 又只看内核缓冲区——于是「明明按了键，kbhit() 说没有」，
//  地图 / 菜单循环就此原地空转，表现就是「卡在某个菜单，所有键都无效」。
//  现在改成全程自己用 read() + 自己的小队列：读多少存多少，kbhit() 先看队列，
//  不会再和 stdio 打架；stdin 断开（EOF）也不会再空转。
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

void emergencyRestore() { restore(); }

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
#include <cerrno>
#include <cstdio>
#include <ctime>
#include <deque>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

namespace game::input {

namespace {

struct termios g_old_termios;
bool g_inited = false;     // 是否真的改过终端属性
bool g_eof = false;        // stdin 已经断开：别再死等
std::deque<unsigned char> g_queue;   // 自己缓冲，绝不和 stdio 抢字节

// 等终端可读（timeoutMs < 0 表示死等）
bool waitReadable(int timeoutMs) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    struct timeval tv;
    struct timeval* ptv = nullptr;
    if (timeoutMs >= 0) {
        tv.tv_sec = timeoutMs / 1000;
        tv.tv_usec = (timeoutMs % 1000) * 1000;
        ptv = &tv;
    }
    int r = select(STDIN_FILENO + 1, &fds, nullptr, nullptr, ptv);
    return r > 0 && FD_ISSET(STDIN_FILENO, &fds);
}

// 把终端里已经到手的字节吸进队列。返回队列里有没有数据。
bool pump(int timeoutMs) {
    for (;;) {
        if (!g_queue.empty()) return true;
        if (g_eof) return false;
        if (!waitReadable(timeoutMs)) return false;
        unsigned char tmp[32];
        ssize_t n = ::read(STDIN_FILENO, tmp, sizeof(tmp));
        if (n > 0) {
            g_queue.insert(g_queue.end(), tmp, tmp + static_cast<std::size_t>(n));
            return true;
        }
        if (n == 0) {
            g_eof = true;            // 终端断开 / 输入被关掉
            return false;
        }
        if (errno == EINTR) continue;                       // 被信号打断：接着等
        if (errno == EAGAIN || errno == EWOULDBLOCK) return false;
        g_eof = true;
        return false;
    }
}

// 取一个字节；-1 表示没有（超时或断开）
int nextByte(int timeoutMs) {
    if (!pump(timeoutMs)) return -1;
    int c = g_queue.front();
    g_queue.pop_front();
    return c;
}

void sleepMs(int ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = static_cast<long>(ms % 1000) * 1000L * 1000L;
    nanosleep(&ts, nullptr);
}

}  // namespace

void init() {
    g_eof = false;
    g_queue.clear();
    if (!isatty(STDIN_FILENO)) {
        // stdin 不是终端（被重定向 / 被别的程序接管）：先说清楚，免得「按键全都没反应」
        const char msg[] =
            "[花至荼蘼] 警告：标准输入不是终端，键盘可能读不到。请直接在终端里运行本程序。\n";
        ssize_t w = ::write(STDERR_FILENO, msg, sizeof(msg) - 1);
        (void)w;
    }
    if (tcgetattr(STDIN_FILENO, &g_old_termios) != 0) return;   // 不是终端：照常往下走

    struct termios raw = g_old_termios;
    raw.c_lflag &= ~static_cast<tcflag_t>(ICANON | ECHO);
    raw.c_iflag &= ~static_cast<tcflag_t>(IXON | ICRNL);   // Ctrl+S 别冻住输出；回车给 '\r'
    raw.c_cc[VMIN] = 1;                                    // 阻塞读，来一个字节返回一个
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0) g_inited = true;
}

void restore() {
    if (g_inited) {
        tcsetattr(STDIN_FILENO, TCSANOW, &g_old_termios);
        g_inited = false;
    }
    g_queue.clear();
}

// 崩溃 / 被 kill 时也要把终端还原，否则会留下一个「按键没反应」的黑洞终端
void emergencyRestore() {
    if (g_inited) tcsetattr(STDIN_FILENO, TCSANOW, &g_old_termios);
}

bool kbhit() { return pump(0); }

int readKey() {
    int c = nextByte(-1);
    if (c < 0) {
        // stdin 断了（比如终端被关掉、输出被重定向）：当成 Esc，让界面能退出去。
        // 以前这里返回 -1，菜单循环收到「无键」就一直空转，看着就是彻底卡死。
        sleepMs(20);
        return KEY_ESC;
    }
    if (c == 27) {   // ESC，也可能是方向键序列 ESC [ A / ESC O A
        int seq[8];
        int n = 0;
        while (n < 8 && pump(25)) {
            seq[n++] = g_queue.front();
            g_queue.pop_front();
        }
        if (n == 0) return KEY_ESC;             // 单独一个 Esc
        const int a = seq[0];
        if (a == '[' || a == 'O') {
            // 方向键可能带修饰（ESC [ 1 ; 2 A 之类），所以看的是序列的最后一个字节
            switch (seq[n - 1]) {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
                case 'C': return KEY_RIGHT;
                case 'D': return KEY_LEFT;
                default: return KEY_NONE;       // Home/End/鼠标序列等：忽略
            }
        }
        return KEY_ESC;                         // 连按两下 Esc 之类
    }
    if (c == '\r' || c == '\n') return KEY_ENTER;
    if (c == 127 || c == 8) return KEY_BACKSPACE;
    if (c == 9) return KEY_TAB;
    if (c == 3) return KEY_ESC;      // Ctrl+C
    if (c == 4) return KEY_ESC;      // Ctrl+D（输入结束）
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
