// ============================================================================
//  花至荼蘼（同人视觉小说）—— 引擎全局配置
//  基于《有兽焉》（靴下猫腰子）的同人原创作品
//  兼容 C++11，便于 Dev-C++（MinGW GCC）等环境直接编译
// ============================================================================
#pragma once

namespace game {

// ---- 标识 ----
constexpr const char* GAME_TITLE    = "花至荼蘼";
constexpr const char* GAME_SUBTITLE = "序章 · 咫尺天涯";

// ---- ANSI 颜色 -----------------------------------------------------------
// Windows 10+ 启用 VT 处理后、以及 POSIX 终端均支持 256 色 ANSI 转义。
// 注意：这些是 constexpr（内部链接），头文件多包含不会产生重复符号。
namespace color {
constexpr const char* RESET  = "\033[0m";
constexpr const char* BOLD   = "\033[1m";
constexpr const char* DIM    = "\033[2m";
constexpr const char* UNDER  = "\033[4m";

constexpr const char* WHITE  = "\033[97m";
constexpr const char* SILVER = "\033[37m";
constexpr const char* GRAY   = "\033[90m";

constexpr const char* AMBER  = "\033[38;5;214m";  // 四不遇 · 琥珀色眼睛
constexpr const char* GOLD   = "\033[38;5;178m";  // 系统 / 暗金
constexpr const char* CYAN   = "\033[38;5;81m";   // 系统 / 天光
constexpr const char* TEAL   = "\033[38;5;116m";  // 青色绒毛
constexpr const char* RED    = "\033[38;5;203m";  // 赤红 / 警示
constexpr const char* ROSE   = "\033[38;5;211m";  // 荼蘼花
constexpr const char* PINK   = "\033[38;5;217m";
constexpr const char* PURPLE = "\033[38;5;183m";
constexpr const char* GREEN  = "\033[38;5;114m";
}

// ---- 角色配色 ----
namespace speaker {
constexpr const char* SIBUXIANG = color::WHITE;   // 四不相 · 白麒麟
constexpr const char* SIBUYU    = color::AMBER;   // 四不遇 · 灰麒麟
constexpr const char* SHANGSI   = color::GRAY;    // 上司
constexpr const char* YOULING   = color::RED;     // 梦中幼崽（赤红眼眸）
constexpr const char* NARRATOR  = color::DIM;     // 旁白
constexpr const char* SYSTEM    = color::GOLD;    // 系统
}

// ---- 打字机速度（每字延迟，微秒）----
constexpr int TYPE_DELAY_US = 16000;   // 16ms/字

// ---- 存档 ----
constexpr const char* SAVE_DIR   = "saves";
constexpr int        SAVE_SLOTS  = 6;

// ---- 探索地图 ----
// 玩家标记用全角字符「＠」（占 2 列），与地图上其它全角格子对齐，避免抖动。
constexpr const char* MAP_PLAYER_CHAR = "\xEF\xBC\xA0";   // 全角＠ (U+FF20)
// 地图上的「别人」用一个字代表自己：四不遇 = 遇，四不相 = 相，上司 = 司。
// 一律用汉字（不是 ◆ ● 这类半角符号）：汉字占 2 列，和地图格子严格对齐，
// 否则一行会短一列，整个方框就歪了。
constexpr const char* MAP_ACTOR_SIBUYU   = "\xE9\x81\x87";   // 遇
constexpr const char* MAP_ACTOR_SIBUXIANG = "\xE7\x9B\xB8";  // 相

// ---- 配图资源目录（预留，接 SDL2 等图形后端时使用）----
constexpr const char* ASSET_IMAGE_DIR = "assets/images";

// ---- 存档文件格式版本 ----
constexpr int SAVE_FORMAT_VERSION = 1;

}  // namespace game
