// ============================================================================
//  花至荼蘼 —— 文本渲染器实现
// ============================================================================
#include "renderer.h"

#ifdef HZ_QT_FRONTEND
#include "qt_frontend.h"
#endif
#include "config.h"
#include "input.h"
#include "cover_art.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <string>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace game {

namespace {

int g_termWidth = 80;
int g_termHeight = 30;

// 全局文字速度档位：0=立即 1=快 2=正常 3=慢
int g_textSpeedLevel = 2;

// 计算字符串在终端上的显示宽度（中文等全角算 2 列，ASCII 算 1 列）
// 注意：U+2500~U+257F（制表符 ─│┌┐└┘）在 Windows 控制台里是**半角**，
// 只占 1 列。以前一律按 3 字节=2 列算，导致所有方框被算成两倍宽，
// 补空格补多了，框线整行溢出——「存档界面最底下冒出不相干文字」
// 以及「框线错位/窗悬空」都是这个原因。
int displayWidth(const std::string& s) {
    auto rune = [&](std::size_t i, std::size_t& len) -> unsigned long {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0x80) { len = 1; return c; }
        if ((c & 0xE0) == 0xC0) { len = 2; return ((c & 0x1Fu) << 6) | (s[i+1] & 0x3Fu); }
        if ((c & 0xF0) == 0xE0) {
            len = 3;
            return ((c & 0x0Fu) << 12) |
                   (static_cast<unsigned long>(static_cast<unsigned char>(s[i+1]) & 0x3Fu) << 6) |
                   (static_cast<unsigned char>(s[i+2]) & 0x3Fu);
        }
        if ((c & 0xF8) == 0xF0) {
            len = 4;
            return ((c & 0x07u) << 18) |
                   (static_cast<unsigned long>(static_cast<unsigned char>(s[i+1]) & 0x3Fu) << 12) |
                   (static_cast<unsigned long>(static_cast<unsigned char>(s[i+2]) & 0x3Fu) << 6) |
                   (static_cast<unsigned char>(s[i+3]) & 0x3Fu);
        }
        len = 1;
        return c;
    };
    int w = 0;
    for (std::size_t i = 0; i < s.size();) {
        std::size_t n = 1;
        unsigned long cp = rune(i, n);
        if (n >= 2) {
            // Box Drawing（U+2500–U+257F）与 Block Elements（U+2580–U+259F）
            // 在 Windows 控制台按半角显示，占 1 列。
            if (cp >= 0x2500u && cp <= 0x257Fu) w += 1;
            else if (cp >= 0x2580u && cp <= 0x259Fu) w += 1;
            // Miscellaneous Symbols 里的常见符号（⚠★☆◇◆▶◀）也按半角算。
            else if (cp == 0x26A0u) w += 1;
            // 「·」（U+00B7 拉丁中点，章节名里的分隔点）是窄字符，只占 1 列。
            // 它虽然是双字节 UTF-8，但按 2 列算的话，存档槽那两列摘要
            // 只要有一条带「·」，那一行就会挤宽 1 列，右边框跟着歪一格。
            else if (cp == 0x00B7u) w += 1;
            else w += 2;
        } else {
            w += 1;
        }
        i += n;
    }
    return w;
}

}  // namespace

#ifdef _WIN32
namespace {

// 结语页那句「我在看着你」是用半块字符（█▀▄）一格一格拼出来的。
// 但中文字体（新宋体/宋体/微软雅黑）里的 ▀ 和 ▄ 左右各留了 1 像素空隙，
// 拼上去就成了「一条条竖线切过笔画」的破烂样子——这是字体本身的问题，
// 换成什么图案都救不回来；只有 █ 是齐边的。
// 所以启动时把控制台字体换成块字符齐边的等宽字体（Consolas 等），
// 字号保持用户原来的大小，窗口列数不变；中文靠 GDI 字体链接照常显示。
// 换字体失败就算了，游戏照常跑。
void preferBlockSafeFont(HANDLE hOut) {
    CONSOLE_FONT_INFOEX cfi;
    ZeroMemory(&cfi, sizeof(cfi));
    cfi.cbSize = sizeof(cfi);
    if (!GetCurrentConsoleFontEx(hOut, FALSE, &cfi)) return;
    if (wcscmp(cfi.FaceName, L"Consolas") == 0) return;
    const wchar_t* const cands[] = { L"Consolas", L"Lucida Console", L"Courier New" };
    for (int i = 0; i < 3; ++i) {
        CONSOLE_FONT_INFOEX f = cfi;
        f.cbSize = sizeof(f);
        f.dwFontSize.X = 0;  // 宽度交给系统按等宽比例算
        if (f.dwFontSize.Y <= 0) f.dwFontSize.Y = 16;
        f.FontFamily = 0;
        f.FontWeight = FW_NORMAL;
        wcsncpy(f.FaceName, cands[i], LF_FACESIZE - 1);
        f.FaceName[LF_FACESIZE - 1] = L'\0';
        if (SetCurrentConsoleFontEx(hOut, FALSE, &f)) return;
    }
}

}  // namespace
#endif

