// ============================================================================
//  花至荼蘼 —— 多槽存档系统实现（纯文本格式，无第三方依赖）
// ============================================================================
#include "save.h"
#include "config.h"

#include <fstream>
#include <ctime>
#include <cerrno>

#ifdef _WIN32
#include <direct.h>   // _mkdir
#include <windows.h>  // GetFileAttributesA
#else
#include <sys/stat.h> // mkdir
#endif

namespace game {

std::string exeDirectory() {
#ifdef _WIN32
    char buf[MAX_PATH];
    DWORD n = GetModuleFileNameA(nullptr, buf, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) {
        // 获取失败时退回当前目录
        return std::string("./");
    }
    std::string path(buf, n);
    std::string::size_type pos = path.find_last_of("\\/");
    if (pos != std::string::npos) {
        path = path.substr(0, pos + 1);   // 保留尾部分隔符
    } else {
        path = "./";
    }
    return path;
#else
    return std::string("./");
#endif
}

namespace {

std::string nowString() {
    std::time_t t = std::time(nullptr);
    struct tm* tm_buf = std::localtime(&t);
    char buf[64];
    if (tm_buf) {
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_buf);
    } else {
        buf[0] = '?';
        buf[1] = '\0';
    }
    return std::string(buf);
}

}  // namespace

SaveManager::SaveManager(const std::string& dir) : dir_(dir) {}

bool SaveManager::ensureDir() {
#ifdef _WIN32
    // 先检查目录是否已存在（避免 errno 被其它调用污染导致误判）
    DWORD attrs = GetFileAttributesA(dir_.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY))
        return true;
    int r = _mkdir(dir_.c_str());
    return r == 0;
#else
    int r = mkdir(dir_.c_str(), 0755);
    return r == 0 || errno == EEXIST;
#endif
}

std::string SaveManager::pathFor(int slot) const {
    return dir_ + "/slot" + std::to_string(slot) + ".sav";
}

