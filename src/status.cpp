// ============================================================================
//  花至荼蘼 —— 角色档案实现
//  所有可变字段存进剧情 flag（key 前缀 "st_"/"eq"/"item_"…），随存档一起保存。
// ============================================================================
#include "status.h"

#include <cstdio>
#include <cstdlib>

#include "config.h"

namespace game {

// ---------------------------------------------------------------- flag 辅助 --
namespace stats {

std::string get(const Flags& f, const std::string& key, const std::string& defVal) {
    Flags::const_iterator it = f.find(key);
    return (it != f.end()) ? it->second : defVal;
}

void set(Flags& f, const std::string& key, const std::string& val) {
    f[key] = val;
}

int getInt(const Flags& f, const std::string& key, int defVal) {
    Flags::const_iterator it = f.find(key);
    if (it == f.end() || it->second.empty()) return defVal;
    return std::atoi(it->second.c_str());
}

void setInt(Flags& f, const std::string& key, int val) {
    f[key] = std::to_string(val);
}

std::vector<std::string> splitList(const std::string& s) {
    std::vector<std::string> out;
    std::string cur;
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '|') {
            out.push_back(cur);
            cur.clear();
        } else {
            cur += s[i];
        }
    }
    out.push_back(cur);
    return out;
}

std::string joinList(const std::vector<std::string>& v) {
    std::string s;
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (i) s += "|";
        s += v[i];
    }
    return s;
}

}  // namespace stats

// ---------------------------------------------------------------- 物品样式 --
// 按名字里的关键字给一个符号和颜色：只为「一眼能分类」，不参与任何数值计算。
// 符号一律用「宽度明确」的全角字符（■□▲△●○◎※），不用 ◇◆★☆▶ 这类宽度不定的
// 字形——它们在中文 cmd 里时而 1 列时而 2 列，会把右侧的「×1」列挤歪。
ItemInfo itemMark(const std::string& name) {
    ItemInfo info;
    info.name = name;

    struct Rule { const char* kw; const char* mark; const char* color; };
    static const Rule kRules[] = {
        {"剑",   "\xE2\x96\xA0", color::RED},     // ■ 剑
        {"护甲", "\xE2\x96\xA1", color::SILVER},  // □ 护甲
        {"面具", "\xE2\x96\xA1", color::SILVER},
        {"兜",   "\xE2\x96\xA1", color::SILVER},
        {"盾",   "\xE2\x96\xA1", color::SILVER},
        {"符",   "\xE2\x96\xB2", color::PURPLE},  // ▲ 符
        {"咒",   "\xE2\x96\xB2", color::PURPLE},
        {"药",   "\xE2\x97\x8F", color::GREEN},   // ● 药
        {"丹",   "\xE2\x97\x8F", color::GREEN},
        {"钥匙", "\xE2\x97\x8B", color::CYAN},    // ◎ 钥匙
        {"面包", "\xE2\x96\xA0", color::AMBER},   // ■ 食物
        {"便当", "\xE2\x96\xA0", color::AMBER},
        {"茶",   "\xE2\x96\xA0", color::AMBER},
        {"简",   "\xE2\x96\xA0", color::AMBER},   // ■ 竹简 / 文卷
        {"卷",   "\xE2\x96\xA0", color::AMBER},
        {"墨",   "\xE2\x96\xA0", color::AMBER},
        {"水",   "\xE2\x96\xB3", color::WHITE},   // △
        {"牛奶", "\xE2\x96\xB3", color::WHITE},
        {"毛",   "\xE2\x97\x8F", color::SILVER},  // ● 毛
        {"绒",   "\xE2\x97\x8F", color::TEAL},
        {"棉",   "\xE2\x97\x8F", color::WHITE},
        {"玉",   "\xE2\x97\x8E", color::GOLD},    // ◎ 玉 / 灯
        {"灯",   "\xE2\x97\x8E", color::GOLD},
        {"佩",   "\xE2\x97\x8E", color::GOLD},
        {"皮",   "\xE2\x80\xBB", color::GRAY},    // ※ 杂项
        {"尘",   "\xE2\x80\xBB", color::GRAY},
        {"灰",   "\xE2\x80\xBB", color::GRAY},
        {"絮",   "\xE2\x80\xBB", color::GRAY},
    };
    for (std::size_t r = 0; r < sizeof(kRules) / sizeof(kRules[0]); ++r) {
        if (name.find(kRules[r].kw) != std::string::npos) {
            info.mark = kRules[r].mark;
            info.color = kRules[r].color;
            return info;
        }
    }
    info.mark = "\xE2\x80\xBB";   // ※ 默认
    info.color = color::SILVER;
    return info;
}