void Renderer::init() {
#ifdef HZ_QT_FRONTEND
    g_termWidth = kQtDisplayColumns;
    g_termHeight = kQtDisplayRows;
#elif defined(_WIN32)
    // 控制台以 UTF-8 解释输出字节
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        preferBlockSafeFont(hOut);
        DWORD mode = 0;
        if (GetConsoleMode(hOut, &mode)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;  // 启用 ANSI 转义
            SetConsoleMode(hOut, mode);
        }
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
            g_termWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            g_termHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        }
    }
#else
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0) {
        g_termWidth = static_cast<int>(ws.ws_col);
        g_termHeight = static_cast<int>(ws.ws_row);
    }
#endif
    if (g_termWidth <= 0) g_termWidth = 80;
    if (g_termHeight <= 0) g_termHeight = 30;
}

// ---------------------------------------------------------------- 清屏 --
// 传统 cmd（Win10 conhost，默认 80×25）上的 `\033[2J\033[H` 有个坑：
// 它只擦「当前可见的视口」，一旦之前的输出把内容顶进了**回滚缓冲区**，
// 再清屏时缓冲区里那几行旧文字不会消失；下一次重新排版时窗口往上一滚，
// 它们就又会露出来——看上去就是「存档/读档界面最底下莫名其妙多出一行
// 不相干的旁白」。
//
// 所以 Windows 下不用转义序列，直接调 API：
//   1. 用 FillConsoleOutputCharacter 把**整个缓冲区**（含回滚区）填满空格；
//   2. 光标归零，并把窗口滚回最顶上。
// 这样无论之前打过多少行，屏幕上都不会残留任何旧字。
void Renderer::clear() {
#if defined(_WIN32) && !defined(HZ_QT_FRONTEND)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE && hOut != nullptr) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
            DWORD cells = static_cast<DWORD>(csbi.dwSize.X) * csbi.dwSize.Y;
            COORD home;
            home.X = 0;
            home.Y = 0;
            DWORD written = 0;
            // 整个缓冲区（包括看不见的回滚区）全部填空格
            FillConsoleOutputCharacterA(hOut, ' ', cells, home, &written);
            FillConsoleOutputAttribute(hOut, csbi.wAttributes, cells, home, &written);
            // 把窗口滚回缓冲区最顶端，再回到左上角
            SMALL_RECT win = csbi.srWindow;
            if (win.Top != 0) {
                SMALL_RECT dst = win;
                dst.Top = 0;
                dst.Bottom = static_cast<SHORT>(win.Bottom - win.Top);
                SetConsoleWindowInfo(hOut, TRUE, &dst);
            }
            SetConsoleCursorPosition(hOut, home);
            return;
        }
    }
#endif
    // POSIX / 兜底：ANSI 清屏（终端一般会连回滚区一起清）
    std::cout << "\033[2J\033[3J\033[H";
    std::cout.flush();
}

void Renderer::newline() {
    std::cout << '\n';
    std::cout.flush();
    if (capturing_) captureBuf_ += '\n';
}

void Renderer::eraseLines(int n) {
    if (n <= 0) return;
    for (int i = 0; i < n; ++i) {
        // \033[F 光标上移一行（列位置不变），\033[2K 清除整行
        std::cout << "\033[F\033[2K";
    }
    std::cout.flush();
    if (capturing_) captureBuf_ += "\033[F\033[2K";
}

// ---------------------------------------------------------------- 抓屏 --
// 只在自动化测试里用：captureBegin 后把输出的每个字符同时记进 captureBuf_，
// captureEnd 按换行切分、剥掉 ANSI 颜色码，得到「屏幕上到底有什么字」。
void Renderer::captureBegin() {
    capturing_ = true;
    captureBuf_.clear();
}

