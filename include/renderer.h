// ============================================================================
//  花至荼蘼 —— 文本渲染器接口
// ============================================================================
#pragma once

#include <string>
#include <utility>
#include <vector>

namespace game {

// 负责：清屏、颜色、打字机逐字演出、居中排版。
// 配图（CG/立绘）接口在此预留：未来接 SDL2/图形后端时，
// 只需替换 showImage() 的实现即可，剧情脚本无需改动。
class Renderer {
public:
    void init();   // 启用 ANSI/VT、设置 UTF-8 控制台输出

    void clear();    // 清屏
    void newline();  // 换行

    // 上移 n 行并逐行清除（用于正文在同一位置顶替显示，避免刷屏）
    void eraseLines(int n);

    // 立即整行打印（可选颜色）
    void printLine(const std::string& text, const char* color = nullptr);

    // 打字机逐字打印；玩家按键可立即显示剩余部分
    // 返回 true 表示玩家按键跳过了打字
    bool typeLine(const std::string& text, const char* color = nullptr, int delayUs = -1);

    void printCentered(const std::string& text, const char* color = nullptr);
    void printRule(char ch = '=', const char* color = nullptr);

    // ---- 菜单窗用的小工具（版式参考 RPG Maker，纯文字方框）----
    // 一行里分几段、每段一个颜色：段与段之间会被补空格对齐到指定列。
    // cols[i] 为第 i 段应结束的显示列（相对行首），0 表示紧随上一段。
    typedef std::pair<std::string, std::string> RichSeg;   // {文本, 颜色}
    void printLineRich(const std::vector<RichSeg>& segs, const std::vector<int>& cols);
    // 方框：宽 w（显示列），内容区 w-2 列；inner 为框内的一行文本（自动补空格）
    void printBoxTop(int w, const std::string& title = "", const char* color = nullptr);
    void printBoxMid(const std::string& inner, int w, const char* color = nullptr);
    void printBoxRule(int w, const char* color = nullptr);
    void printBoxBottom(int w, const char* color = nullptr);

    // 显示宽度（全角 2 列 / 半角 1 列），供上层排版计算
    static int widthOf(const std::string& s);
    // 按显示宽度截断（不切断多字节字符），超出部分丢弃
    static std::string clip(const std::string& s, int cols);

    // 配图：根据 imageId 用字符画渲染 CG/立绘（见 src/cover_art.h）
    void showImage(const std::string& imageId);

    // 标题画面专用：在标题正下方显示配图（等待按键，回车回到标题本行）
    void showTitleImage();

    int terminalWidth() const;
    int terminalHeight() const;

    // 绘制游戏标题画面
    void drawTitle(const std::string& subtitle);

    // ---- 全局文字速度设置（供「设置」菜单读写，跨 Renderer 实例保持）----
    // 档位：0=立即(关闭打字机) 1=快 2=正常 3=慢
    static int textSpeed();                 // 当前档位
    static void setTextSpeed(int level);    // 设置档位（自动夹到 0..3）
    static int speedDelayUs();              // 当前档位对应的每字延迟(微秒)，0 表示立即

    // ---- 屏幕内容抓取（只给自动化测试用）----
    // captureBegin 之后所有输出同时记进内存，captureEnd 取走按行切分的文本。
    void captureBegin();
    void captureEnd(std::vector<std::string>& lines);

private:
    bool capturing_ = false;
    std::string captureBuf_;
};

}  // namespace game