// 开场就带在身上的重要物品（全部按「未装备的候选」登记进背包和装备槽）
std::vector<std::string> defaultWarehouse() {
    std::vector<std::string> v;
    v.push_back("公文一摞");
    v.push_back("未抄完的文卷");
    v.push_back("双螭玉佩");
    v.push_back("一团灰色的绒毛");
    return v;
}

// ---------------------------------------------------------------- Character --
void Character::configure(const std::string& id, const std::string& name,
                          const std::string& race, const std::string& lv,
                          int hpMax, int mpMax, const std::string& desc,
                          const std::string& joinFlag) {
    id_ = id;
    name_ = name;
    race_ = race;
    lv_ = lv;
    hpMax_ = hpMax;
    mpMax_ = mpMax;
    desc_ = desc;
    joinFlag_ = joinFlag;
    emblem_.mark.clear();            // 默认名字前不加任何记号
    emblem_.color = color::SILVER;
}

bool Character::inParty(const Flags& f) const {
    if (joinFlag_.empty()) return true;
    return stats::get(f, joinFlag_) == "1";
}

namespace {

std::string keyOf(const std::string& prefix, const std::string& id) {
    return prefix + id;
}

}  // namespace

std::string Character::level(const Flags& f) const {
    std::string v = stats::get(f, keyOf("lv_", id_));
    return v.empty() ? lv_ : v;
}

int Character::hp(const Flags& f) const {
    int v = stats::getInt(f, keyOf("hp_", id_), -1);
    return (v < 0) ? hpMax_ : v;
}

int Character::hpMax(const Flags& f) const {
    return stats::getInt(f, keyOf("hpm_", id_), hpMax_);
}

int Character::mp(const Flags& f) const {
    int v = stats::getInt(f, keyOf("mp_", id_), -1);
    return (v < 0) ? mpMax_ : v;
}

int Character::mpMax(const Flags& f) const {
    return stats::getInt(f, keyOf("mpm_", id_), mpMax_);
}

int Character::tp(const Flags& f) const {
    return stats::getInt(f, keyOf("tp_", id_), 0);
}

std::vector<std::string> Character::skillsIn(const Flags& f, std::size_t tab) const {
    std::vector<std::string> out;
    if (tab >= skillTabs_.size()) return out;
    std::string learnt = stats::get(f, "sk_" + id_ + "_" + std::to_string(tab));
    if (learnt.empty()) return out;   // 未习得该分支 -> 这一栏空着
    std::vector<std::string> names = stats::splitList(learnt);
    for (std::size_t i = 0; i < names.size(); ++i) {
        if (!names[i].empty()) out.push_back(names[i]);
    }
    return out;
}

std::string Character::equipped(const Flags& f, std::size_t slot) const {
    if (slot >= equipSlots_.size()) return "";
    std::string key = stats::get(f, "eq_" + id_ + "_" + std::to_string(slot));
    if (key.empty()) return "";
    return stats::get(f, "item_" + key);
}

std::vector<std::string> Character::slotCandidates(const Flags& f,
                                                  std::size_t slot) const {
    std::vector<std::string> out;
    if (slot >= equipSlots_.size()) return out;
    std::string raw = stats::get(f, "eqo_" + id_ + "_" + std::to_string(slot));
    std::vector<std::string> keys = stats::splitList(raw);
    // 首项必须是「最强装备」（空字符串表示该槽没有可用的东西）
    for (std::size_t i = 0; i < keys.size(); ++i) {
        std::string disp = stats::get(f, "item_" + keys[i]);
        out.push_back(disp);
    }
    return out;
}