void Renderer::captureEnd(std::vector<std::string>& lines) {
    capturing_ = false;
    std::string cur;
    for (std::size_t i = 0; i < captureBuf_.size(); ++i) {
        char ch = captureBuf_[i];
        if (ch == '\033') {
            // 跳过 CSI 序列 \033[...m 与 \033[F / \033[2J 等
            std::size_t j = i + 1;
            if (j < captureBuf_.size() && captureBuf_[j] == '[') {
                ++j;
                while (j < captureBuf_.size()) {
                    char t = captureBuf_[j];
                    if ((t >= 'A' && t <= 'Z') || (t >= 'a' && t <= 'z')) break;
                    ++j;
                }
            }
            i = j;
            continue;
        }
        if (ch == '\r') continue;
        if (ch == '\n') { lines.push_back(cur); cur.clear(); continue; }
        if (ch == '\b' || ch == '\0') continue;
        cur += ch;
    }
    if (!cur.empty()) lines.push_back(cur);
    captureBuf_.clear();
}

void Renderer::printLine(const std::string& text, const char* color) {
    if (color) std::cout << color;
    std::cout << text;
    if (color) std::cout << color::RESET;
    std::cout << '\n';
    std::cout.flush();
    if (capturing_) captureBuf_ += text + '\n';
}

bool Renderer::typeLine(const std::string& text, const char* color, int delayUs) {
    if (delayUs < 0) delayUs = speedDelayUs();   // 未显式指定时，用全局速度设置
    if (color) std::cout << color;

    std::size_t i = 0;
    bool skipped = false;
    for (; i < text.size(); ++i) {
        std::cout.put(text[i]);
        std::cout.flush();
        if (input::kbhit()) {
            input::readKey();   // 消费该键（视为快进）
            skipped = true;
            break;
        }
        if (delayUs > 0) {
            std::this_thread::sleep_for(std::chrono::microseconds(delayUs));
        }
    }
    // 跳过时一次性输出剩余部分
    if (skipped && i + 1 < text.size()) {
        std::cout << text.substr(i + 1);
    }
    if (color) std::cout << color::RESET;
    std::cout << '\n';
    std::cout.flush();
    if (capturing_) captureBuf_ += text + '\n';
    return skipped;
}

void Renderer::printCentered(const std::string& text, const char* color) {
    int w = displayWidth(text);
    int pad = (g_termWidth - w) / 2;
    if (pad < 0) pad = 0;
    if (color) std::cout << color;
    std::cout << std::string(static_cast<std::size_t>(pad), ' ') << text << '\n';
    if (color) std::cout << color::RESET;
    std::cout.flush();
    if (capturing_)
        captureBuf_ += std::string(static_cast<std::size_t>(pad), ' ') + text + '\n';
}

void Renderer::printRule(char ch, const char* color) {
    if (color) std::cout << color;
    std::cout << std::string(static_cast<std::size_t>(g_termWidth), ch) << '\n';
    if (color) std::cout << color::RESET;
    std::cout.flush();
}

int Renderer::widthOf(const std::string& s) {
    return displayWidth(s);
}

std::string Renderer::clip(const std::string& s, int cols) {
    std::string out;
    std::size_t i = 0;
    int acc = 0;
    while (i < s.size()) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        std::size_t n = 1;
        if ((c & 0xF8) == 0xF0) n = 4;
        else if ((c & 0xF0) == 0xE0) n = 3;
        else if ((c & 0xE0) == 0xC0) n = 2;
        if (i + n > s.size()) break;
        int w = displayWidth(s.substr(i, n));
        if (acc + w > cols) break;
        out += s.substr(i, n);
        acc += w;
        i += n;
    }
    return out;
}
// ---------------------------------------------------------------- 菜单窗 --
// 一行 = 若干 {文本,颜色} 段；cols[i] 是该段应结束的显示列（相对行首）。
// 段之间用空格补齐，保证中英混排时列仍然对齐。
void Renderer::printLineRich(const std::vector<RichSeg>& segs,
                             const std::vector<int>& cols) {
    std::string out;
    int col = 0;
    for (std::size_t i = 0; i < segs.size(); ++i) {
        if (i < cols.size() && cols[i] > col) {
            out.append(static_cast<std::size_t>(cols[i] - col), ' ');
            col = cols[i];
        }
        if (!segs[i].second.empty()) out += segs[i].second;
        out += segs[i].first;
        if (!segs[i].second.empty()) out += color::RESET;
        col += displayWidth(segs[i].first);
    }
    std::cout << out << '\n';
    std::cout.flush();
}

