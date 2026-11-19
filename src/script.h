// ============================================================================
//  花至荼蘼 —— 剧情脚本数据模型与流式构建器（header-only）
// ============================================================================
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

namespace game {

// ---- 节点类型 ----
enum class NodeType {
    Dialogue,    // 对白：speaker 说话，text 内容
    Narration,   // 旁白 / 心理描写
    Scene,       // 场景切换：清屏 + 标题 + 配图 id（预留）
    Clear,       // 仅清屏
    Choice,      // 选项分支：choices -> 各自 target label
    Jump,        // 无条件跳转到 target label
    SetFlag,     // 设置变量 flag = value
    Branch,      // 条件跳转：flag == value -> target，否则顺序下一节点
    Map,         // 可走动探索地图：走到地点并按回车 -> 跳转对应 label
    SavePrompt,  // 存档点：提示「是否进行存档？」1、是 -> 存档界面；2、不是
    Item,        // 获得道具：弹出「你获得了[xxx]」，按回车继续
    End,         // 结局画面
    ChapterEnd,  // 章节结束画面（回标题 / 进入下一章）
};

struct ChoiceOption {
    std::string text;    // 选项显示文本
    std::string target;  // 目标 label
};

// 地图出口：玩家走到 location 地点并按回车时，跳转到 label
struct MapExit {
    std::string location;  // 地点 id（如 "宿舍"）
    std::string label;     // 目标 label
};

// ---------------------------------------------------------------- 地图角色 --
// 地图上**自己会动**的角色（如深夜宿舍里的四不遇）：
// 他一进地图就按 steps 里的脚本行动——走过去、说台词、停下、再走、消失。
// 玩家可以照常在旁边走动；角色每一步都会绕开玩家，不会踩到人身上。
//
// step 的 kind：
//   "move"       走到 (x, y)，一格一格走，每格停一下（看得出在动）
//   "pmove"      让**玩家**自己走到 (x, y)（撞上四不遇那种不能操作的演出）
//   "narration"  出一句旁白（屏幕上按正文显示，回车继续）
//   "dialogue"   出一句对白（speaker / text）
//   "wait"       原地停一会儿（毫秒）
//   "disappear"  从地图上消失
//   "exit"       角色脚本结束（字幕走完，回到正常地图）
struct MapActorStep {
    std::string kind = "move";
    int x = 0;
    int y = 0;
    int ms = 400;
    std::string speaker;
    std::string text;
    std::string color;
};

// 一个地图角色
struct MapActor {
    std::string id;                  // 角色 id（"sibuyu"）
    std::string name;                // 显示名（"四不遇" / 地图上那一个字）
    std::string glyph;               // 地图上的一个字（默认取 name 的第一个字）
    std::string color;               // 显示颜色
    int x = 0;                       // 出场位置
    int y = 0;
    bool autoRun = true;             // 一进地图就自己开始演
    // 不画在地图上：只负责把一串旁白 / 对白挂到地图下面（回廊开场、文书阁开场）。
    // 用它就能让「一整段文字」在**地图下方**逐句出现，玩家一边看地图一边按空格。
    bool unseen = false;
    std::vector<MapActorStep> steps;
};

struct Node {
    NodeType type = NodeType::Narration;

    std::string speaker;     // 说话人名字
    std::string spkColor;    // 说话人名字颜色
    std::string text;        // 文本内容
    std::string color;       // 文本颜色
    std::string sceneId;     // 场景/配图 id（预留）
    std::string sceneTitle;  // 场景标题
    std::string target;      // Jump/Branch 的目标 label
    std::string label;       // 本节点 label（供调试/文档，跳转由 Script::labels 维护）
    std::vector<ChoiceOption> choices;
    std::string flag;        // SetFlag / Branch 用
    std::string value;       // SetFlag 的值 / Branch 的期望值
    std::string itemDesc;    // Item 节点：道具说明文字
    bool isBad = false;      // 是否为坏结局

    // ---- Map 节点 ----
    std::string mapId = "";          // 地图 id（如 "tianting"）
    int mapStartX = 0;               // 玩家出生点（列）
    int mapStartY = 0;               // 玩家出生点（行）
    std::vector<MapExit> mapExits;   // 地点 -> label 出口表
    std::vector<MapActor> mapActors; // 地图上自己会动的角色（可空）
};

struct Script {
    using labels_t = std::unordered_map<std::string, std::size_t>;
    std::vector<Node> nodes;
    labels_t labels;
};

// 全局变量表：记录玩家选择、章节关键 flag（如「温茶」分支）等
using Flags = std::unordered_map<std::string, std::string>;

// ---- 流式构建器：让剧情代码接近剧本 ----
class ScriptBuilder {
public:
    // 记录一个跳转标签，指向「下一个将被添加的节点」
    ScriptBuilder& label(const std::string& name) {
        script.labels[name] = script.nodes.size();
        return *this;
    }

    // 对白
    ScriptBuilder& say(const std::string& speaker, const std::string& text,
                       const char* spkColor = nullptr, const char* txtColor = nullptr) {
        Node n;
        n.type = NodeType::Dialogue;
        n.speaker = speaker;
        n.text = text;
        if (spkColor) n.spkColor = spkColor;
        if (txtColor) n.color = txtColor;
        script.nodes.push_back(std::move(n));
        return *this;
    }

    // 旁白 / 心理
    ScriptBuilder& narr(const std::string& text, const char* color = nullptr) {
        Node n;
        n.type = NodeType::Narration;
        n.text = text;
        if (color) n.color = color;
        script.nodes.push_back(std::move(n));
        return *this;
    }