// 「最强装备」：把每个槽位的候选按「该槽的基础加成」从高到低排序，
// 再把每个槽位切换成排在第一位的那件。纯按固定加成表排，结果稳定、可反复按。
void Character::autoBestEquip(Flags& f) const {
    for (std::size_t s = 0; s < equipSlots_.size(); ++s) {
        std::string orderKey = "eqo_" + id_ + "_" + std::to_string(s);
        std::vector<std::string> cand = stats::splitList(stats::get(f, orderKey));
        if (cand.empty()) continue;

        std::vector<std::pair<int, std::string>> scored;
        for (std::size_t i = 0; i < cand.size(); ++i) {
            int power = stats::getInt(f, "eqp_" + cand[i], 0);   // 该装备的「强度」
            scored.push_back(std::make_pair(power, cand[i]));
        }
        // 手写插入排序（C++11 无 std::stable_sort 的严格要求，本实现保持稳定）
        for (std::size_t i = 1; i < scored.size(); ++i) {
            std::pair<int, std::string> cur = scored[i];
            std::size_t j = i;
            while (j > 0 && scored[j - 1].first < cur.first) {
                scored[j] = scored[j - 1];
                --j;
            }
            scored[j] = cur;
        }
        // 最强的那件装到身上；空字符串表示「（空）」，也允许它排第一
        stats::set(f, "eq_" + id_ + "_" + std::to_string(s), scored[0].second);
    }
}

void Character::clearEquip(Flags& f) const {
    for (std::size_t s = 0; s < equipSlots_.size(); ++s) {
        stats::set(f, "eq_" + id_ + "_" + std::to_string(s), "");
    }
}

int Character::statValue(const Flags& f, std::size_t idx) const {
    if (idx >= stats_.size()) return 0;
    int base = stats_[idx].second;
    const std::string& sname = stats_[idx].first;
    // 装备加成：eqb_<角色>_<槽位>，值是「槽位主要属性 +数值」
    for (std::size_t s = 0; s < equipSlots_.size(); ++s) {
        if (equipSlots_[s].baseStat != sname) continue;
        std::string eqKey = stats::get(f, "eq_" + id_ + "_" + std::to_string(s));
        if (eqKey.empty()) continue;
        base += stats::getInt(f, "eqb_" + id_ + "_" + std::to_string(s) + "_" + eqKey, 0);
    }
    return base;
}

long long Character::exp(const Flags& f) const {
    Flags::const_iterator it = f.find("exp_" + id_);
    if (it == f.end() || it->second.empty()) return 0;
    long long v = 0;
    for (std::size_t i = 0; i < it->second.size(); ++i) {
        if (it->second[i] < '0' || it->second[i] > '9') break;
        v = v * 10 + (it->second[i] - '0');
    }
    return v;
}

// ---------------------------------------------------------------- Roster ----
Roster::Roster() {
    // 入队状态存在剧情 flag 里（随存档保存）：
    //   party_sibuxiang = 1  —— 四不相一开始就在队里
    //   party_sibuyu / party_shangsi —— 目前都是 0（还没入队，所以菜单里看不到）
    // 以后剧本里让谁入队，只要在对话后 setFlag("party_xxx", "1") 即可。

    // ---- 四不相：白毛麒麟，基层小吏 ----
    Character sx;
    sx.configure("sibuxiang", "四不相", "麒麟", "Lv1", 96, 40,
                 "一只总在迟到边缘的基层小吏。\n"
                 "毛毛乱糟糟的，爪尖却总在发抖。\n"
                 "每天最大的愿望——只是睡饱。",
                 "party_sibuxiang");
    sx.addStat("攻击力", 12)
      .addStat("防御力", 9)
      .addStat("法术攻击", 8)
      .addStat("法术防御", 8)
      .addStat("敏捷", 22)
      .addStat("幸运", 3)
      .setExpToNext(120);
    // 技能分支：法术 / 灵力（暂时不填内容）
    sx.addSkillTab("法术").addSkillTab("灵力");
    sx.addEquipSlot("武器").addEquipSlot("盾牌").addEquipSlot("头部")
      .addEquipSlot("身体").addEquipSlot("饰品");
    characters_.push_back(sx);

    // ---- 四不遇：灰麒麟，典狱长，面具侧戴（尚未入队） ----
    Character sy;
    sy.configure("sibuyu", "四不遇", "麒麟", "Lv9", 180, 60,
                 "总在夜半站在床边的灰白身影。\n"
                 "面具侧戴，声音轻得像在哄谁。\n"
                 "仿佛在等一个等了很久的人。",
                 "party_sibuyu");
    sy.addStat("攻击力", 38)
      .addStat("防御力", 30)
      .addStat("法术攻击", 26)
      .addStat("法术防御", 24)
      .addStat("敏捷", 20)
      .addStat("幸运", 2)
      .setExpToNext(600);
    sy.addSkillTab("法术").addSkillTab("灵力");
    sy.addEquipSlot("武器").addEquipSlot("盾牌").addEquipSlot("头部")
      .addEquipSlot("身体").addEquipSlot("饰品");
    characters_.push_back(sy);

    // ---- 上司：文书阁里那位笑眯眯的家伙（尚未入队） ----
    Character ss;
    ss.configure("shangsi", "上司", "狐妖", "Lv15", 240, 80,
                 "文书阁里那位笑眯眯的家伙。\n"
                 "手里总捏着一卷竹简，从不紧不慢。\n"
                 "你永远猜不到他在想什么。",
                 "party_shangsi");
    ss.addStat("攻击力", 44)
      .addStat("防御力", 40)
      .addStat("法术攻击", 50)
      .addStat("法术防御", 46)
      .addStat("敏捷", 28)
      .addStat("幸运", 6)
      .setExpToNext(1200);
    ss.addSkillTab("法术").addSkillTab("灵力");
    ss.addEquipSlot("武器").addEquipSlot("盾牌").addEquipSlot("头部")
      .addEquipSlot("身体").addEquipSlot("饰品");
    characters_.push_back(ss);
}

