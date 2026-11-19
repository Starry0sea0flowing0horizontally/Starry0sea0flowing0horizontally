// ============================================================================
//  花至荼蘼 —— 游戏主控接口
// ============================================================================
#pragma once

#include <map>
#include <string>
#include <vector>

#include "config.h"
#include "map.h"
#include "renderer.h"
#include "save.h"
#include "script.h"
#include "status.h"

namespace game {

class Game {
public:
    Game() : saves_(exeDirectory() + SAVE_DIR) {}
    void run();

private:
    // 文本节点处理完后的推进动作
    enum class AfterAction { Next, Stay, Exit };
    // 系统菜单动作
    enum class MenuAct { Continue, Reload, Title, Quit };

    Renderer renderer_;
    SaveManager saves_;
    Map map_;
    Roster roster_;                             // 角色档案（等级/种族/HP/MP/TP/属性）
    bool quitting_ = false;

    std::string currentChapter_ = "序章";
    std::string currentScene_ = "";

    // 当前正文（对白/旁白）已打印的行数，用于下一句在同一位置顶替显示
    int bodyLines_ = 0;

    // ---- 生命周期 ----
    void titleLoop();
    void playChapter(const std::string& chapter);
    void playFromSave(int slot);
    void skipSavePrompt(Script& script, std::size_t& pc);  // 读档跳过「是否存档」提示

    Script buildChapter(const std::string& chapter);
    void runScript(Script& script, std::size_t& pc, Flags& flags);
    // 运行一个 Map 节点；返回 true 表示应退出 runScript（回标题）
    bool runMap(const Node& n, Script& script, std::size_t& pc, Flags& flags);

    // 剧情档案初始化 / 归一化（把新增物品、装备槽补进存档 flag，幂等）
    void initProfile(Flags& flags, bool fresh);
    // 核对「当前角色」是否在队里（没入队就换成四不相）
    void syncCurrentChar(Flags& flags);

    // 节点渲染
    void renderDialogue(const Node& n);
    void renderNarration(const Node& n);
    void renderScene(const Node& n);
    void renderEnd(const Node& n);
    void renderChapterEnd(const Node& n);
    void renderItem(const Node& n, Flags& flags);   // 获得道具提示
    int  renderChoice(const Node& n);               // 返回选中项下标

    // 交互
    AfterAction afterText(Script& script, std::size_t& pc, Flags& flags);

    // 菜单
    void settingsMenu();                                    // 「设置」
    // 系统菜单（照 RPG 版式：左列表 + 右队伍状态）。
    // 返回 Title/Quit 表示要离开本章；Reload 表示已就地读完档，调用方重新进入循环。
    MenuAct systemMenu(Script& script, std::size_t& pc, Flags& flags);
    void itemsMenu(Flags& flags, std::size_t charIdx);      // 物品（四栏分类）
    void skillsMenu(Flags& flags, std::size_t charIdx);     // 技能（法术 / 灵力）
    void equipMenu(Flags& flags, std::size_t charIdx);      // 装备（最强装备 / 清空 / 结束）
    void statusMenu(Flags& flags, std::size_t charIdx);     // 状态
    void orderMenu(Flags& flags);                           // 整队（只列已入队的人）
    bool chooseCharacter(Flags& flags, const std::string& title);  // 返回选中角色（写 flags）
    std::string currentChar(const Flags& flags) const;
    // 名字前的记号：**目前一律不加**（两个函数都返回空串，只留作将来扩展点）
    std::string emblemName(const Character& c) const;
    std::string emblemColor(const Character& c) const;

    // 存档
    void saveMenu(Script& script, std::size_t& pc, Flags& flags);
    bool savePromptMenu();                    // 返回 true=是（存档），false=不是
    int  loadMenu();                          // 返回槽位，-1 取消
    int  slotPicker(const std::string& title);  // 返回槽位，-1 取消
    void waitKey(const std::string& hint = "〔回车〕返回");
};

}  // namespace game