// 方框宽度常量：60 列（内容区 58 列），10 行终端里能并排放两个。
namespace {
const char* kBoxH     = "\xE2\x94\x80";   // ─
const char* kBoxV     = "\xE2\x94\x82";   // │
const char* kBoxTL    = "\xE2\x94\x8C";   // ┌
const char* kBoxTR    = "\xE2\x94\x90";   // ┐
const char* kBoxBL    = "\xE2\x94\x94";   // └
const char* kBoxBR    = "\xE2\x94\x98";   // ┘
const char* kBoxLeft  = "\xE2\x94\x9C";   // ├
const char* kBoxRight = "\xE2\x94\xA4";   // ┤

std::string hRep(int n) {
    if (n <= 0) return "";
    std::string s;
    for (int i = 0; i < n; ++i) s += kBoxH;
    return s;
}
}  // namespace

void Renderer::printBoxTop(int w, const std::string& title, const char* color) {
    // ┌─ 标题 ────────┐
    int inner = w - 2;
    std::string line = std::string(kBoxTL);
    std::string rest;
    if (!title.empty()) {
        std::string t = " " + title + " ";
        int tw = displayWidth(t);
        if (tw > inner - 2) {                 // 太长就截断
            t.clear();
            std::size_t i = 0;
            int acc = 0;
            while (i < title.size()) {
                std::size_t n = 1;
                unsigned char c = static_cast<unsigned char>(title[i]);
                if ((c & 0xF0) == 0xE0) n = 3;
                else if ((c & 0xE0) == 0xC0) n = 2;
                else if ((c & 0xF8) == 0xF0) n = 4;
                int cw = displayWidth(title.substr(i, n));
                if (acc + cw > inner - 4) break;
                t += title.substr(i, n);
                acc += cw;
                i += n;
            }
            t = " " + t + " ";
            tw = displayWidth(t);
        }
        line += hRep(1) + t + hRep(inner - 1 - tw);
    } else {
        line += hRep(inner);
    }
    line += kBoxTR;
    if (color) std::cout << color;
    std::cout << line;
    if (color) std::cout << color::RESET;
    std::cout << '\n';
    std::cout.flush();
}

void Renderer::printBoxMid(const std::string& inner, int w, const char* color) {
    int cols = w - 2;
    int iw = displayWidth(inner);
    std::string body = inner;
    if (iw > cols) {                          // 超宽：按显示宽度截断
        body.clear();
        std::size_t i = 0;
        int acc = 0;
        while (i < inner.size()) {
            std::size_t n = 1;
            unsigned char c = static_cast<unsigned char>(inner[i]);
            if ((c & 0xF0) == 0xE0) n = 3;
            else if ((c & 0xE0) == 0xC0) n = 2;
            else if ((c & 0xF8) == 0xF0) n = 4;
            int cw = displayWidth(inner.substr(i, n));
            if (acc + cw > cols) break;
            body += inner.substr(i, n);
            acc += cw;
            i += n;
        }
        iw = acc;
    }
    if (color) std::cout << color;
    std::cout << kBoxV;
    if (color) std::cout << color::RESET;
    std::cout << body;
    std::cout << std::string(static_cast<std::size_t>(cols - iw), ' ');
    if (color) std::cout << color;
    std::cout << kBoxV;
    if (color) std::cout << color::RESET;
    std::cout << '\n';
    std::cout.flush();
}

void Renderer::printBoxRule(int w, const char* color) {
    if (color) std::cout << color;
    std::cout << kBoxLeft << hRep(w - 2) << kBoxRight;
    if (color) std::cout << color::RESET;
    std::cout << '\n';
    std::cout.flush();
}

void Renderer::printBoxBottom(int w, const char* color) {
    if (color) std::cout << color;
    std::cout << kBoxBL << hRep(w - 2) << kBoxBR;
    if (color) std::cout << color::RESET;
    std::cout << '\n';
    std::cout.flush();
}