int Roster::indexOf(const std::string& id) const {
    for (std::size_t i = 0; i < characters_.size(); ++i) {
        if (characters_[i].id() == id) return static_cast<int>(i);
    }
    return -1;
}

std::vector<std::size_t> Roster::party(const Flags& f) const {
    std::vector<std::size_t> out;
    for (std::size_t i = 0; i < characters_.size(); ++i) {
        if (characters_[i].inParty(f)) out.push_back(i);
    }
    return out;
}

// ---------------------------------------------------------------- 物品数量 --
// 物品的「种类」= flag  item_<id> = 显示名；
// 物品的「数量」= flag  cnt_<id>  = 数字（缺省视为 1，兼容旧存档）。
// 拿到已经有的东西时只把数量 +1，物品栏里不再多出一行。
namespace inventory {

namespace {
// 由显示名找到它的 id（item_<id> 的值 == 显示名）；找不到返回 ""。
std::string idOf(const Flags& f, const std::string& name) {
    for (Flags::const_iterator it = f.begin(); it != f.end(); ++it) {
        if (it->first.compare(0, 5, "item_") != 0) continue;
        if (it->second == name) return it->first.substr(5);
    }
    return "";
}
}  // namespace

int count(const Flags& f, const std::string& name) {
    std::string id = idOf(f, name);
    if (id.empty()) return 0;
    int n = stats::getInt(f, "cnt_" + id, 1);
    return n < 1 ? 1 : n;
}

bool add(Flags& f, const std::string& name, int n) {
    if (name.empty()) return false;
    if (n < 1) n = 1;
    std::string id = idOf(f, name);
    if (id.empty()) {
        // 第一次拿到：按名字生成一个稳定的 id
        std::string base = "i";
        for (std::size_t i = 0; i < name.size(); ++i) {
            unsigned char c = static_cast<unsigned char>(name[i]);
            if (c < 0x80) {
                if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                    base += static_cast<char>(c);
            } else if ((c & 0xC0) != 0x80) {
                // 只用首字节区分不同的汉字，够用且稳定
                char buf[8];
                std::snprintf(buf, sizeof(buf), "x%02x", static_cast<unsigned>(c));
                base += buf;
            }
        }
        // 防撞：已经存在同 id 的别的东西就加个后缀
        std::string id2 = base;
        for (int k = 2; f.find("item_" + id2) != f.end(); ++k)
            id2 = base + "_" + std::to_string(k);
        stats::set(f, "item_" + id2, name);
        stats::setInt(f, "cnt_" + id2, n);
        return true;
    }
    stats::setInt(f, "cnt_" + id, count(f, name) + n);
    return false;
}

std::vector<std::pair<std::string, int>> all(const Flags& f) {
    std::vector<std::pair<std::string, int>> out;
    for (Flags::const_iterator it = f.begin(); it != f.end(); ++it) {
        if (it->first.compare(0, 5, "item_") != 0) continue;
        if (it->second.empty()) continue;
        out.push_back(std::make_pair(it->second, count(f, it->second)));
    }
    return out;
}

std::size_t total(const Flags& f) {
    return all(f).size();
}

}  // namespace inventory

}  // namespace game
