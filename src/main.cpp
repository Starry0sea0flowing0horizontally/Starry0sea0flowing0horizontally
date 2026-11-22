// ============================================================================
//  花至荼蘼 —— 程序入口
// ============================================================================
#include "game.h"

#ifdef _WIN32
int main() {
    game::Game game;
    game.run();
    return 0;
}
#else
// Linux / WSL / macOS：
// 游戏会把终端切成「无回显、逐键」模式。万一中途崩了或被 kill 掉，
// 终端就留在那个模式里——看起来就是「画面卡住、按什么键都没反应」。
// 这里给几个常见信号挂上处理函数：先把终端还原，再报一句死因，然后退出。
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <unistd.h>

#include "input.h"

namespace {

void onFatalSignal(int sig) {
    game::input::emergencyRestore();
    const char msg[] = "\033[0m\033[?25h\n[花至荼蘼] 程序异常结束，终端已还原。\n";
    ssize_t n = ::write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    (void)n;
    _exit(128 + sig);
}

void installSignalHandlers() {
    const int sigs[] = {SIGSEGV, SIGBUS, SIGFPE, SIGABRT, SIGILL, SIGTERM, SIGINT, SIGHUP};
    for (std::size_t i = 0; i < sizeof(sigs) / sizeof(sigs[0]); ++i) {
        std::signal(sigs[i], onFatalSignal);
    }
}

}  // namespace

int main() {
    installSignalHandlers();
    game::Game game;
    game.run();
    return 0;
}
#endif