    // 场景切换
    ScriptBuilder& scene(const std::string& title, const std::string& imageId = "") {
        Node n;
        n.type = NodeType::Scene;
        n.sceneTitle = title;
        n.sceneId = imageId;
        script.nodes.push_back(std::move(n));
        return *this;
    }

    // 仅清屏
    ScriptBuilder& cls() {
        Node n;
        n.type = NodeType::Clear;
        script.nodes.push_back(std::move(n));
        return *this;
    }

    // 选项分支
    ScriptBuilder& choice(const std::string& prompt, std::vector<ChoiceOption> options) {
        Node n;
        n.type = NodeType::Choice;
        n.text = prompt;
        n.choices = std::move(options);
        script.nodes.push_back(std::move(n));
        return *this;
    }

    // 无条件跳转
    ScriptBuilder& jump(const std::string& target) {
        Node n;
        n.type = NodeType::Jump;
        n.target = target;
        script.nodes.push_back(std::move(n));
        return *this;
    }

    // 设置变量
    ScriptBuilder& setFlag(const std::string& flag, const std::string& value) {
        Node n;
        n.type = NodeType::SetFlag;
        n.flag = flag;
        n.value = value;
        script.nodes.push_back(std::move(n));
        return *this;
    }

    // 条件跳转
    ScriptBuilder& branch(const std::string& flag, const std::string& expect,
                          const std::string& target) {
        Node n;
        n.type = NodeType::Branch;
        n.flag = flag;
        n.value = expect;
        n.target = target;
        script.nodes.push_back(std::move(n));
        return *this;
    }

    // 可走动地图：玩家从 (startX,startY) 出发，走到 exits 中某地点并按回车时跳转
    ScriptBuilder& map(const std::string& mapId, int startX, int startY,
                       std::vector<MapExit> exits) {
        Node n;
        n.type = NodeType::Map;
        n.mapId = mapId;
        n.mapStartX = startX;
        n.mapStartY = startY;
        n.mapExits = std::move(exits);
        script.nodes.push_back(std::move(n));
        return *this;
    }

    // 带「会自己动的角色」的地图（深夜宿舍里的四不遇就是这种）。
    // exits 是玩家的出口表，actors 是各路 NPC 的出场位置 + 行动脚本。
    ScriptBuilder& mapWithActors(const std::string& mapId, int startX, int startY,
                                 std::vector<MapExit> exits,
                                 std::vector<MapActor> actors) {
        Node n;
        n.type = NodeType::Map;
        n.mapId = mapId;
        n.mapStartX = startX;
        n.mapStartY = startY;
        n.mapExits = std::move(exits);
        n.mapActors = std::move(actors);
        script.nodes.push_back(std::move(n));
        return *this;
    }

    // ---- 地图角色的行动脚本（写法接近剧本）----
    // 说明：这些函数返回 MapActorStep，供 mapWithActors 的花括号列表使用。
    static MapActorStep actMove(int x, int y, int ms = 260) {
        MapActorStep s;
        s.kind = "move";
        s.x = x;
        s.y = y;
        s.ms = ms;
        return s;
    }
    // 让玩家自己走到 (x,y)：演出期间玩家不能操作，脚自己动
    static MapActorStep actPMove(int x, int y, int ms = 110) {
        MapActorStep s;
        s.kind = "pmove";
        s.x = x;
        s.y = y;
        s.ms = ms;
        return s;
    }
    static MapActorStep actWait(int ms) {
        MapActorStep s;
        s.kind = "wait";
        s.ms = ms;
        return s;
    }
    static MapActorStep actNarr(const std::string& text, const char* color = nullptr) {
        MapActorStep s;
        s.kind = "narration";
        s.text = text;
        if (color) s.color = color;
        return s;
    }
    static MapActorStep actSay(const std::string& speaker, const std::string& text,
                               const char* color = nullptr) {
        MapActorStep s;
        s.kind = "dialogue";
        s.speaker = speaker;
        s.text = text;
        if (color) s.color = color;
        return s;
    }
    static MapActorStep actDisappear() {
        MapActorStep s;
        s.kind = "disappear";
        return s;
    }
    static MapActorStep actFinish() {
        MapActorStep s;
        s.kind = "exit";
        return s;
    }

    // 存档点：清屏后提示「是否进行存档？」，选「是」进入存档界面，「不是」继续
    ScriptBuilder& savePrompt() {
        Node n;
        n.type = NodeType::SavePrompt;
        script.nodes.push_back(std::move(n));
        return *this;
    }

    // 获得道具：text 为道具名，desc 为一句说明（可空）
    ScriptBuilder& item(const std::string& name, const std::string& desc = "") {
        Node n;
        n.type = NodeType::Item;
        n.text = name;
        n.itemDesc = desc;
        script.nodes.push_back(std::move(n));
        return *this;
    }

    // 结局
    ScriptBuilder& end(const std::string& title, const std::string& text, bool isBad = false) {
        Node n;
        n.type = NodeType::End;
        n.sceneTitle = title;
        n.text = text;
        n.isBad = isBad;
        script.nodes.push_back(std::move(n));
        return *this;
    }

    // 章节结束
    ScriptBuilder& chapterEnd(const std::string& title, const std::string& text) {
        Node n;
        n.type = NodeType::ChapterEnd;
        n.sceneTitle = title;
        n.text = text;
        script.nodes.push_back(std::move(n));
        return *this;
    }

    Script done() { return std::move(script); }

private:
    Script script;
};

}  // namespace game
