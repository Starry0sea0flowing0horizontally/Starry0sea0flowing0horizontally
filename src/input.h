// ============================================================================
//  花至荼蘼 —— 跨平台按键输入（Windows: conio / POSIX: termios）
// ============================================================================
#pragma once

#include <string>
#include <utility>

namespace game::input {

// 统一的逻辑键码（与平台无关）
enum Key {
    KEY_NONE = -1,
    KEY_ENTER = 10,
    KEY_ESC = 27,
    KEY_SPACE = 32,
    KEY_BACKSPACE = 8,
    KEY_TAB = 9,
    // 方向键（自定义高值，避免与 ASCII 冲突）
    KEY_UP = 1000,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
};

// 进入「无回显、逐键读取」模式（POSIX 需先调用；Windows 空实现）
void init();

// 恢复终端原有模式（程序退出前必须调用）
void restore();

// 崩溃 / 被信号杀死时也能还原终端（POSIX 有效，Windows 等价于 restore）。
// 只做 tcsetattr 这类异步信号安全的事，可以直接在信号处理函数里调用。
void emergencyRestore();

// 非阻塞检测：当前是否有按键待读（不消费输入）
bool kbhit();

// 阻塞读取一个键，返回统一 Key / 字符 ASCII 值
int readKey();

// 阻塞读取一个「输入事件」：可能是按键，也可能是鼠标左键点击。
// 返回 std::pair<int, bool>：
//   · first  = 按键值（同 readKey 的 Key / ASCII）；鼠标点击时为 KEY_NONE
//   · second = 是否为鼠标左键点击（true 表示 MouseClick，坐标见 outX/outY）
// 鼠标坐标以「终端字符单元格」为单位（X 列 / Y 行，0 起）。
// outX/outY 在鼠标点击时被填充为点击的单元格坐标。
std::pair<int, bool> readEvent(int& outX, int& outY);

// 读取一行文本（用于将来输入姓名等；内部临时恢复行编辑模式）
std::string readLine();

}  // namespace game::input
