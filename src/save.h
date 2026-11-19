// ============================================================================
//  花至荼蘼 —— 多槽存档系统接口
// ============================================================================
#pragma once

#include <string>

#include "script.h"  // Flags

namespace game {

// 一份存档的内容
struct SaveData {
    std::size_t pc = 0;            // 剧情节点位置
    Flags flags;                   // 全局变量（选择分支 / 关键 flag）
    std::string chapter = "序章";  // 章节名
    std::string sceneTitle = "";   // 当前场景名（用于菜单摘要）
    std::string timestamp = "";    // 存档时间
};

class SaveManager {
public:
    explicit SaveManager(const std::string& dir);

    bool ensureDir();              // 确保存档目录存在
    std::string pathFor(int slot) const;

    bool save(int slot, const SaveData& data);
    bool load(int slot, SaveData& out) const;
    bool exists(int slot) const;
    std::string summary(int slot) const;  // 一行摘要（章节 · 场景 · 时间），空槽返回 ""
    std::string brief(int slot) const;    // 只含 章节 · 场景（不含时间，省列宽）
    int latestSlot() const;               // 最近存档槽，-1 表示无

private:
    std::string dir_;
};

}  // namespace game

// 返回当前可执行文件所在目录（含尾部分隔符），不受启动工作目录影响。
// 用于把存档等相对路径锚定到 exe 目录，避免从其它目录启动时写丢。
namespace game {
std::string exeDirectory();
}
