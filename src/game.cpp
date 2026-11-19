// ============================================================================
//  花至荼蘼 —— 游戏主控实现（标题菜单 / 系统菜单 / 存档 / 剧情状态机）
//
//  菜单版式参考 RPG Maker 的菜单窗：左侧竖排列表 + 右侧队伍状态
//  （Lv / 种族 / HP / MP / TP），只用文字和方框，不画人物头图。
//  整窗高度固定 10 行，两个窗并排也不超过传统 cmd 的 25 行。
// ============================================================================
#include "game.h"

#include <chrono>
#include <cstdio>
#include <deque>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "config.h"
#include "input.h"
#include "prologue.h"

namespace game {

namespace {

// ---- 菜单窗尺寸 ----
const int kBoxHalf = 36;   // 左边列表窗宽（显示列）
const int kBoxWide = 36;   // 右边队伍窗宽
const int kBoxRows = 12;   // 窗体总行数（含上下边框）：36 + 2 + 36 = 74 列，80 列窗口放得下

// 数字转字符串（C++11 有 std::to_string，这里统一走它）
std::string num(long long v) { return std::to_string(v); }

// 千分位：1944 -> "1,944"
std::string group3(long long v) {
    std::string s = std::to_string(v < 0 ? -v : v);
    std::string out;
    int c = 0;
    for (std::size_t i = s.size(); i > 0; --i) {
        out.insert(out.begin(), s[i - 1]);
        if (++c % 3 == 0 && i > 1) out.insert(out.begin(), ',');
    }
    return (v < 0 ? "-" : "") + out;
}

// 左对齐补空格到指定显示列
std::string padRight(const std::string& s, int cols) {
    int w = Renderer::widthOf(s);
    if (w >= cols) return Renderer::clip(s, cols);
    return s + std::string(static_cast<std::size_t>(cols - w), ' ');
}

// 右对齐补空格到指定显示列
std::string padLeft(const std::string& s, int cols) {
    int w = Renderer::widthOf(s);
    if (w >= cols) return Renderer::clip(s, cols);
    return std::string(static_cast<std::size_t>(cols - w), ' ') + s;
}

// 截断后再补空格
std::string fit(const std::string& s, int cols) { return padRight(Renderer::clip(s, cols), cols); }

// 在给定宽度里画一条分隔线（不含左右边框）
std::string innerRule(int cols) {
    std::string s;
    for (int i = 0; i < cols; ++i) s += "\xE2\x94\x80";   // ─
    return s;
}

// ---- 方框（显示列宽度精确对齐，只用明确宽度的全角 / 半角字符）----
std::string boxTop(int cols, const std::string& title) {
    const int inner = cols - 2;
    std::string head;
    std::string fill;
    if (title.empty()) {
        fill = innerRule(inner);
    } else {
        head = " " + Renderer::clip(title, inner - 4) + " ";
        int hw = Renderer::widthOf(head);
        fill = head + innerRule(inner - hw);
    }
    return std::string(color::GOLD) + "\xE2\x94\x8C" + fill + "\xE2\x94\x90" +
           std::string(color::RESET);
}

std::string boxRule(int cols) {
    return std::string(color::GOLD) + "\xE2\x94\x9C" + innerRule(cols - 2) + "\xE2\x94\xA4" +
           std::string(color::RESET);
}

std::string boxBottom(int cols) {
    return std::string(color::GOLD) + "\xE2\x94\x94" + innerRule(cols - 2) + "\xE2\x94\x98" +
           std::string(color::RESET);
}

// 框内一行：内容按显示宽度补空格；color 只作用于内容，边框始终是金色。
std::string boxMid(const std::string& text, int cols, const char* color = nullptr) {
    const char* c = color ? color : color::SILVER;
    return std::string(color::GOLD) + "\xE2\x94\x82" + std::string(color::RESET) +
           std::string(c) + fit(text, cols - 2) + std::string(color::RESET) +
           std::string(color::GOLD) + "\xE2\x94\x82" + std::string(color::RESET);
}

}  // namespace

// 鼠标事件先进入此队列，再由 readEventPair 转成按键。
// 原因：Windows 控制台在开启鼠标输入后，MOUSE_EVENT 与 KEY_EVENT 共用同一个
// 输入缓冲区；若某处用 _getch() 直接读（打字机快进、菜单），会先把鼠标事件
// 读掉却返回 0/224，表现为「鼠标一动，按键就被吞」。统一走 INPUT_RECORD
// 读取后，鼠标事件不再污染按键流，键盘（数字键等）在任何界面都可靠。
static std::deque<std::pair<int, int>> g_clickQueue;

// 取下一个输入事件；返回 {键值, 是否鼠标点击}，点击坐标压入 g_clickQueue。
static std::pair<int, bool> readEventPair(int& /*mx*/, int& /*my*/) {
    while (true) {
        int x = 0, y = 0;
        std::pair<int, bool> ev = input::readEvent(x, y);
        if (ev.second) {
            g_clickQueue.push_back(std::make_pair(x, y));
            continue;   // 点击只进地图用，按键流程继续等按键
        }
        return ev;
    }
}

// ---------------------------------------------------------------- 章节分发 --
// 供自动化测试驱动地图循环用的输入注入（正常游戏里恒为 null）。
MapInputHook* g_mapInputHook = nullptr;

void Game::beginFrame() {
    frameLines_.clear();
    if (g_mapInputHook) {
        renderer_.captureBegin();
    }
}
void Game::endFrame() {
    if (g_mapInputHook) {
        std::vector<std::string> got;
        renderer_.captureEnd(got);
        for (std::size_t i = 0; i < got.size(); ++i) frameLines_.push_back(got[i]);
    }
}

// 目前只有序章；未来新增章节时在此按 chapter 名返回对应脚本。
Script Game::buildChapter(const std::string& chapter) {
    (void)chapter;
    return buildPrologue();
}

// ------------------------------------------------------------------- 入口 --
void Game::run() {
    renderer_.init();
    input::init();
    titleLoop();
    input::restore();
}

// ---------------------------------------------------------------- 标题画面 --
void Game::titleLoop() {
    while (!quitting_) {
        renderer_.drawTitle(GAME_SUBTITLE);
        renderer_.showTitleImage();   // 配图显示在标题「花至荼蘼」正下方

        int latest = saves_.latestSlot();
        renderer_.printCentered("1. 新的开始", color::WHITE);
        renderer_.printCentered("2. 继续游戏", latest >= 0 ? color::WHITE : color::GRAY);
        renderer_.printCentered("3. 设置", color::WHITE);
        renderer_.printCentered("4. 退出游戏", color::WHITE);
        renderer_.newline();
        renderer_.printCentered("请按键选择：", color::DIM);

        int k = input::readKey();
        if (k == '1') {
            playChapter("序章");
        } else if (k == '2') {
            // 继续游戏：直接打开存档选择页面，选档后读档（不再自动读最新档）
            int slot = loadMenu();
            if (slot >= 0) playFromSave(slot);
        } else if (k == '3') {
            settingsMenu();
        } else if (k == '4' || k == 'q' || k == 'Q' || k == input::KEY_ESC) {
            quitting_ = true;
        }
    }
}

// ---------------------------------------------------------------- 设置菜单 --
void Game::settingsMenu() {
    static const char* speedNames[4] = {"立即（关闭打字）", "快", "正常", "慢"};
    std::size_t sel = 0;   // 只有一个可调项：文字速度；保留多行以利将来扩展
    while (true) {
        int level = Renderer::textSpeed();
        renderer_.clear();
        renderer_.newline();
        renderer_.printCentered("—— 设置 ——", color::GOLD);
        renderer_.newline();
        std::string line = "文字速度：";
        line += speedNames[level];
        const char* c = (sel == 0) ? color::CYAN : color::SILVER;
        renderer_.printLine((sel == 0 ? "  》" : "    ") + line, c);
        renderer_.newline();
        renderer_.printLine("　　【左右键】调节　【回车】确认　【Esc】返回", color::DIM);

        int k = input::readKey();
        if (k == input::KEY_ESC) {
            return;
        } else if (k == input::KEY_ENTER || k == input::KEY_SPACE) {
            return;
        } else if (k == input::KEY_LEFT) {
            Renderer::setTextSpeed(level - 1);
        } else if (k == input::KEY_RIGHT) {
            Renderer::setTextSpeed(level + 1);
        }
    }
}

void Game::playChapter(const std::string& chapter) {
    Script script = buildChapter(chapter);
    Flags flags;
    initProfile(flags, true);
    std::size_t pc = 0;
    currentChapter_ = chapter;
    currentScene_ = chapter;
    runScript(script, pc, flags);
}

void Game::playFromSave(int slot) {
    SaveData d;
    if (!saves_.load(slot, d)) return;
    Script script = buildChapter(d.chapter);
    Flags flags = d.flags;
    initProfile(flags, false);
    std::size_t pc = d.pc;
    // 优先按「label + 偏移」还原位置：剧情改过稿以后，旧存档的裸 pc 会指到别的段落里，
    // 一读档就多冒出一句不相干的台词（比如站在玉佩上却看到「地上积了一层灰尘」）。
    if (!d.resumeLabel.empty()) {
        Script::labels_t::const_iterator it = script.labels.find(d.resumeLabel);
        if (it != script.labels.end()) {
            pc = it->second + d.resumeOffset;
            if (pc >= script.nodes.size()) pc = it->second;
        }
    }
    if (pc >= script.nodes.size()) pc = 0;
    // 读档前先擦干净：绝不把标题画面 / 上一局留下的字带进剧情。
    renderer_.clear();
    bodyLines_ = 0;
    // 跳过存档点自身的「是否存档」提示节点：读档回来不该再问一次。
    skipSavePrompt(script, pc);
    currentChapter_ = d.chapter;
    currentScene_ = d.sceneTitle;
    runScript(script, pc, flags);
}

// 若 pc 正指向「存档点提示」节点，则跳到它的下一个节点，
// 避免读档后立刻弹出「是否进行存档？」打断继续。
void Game::skipSavePrompt(Script& script, std::size_t& pc) {
    if (pc < script.nodes.size() && script.nodes[pc].type == NodeType::SavePrompt) {
        pc++;
    }
}

// ------------------------------------------------------------ 角色档案初始化 --
// 全部写在剧情 flag 里（随存档保存）。fresh=true 时写入开场持有物；
// fresh=false（读档）时只补「新版本才加进来的东西」，不覆盖玩家的装备选择。
void Game::initProfile(Flags& flags, bool fresh) {
    const std::vector<Character>& chars = roster_.all();

    // 开场就带在身上的重要物品（全部按「未装备的候选」登记进背包和装备槽）
    if (fresh) {
        std::vector<std::string> items = defaultWarehouse();
        for (std::size_t i = 0; i < items.size(); ++i) {
            inventory::add(flags, items[i]);
        }
    }

    // ---- 入队状态：四不相一开始就在队里，四不遇 / 上司还没有入队 ----
    if (stats::get(flags, "party_sibuxiang").empty())
        stats::set(flags, "party_sibuxiang", "1");
    if (stats::get(flags, "party_sibuyu").empty())
        stats::set(flags, "party_sibuyu", "0");
    if (stats::get(flags, "party_shangsi").empty())
        stats::set(flags, "party_shangsi", "0");

    for (std::size_t ci = 0; ci < chars.size(); ++ci) {
        const Character& c = chars[ci];
        // HP / MP 上限（将来升级时改这里）
        if (stats::get(flags, "hpm_" + c.id()).empty())
            stats::setInt(flags, "hpm_" + c.id(), c.hpMax(flags));
        if (stats::get(flags, "mpm_" + c.id()).empty())
            stats::setInt(flags, "mpm_" + c.id(), c.mpMax(flags));
        if (fresh) {
            // 新游戏：全部满血满灵力，TP 归零
            stats::setInt(flags, "hp_" + c.id(), c.hpMax(flags));
            stats::setInt(flags, "mp_" + c.id(), c.mpMax(flags));
            stats::setInt(flags, "tp_" + c.id(), 0);
            stats::setInt(flags, "exp_" + c.id(), 0);
        }
        // 每个装备槽：候选顺序表 + 当前装备（缺省=空）
        for (std::size_t s = 0; s < c.equipSlots().size(); ++s) {
            std::string okey = "eqo_" + c.id() + "_" + std::to_string(s);
            if (stats::get(flags, okey).empty()) {
                // 四不相的槽位里放开场持有物当候选，其余角色暂为空
                std::vector<std::string> cand;
                if (fresh && c.id() == "sibuxiang") {
                    cand.push_back("双螭玉佩");
                    cand.push_back("一团灰色的绒毛");
                }
                stats::set(flags, okey, stats::joinList(cand));
            }
            std::string ekey = "eq_" + c.id() + "_" + std::to_string(s);
            if (stats::get(flags, ekey).empty()) stats::set(flags, ekey, "");
        }
    }

    // 上次打开的是哪一位的角色页——没入队的一律退回四不相
    std::string cur = stats::get(flags, "st_cur");
    int ci = roster_.indexOf(cur);
    if (ci < 0 || !roster_.all()[static_cast<std::size_t>(ci)].inParty(flags))
        stats::set(flags, "st_cur", "sibuxiang");
}

// 每次打开菜单前调用：把「上一次存进 flags 的当前角色」重新核对一遍，
// 没入队的一律换成四不相。这样菜单上永远不会出现没入队的角色。
void Game::syncCurrentChar(Flags& flags) {
    std::string cur = stats::get(flags, "st_cur");
    int ci = roster_.indexOf(cur);
    if (ci >= 0 && roster_.all()[static_cast<std::size_t>(ci)].inParty(flags)) return;
    stats::set(flags, "st_cur", "sibuxiang");
}

std::string Game::currentChar(const Flags& flags) const {
    std::string id = stats::get(flags, "st_cur", "sibuxiang");
    int ci = roster_.indexOf(id);
    if (ci < 0 || !roster_.all()[static_cast<std::size_t>(ci)].inParty(flags))
        return "sibuxiang";
    return id;
}

void Game::waitKey(const std::string& hint) {
    renderer_.newline();
    renderer_.printLine("　　" + hint, color::DIM);
    input::readKey();
}

// ---------------------------------------------------------------- 剧情状态机 --
void Game::runScript(Script& script, std::size_t& pc, Flags& flags) {
    while (pc < script.nodes.size()) {
        Node& n = script.nodes[pc];

        switch (n.type) {
            case NodeType::Dialogue: {
                renderDialogue(n);
                AfterAction a = afterText(script, pc, flags);
                if (a == AfterAction::Exit) return;
                if (a == AfterAction::Next) pc++;
                break;
            }
            case NodeType::Narration: {
                renderNarration(n);
                AfterAction a = afterText(script, pc, flags);
                if (a == AfterAction::Exit) return;
                if (a == AfterAction::Next) pc++;
                break;
            }
            case NodeType::Scene: {
                renderScene(n);
                pc++;            // 场景标题闪过即进入正文，不额外等按键
                break;
            }
            case NodeType::Clear: {
                renderer_.clear();
                bodyLines_ = 0;
                pc++;
                break;
            }
            case NodeType::Choice: {
                bodyLines_ = 0;   // 选项菜单会清屏重画，正文顶替状态重置
                int idx = renderChoice(n);
                if (idx < 0 || idx >= static_cast<int>(n.choices.size())) {
                    pc++;
                    break;
                }
                const std::string& target = n.choices[idx].target;
                Script::labels_t::const_iterator it = script.labels.find(target);
                pc = (it != script.labels.end()) ? it->second : pc + 1;
                break;
            }
            case NodeType::Jump: {
                Script::labels_t::const_iterator it = script.labels.find(n.target);
                pc = (it != script.labels.end()) ? it->second : pc + 1;
                break;
            }
            case NodeType::SetFlag: {
                flags[n.flag] = n.value;
                // 剧情得到的道具同步进「物品」栏（item_xxx 同时作为图鉴与持有物）
                pc++;
                break;
            }
            case NodeType::Branch: {
                Flags::const_iterator it = flags.find(n.flag);
                std::string v = (it != flags.end()) ? it->second : "";
                if (v == n.value) {
                    Script::labels_t::const_iterator lit = script.labels.find(n.target);
                    pc = (lit != script.labels.end()) ? lit->second : pc + 1;
                } else {
                    pc++;
                }
                break;
            }
            case NodeType::Map: {
                if (runMap(n, script, pc, flags)) return;
                break;
            }
            case NodeType::SavePrompt: {
                bodyLines_ = 0;
                if (savePromptMenu()) {
                    saveMenu(script, pc, flags);
                }
                pc++;
                break;
            }
            case NodeType::Item: {
                renderItem(n, flags);
                pc++;
                break;
            }
            case NodeType::End: {
                renderEnd(n);
                input::readKey();
                return;
            }
            case NodeType::ChapterEnd: {
                renderChapterEnd(n);
                input::readKey();
                return;
            }
        }
    }
}

// ---------------------------------------------------------------- 探索地图 --
// 运行一个 Map 节点：进入可走动地图循环。
// 玩家走到出口地点并按回车 -> 跳转到对应 label，返回 false 继续状态机；
// 选择「回到标题 / 退出」-> 返回 true，runScript 结束。
bool Game::runMap(const Node& n, Script& script, std::size_t& pc, Flags& flags) {
    if (n.mapId == "dorm_night") map_.loadDormNight();
    else if (n.mapId == "dorm_morning") map_.loadDormMorning();
    else if (n.mapId == "storeroom") map_.loadStoreroom();
    else if (n.mapId == "dorm") map_.loadDorm();
    else map_.loadDefaultTianting();
    // mapStartX/mapStartY 为负数时，沿用玩家上一次站的位置（互动后留在原地）
    if (n.mapStartX >= 0 && n.mapStartY >= 0)
        map_.setPlayer(n.mapStartX, n.mapStartY);
    // 深夜：四不相是睡着的。把他直接放在床上那一格——整夜躺在那儿，
    // 一格都动不了（方向键/WASD/鼠标全部不响应），天亮才醒来。
    if (n.mapId == "dorm_night") {
        bool put = false;
        for (int y = 1; y < map_.height() - 1 && !put; ++y) {
            for (int x = 1; x < map_.width() - 1 && !put; ++x) {
                if (map_.locAt(x, y) != "床") continue;
                map_.setPlayer(x, y);
                put = true;
            }
        }
    }
    bodyLines_ = 0;   // 地图清屏重画，正文顶替状态重置
    g_clickQueue.clear();   // 上一段剧情里积下的鼠标点击不要带进地图

    // ---- 地图上的角色（深夜宿舍里的四不遇）----
    // 每个角色一份行动进度：move / narration / dialogue / wait / disappear / exit。
    struct ActorState {
        int x = 0;
        int y = 0;
        std::string glyph;
        std::string color;
        std::size_t step = 0;
        int nextAt = 0;        // 下一步最早开始时刻（毫秒，相对 actorTiming）
        bool hidden = false;   // 已离场
        bool done = false;     // 脚本走完
    };
    int actorTiming = 0;   // 角色演出用毫秒时钟（真实流逝，不用 sleep 阻塞）
    std::vector<ActorState> actors;
    for (std::size_t i = 0; i < n.mapActors.size(); ++i) {
        const MapActor& a = n.mapActors[i];
        ActorState st;
        st.x = a.x;
        st.y = a.y;
        st.glyph = a.glyph.empty() ? std::string(MAP_ACTOR_SIBUYU) : a.glyph;   // 「遇」
        st.color = a.color;
        actors.push_back(st);
    }

    // 当前目标地点（地图上对应的门 / 物件用金色强调）
    const std::string target = n.mapExits.empty() ? "" : n.mapExits[0].location;

    // 触发地点的剧情：把 pc 跳到对应 label。返回是否成功触发。
    auto triggerExit = [&](const std::string& loc) -> bool {
        for (std::size_t i = 0; i < n.mapExits.size(); ++i) {
            if (n.mapExits[i].location == loc) {
                Script::labels_t::const_iterator it = script.labels.find(n.mapExits[i].label);
                pc = (it != script.labels.end()) ? it->second : pc + 1;
                return true;
            }
        }
        return false;
    };
    // 触发互动：必须按回车，不因移动自动互动。
    //  · 家具 / 门：站在那一格上按回车（门是出口，数据上可通行）。
    //  · 窗：嵌在墙里站不上去，站在其相邻格即可互动。
    auto triggerHere = [&]() -> bool {
        std::string loc = map_.playerLocation();
        if (!loc.empty() && triggerExit(loc)) return true;

        int px = map_.playerX();
        int py = map_.playerY();
        static const int dxy[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
        for (int i = 0; i < 4; ++i) {
            int nx = px + dxy[i][0], ny = py + dxy[i][1];
            if (!map_.inBounds(nx, ny)) continue;
            std::string c = map_.cellAt(nx, ny);
            if (c != "窗" && c != "门") continue;   // 只让嵌在墙里的窗 / 旧式门格可隔一格互动
            std::string l = map_.locationAt(nx, ny);
            if (!l.empty() && triggerExit(l)) return true;
        }
        return false;
    };
    // 找「数字键直达 / 鼠标点击」应停靠的格子：
    //  · 家具 / 门 -> 那一格本身（门是出口，可以走上去）
    //  · 窗（嵌在墙里，站不上去）-> 停在旁边的可走格
    auto locateExit = [&](const MapExit& ex, int& ox, int& oy) -> bool {
        for (int y = 0; y < map_.height(); ++y) {
            for (int x = 0; x < map_.width(); ++x) {
                if (map_.locationAt(x, y) != ex.location) continue;
                if (map_.isWalkable(x, y)) { ox = x; oy = y; return true; }
                // 不可站立（窗）：找相邻可走格
                static const int dxy[4][2] = {{0,1},{0,-1},{1,0},{-1,0}};
                for (int i = 0; i < 4; ++i) {
                    int nx = x + dxy[i][0], ny = y + dxy[i][1];
                    if (map_.isWalkable(nx, ny)) { ox = nx; oy = ny; return true; }
                }
            }
        }
        return false;
    };
    // 格子 (x,y) 是否可以走过去（鼠标点击用）：普通可走格直接走；
    // 鼠标点到嵌在墙里、站不上去的地点（窗）时，退而求其次走到它旁边。
    auto reachableSpot = [&](int gx, int gy, int& ox, int& oy) -> bool {
        if (map_.isWalkable(gx, gy)) { ox = gx; oy = gy; return true; }
        if (map_.isInteractive(gx, gy)) {
            static const int dxy[4][2] = {{0,1},{0,-1},{1,0},{-1,0}};
            for (int i = 0; i < 4; ++i) {
                if (map_.isWalkable(gx + dxy[i][0], gy + dxy[i][1])) {
                    ox = gx + dxy[i][0];
                    oy = gy + dxy[i][1];
                    return true;
                }
            }
        }
        return false;
    };

    // 宿舍地图每格是全角字符，显示占 2 列；40 格宽需 80 列。
    // 记录地图在终端上的起始行号，用于鼠标坐标换算。
    // clear() 后依次：空行(行0) + 居中标题(行1) + 空行(行2)，地图第 0 行起于行 3。
    const int mapTopRow = 3;

    // ---- 地图角色的画面 ----
    auto actorViews = [&]() {
        std::vector<Map::MapActorView> vs;
        for (std::size_t i = 0; i < actors.size(); ++i) {
            if (actors[i].hidden) continue;
            Map::MapActorView v;
            v.x = actors[i].x;
            v.y = actors[i].y;
            v.glyph = actors[i].glyph;
            v.color = actors[i].color;
            vs.push_back(v);
        }
        return vs;
    };
    // 某格是否被角色占着（玩家不能踩上去）
    auto actorAt = [&](int x, int y) -> int {
        for (std::size_t i = 0; i < actors.size(); ++i) {
            if (!actors[i].hidden && actors[i].x == x && actors[i].y == y)
                return static_cast<int>(i);
        }
        return -1;
    };

    bool anyActorAlive = false;
    for (std::size_t i = 0; i < actors.size(); ++i) {
        if (!actors[i].done) anyActorAlive = true;
    }

    // ---- 底部字幕条：角色说话时挂在画面最下方，不打断他走位 ----
    // 一律「玩家按空格 / 回车」才翻页，没有任何自动翻页、也不会因为等太久自己消失：
    //  · 正在逐字吐的时候按空格/回车 = 立刻把这一页吐完；
    //  · 这一页已经吐完，再按一下 = 翻到下一页（最后一页就收起，放他继续走）；
    //  · 一句话太长、一页放不下时会自动分成几页，仍然一页一页按着看；
    //  · 台词没翻完之前角色不往下走，所以每一句都保证读得到。
    struct Subtitle {
        bool isDialogue = false;
        std::string speaker;
        std::string text;      // 完整台词
        int shown = 0;         // 已显示到第几个字节（UTF-8 安全推进）
        int nextAt = 0;        // 下一个字出现的时刻（毫秒）
        std::string color;
        int remainMs = 0;      // >0 = 正等玩家按键；不会随时间递减
        // 折行结果：每行在 text 里的字节区间 [first, second)
        std::vector<std::pair<std::size_t, std::size_t> > lines;
        std::vector<int> pageEnd;   // 每页结束的字节偏移
        std::size_t page = 0;       // 当前第几页（从 0 起）
    };
    Subtitle subtitle;
    // 屏幕下方固定留 3 行给台词，第 4 行放提示，画面高度永远不变。
    const std::size_t kSubLinesPerPage = 3;
    // 打字机节奏：与正文同一套速度设置（设置界面的「文字速度」）
    int subDelayMs = Renderer::speedDelayUs() / 1000;
    if (subDelayMs < 0) subDelayMs = 0;

    // 下一个 UTF-8 字符的边界（不切碎多字节汉字）
    auto nextBoundary = [](const std::string& s, std::size_t i) -> std::size_t {
        if (i >= s.size()) return s.size();
        ++i;
        while (i < s.size() && (static_cast<unsigned char>(s[i]) & 0xC0) == 0x80) ++i;
        return i;
    };
    // 按显示宽度折行：全角汉字占 2 列，不能按字节数折
    auto wrapSub = [](const std::string& s, int maxCols) {
        std::vector<std::pair<std::size_t, std::size_t> > out;
        if (maxCols < 8) maxCols = 8;
        std::size_t i = 0, b = 0;
        int w = 0;
        while (i < s.size()) {
            std::size_t j = i + 1;
            while (j < s.size() && (static_cast<unsigned char>(s[j]) & 0xC0) == 0x80) ++j;
            int cw = Renderer::widthOf(s.substr(i, j - i));
            if (w + cw > maxCols && i > b) {
                out.push_back(std::make_pair(b, i));
                b = i;
                w = 0;
            }
            w += cw;
            i = j;
        }
        out.push_back(std::make_pair(b, s.size()));
        return out;
    };
    // 字幕前缀宽度：对白是「　　【名字】」，旁白只有缩进
    auto subIndentW = [](const Subtitle& s) -> int {
        return s.isDialogue ? (4 + 2 + Renderer::widthOf(s.speaker) + 2) : 4;
    };
    // 当前这一页结束在哪个字节
    auto subPageStop = [&]() -> int {
        if (subtitle.pageEnd.empty()) return static_cast<int>(subtitle.text.size());
        std::size_t p = subtitle.page;
        if (p >= subtitle.pageEnd.size()) p = subtitle.pageEnd.size() - 1;
        return subtitle.pageEnd[p];
    };
    // 这一页是否已经吐完、正等玩家按空格/回车
    auto subFull = [&]() -> bool {
        return subtitle.remainMs > 0 && subtitle.shown >= subPageStop();
    };

    // 角色往前走一格（一步一格，看得见他在走）。返回 true 表示画面需要重画。
    // 绕开玩家与其它角色；被挡住就原地等，不硬挤过去。
    auto actorStepMove = [&](std::size_t ai) -> bool {
        ActorState& st = actors[ai];
        const MapActorStep& s = n.mapActors[ai].steps[st.step];
        std::vector<char> path = map_.pathFrom(st.x, st.y, s.x, s.y);
        if (path.empty()) {
            st.x = s.x;
            st.y = s.y;
            st.step++;
            return true;
        }
        char d = path[0];
        int nx = st.x, ny = st.y;
        if (d == 'U') ny--;
        else if (d == 'D') ny++;
        else if (d == 'L') nx--;
        else if (d == 'R') nx++;
        if (actorAt(nx, ny) >= 0 || (nx == map_.playerX() && ny == map_.playerY())) {
            // 玩家（或别的角色）挡在面前：如果他已经贴着目的地了，就站这儿不动，
            // 免得因为玩家正好站在床边，他就永远走不到、看起来「根本没走过去」。
            int ddx = st.x - s.x;
            int ddy = st.y - s.y;
            if (ddx < 0) ddx = -ddx;
            if (ddy < 0) ddy = -ddy;
            if (ddx + ddy == 1) { st.step++; return true; }
            return false;   // 否则原地等一会儿，不硬挤过去
        }
        st.x = nx;
        st.y = ny;
        if (st.x == s.x && st.y == s.y) st.step++;
        return true;
    };

    // 往前推进一步（不含计时，计时由主循环里的 actorClock 负责）。
    // 台词只登记字幕、立刻返回：他自己一边说一边走，不会被一段话钉在原地。
    auto actorTick = [&]() -> bool {
        for (std::size_t ai = 0; ai < actors.size(); ++ai) {
            ActorState& st = actors[ai];
            if (st.done) continue;
            const MapActor& a = n.mapActors[ai];
            if (st.step >= a.steps.size()) { st.done = true; continue; }
            const MapActorStep& s = a.steps[st.step];
            if (s.kind == "move") {
                // 已经离场的角色只推进步数，不再在地图上跑（免得「消失」之后
                // 又冒出来，或者因为目标格被占住而永远走不出去）。
                if (st.hidden) {
                    st.step++;
                    st.nextAt = actorTiming + 60;
                    return true;
                }
                st.nextAt = actorTiming + (s.ms > 0 ? s.ms : 240);
                return actorStepMove(ai);
            }
            if (s.kind == "wait") {
                st.nextAt = actorTiming + (s.ms > 0 ? s.ms : 1);
                st.step++;
                return true;
            }
            if (s.kind == "disappear") {
                st.hidden = true;
                st.step++;
                st.nextAt = actorTiming + 200;
                return true;
            }
            if (s.kind == "exit") {
                st.done = true;
                st.step++;
                st.nextAt = actorTiming + 200;
                continue;
            }
            // narration / dialogue：挂一条字幕（打字机逐字显示），等玩家按空格/回车
            {
                Subtitle sub;
                sub.isDialogue = (s.kind == "dialogue");
                sub.speaker = s.speaker;
                sub.text = s.text;
                sub.color = (s.kind == "dialogue") ? s.color : (s.color.empty() ? color::DIM : s.color);
                sub.shown = 0;
                sub.nextAt = actorTiming;
                sub.remainMs = 1;   // 有效标记：只有玩家按空格/回车才会清零
                sub.lines = wrapSub(sub.text, 78 - subIndentW(sub));
                for (std::size_t i = kSubLinesPerPage - 1; i + 1 < sub.lines.size();
                     i += kSubLinesPerPage) {
                    sub.pageEnd.push_back(static_cast<int>(sub.lines[i].second));
                }
                sub.pageEnd.push_back(static_cast<int>(sub.text.size()));
                sub.page = 0;
                subtitle = sub;
                st.step++;
                st.nextAt = actorTiming;
                return true;
            }
        }
        return false;
    };

    bool npcDone = false;
    bool npcEpilogue = false;      // 深夜那场演完后的收尾（自动接清晨）
    // 深夜宿舍：四不相在床上睡着。四不遇动的时候，他一格都不能动——
    // 方向键、WASD、鼠标点击全部不响应，只有空格/回车能翻台词。
    // 这是设计上的「他还睡着」，不是卡住了。
    const bool playerSleeping = (n.mapId == "dorm_night");
    int frameNo = 0;
    // 角色「卡住」检测：连续这么多帧没有任何进展，就直接把脚本推进下去，
    // 保证演出一定会结束（玩家被挡路、地图被点错都不会让这场戏永远停摆）。
    int stuckFrames = 0;
    std::string lastDrawnSig;   // 上一帧画面指纹：没变就不重画，避免全屏闪烁
    std::chrono::steady_clock::time_point lastClock = std::chrono::steady_clock::now();
    while (true) {
        // 统一时钟：角色走位与字幕打字都用真实流逝的毫秒数，不再靠 sleep 卡住流程。
        std::chrono::steady_clock::time_point tickNow = std::chrono::steady_clock::now();
        int elapsedMs = static_cast<int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(tickNow - lastClock).count());
        if (elapsedMs < 0) elapsedMs = 0;
        if (elapsedMs > 200) elapsedMs = 200;   // 切窗口回来别一次跳一大截
        lastClock = tickNow;
        actorTiming += elapsedMs;

        // 字幕打字机：按速度设置逐字吐出来，但绝不越过当前这一页的结尾。
        // 「立即」档（0ms）会一次吐完这一页；其余档位每帧至多吐一个字。
        int emitGuard = 0;
        while (subtitle.remainMs > 0 && subtitle.shown < subPageStop() &&
               actorTiming >= subtitle.nextAt && emitGuard++ < 512) {
            subtitle.shown = static_cast<int>(nextBoundary(subtitle.text,
                static_cast<std::size_t>(subtitle.shown)));
            if (subtitle.shown > subPageStop()) subtitle.shown = subPageStop();
            subtitle.nextAt = actorTiming + subDelayMs;
            if (subDelayMs > 0) break;
        }

        // 有角色在演：卡着台词等他按回车，台词读完才让他继续走——所以他会「停住」，
        // 但不会僵死（方向键、回车、ESC 任何时候都有效）。
        bool keepMoving = false;
        if (anyActorAlive && !npcDone) {
            bool aliveNow = false;
            for (std::size_t i = 0; i < actors.size(); ++i)
                if (!actors[i].done) aliveNow = true;
            if (!aliveNow) {
                npcDone = true;
            } else {
                keepMoving = true;
            }
        }
        if (keepMoving) {
            beginFrame();
            // 画面指纹：角色位置 / 字幕进度 没变就完全不重画。
            // 老 cmd 的 clear() 是整屏填充，每 20ms 重画一次看着就是在闪。
            std::string sig = std::to_string(subtitle.remainMs) + "|" +
                              std::to_string(subtitle.shown) + "|" +
                              std::to_string(subtitle.page);
            for (std::size_t i = 0; i < actors.size(); ++i) {
                sig += "|" + std::to_string(actors[i].x) + "," + std::to_string(actors[i].y) +
                       (actors[i].hidden ? "h" : "") + (actors[i].done ? "d" : "");
            }
            sig += "|" + std::to_string(map_.playerX()) + "," + std::to_string(map_.playerY());
            if (sig != lastDrawnSig) {
            lastDrawnSig = sig;
            renderer_.clear();
            renderer_.newline();
            renderer_.printCentered("—— " + map_.title() + " ——", color::CYAN);
            renderer_.newline();
            map_.renderActors(renderer_, actorViews());

            // 字幕条：固定 3 行台词 + 1 行提示，共 4 行，画面高度永远不变。
            // 长句按显示宽度折行；一页最多 3 行，超出的分页，仍然一页一页按着看。
            bool waiting = subFull();
            {
                std::size_t first = subtitle.page * kSubLinesPerPage;
                for (std::size_t li = 0; li < kSubLinesPerPage; ++li) {
                    std::string body;
                    if (subtitle.remainMs > 0) {
                        std::size_t idx = first + li;
                        if (idx < subtitle.lines.size()) {
                            std::size_t b = subtitle.lines[idx].first;
                            std::size_t e = subtitle.lines[idx].second;
                            std::size_t shownB = static_cast<std::size_t>(subtitle.shown);
                            if (b < shownB) {
                                if (e > shownB) e = shownB;
                                body = subtitle.text.substr(b, e - b);
                            }
                        }
                    }
                    std::string head;
                    if (li == 0) {
                        head = subtitle.isDialogue ? ("　　【" + subtitle.speaker + "】")
                                                   : std::string("　　");
                    } else {
                        head.assign(static_cast<std::size_t>(subIndentW(subtitle)), ' ');
                    }
                    if (subtitle.remainMs > 0)
                        renderer_.printLine(head + body, subtitle.color.c_str());
                    else
                        renderer_.printLine("", color::DIM);
                }
                if (subtitle.remainMs > 0) {
                    std::string h = waiting ? std::string("〔空格 / 回车〕继续")
                                            : std::string("〔空格 / 回车〕立刻显示完整这句");
                    if (waiting && subtitle.pageEnd.size() > 1) {
                        h += "　（" + std::to_string(subtitle.page + 1) + "/" +
                             std::to_string(subtitle.pageEnd.size()) + "）";
                    }
                    renderer_.printLine("　　　　" + h, color::DIM);
                } else {
                    renderer_.printLine(playerSleeping
                        ? std::string("　　遇 = 四不遇　　你还在睡着，一动也动不了　　空格 / 回车 继续")
                        : std::string("　　遇 = 四不遇　　相 = 你　　〔方向键/WASD 你也能走〕"),
                        color::DIM);
                }
            }
            }

            // ---- 输入：任何时候都可以按（测试时由钩子注入） ----
            int kk = input::KEY_NONE;
            bool clickFlag = false;
            int mx2 = 0, my2 = 0;
            if (g_mapInputHook) {
                std::pair<int, bool> ev2 = g_mapInputHook->poll();
                kk = ev2.first;
                clickFlag = ev2.second;
            } else if (input::kbhit()) {
                std::pair<int, bool> ev2 = readEventPair(mx2, my2);
                kk = ev2.first;
                clickFlag = ev2.second;
            }
            if (clickFlag) {
                // 睡着的夜里不响应鼠标走位（点哪儿都不动）。其它地图照常。
                if (playerSleeping) { clickFlag = false; kk = input::KEY_NONE; }
            }
            if (clickFlag) {
                // 鼠标点地图 -> 走到那一格
                int ox = 0, oy = 0;
                if (reachableSpot(mx2 / 2, my2 - mapTopRow, ox, oy)) {
                    std::vector<char> pth = map_.pathTo(ox, oy);
                    for (std::size_t i = 0; i < pth.size(); ++i) {
                        switch (pth[i]) {
                            case 'U': map_.movePlayer(0, -1); break;
                            case 'D': map_.movePlayer(0, 1); break;
                            case 'L': map_.movePlayer(-1, 0); break;
                            case 'R': map_.movePlayer(1, 0); break;
                            default: break;
                        }
                    }
                }
                kk = input::KEY_NONE;
            }
            if (kk == input::KEY_ENTER || kk == input::KEY_SPACE) {
                if (subtitle.remainMs > 0) {
                    if (subtitle.shown < subPageStop()) {
                        subtitle.shown = subPageStop();          // 先把这一页吐完
                    } else if (subtitle.page + 1 < subtitle.pageEnd.size()) {
                        subtitle.page++;                          // 翻到下一页
                    } else {
                        subtitle.remainMs = 0;   // 最后一句读完，收起字幕，放他继续走
                    }
                }
            } else if (!playerSleeping && (kk == input::KEY_UP || kk == 'w' || kk == 'W')) {
                map_.movePlayer(0, -1);
            } else if (!playerSleeping && (kk == input::KEY_DOWN || kk == 's' || kk == 'S')) {
                map_.movePlayer(0, 1);
            } else if (!playerSleeping && (kk == input::KEY_LEFT || kk == 'a' || kk == 'A')) {
                map_.movePlayer(-1, 0);
            } else if (!playerSleeping && (kk == input::KEY_RIGHT || kk == 'd' || kk == 'D')) {
                map_.movePlayer(1, 0);
            }
            // 其它键一律不碰台词：不会因为误按一个键就把一整句话吞掉。

            // ---- 角色推进：台词没读完就原地等着（他会停在那里，不是僵死） ----
            // 注意：判断「谁该动」必须看这一步自己的时间戳，不能用「整场第几步」比较——
            // 步数是全图累加的，前面几步耗时长，后面的时间戳反而显得"已经过期"，
            // 于是每帧都会白推一步，台词一冒出来就被下一步顶掉。
            bool acted = false;
            for (std::size_t ai = 0; ai < actors.size() && !acted; ++ai) {
                ActorState& st = actors[ai];
                if (st.done) continue;
                const MapActor& a = n.mapActors[ai];
                if (st.step >= a.steps.size()) { st.done = true; continue; }
                const MapActorStep& s = a.steps[st.step];
                if ((s.kind == "narration" || s.kind == "dialogue") && subtitle.remainMs > 0) {
                    // 上一句玩家还没翻页：停住等，不吞掉这句话
                    continue;
                }
                if (actorTiming < st.nextAt) continue;   // 这一步还没到开始时间
                actorTick();
                acted = true;
            }
            (void)acted;

            // 卡住兜底：只有「该走却走不动」（被玩家堵路、地图异常）才强行推进。
            // 正等着玩家按空格/回车时不算卡住——那是他在读，等多久都得等。
            bool waitingKey = subtitle.remainMs > 0;
            if (!acted && !waitingKey) ++stuckFrames;
            else stuckFrames = 0;
            if (stuckFrames > 400) {
                stuckFrames = 0;
                for (std::size_t i = 0; i < actors.size(); ++i) {
                    if (actors[i].done) continue;
                    const MapActor& a2 = n.mapActors[i];
                    if (actors[i].step + 1 >= a2.steps.size()) actors[i].done = true;
                    else actors[i].step++;
                    break;
                }
            }

            endFrame();
            if (g_mapInputHook) g_mapInputHook->onFrame(++frameNo, frameLines_);
            if (!g_mapInputHook && !input::kbhit()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
            continue;
        }

        // 四不遇演完、走了：这一夜就过去了。
        // 地上什么都不留——荼蘼要等第二天早上才看得见（清晨那段里发现）。
        if (npcDone && !npcEpilogue) {
            npcEpilogue = true;
            lastDrawnSig.clear();
            for (std::size_t i = 0; i < n.mapExits.size(); ++i) {
                if (n.mapExits[i].location != "床") continue;
                int ox = 0, oy = 0;
                if (locateExit(n.mapExits[i], ox, oy)) map_.setPlayer(ox, oy);
                if (triggerHere()) return false;
                break;
            }
        }

        renderer_.clear();
        renderer_.printCentered("—— " + map_.title() + " ——", color::CYAN);
        renderer_.newline();
        beginFrame();
        map_.renderActors(renderer_, actorViews());

        // 折行保护：地图显示宽度超过终端时给出醒目提示（否则显示/鼠标都会错乱）
        int needCols = map_.width() * 2;
        int termCols = renderer_.terminalWidth();

        // 状态行（原来 2 行提示压成 1 行，给地图多留一行余量，
        // 免得传统 cmd 的 25 行窗口把上边框滚出屏幕）。
        {
            std::string hint;
            const char* hc = color::DIM;
            std::string here = map_.playerLocation();
            auto findExit = [&](const std::string& loc) -> bool {
                if (loc.empty()) return false;
                for (std::size_t i = 0; i < n.mapExits.size(); ++i) {
                    if (n.mapExits[i].location == loc) return true;
                }
                return false;
            };
            bool isTarget = findExit(here);
            // 门 / 窗：站在旁边一格也算「可以互动」，否则走到门前会以为没反应。
            std::string beside;
            if (here.empty()) {
                static const int dxy[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
                for (int i = 0; i < 4; ++i) {
                    int nx = map_.playerX() + dxy[i][0];
                    int ny = map_.playerY() + dxy[i][1];
                    if (!map_.inBounds(nx, ny)) continue;
                    std::string cc = map_.cellAt(nx, ny);
                    if (cc != "窗" && cc != "门" && !map_.isDoorCell(nx, ny)) continue;
                    std::string l = map_.locAt(nx, ny);
                    if (findExit(l)) { beside = l; break; }
                }
            }
            if (needCols > termCols) {
                hint = "⚠ 地图需 " + std::to_string(needCols) + " 列，当前仅 " +
                       std::to_string(termCols) + " 列，请拉宽窗口！";
                hc = color::RED;
            } else if (!here.empty() && isTarget) {
                hint = "· 你正站在【" + here + "】上。按回车" +
                       (here == "窗" ? std::string("查看") : std::string("打开")) + "。";
                hc = color::GOLD;
            } else if (!beside.empty()) {
                hint = "· 你旁边是【" + beside + "】。按回车" +
                       (beside == "窗" ? std::string("查看") : std::string("打开")) + "。";
                hc = color::GOLD;
            } else if (!here.empty()) {
                hint = "· 现在还不是来【" + here + "】的时候。";
            } else {
                // 数字键清单：每行最多 5 项，保证在 80 列窗口里不会折行。
                // 这一行就是「按几号键能去哪儿」的唯一说明，缺了它数字键等于没写。
                std::string list;
                int perLine = 0;
                for (std::size_t i = 0; i < n.mapExits.size() && i < 9; ++i) {
                    int no = static_cast<int>(i) + 1;
                    std::string item = std::to_string(no) + "=" + n.mapExits[i].location;
                    int need = Renderer::widthOf(item) + 1;
                    if (perLine > 0 && perLine + need > 46) {
                        list += "\n" + std::string("　　");
                        perLine = 0;
                    } else if (perLine > 0) {
                        list += " ";
                        perLine += 1;
                    }
                    list += item;
                    perLine += Renderer::widthOf(item);
                }
                hint = "· 走到物件上按回车互动；按数字键直达：\n　　" + list;
                hc = color::SILVER;
            }
            // hint 里可能含换行：逐行打印，行首统一缩进
            std::size_t start = 0;
            while (true) {
                std::size_t pos = hint.find('\n', start);
                std::string one = (pos == std::string::npos) ? hint.substr(start)
                                                             : hint.substr(start, pos - start);
                if (one.compare(0, 2, "　　") == 0)
                    renderer_.printLine(one, hc);
                else
                    renderer_.printLine("　　" + one, hc);
                if (pos == std::string::npos) break;
                start = pos + 1;
            }
        }
        renderer_.printLine("　　〔方向键/WASD 走　鼠标点击走　数字键直达　座位上回车互动〕",
                            color::DIM);

        endFrame();
        if (g_mapInputHook) g_mapInputHook->onFrame(++frameNo, frameLines_);

        // 读取输入事件：可能是按键，也可能是鼠标左键点击
        int mx = 0, my = 0;
        int k = input::KEY_NONE;
        bool isClick = false;
        if (g_mapInputHook) {
            std::pair<int, bool> ev = g_mapInputHook->poll();
            k = ev.first;
            isClick = ev.second;
        } else if (!g_clickQueue.empty()) {
            // 之前积攒下来的鼠标点击：与本次按键分开处理，先走寻路
            mx = g_clickQueue.front().first;
            my = g_clickQueue.front().second;
            g_clickQueue.pop_front();
            isClick = true;
        } else {
            std::pair<int, bool> ev = readEventPair(mx, my);
            k = ev.first;
            isClick = ev.second;
        }

        // 沿给定路径逐格移动并逐步重绘（自动走过去的动画）。
        auto walkAlong = [&](const std::vector<char>& path) {
            for (std::size_t i = 0; i < path.size(); ++i) {
                switch (path[i]) {
                    case 'U': map_.movePlayer(0, -1); break;
                    case 'D': map_.movePlayer(0, 1); break;
                    case 'L': map_.movePlayer(-1, 0); break;
                    case 'R': map_.movePlayer(1, 0); break;
                    default: break;
                }
                renderer_.clear();
                renderer_.newline();
                renderer_.printCentered("—— " + map_.title() + " ——", color::CYAN);
                renderer_.newline();
                map_.renderActors(renderer_, actorViews());
                // 走路动画期间也把图例/说明行补齐，免得画面忽高忽低
                renderer_.printLine("", color::DIM);
                renderer_.printLine("　　正在走过去……", color::DIM);
                std::this_thread::sleep_for(std::chrono::milliseconds(70));
            }
        };

        if (isClick) {
            // 鼠标点击：把终端「字符单元格」坐标换算成地图格坐标再寻路。
            int gy = my - mapTopRow;
            int gx = mx / 2;
            int ox = 0, oy = 0;
            if (reachableSpot(gx, gy, ox, oy)) {
                std::vector<char> path = map_.pathTo(ox, oy);
                walkAlong(path);
            }
        } else if (k == input::KEY_UP || k == 'w' || k == 'W') {
            map_.movePlayer(0, -1);
        } else if (k == input::KEY_DOWN || k == 's' || k == 'S') {
            map_.movePlayer(0, 1);
        } else if (k == input::KEY_LEFT || k == 'a' || k == 'A') {
            map_.movePlayer(-1, 0);
        } else if (k == input::KEY_RIGHT || k == 'd' || k == 'D') {
            map_.movePlayer(1, 0);
        } else if (k == input::KEY_ENTER || k == input::KEY_SPACE) {
            if (triggerHere()) return false;
        } else if (k >= '1' && k <= '9') {
            // 数字键直达：自动寻路到对应地点（门直接走到门上，窗走到旁边）。
            // 到这里自己按回车就互动，所以走到位即停，不会自动触发剧情。
            int idx = k - '1';
            if (idx >= 0 && idx < static_cast<int>(n.mapExits.size())) {
                int ox = 0, oy = 0;
                if (locateExit(n.mapExits[static_cast<std::size_t>(idx)], ox, oy)) {
                    std::vector<char> path = map_.pathTo(ox, oy);
                    walkAlong(path);
                }
            }
        } else if (k == input::KEY_ESC) {
            MenuAct act = systemMenu(script, pc, flags);
            if (act == MenuAct::Title || act == MenuAct::Quit) return true;
            if (act == MenuAct::Reload) return false;   // script/pc/flags 已就地更新
        }
    }
    if (g_mapInputHook) g_mapInputHook->onEnd();
    return true;
}

// ---------------------------------------------------------------- 节点渲染 --
void Game::renderDialogue(const Node& n) {
    if (bodyLines_ > 0) renderer_.eraseLines(bodyLines_);   // 清除上一句，顶替显示
    const char* spkColor = n.spkColor.empty() ? color::WHITE : n.spkColor.c_str();
    renderer_.printLine(n.speaker, spkColor);
    renderer_.typeLine("　　" + n.text, n.color.empty() ? nullptr : n.color.c_str());
    bodyLines_ = 2;
}

void Game::renderNarration(const Node& n) {
    if (bodyLines_ > 0) renderer_.eraseLines(bodyLines_);   // 清除上一句，顶替显示
    const char* c = n.color.empty() ? color::DIM : n.color.c_str();
    renderer_.typeLine("　　" + n.text, c);
    bodyLines_ = 1;
}

void Game::renderScene(const Node& n) {
    renderer_.clear();
    bodyLines_ = 0;
    if (!n.sceneId.empty()) renderer_.showImage(n.sceneId);
    renderer_.newline();
    renderer_.printCentered("—— " + n.sceneTitle + " ——", color::GOLD);
    renderer_.printRule('-', color::GRAY);
    renderer_.newline();
    currentScene_ = n.sceneTitle;
}

void Game::renderEnd(const Node& n) {
    renderer_.clear();
    bodyLines_ = 0;
    renderer_.newline();
    renderer_.newline();
    const char* titleColor = n.isBad ? color::RED : color::GOLD;
    renderer_.printCentered("—— " + n.sceneTitle + " ——", titleColor);
    renderer_.newline();
    renderer_.typeLine("　　" + n.text, color::SILVER);
    renderer_.newline();
    renderer_.printCentered("〔回车〕返回标题", color::DIM);
}

void Game::renderChapterEnd(const Node& n) {
    renderer_.clear();
    bodyLines_ = 0;
    renderer_.newline();
    renderer_.newline();
    renderer_.printCentered("—— " + n.sceneTitle + " ——", color::CYAN);
    renderer_.newline();
    renderer_.typeLine("　　" + n.text, color::SILVER);
    renderer_.newline();
    renderer_.printCentered("〔回车〕返回标题", color::DIM);
}

// 获得道具：独立一个提示窗（RPG Maker 风格的方框），按回车继续剧情。
// 拿到已经有的东西时只把数量 +1，弹窗里显示「×N」，不再重复记一项。
void Game::renderItem(const Node& n, Flags& flags) {
    const ItemInfo info = itemMark(n.text);

    // 先记账再显示数量：新拿到的永远是 1，重复拿到的会显示累计数量
    inventory::add(flags, n.text);
    const int cnt = inventory::count(flags, n.text);

    renderer_.clear();
    bodyLines_ = 0;
    renderer_.newline();
    renderer_.newline();
    renderer_.printCentered("获得道具", color::GOLD);
    renderer_.newline();

    const int w = kBoxHalf + 18;   // 58 列，88 列终端里居中还有余量
    std::string indent;
    {
        int pad = (renderer_.terminalWidth() - w) / 2;
        if (pad < 0) pad = 0;
        indent.assign(static_cast<std::size_t>(pad), ' ');
    }
    const int inner = w - 2;

    renderer_.printLine(indent + "\xE2\x94\x8C" + innerRule(inner) + "\xE2\x94\x90", color::GOLD);
    // 道具那一行：符号 + 名字 + ×N
    {
        std::string body = " " + info.mark + " " + n.text + "　×" + num(cnt);
        renderer_.printLine(indent + "\xE2\x94\x82" + padRight(body, inner) + "\xE2\x94\x82",
                            color::GOLD);
    }
    renderer_.printLine(indent + "\xE2\x94\x9C" + innerRule(inner) + "\xE2\x94\xA4", color::GOLD);
    if (!n.itemDesc.empty()) {
        std::string body = " " + n.itemDesc;
        renderer_.printLine(indent + "\xE2\x94\x82" + padRight(body, inner) + "\xE2\x94\x82",
                            color::SILVER);
    }
    renderer_.printLine(indent + "\xE2\x94\x94" + innerRule(inner) + "\xE2\x94\x98", color::GOLD);
    renderer_.newline();
    renderer_.printCentered("你获得了【" + n.text + "】", color::AMBER);

    renderer_.newline();
    renderer_.printCentered("〔回车〕继续", color::DIM);
    input::readKey();
}

int Game::renderChoice(const Node& n) {
    std::size_t sel = 0;
    while (true) {
        renderer_.clear();
        renderer_.newline();
        renderer_.printLine("　　" + n.text, color::CYAN);
        renderer_.newline();
        for (std::size_t i = 0; i < n.choices.size(); ++i) {
            std::string prefix = (i == sel) ? "  》 " : "    ";
            const char* c = (i == sel) ? color::CYAN : color::SILVER;
            renderer_.printLine(prefix + n.choices[i].text, c);
        }
        renderer_.newline();
        renderer_.printLine("　　【方向键】选择　【回车】确认　【数字键】快速选择", color::DIM);

        int k = input::readKey();
        if (k == input::KEY_UP) {
            if (sel > 0) sel--;
        } else if (k == input::KEY_DOWN) {
            if (sel + 1 < n.choices.size()) sel++;
        } else if (k == input::KEY_ENTER || k == input::KEY_SPACE) {
            return static_cast<int>(sel);
        } else if (k >= '1' && k <= '9') {
            int idx = k - '1';
            if (idx >= 0 && idx < static_cast<int>(n.choices.size())) return idx;
        }
    }
}

// ------------------------------------------------------------------- 交互 --
// 正文（对白 / 旁白）打完后的等待：回车/空格继续，Esc 打开系统菜单。
Game::AfterAction Game::afterText(Script& script, std::size_t& pc, Flags& flags) {
    while (true) {
        int k = input::readKey();
        if (k == input::KEY_ESC) {
            MenuAct act = systemMenu(script, pc, flags);
            if (act == MenuAct::Title || act == MenuAct::Quit) return AfterAction::Exit;
            if (act == MenuAct::Reload) return AfterAction::Exit;   // 读档后重跑
        } else {
            return AfterAction::Next;
        }
    }
}

// ---------------------------------------------------------------- 系统菜单 --
// 版式照图 1：左边一列菜单项，右边队伍状态（等级 / 种族 / HP / MP / TP）。
// 七项主菜单照图 1：物品 / 技能 / 装备 / 状态 / 整队 / 设置 / 游戏结束。
// 存档 · 读档 · 回到标题 收在「暂停」里，不跟主菜单混在一起。
//
// 排版约束（很重要）：
//   · 两个窗各 36 列，中间空 2 列，合计 74 列，传统 cmd 的 80 列放得下。
//   · 只用「明确宽度」的全角符号：方向箭头、实心/空心三角形、

//     同一行里混着会出现时宽时窄、框线错位，所以这里一律换成「》」「【】」等。
Game::MenuAct Game::systemMenu(Script& script, std::size_t& pc, Flags& flags) {
    static const char* items[] = {
        "物品", "技能", "装备", "状态", "整队", "设置", "游戏结束"
    };
    static const std::size_t N = sizeof(items) / sizeof(items[0]);
    std::size_t sel = 0;
    int outer = 0;   // 0=主菜单，1=暂停菜单（存档/读档/回标题）
    syncCurrentChar(flags);

    while (true) {
        renderer_.clear();
        renderer_.newline();

        if (outer == 0) {
            // ================= 左窗：菜单列表 =================
            const int lw = kBoxHalf;
            std::vector<std::string> left;
            left.push_back(boxTop(lw, ""));
            for (std::size_t i = 0; i < N; ++i) {
                bool on = (i == sel);
                // 选中项用「》」做记号（明确的宽度，不会挤歪框线）
                std::string label = fit(std::string(on ? "》" : "　") + items[i], lw - 2);
                left.push_back(boxMid(label, lw, on ? color::CYAN : color::SILVER));
            }
            while (left.size() < static_cast<std::size_t>(kBoxRows - 1))
                left.push_back(boxMid(std::string(static_cast<std::size_t>(lw - 2), ' '), lw));
            left.push_back(boxBottom(lw));

            // ================= 右窗：队伍状态 =================
            // 表头 2 行 + 每人 4 行（名字+种族+等级 / HP / MP / TP）。
            // **只列已入队的角色**：没入队的不出现（四不遇 / 上司目前都不在队里，
            // 所以队伍窗里只有四不相一个）。
            const int rw = kBoxWide;
            std::vector<std::string> right;
            right.push_back(boxTop(rw, "队伍"));
            right.push_back(boxRule(rw));

            const std::vector<Character>& chars = roster_.all();
            const std::string curId = currentChar(flags);

            std::size_t shown = 0;
            bool anyShown = false;
            for (std::size_t ci = 0; ci < chars.size(); ++ci) {
                const Character& c = chars[ci];
                if (!c.inParty(flags)) continue;   // 未入队：不显示
                if (shown >= 2) break;             // 版面只排得下两位
                ++shown;
                anyShown = true;
                bool on = (c.id() == curId);
                std::string l1 = " " + emblemName(c) + c.name() + "　" + c.race() +
                                 "　" + c.level(flags);
                std::string l2 = " HP " + group3(c.hp(flags)) + "/" + group3(c.hpMax(flags));
                std::string l3 = " MP " + group3(c.mp(flags)) + "/" + group3(c.mpMax(flags));
                std::string l4 = " TP " + group3(c.tp(flags));
                right.push_back(boxMid(l1, rw, on ? color::CYAN : color::WHITE));
                right.push_back(boxMid(l2, rw, color::AMBER));
                right.push_back(boxMid(l3, rw, color::AMBER));
                right.push_back(boxMid(l4, rw, color::AMBER));
            }
            if (!anyShown) {
                right.push_back(boxMid(" （队伍里还没有人）", rw, color::GRAY));
            }
            while (right.size() < static_cast<std::size_t>(kBoxRows - 1))
                right.push_back(boxMid(std::string(static_cast<std::size_t>(rw - 2), ' '), rw));
            right.push_back(boxBottom(rw));

            for (std::size_t r = 0; r < static_cast<std::size_t>(kBoxRows); ++r)
                renderer_.printLine(left[r] + "  " + right[r]);

            renderer_.newline();
            renderer_.printLine("　　【方向键】选择　【回车】确认　【数字键】直选　【Esc】返回游戏",
                                color::DIM);
        } else {
            // ================= 暂停：继续 / 存档 / 读档 / 回到标题 =================
            static const char* pitems[] = {"继续游戏", "存档", "读档", "回到标题"};
            const std::size_t PN = sizeof(pitems) / sizeof(pitems[0]);
            renderer_.printCentered("—— 暂停 ——", color::GOLD);
            renderer_.newline();
            for (std::size_t i = 0; i < PN; ++i) {
                bool on = (i == sel);
                renderer_.printCentered(std::string(on ? "》 " : "　 ") + pitems[i],
                                        on ? color::CYAN : color::SILVER);
            }
            renderer_.newline();
            renderer_.printLine("　　【方向键】选择　【回车】确认　【Esc】返回主菜单", color::DIM);
        }

        int k = input::readKey();
        if (k == input::KEY_UP) {
            if (sel > 0) sel--;
        } else if (k == input::KEY_DOWN) {
            if (sel + 1 < (outer == 0 ? N : std::size_t(4))) sel++;
        } else if (k == input::KEY_ESC) {
            if (outer == 0) return MenuAct::Continue;
            outer = 0;
            sel = 0;
        } else if (k >= '1' && k <= '9') {
            int idx = k - '1';
            if (outer == 0 && idx >= 0 && idx < static_cast<int>(N))
                sel = static_cast<std::size_t>(idx);
            if (outer == 1 && idx >= 0 && idx < 4) sel = static_cast<std::size_t>(idx);
        } else if (k == input::KEY_ENTER || k == input::KEY_SPACE) {
            if (outer == 0) {
                if (sel == 0) {
                    if (chooseCharacter(flags, "物品")) itemsMenu(flags, 0);
                } else if (sel == 1) {
                    if (chooseCharacter(flags, "技能")) skillsMenu(flags, 0);
                } else if (sel == 2) {
                    if (chooseCharacter(flags, "装备")) equipMenu(flags, 0);
                } else if (sel == 3) {
                    if (chooseCharacter(flags, "状态")) statusMenu(flags, 0);
                } else if (sel == 4) {
                    orderMenu(flags);
                } else if (sel == 5) {
                    settingsMenu();
                } else {
                    // 「游戏结束」= 回到暂停页的退出分支
                    outer = 1;
                    sel = 3;
                }
            } else {
                if (sel == 0) { outer = 0; sel = 0; }
                else if (sel == 1) { saveMenu(script, pc, flags); }
                else if (sel == 2) {
                    int slot = loadMenu();
                    if (slot >= 0) {
                        SaveData d;
                        if (saves_.load(slot, d)) {
                            flags = d.flags;
                            initProfile(flags, false);
                            script = buildChapter(d.chapter);
                            pc = d.pc;
                            skipSavePrompt(script, pc);
                            currentChapter_ = d.chapter;
                            currentScene_ = d.sceneTitle;
                            return MenuAct::Reload;
                        }
                    }
                } else {
                    return MenuAct::Title;
                }
            }
        }
    }
}

// ---------------------------------------------------------------- 选角色 --
// **只列出已入队的角色**。目前队伍里只有四不相，所以这个窗口会直接跳过；
// 将来谁入队了（setFlag("party_xxx","1")），这里就会自动多出一行。
bool Game::chooseCharacter(Flags& flags, const std::string& title) {
    syncCurrentChar(flags);
    const std::vector<Character>& chars = roster_.all();
    std::vector<std::size_t> ids = roster_.party(flags);
    if (ids.empty()) return false;
    if (ids.size() == 1) {   // 只有一个人：不用选
        stats::set(flags, "st_cur", chars[ids[0]].id());
        return true;
    }

    std::size_t sel = 0;
    const std::string cur = currentChar(flags);
    for (std::size_t i = 0; i < ids.size(); ++i)
        if (chars[ids[i]].id() == cur) sel = i;

    while (true) {
        renderer_.clear();
        renderer_.newline();
        renderer_.printCentered("—— " + title + " · 选择角色 ——", color::GOLD);
        renderer_.newline();
        for (std::size_t i = 0; i < ids.size(); ++i) {
            const Character& c = chars[ids[i]];
            bool on = (i == sel);
            std::string line = std::string(on ? "》 " : "　 ") + emblemName(c) + c.name() +
                               "　" + c.race() + "　" + c.level(flags);
            renderer_.printCentered(line, on ? color::CYAN : color::SILVER);
        }
        renderer_.newline();
        renderer_.printLine("　　【方向键】选择　【回车】确认　【Esc】返回", color::DIM);
        int k = input::readKey();
        if (k == input::KEY_UP) { if (sel > 0) --sel; }
        else if (k == input::KEY_DOWN) { if (sel + 1 < ids.size()) ++sel; }
        else if (k == input::KEY_ESC) { return false; }
        else if (k == input::KEY_ENTER || k == input::KEY_SPACE) {
            stats::set(flags, "st_cur", chars[ids[sel]].id());
            return true;
        } else if (k >= '1' && k <= '9') {
            int idx = k - '1';
            if (idx >= 0 && idx < static_cast<int>(ids.size())) {
                stats::set(flags, "st_cur", chars[ids[static_cast<std::size_t>(idx)]].id());
                return true;
            }
        }
    }
}

// 角色名前的记号：**一律不加**（队伍窗、选角色、整队、技能、状态都只用名字）。
// 留这两个函数是为了以后真有需要时只改一处：现在都返回空串。
std::string Game::emblemName(const Character& /*c*/) const {
    return "";
}

std::string Game::emblemColor(const Character& /*c*/) const {
    return "";
}

// 整队：只列已入队的角色（没入队的不出现在这里）。
void Game::orderMenu(Flags& flags) {
    syncCurrentChar(flags);
    const std::vector<Character>& chars = roster_.all();
    std::vector<std::size_t> ids = roster_.party(flags);

    renderer_.clear();
    renderer_.newline();
    renderer_.printCentered("—— 整队 ——", color::GOLD);
    renderer_.newline();
    for (std::size_t i = 0; i < ids.size(); ++i) {
        const Character& c = chars[ids[i]];
        bool cur = (c.id() == currentChar(flags));
        std::string line = emblemColor(c) + emblemName(c) + c.name() +
                           "　" + c.race() + "　" + c.level(flags) +
                           "　HP " + group3(c.hp(flags)) + "/" + group3(c.hpMax(flags)) +
                           (cur ? "　【当前】" : "");
        renderer_.printCentered(line, cur ? color::CYAN : color::SILVER);
    }
    if (ids.empty()) renderer_.printCentered("（队伍里还没有人）", color::GRAY);
    renderer_.newline();
    renderer_.printCentered("队伍里现在只有这些，往后就靠缘分了。", color::DIM);
    waitKey();
}

// ---------------------------------------------------------------- 物品栏 --
// 版式照图 2：顶部四个分类页签，下方两栏各 6 行，右侧显示数量。
// 整页控制在 80 列以内。
void Game::itemsMenu(Flags& flags, std::size_t /*charIdx*/) {
    static const char* tabs[] = {"物品", "武器", "护甲", "重要物品"};
    const std::size_t NT = sizeof(tabs) / sizeof(tabs[0]);
    std::size_t tab = 3;   // 默认停在「重要物品」：序章能拿到的东西都在这一栏

    // 按名字粗略分类（不做数值计算，只决定放在哪一栏）
    auto categorize = [](const std::string& nm) -> int {
        if (nm.find("剑") != std::string::npos) return 1;                      // 武器
        if (nm.find("盾") != std::string::npos ||
            nm.find("护甲") != std::string::npos ||
            nm.find("面具") != std::string::npos ||
            nm.find("兜") != std::string::npos) return 2;                      // 护甲
        if (nm.find("玉") != std::string::npos ||
            nm.find("绒") != std::string::npos ||
            nm.find("毛") != std::string::npos ||
            nm.find("公文") != std::string::npos ||
            nm.find("文卷") != std::string::npos ||
            nm.find("棉") != std::string::npos) return 3;                      // 重要物品
        return 0;
    };

    while (true) {
        // 同种物品合并成一行：list 是 {名字, 数量}
        std::vector<std::pair<std::string, int>> list;
        std::vector<std::pair<std::string, int>> allItems = inventory::all(flags);
        for (std::size_t i = 0; i < allItems.size(); ++i) {
            if (categorize(allItems[i].first) == static_cast<int>(tab)) list.push_back(allItems[i]);
        }
        // 按名字排序，顺序稳定（unordered_map 的遍历顺序每次都可能不同）
        for (std::size_t i = 1; i < list.size(); ++i) {
            std::pair<std::string, int> cur = list[i];
            std::size_t j = i;
            while (j > 0 && list[j - 1].first > cur.first) {
                list[j] = list[j - 1];
                --j;
            }
            list[j] = cur;
        }

        renderer_.clear();
        renderer_.newline();
        renderer_.printLine("　【物品】",
                            color::GOLD);

        // 页签行：每栏 17 列，共 68 列
        {
            std::string line;
            const int tabW = 17;
            for (std::size_t i = 0; i < NT; ++i) {
                // 页签标记：选中的用全角「〔」开头（占 2 列，和空白「　」同宽），
                // 保证所有页签宽度一致、列不错位。
                std::string t = std::string(tab == i ? "〔" : "　") + tabs[i];
                line += std::string(tab == i ? color::CYAN : color::GRAY) + fit(t, tabW) +
                        std::string(color::RESET);
            }
            renderer_.printLine(line);
            renderer_.printLine(std::string(color::GOLD) + innerRule(68) +
                                std::string(color::RESET));
        }

        const std::size_t perCol = 6;
        const int cellW = 34;
        for (std::size_t r = 0; r < perCol; ++r) {
            std::string line = " ";
            for (std::size_t col = 0; col < 2; ++col) {
                std::size_t idx = col * perCol + r;
                std::string cell;
                if (idx < list.size()) {
                    ItemInfo info = itemMark(list[idx].first);
                    // 名称按 24 列排，数量靠右对齐到第 30 列的「×N」
                    std::string nm = info.mark + " " + list[idx].first;
                    cell = fit(nm, 24) + padLeft("×" + num(list[idx].second), 6);
                }
                line += fit(cell, cellW);
            }
            renderer_.printLine(line, color::SILVER);
        }

        renderer_.newline();
        renderer_.printLine("　　【左右键】切换分类　【Esc】返回　　共 " +
                                num(static_cast<long long>(inventory::total(flags))) + " 种",
                            color::DIM);

        int k = input::readKey();
        if (k == input::KEY_ESC || k == input::KEY_ENTER || k == input::KEY_SPACE) return;
        if (k == input::KEY_LEFT) { if (tab > 0) --tab; }
        else if (k == input::KEY_RIGHT) { if (tab + 1 < NT) ++tab; }
        else if (k >= '1' && k <= '9') {
            int idx = k - '1';
            if (idx >= 0 && idx < static_cast<int>(NT)) tab = static_cast<std::size_t>(idx);
        }
    }
}

// ---------------------------------------------------------------- 技能栏 --
// 版式照图 3：左栏列出分支（法术 / 灵力），右侧列出该分支下的技能。
// 四不相的技能暂时留空——等剧本写好，把名字用 '|' 连起来写进
// flag  sk_<角色id>_<分支序号>  就会显示出来。
void Game::skillsMenu(Flags& flags, std::size_t /*charIdx*/) {
    const std::string id = currentChar(flags);
    int ci = roster_.indexOf(id);
    if (ci < 0) return;
    const Character& c = roster_.all()[static_cast<std::size_t>(ci)];

    renderer_.clear();
    renderer_.newline();
    renderer_.printCentered("—— 技能 · " + c.name() + " ——", color::GOLD);
    renderer_.newline();
    renderer_.printLine("　" + emblemColor(c) + emblemName(c) + c.name() +
                            "　" + c.race() + "　" + c.level(flags) +
                            "　　MP " + group3(c.mp(flags)) + "/" + group3(c.mpMax(flags)),
                        color::SILVER);
    renderer_.newline();

    const int labelW = 14;
    for (std::size_t t = 0; t < c.skillTabs().size(); ++t) {
        std::vector<std::string> sk = c.skillsIn(flags, t);
        renderer_.printLine(std::string(color::GOLD) + fit("  " + c.skillTabs()[t], labelW + 2) +
                                std::string(color::RESET) + std::string(color::GOLD) +
                                "│" + std::string(color::RESET),
                            nullptr);
        bool any = false;
        for (std::size_t i = 0; i < sk.size(); ++i) {
            renderer_.printLine("  " + std::string(static_cast<std::size_t>(labelW), ' ') +
                                    std::string(color::CYAN) + sk[i],
                                nullptr);
            any = true;
        }
        if (!any) {
            renderer_.printLine("  " + std::string(static_cast<std::size_t>(labelW), ' ') +
                                    std::string(color::GRAY) + "（暂无）",
                                nullptr);
        }
        renderer_.printLine(std::string(color::GRAY) + innerRule(56) +
                                std::string(color::RESET));
    }

    renderer_.newline();
    renderer_.printLine("　　（四不相的技能暂时不填，随剧情推进再加。）", color::DIM);
    waitKey();
}

// ---------------------------------------------------------------- 装备栏 --
// 版式照图 4：左栏「装备 / 最强装备 / 清空 / 结束」+ 五个槽位，右侧候选列表。
// 「最强装备」= 把每个槽位自动换成该槽最强的那件。
void Game::equipMenu(Flags& flags, std::size_t /*charIdx*/) {
    static const char* cmds[] = {"装备", "最强装备", "清空", "结束"};
    const std::size_t N = sizeof(cmds) / sizeof(cmds[0]);
    const std::string id = currentChar(flags);
    int ci = roster_.indexOf(id);
    if (ci < 0) return;
    const Character& c = roster_.all()[static_cast<std::size_t>(ci)];
    const std::size_t nSlot = c.equipSlots().size();

    std::size_t sel = 0;
    std::string msg;

    while (true) {
        renderer_.clear();
        renderer_.newline();
        renderer_.printCentered("—— 装备 · " + c.name() + " ——", color::GOLD);
        renderer_.newline();

        for (std::size_t i = 0; i < N; ++i) {
            bool on = (i == sel);
            renderer_.printLine("  " + std::string(on ? "》 " : "　 ") + cmds[i],
                                on ? color::CYAN : color::SILVER);
        }
        renderer_.printLine(std::string(color::GOLD) + innerRule(40) +
                                std::string(color::RESET));
        for (std::size_t s = 0; s < nSlot; ++s) {
            std::string eq = c.equipped(flags, s);
            bool on = (sel == N + s);
            ItemInfo info = itemMark(eq.empty() ? "（空）" : eq);
            renderer_.printLine("  " + std::string(on ? "》 " : "　 ") +
                                    fit(c.equipSlots()[s].label, 8) + info.mark + " " +
                                    (eq.empty() ? "（空）" : eq),
                                on ? color::CYAN : (eq.empty() ? color::GRAY : color::WHITE));
        }
        renderer_.printLine(std::string(color::GOLD) + innerRule(40) +
                                std::string(color::RESET));

        if (sel >= N) {
            // 候选：首项 = 该槽最强
            std::size_t s = sel - N;
            std::vector<std::string> cand = c.slotCandidates(flags, s);
            renderer_.printLine("  候选（第一项是最强）：", color::GOLD);
            std::string curKey = stats::get(flags, "eq_" + id + "_" + std::to_string(s));
            std::string curDisp = stats::get(flags, "item_" + curKey);
            for (std::size_t i = 0; i < cand.size(); ++i) {
                bool onEquipped = (!curDisp.empty() && curDisp == cand[i]);
                std::string shown = cand[i].empty() ? "（空）" : cand[i];
                // 「◆」在 cmd 里宽度不定，用全角「》」保证对齐
                renderer_.printLine("  " + std::string(i == 0 ? "》" : "　") + " " + shown +
                                        (onEquipped ? "　【已装备】" : ""),
                                    onEquipped ? color::CYAN : color::SILVER);
            }
        } else {
            renderer_.printLine("  属性：", color::GOLD);
            for (std::size_t i = 0; i < c.stats().size(); ++i) {
                std::string v = group3(c.statValue(flags, i));
                if (c.statBoosted(flags, i)) v += " 升高";
                renderer_.printLine("  " + fit(c.stats()[i].first, 12) + v,
                                    c.statBoosted(flags, i) ? color::GREEN : color::SILVER);
            }
        }

        renderer_.newline();
        if (!msg.empty()) {
            renderer_.printLine("　　" + msg, color::GREEN);
        } else {
            renderer_.printLine("　　【方向键】选择　【回车】确认　【Esc】返回", color::DIM);
        }

        int k = input::readKey();
        msg.clear();
        if (k == input::KEY_UP) { if (sel > 0) --sel; }
        else if (k == input::KEY_DOWN) { if (sel + 1 < N + nSlot) ++sel; }
        else if (k == input::KEY_ESC) return;
        else if (k == input::KEY_ENTER || k == input::KEY_SPACE) {
            if (sel == 0) {
                sel = N;                                   // 「装备」把光标挪到第一个槽位
            } else if (sel == 1) {
                c.autoBestEquip(flags);
                msg = "已给「" + c.name() + "」换上最强装备。";
            } else if (sel == 2) {
                c.clearEquip(flags);
                msg = "已清空「" + c.name() + "」的装备。";
            } else if (sel == 3) {
                return;
            } else {
                std::size_t s = sel - N;
                std::vector<std::string> keys =
                    stats::splitList(stats::get(flags, "eqo_" + id + "_" + std::to_string(s)));
                if (!keys.empty()) {
                    stats::set(flags, "eq_" + id + "_" + std::to_string(s), keys[0]);
                    std::string disp = stats::get(flags, "item_" + keys[0]);
                    msg = "已装备「" + (disp.empty() ? std::string("（空）") : disp) + "」。";
                } else {
                    msg = "这一栏还没有可用的装备。";
                }
            }
        } else if (k >= '1' && k <= '9') {
            int idx = k - '1';
            if (idx >= 0 && idx < static_cast<int>(N + nSlot))
                sel = static_cast<std::size_t>(idx);
        }
    }
}

// ---------------------------------------------------------------- 状态页 --
// 版式照图 5：名字 / 种族 / 等级 / HP / MP、经验值、六项属性、装备栏、底部介绍。
// 整页 80 列以内，不画人物头图。
void Game::statusMenu(Flags& flags, std::size_t /*charIdx*/) {
    const std::string id = currentChar(flags);
    int ci = roster_.indexOf(id);
    if (ci < 0) return;
    const Character& c = roster_.all()[static_cast<std::size_t>(ci)];

    renderer_.clear();
    renderer_.newline();
    renderer_.printLine(std::string(color::GOLD) + fit(" " + c.name() + "　" + c.race(), 40) +
                            fit(c.name() + " ", 40) + std::string(color::RESET));
    renderer_.printLine(std::string(color::GOLD) + innerRule(80) + std::string(color::RESET));

    renderer_.printLine(" " + std::string(color::CYAN) + c.level(flags) +
                            std::string(color::RESET) + "　现在经验值 " + group3(c.exp(flags)) +
                            "　距离下一级 " + group3(c.expToNext()));
    renderer_.printLine(" HP " + group3(c.hp(flags)) + "/" + group3(c.hpMax(flags)) +
                            "　MP " + group3(c.mp(flags)) + "/" + group3(c.mpMax(flags)) +
                            "　TP " + group3(c.tp(flags)),
                        color::AMBER);
    renderer_.printLine(std::string(color::GOLD) + innerRule(80) + std::string(color::RESET));

    const std::size_t nStat = c.stats().size();
    const std::size_t nSlot = c.equipSlots().size();
    const std::size_t rows = nStat > nSlot ? nStat : nSlot;
    for (std::size_t i = 0; i < rows; ++i) {
        std::string left;
        if (i < nStat) {
            std::string v = group3(c.statValue(flags, i));
            if (c.statBoosted(flags, i)) v += " 升";
            left = padRight(" " + c.stats()[i].first, 12) + padLeft(v, 10);
            left = padRight(left, 38);
        } else {
            left = std::string(38, ' ');
        }
        std::string right;
        if (i < nSlot) {
            std::string eq = c.equipped(flags, i);
            ItemInfo info = itemMark(eq.empty() ? "（空）" : eq);
            right = " " + fit(c.equipSlots()[i].label, 8) + info.mark + " " +
                    (eq.empty() ? "（空）" : eq);
        }
        renderer_.printLine(left + right,
                            (i < nStat && c.statBoosted(flags, i)) ? color::GREEN
                                                                   : color::SILVER);
    }

    renderer_.printLine(std::string(color::GOLD) + innerRule(80) + std::string(color::RESET));
    // 介绍：按 '\n' 拆行，最多 3 行
    {
        const std::string& d = c.desc();
        std::size_t start = 0;
        int n = 0;
        while (start <= d.size() && n < 3) {
            std::size_t pos = d.find('\n', start);
            std::string line =
                (pos == std::string::npos) ? d.substr(start) : d.substr(start, pos - start);
            if (!line.empty()) renderer_.printLine(" " + line, color::DIM);
            if (pos == std::string::npos) break;
            start = pos + 1;
            ++n;
        }
    }
    waitKey("【Esc】【回车】返回");
}

// ---------------------------------------------------------------- 存档 --
// 存档槽选择窗关掉后，**不再在下面打任何确认文字**——`clear()` 一执行就是新的一屏，
// 旧版在清屏前又打了一行「已保存」，结果那行残留在屏幕别处（或挤到下一屏开头），
// 看上去就是「存档的时候最底下会出现不相干文字」。现在改成：
//   保存成功与否用一个只有「一行」的小提示窗，看完按回车立刻清屏。
void Game::saveMenu(Script& script, std::size_t& pc, Flags& flags) {
    int slot = slotPicker("选择存档槽");
    if (slot < 0) return;
    SaveData d;
    d.pc = pc;
    // 记下当前位置最近的 label 与偏移，读档时按 label 还原——改稿也不会串台词。
    std::size_t bestIdx = 0;
    bool bestFound = false;
    for (Script::labels_t::const_iterator it = script.labels.begin();
         it != script.labels.end(); ++it) {
        if (it->second > pc) continue;
        if (!bestFound || it->second > bestIdx) {
            bestIdx = it->second;
            d.resumeLabel = it->first;
            bestFound = true;
        }
    }
    if (bestFound) d.resumeOffset = pc - bestIdx;
    d.flags = flags;
    d.chapter = currentChapter_;
    d.sceneTitle = currentScene_;
    bool ok = saves_.save(slot, d);

    renderer_.clear();
    renderer_.newline();
    renderer_.newline();
    if (ok) {
        renderer_.printCentered("—— 已保存 ——", color::GREEN);
        renderer_.newline();
        renderer_.printCentered("存档 " + std::to_string(slot + 1) + "　" + saves_.brief(slot),
                                color::SILVER);
    } else {
        renderer_.printCentered("—— 存档失败 ——", color::RED);
        renderer_.newline();
        renderer_.printCentered("（存档目录可能没有写入权限）", color::GRAY);
    }
    renderer_.newline();
    renderer_.printCentered("〔回车〕继续", color::DIM);
    input::readKey();
    renderer_.clear();
}

bool Game::savePromptMenu() {
    static const char* opts[] = {"是", "不是"};
    std::size_t sel = 0;
    while (true) {
        renderer_.clear();
        renderer_.newline();
        renderer_.newline();
        renderer_.printCentered("是否进行存档？", color::GOLD);
        renderer_.newline();
        for (std::size_t i = 0; i < 2; ++i) {
            bool on = (i == sel);
            renderer_.printCentered(std::string(on ? "》 " : "　 ") + std::to_string(i + 1) +
                                        "、" + opts[i],
                                    on ? color::CYAN : color::SILVER);
        }
        renderer_.newline();
        renderer_.printLine("　　【方向键】选择　【回车】确认　【1】【2】快速选择", color::DIM);

        int k = input::readKey();
        if (k == input::KEY_UP) {
            if (sel > 0) sel--;
        } else if (k == input::KEY_DOWN) {
            if (sel + 1 < 2) sel++;
        } else if (k == input::KEY_ENTER || k == input::KEY_SPACE) {
            return sel == 0;
        } else if (k == '1') {
            return true;
        } else if (k == '2') {
            return false;
        }
    }
}

int Game::loadMenu() {
    int slot = slotPicker("读取存档");
    if (slot < 0) return -1;
    if (!saves_.exists(slot)) {
        renderer_.clear();
        renderer_.newline();
        renderer_.newline();
        renderer_.printCentered("—— 该存档为空 ——", color::RED);
        renderer_.newline();
        renderer_.printCentered("〔回车〕返回", color::DIM);
        input::readKey();
        renderer_.clear();
        return -1;
    }
    return slot;
}

// 存档槽选择窗。
// 只画「标题 + 框 + 6 行 + 提示」，之后不再追加任何内容——旧版在下面又打了
// 一段信息，读档界面下方就会出现不该出现的东西。
// 槽位里显示的摘要只来自 SaveManager::brief()，它在读存档时已经清洗过
// 控制字符 / BOM / CRLF / 换行 / 超长内容，这里再做一次宽度裁剪兜底。
int Game::slotPicker(const std::string& title) {
    std::size_t sel = 0;
    while (true) {
        renderer_.clear();
        renderer_.newline();
        renderer_.printCentered("—— " + title + " ——", color::GOLD);
        renderer_.newline();

        const int w = 68;
        renderer_.printLine(boxTop(w, ""));
        for (int i = 0; i < SAVE_SLOTS; ++i) {
            std::string sum = saves_.brief(i);
            sum = Renderer::clip(sum, 40);          // 兜底：一行最多 40 显示列
            bool on = (static_cast<std::size_t>(i) == sel);
            std::string line = " " + std::string(on ? "》" : "　") +
                               "存档" + std::to_string(i + 1) + "　";
            if (sum.empty()) line += "（空）";
            else line += sum;
            renderer_.printLine(boxMid(line, w,
                                       on ? color::CYAN : (sum.empty() ? color::GRAY
                                                                       : color::SILVER)));
        }
        renderer_.printLine(boxBottom(w));

        renderer_.newline();
        renderer_.printLine("　　【方向键】选择　【回车】确认　【Esc】取消", color::DIM);

        int k = input::readKey();
        if (k == input::KEY_UP) {
            if (sel > 0) sel--;
        } else if (k == input::KEY_DOWN) {
            if (sel + 1 < static_cast<std::size_t>(SAVE_SLOTS)) sel++;
        } else if (k == input::KEY_ENTER || k == input::KEY_SPACE) {
            renderer_.clear();   // 关窗即清屏，绝不让这一屏残留到下一屏
            return static_cast<int>(sel);
        } else if (k == input::KEY_ESC) {
            renderer_.clear();
            return -1;
        } else if (k >= '1' && k <= '9') {
            int idx = k - '1';
            if (idx < SAVE_SLOTS) {
                renderer_.clear();
                return idx;
            }
        }
    }
}

}  // namespace game
