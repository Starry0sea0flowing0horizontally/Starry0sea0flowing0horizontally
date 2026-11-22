// ============================================================================
//  花至荼蘼 —— 角色档案（等级 / 种族 / HP·MP·TP / 技能 / 装备 / 状态 / 持有物）
//  版式参考 RPG Maker 的菜单窗，只显示文字与方框，不显示人物头图。
//
//  所有可变字段一律落到剧情 flag 里（key 前缀 "st_"/"item_"/"eq*"/"lv_"…），
//  这样存档 / 读档会自动带上，不需要改存档格式。
// ============================================================================
#pragma once

#include <string>
#include <utility>
#include <vector>

#include "script.h"

namespace game {

// 一件装备（或一个持有物）的显示样式：名称前的一个符号 + 颜色
struct ItemInfo {
    std::string name;    // 显示名
    std::string mark;    // 名称前的一个符号（◇/◆/□/■/·/★/● …）
    std::string color;   // 名称颜色（ANSI 转义，取自 config.h color::）
};

// 装备槽
struct EquipSlot {
    std::string label;       // 槽位名（武器 / 盾牌 / 头部 / 身体 / 饰品）
    std::string baseStat;    // 该槽位主要影响的属性名（如「攻击力」）
};

// 角色档案
class Character {
public:
    Character() {}

    // ---- 不变的部分 ----
    // joinFlag：入队 flag 的名字。该 flag 值为 "1" 才算入队；
    // 没入队的角色不出现在任何菜单里（队伍窗 / 选角色 / 整队）。
    void configure(const std::string& id, const std::string& name, const std::string& race,
                   const std::string& lv, int hpMax, int mpMax, const std::string& desc,
                   const std::string& joinFlag = "");

    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& race() const { return race_; }   // 种族（麒麟 / 月兔 / 狐妖 …）
    const std::string& desc() const { return desc_; }   // 状态页底部的一两句介绍
    const std::string& joinFlag() const { return joinFlag_; }
    // 是否已入队（不设 joinFlag 的角色视为一直在队里）
    bool inParty(const Flags& f) const;
    // 名字前的一个符号 + 颜色（**默认留空 = 不加任何记号**，等需要时再配）
    const ItemInfo& emblem() const { return emblem_; }
    Character& setEmblem(const std::string& mark, const std::string& color) {
        emblem_.mark = mark;
        emblem_.color = color;
        return *this;
    }

    // ---- 可变的部分（全部从 flag 读取，随存档保存）----
    std::string level(const Flags& f) const;   // "Lv1"
    int  hp(const Flags& f) const;
    int  hpMax(const Flags& f) const;
    int  mp(const Flags& f) const;
    int  mpMax(const Flags& f) const;
    int  tp(const Flags& f) const;

    // ---- 技能：两个分支（法术 / 灵力），四不相暂时留空 ----
    const std::vector<std::string>& skillTabs() const { return skillTabs_; }
    std::vector<std::string> skillsIn(const Flags& f, std::size_t tab) const;

    // ---- 装备 ----
    const std::vector<EquipSlot>& equipSlots() const { return equipSlots_; }
    std::string equipped(const Flags& f, std::size_t slot) const;   // "" 表示空
    // 某槽位的候选列表（按当前排序，首项为该槽「最强装备」；"" 代表「（空）」）
    std::vector<std::string> slotCandidates(const Flags& f, std::size_t slot) const;
    // 最强装备：把每个槽位切换成该槽最强的可用装备
    void autoBestEquip(Flags& f) const;
    // 清空全部装备
    void clearEquip(Flags& f) const;

    // ---- 属性 ----
    const std::vector<std::pair<std::string, int>>& stats() const { return stats_; }
    int statValue(const Flags& f, std::size_t idx) const;   // 基础值 + 装备加成
    bool statBoosted(const Flags& f, std::size_t idx) const {
        return statValue(f, idx) > stats_[idx].second;
    }
    long long exp(const Flags& f) const;
    long long expToNext() const { return expToNext_; }

    // ---- 写作辅助（建表用）----
    Character& addStat(const std::string& name, int base) {
        stats_.push_back(std::make_pair(name, base));
        return *this;
    }
    Character& addSkillTab(const std::string& name) {
        skillTabs_.push_back(name);
        return *this;
    }
    Character& addEquipSlot(const std::string& label) {
        EquipSlot s;
        s.label = label;
        equipSlots_.push_back(s);
        return *this;
    }
    Character& setExpToNext(long long v) { expToNext_ = v; return *this; }

private:
    std::string id_;
    std::string name_;
    std::string race_;
    std::string lv_;
    int hpMax_ = 100;
    int mpMax_ = 100;
    std::string desc_;
    std::string joinFlag_;
    ItemInfo emblem_;

    std::vector<std::string> skillTabs_;
    std::vector<EquipSlot> equipSlots_;
    std::vector<std::pair<std::string, int>> stats_;
    long long expToNext_ = 0;
};

// ---- flag 读写辅助（全部落在 Flags 里，随存档保存）----
namespace stats {
std::string get(const Flags& f, const std::string& key, const std::string& defVal = "");
void set(Flags& f, const std::string& key, const std::string& val);
int  getInt(const Flags& f, const std::string& key, int defVal = 0);
void setInt(Flags& f, const std::string& key, int val);

std::vector<std::string> splitList(const std::string& s);          // 按 '|' 拆分
std::string joinList(const std::vector<std::string>& v);           // 用 '|' 合并
}  // namespace stats

// 按名字给一件物品一个符号 + 颜色（纯显示，不参与数值计算）
ItemInfo itemMark(const std::string& name);

// 玩家「重要物品」箱：开场自带几样，之后由剧情 / 互动往里加
std::vector<std::string> defaultWarehouse();

// 角色档案表（四不相 / 四不遇 / 上司）
class Roster {
public:
    Roster();
    const std::vector<Character>& all() const { return characters_; }
    int indexOf(const std::string& id) const;
    // 已入队的角色下标（没入队的一律不返回，菜单里也就不会出现）
    std::vector<std::size_t> party(const Flags& f) const;

private:
    std::vector<Character> characters_;
};

// 物品栏里的「同种物品合并」：拿到已有物品时只把数量 +1，不再多列一行。
namespace inventory {
int count(const Flags& f, const std::string& name);         // 0 表示没有
// 增加 n 个；返回 true 表示这是**第一次**获得（调用方据此决定是否弹「获得道具」）
bool add(Flags& f, const std::string& name, int n = 1);
std::vector<std::pair<std::string, int>> all(const Flags& f);   // {名字, 数量}
std::size_t total(const Flags& f);                             // 种类数
// 新局开场：擦掉上一局的道具键，返回开场持有物（保证物品栏不串档）
std::vector<std::string> freshDefaults(Flags& f);
}  // namespace inventory

}  // namespace game