namespace {
// 打印一套给定尺寸的字符画：居中缩进，末尾可选一行提示。
// 返回 false 表示窗口放不下（调用方决定是否跳过）。
bool printArt(const char* const* lines, int w, int h, int termW, int termH,
              const char* hint) {
    if (termW < w || termH < h + (hint ? 2 : 1)) return false;
    const int pad = (termW - w) / 2;
    const std::string indent(static_cast<std::size_t>(pad > 0 ? pad : 0), ' ');
    std::cout << '\n';
    for (int i = 0; i < h; ++i) {
        std::cout << indent << lines[i] << '\n';
    }
    std::cout.flush();
    if (hint) {
        // 居中提示：这里直接复用外面的 printCentered 会依赖 Renderer 实例，
        // 因此把提示也在此处手工居中打印。
        (void)hint;
    }
    return true;
}
}  // namespace

void Renderer::showImage(const std::string& imageId) {
    // 「配图」在终端里用上半块字符 U+2580 + 24bit 真彩色拼出来：
    // 每行一个字符串，见 src/cover_art.h（由 tools/cover_art.py 生成）。
    if (imageId == "prologue_cover") {
        // 老终端不支持真彩色时会印出一堆灰块：设 HZ_COVER_PLAIN=1 可跳过配图
        const char* plain = std::getenv("HZ_COVER_PLAIN");
        if (plain && plain[0] == '1' && plain[1] == '\0') return;

        // 留 4 行给上面的空行和下面的「按任意键」
        // 变量名不能叫 small：Windows SDK 的 rpcndr.h 里有 #define small char，
        // 一到 MSVC 下就变成「const bool char」，报 C2632。
        const bool fitLarge = g_termWidth >= cover_art::kLargeWidth &&
                              g_termHeight >= cover_art::kLargeHeight + 4;
        const bool fitSmall = g_termWidth >= cover_art::kSmallWidth &&
                              g_termHeight >= cover_art::kSmallHeight + 4;
        if (!fitLarge && !fitSmall) {          // 窗口装不下，硬画会折行/滚动
            printLine("〔 画面太小，建议把窗口拉宽到 80 列、高到 30 行以上 〕", color::DIM);
            return;
        }
        const int w = fitLarge ? cover_art::kLargeWidth : cover_art::kSmallWidth;
        const int h = fitLarge ? cover_art::kLargeHeight : cover_art::kSmallHeight;
        const char* const* lines = fitLarge ? cover_art::kLarge : cover_art::kSmall;

        printArt(lines, w, h, g_termWidth, g_termHeight, nullptr);
        newline();
        printCentered("〔 按任意键开始 〕", color::DIM);
        while (input::kbhit()) input::readKey();   // 清掉积压的按键
        input::readKey();                          // 等玩家看一眼
        clear();
        return;
    }
    // 其它配图暂未绘制：静默跳过，剧情照常进行
    (void)imageId;
}

void Renderer::showTitleImage() {
    // 在标题画面「花至荼蘼」正下方显示配图，用 Title 尺寸（竖直空间有限）。
    const char* plain = std::getenv("HZ_COVER_PLAIN");
    if (plain && plain[0] == '1' && plain[1] == '\0') return;

    const int w = cover_art::kTitleWidth;
    const int h = cover_art::kTitleHeight;
    if (!printArt(cover_art::kTitle, w, h, g_termWidth, g_termHeight, nullptr)) {
        return;   // 放不下就静默跳过，不死机
    }
}

int Renderer::terminalWidth() const {
    return g_termWidth;
}

int Renderer::terminalHeight() const {
    return g_termHeight;
}

// ---- 全局文字速度设置 ----
int Renderer::textSpeed() {
    return g_textSpeedLevel;
}

void Renderer::setTextSpeed(int level) {
    if (level < 0) level = 0;
    if (level > 3) level = 3;
    g_textSpeedLevel = level;
}

int Renderer::speedDelayUs() {
    // 0=立即(0) 1=快(30ms) 2=正常(55ms) 3=慢(100ms)
    switch (g_textSpeedLevel) {
        case 0: return 0;
        case 1: return 30000;
        case 3: return 100000;
        case 2:
        default: return TYPE_DELAY_US;
    }
}

void Renderer::drawTitle(const std::string& subtitle) {
    clear();
    newline();
    newline();
    {
        std::string c = std::string(color::BOLD) + color::ROSE;
        printCentered("花  至  荼  蘼", c.c_str());
    }
    newline();
    printCentered(subtitle, color::CYAN);
    newline();
    printRule('=', color::GOLD);
    newline();
}

}  // namespace game