namespace {

// 去掉行首的 UTF-8 BOM（EF BB BF）。用记事本等编辑器手改过存档时，
// 编辑器会加上 BOM，不剥掉的话 "version "/"chapter " 等前缀就对不上，
// 存档被解析成一堆「未知字段」的脏数据。
std::string stripBom(const std::string& line) {
    if (line.size() >= 3 &&
        static_cast<unsigned char>(line[0]) == 0xEF &&
        static_cast<unsigned char>(line[1]) == 0xBB &&
        static_cast<unsigned char>(line[2]) == 0xBF) {
        return line.substr(3);
    }
    return line;
}

// 去掉行首的空白（含 \t）以及**字节级残留**。
// 记事本 / 某些编辑器把「行尾空格 + 换行」存成「\r 空格」，读回来时 \r 会卡在
// 行中间；\r 一进 std::cout 就是「回到行首」，后面打的字会盖掉前面那一行，
// 在界面上看就是「下面冒出一段不相干的文字」。所以：
//   · 行中间的控制字符（除 \t）一律删掉；
//   · 行首 \r / 空格 / \t 全部去掉。
std::string stripLead(const std::string& line) {
    std::string s;
    for (std::size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        unsigned char u = static_cast<unsigned char>(c);
        if (u < 0x20 && c != '\t') continue;   // \r / \0 / 其它控制字符：删掉
        s += c;
    }
    std::size_t b = 0;
    while (b < s.size() && (s[b] == ' ' || s[b] == '\t')) ++b;
    return s.substr(b);
}

// 去掉行尾的 空格 / Tab。
std::string stripCr(const std::string& line) {
    std::string s = line;
    while (!s.empty() && (s[s.size() - 1] == '\r' || s[s.size() - 1] == ' ' ||
                          s[s.size() - 1] == '\t')) {
        s.erase(s.size() - 1);
    }
    return s;
}

// 只保留第一个 ASCII 制表符 / 空格之前的数字，转成 size_t。
// 存档里的「pc」被手改坏（如「pc 85 备注」「pc 1a2」「pc」）时返回 0，
// 而不是让 std::stoull 抛异常（异常在 Windows 下会把程序直接打崩）。
bool parsePc(const std::string& raw, std::size_t& out) {
    std::size_t i = 0;
    while (i < raw.size() && (raw[i] == ' ' || raw[i] == '\t')) ++i;
    std::size_t v = 0;
    bool any = false;
    for (; i < raw.size(); ++i) {
        if (raw[i] < '0' || raw[i] > '9') break;
        v = v * 10 + static_cast<std::size_t>(raw[i] - '0');
        any = true;
        if (v > 100000000u) return false;   // 明显损坏
    }
    if (!any) return false;
    out = v;
    return true;
}

// flag 的键 / 值写入存档前的清洗。
//  · 键：不允许出现换行和 '|'（'|' 是装备顺序表的分隔符），一律换成空格。
//  · 值：只把换行 / 控制字符换成空格，保留 '|'——
//    装备顺序表 eqo_xxx 是用 '|' 把候选名连起来的，洗掉就全乱了。
// 存档里混进换行或控制字符，读档界面下方就会出现不该出现的「东西」。
std::string sanitizeKey(const std::string& s) {
    std::string out;
    for (std::size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (c == '\n' || c == '\r' || c == '|') { out += ' '; continue; }
        unsigned char u = static_cast<unsigned char>(c);
        if (u < 0x20 && c != '\t') continue;   // 其它控制字符直接丢掉
        out += c;
    }
    return out;
}

std::string sanitizeValue(const std::string& s) {
    std::string out;
    for (std::size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (c == '\n' || c == '\r') { out += ' '; continue; }
        unsigned char u = static_cast<unsigned char>(c);
        if (u < 0x20 && c != '\t') continue;
        out += c;
    }
    return out;
}

// 存档槽简介：给存档 / 读档界面显示的**唯一**文字来源。
// 任何从存档里读出来的字符（包括玩家用记事本手改的）在这里统一清洗一遍：
// 去掉控制字符、把 '|' 换成空格、压掉重复空白、按显示宽度截断。
// 这样即使存档被改坏，界面上也绝不会冒出一段不相干的文字。
std::string cleanForUi(const std::string& in, std::size_t maxBytes) {
    std::string s;
    for (std::size_t i = 0; i < in.size(); ++i) {
        char c = in[i];
        unsigned char u = static_cast<unsigned char>(c);
        if (c == '|') { s += ' '; continue; }
        if (u < 0x20 || u == 0x7F) { s += ' '; continue; }
        s += c;
    }
    // 压掉连续空格
    std::string out;
    bool prevSpace = false;
    for (std::size_t i = 0; i < s.size(); ++i) {
        bool sp = (s[i] == ' ');
        if (sp && prevSpace) continue;
        out += s[i];
        prevSpace = sp;
    }
    // 按显示宽度截断（一行最多 22 显示列，够放「序章 · 咫尺天涯」）
    std::string cut;
    int cols = 0;
    std::size_t i = 0;
    while (i < out.size() && i < maxBytes) {
        unsigned char c = static_cast<unsigned char>(out[i]);
        std::size_t n = 1;
        if ((c & 0xF8) == 0xF0) n = 4;
        else if ((c & 0xF0) == 0xE0) n = 3;
        else if ((c & 0xE0) == 0xC0) n = 2;
        if (i + n > out.size()) break;
        int w = (n == 1) ? 1 : 2;
        if (cols + w > 22) break;
        cut += out.substr(i, n);
        cols += w;
        i += n;
    }
    // 去掉尾部空格
    while (!cut.empty() && cut[cut.size() - 1] == ' ') cut.erase(cut.size() - 1);
    return cut;
}

}  // namespace

bool SaveManager::save(int slot, const SaveData& data) {
    if (!ensureDir()) return false;

    std::ofstream f(pathFor(slot).c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (!f) return false;

    f << "version " << SAVE_FORMAT_VERSION << "\n";
    f << "chapter " << sanitizeValue(data.chapter) << "\n";
    f << "pc " << data.pc << "\n";
    // 读档锚点：label 名 + 偏移（旧版本没有这行，读不到就退回裸 pc）
    if (!data.resumeLabel.empty()) {
        f << "resume " << sanitizeValue(data.resumeLabel) << " "
          << data.resumeOffset << "\n";
    }
    f << "scene " << sanitizeValue(data.sceneTitle) << "\n";
    f << "timestamp " << sanitizeValue(data.timestamp.empty() ? nowString() : data.timestamp) << "\n";
    for (Flags::const_iterator it = data.flags.begin(); it != data.flags.end(); ++it) {
        f << "flag " << sanitizeKey(it->first) << "=" << sanitizeValue(it->second) << "\n";
    }
    f.close();

    // 记录最近存档槽，供「继续游戏」使用
    std::ofstream lf((dir_ + "/last.txt").c_str(), std::ios::out | std::ios::trunc);
    if (lf) lf << slot;
    return true;
}

bool SaveManager::load(int slot, SaveData& out) const {
    std::ifstream f(pathFor(slot).c_str(), std::ios::in | std::ios::binary);
    if (!f) return false;

    out = SaveData();
    std::string line;
    bool first = true;
    while (std::getline(f, line)) {
        if (first) { line = stripBom(line); first = false; }
        line = stripCr(line);
        line = stripLead(line);
        if (line.empty()) continue;

        if (line.compare(0, 8, "version ") == 0) {
            // 版本号（当前仅占位，未来可用于迁移）
        } else if (line.compare(0, 8, "chapter ") == 0) {
            out.chapter = line.substr(8);
        } else if (line.compare(0, 3, "pc ") == 0) {
            std::size_t v = 0;
            if (parsePc(line.substr(3), v)) out.pc = v;
        } else if (line.compare(0, 7, "resume ") == 0) {
            std::string v = line.substr(7);
            std::string::size_type sp = v.rfind(' ');
            if (sp != std::string::npos && sp > 0) {
                out.resumeLabel = v.substr(0, sp);
                std::size_t off = 0;
                if (parsePc(v.substr(sp + 1), off)) out.resumeOffset = off;
            }
        } else if (line.compare(0, 6, "scene ") == 0) {
            out.sceneTitle = line.substr(6);
        } else if (line.compare(0, 10, "timestamp ") == 0) {
            out.timestamp = line.substr(10);
        } else if (line.compare(0, 5, "flag ") == 0) {
            std::string kv = line.substr(5);
            std::string::size_type pos = kv.find('=');
            if (pos != std::string::npos) {
                std::string k = kv.substr(0, pos);
                if (!k.empty()) out.flags[k] = kv.substr(pos + 1);
            }
        }
        // 其它不认识的行直接丢掉——绝不能把它们当成「字段」再打印到界面上。
    }
    return true;
}

bool SaveManager::exists(int slot) const {
    std::ifstream f(pathFor(slot).c_str());
    return f.good();
}

std::string SaveManager::summary(int slot) const {
    std::string s = brief(slot);
    SaveData d;
    if (s.empty()) return "";
    if (load(slot, d) && !d.timestamp.empty()) {
        std::string ts = cleanForUi(d.timestamp, 32);
        // 时间戳按「YYYY-MM-DD HH:MM:SS」的格式核对一遍，对不上就当没有——
        // 免得把一段随手写进去的字当成时间显示在存档槽里。
        bool okTs = (ts.size() == 19 && ts[4] == '-' && ts[7] == '-' &&
                     ts[10] == ' ' && ts[13] == ':' && ts[16] == ':');
        for (std::size_t i = 0; okTs && i < ts.size(); ++i) {
            if (i == 4 || i == 7 || i == 10 || i == 13 || i == 16) continue;
            if (ts[i] < '0' || ts[i] > '9') okTs = false;
        }
        if (okTs) s += " · " + ts;
    }
    return s;
}

std::string SaveManager::brief(int slot) const {
    SaveData d;
    if (!load(slot, d)) return "";
    std::string chapter = cleanForUi(d.chapter, 64);
    std::string scene = cleanForUi(d.sceneTitle, 96);
    if (chapter.empty() && scene.empty()) return "";
    // 章节名和场景名经常重复（「序章」/「序章 · 咫尺天涯」），重复就不写两遍。
    if (chapter.empty()) return scene;
    if (scene.empty()) return chapter;
    if (scene.compare(0, chapter.size(), chapter) == 0) {
        // 「序章 · 咫尺天涯」这种：章名只是前缀，后面还有真内容，就整句显示。
        if (scene.size() > chapter.size()) return scene;
        return chapter;
    }
    // 场景名必须以「序章 / 第N章 / 终章」打头；否则说明它是一句混进存档的
    // 剧情旁白（旧版本存档 / 手改存档），绝不能当成摘要显示在存档槽里。
    bool looksLikeChapter = false;
    {
        // 「序」= E5 BA 8F，「第」= E7 AC AC，「终」= E7 BB 88
        const char* heads[3] = {"\xE5\xBA\x8F", "\xE7\xAC\xAC", "\xE7\xBB\x88"};
        for (int i = 0; i < 3; ++i) {
            if (scene.compare(0, 3, heads[i]) == 0) { looksLikeChapter = true; break; }
        }
    }
    // 有序号的场景名（「二 · 清晨，宿舍」「四 · 文书阁」）：
    // 前面那一个汉字 + " · " 就是分节号，直接拿来当摘要——
    // 二十个存档槽如果全都只显示「序章」，就分不清哪一格是哪一段了。
    if (scene.size() >= 6 && scene[3] == ' ' && scene.compare(4, 2, "\xC2\xB7") == 0) {
        return scene;
    }
    if (!looksLikeChapter) return chapter;
    return chapter + " · " + scene;
}

int SaveManager::latestSlot() const {
    std::ifstream f((dir_ + "/last.txt").c_str());
    if (!f) return -1;
    int slot = -1;
    f >> slot;
    return (slot >= 0 && slot < SAVE_SLOTS) ? slot : -1;
}

}  // namespace game
