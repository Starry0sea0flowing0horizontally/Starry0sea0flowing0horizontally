// ============================================================================
//  花至荼蘼 —— 可走动探索地图实现
//  支持两种风格：
//    · 天庭（单字符俯视图：# 墙、. 地板、A-D 地点）
//    · 宿舍（汉字图标 + 框线：床 / 桌 / 窗 / 门 / 破 等）
// ============================================================================
#include "map.h"

#include <algorithm>

#include "config.h"

namespace game {

Map::Map() {}

// ---- 工具：把每格内容写入 tiles_ --------------------------------
// 用 UTF-8 逐字切分：非 ASCII 首字节（>=0x80）连同后续字节算一格，
// 首字节是 0xE0~0xEF 表示 3 字节汉字。这里按「一个全角字符=3字节」处理。
namespace {
inline std::size_t utf8Len(unsigned char c) {
    if (c < 0x80) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

// 把一串「单元格」拼成 tiles_：每个单元格就是一个逻辑格。
// rows 里每行由若干「格」组成，格之间无分隔困难——此处直接用 UTF-8 字符拆分。
void buildRows(std::vector<std::vector<std::string>>& tiles,
               const std::vector<std::string>& rows) {
    tiles.clear();
    for (std::size_t y = 0; y < rows.size(); ++y) {
        const std::string& line = rows[y];
        std::vector<std::string> row;
        std::size_t i = 0;
        while (i < line.size()) {
            unsigned char c = static_cast<unsigned char>(line[i]);
            std::size_t len = utf8Len(c);
            row.push_back(line.substr(i, len));
            i += len;
        }
        tiles.push_back(row);
    }
}
}  // namespace

// ============================================================================
//  天庭：单字符俯视图（保留原有布局）
// ============================================================================
void Map::loadDefaultTianting() {
    if (id_ == "tianting") return;

    std::vector<std::string> rows;   // 每行是 ASCII 单字符，直接拼
    const int W = 30;
    const int H = 10;
    std::vector<std::string> g(H, std::string(W, '.'));
    for (int x = 0; x < W; ++x) {
        g[0][x] = '#';
        g[H - 1][x] = '#';
    }
    for (int y = 0; y < H; ++y) {
        g[y][0] = '#';
        g[y][W - 1] = '#';
    }
    for (int x = 0; x < W; ++x) {
        if (x == 14 || x == 15) continue;
        g[5][x] = '#';
    }
    g[1][3] = 'A';
    g[1][26] = 'B';
    g[8][3] = 'C';
    g[8][26] = 'D';

    rows = g;

    buildRows(tiles_, rows);
    locByCell_.clear();
    locAt_.clear();
    doorCells_.clear();      // 天庭没有门格
    locByCell_["A"] = "宿舍";
    locByCell_["B"] = "走廊";
    locByCell_["C"] = "文书阁";
    locByCell_["D"] = "值守室";

    id_ = "tianting";
    title_ = "天庭";
    px_ = 3;
    py_ = 1;
}

// ============================================================================
//  宿舍：汉字图标 + 框线俯视图（主体房间 / 破旧感）
//  格子含义：
//    墙      墙壁（四周外墙用汉字「墙」铺满，不用框线字符以免宽度错位）
//    ·       地板
//    床      单人木床（横三竖四，左上角靠墙）
//    枕/被   枕头 / 棉被
//    桌      旧木桌（中左）
//    椅      缺腿的椅子（桌旁）
//    灯      一盏暗黄的油灯（桌角）
//    卷      散落的文卷
//    简      堆叠的竹简 / 文书
//    墨      砚台
//    柜      木柜（右侧）
//    窗      窗（上墙正中开的一格，可打开）
//    门      门嵌在墙体里，只有两扇：右墙正中 = 杂物间的门，
//            下墙正中 = 正门（都在墙上，站不上去，站在旁边按回车互动）
//    尘      地上的灰尘（两处，可互动）
//    螭      双螭玉佩（左下角，存档点）
//    相      玩家（四不相），用他自己的代表字「相」画在地上
// ============================================================================
void Map::loadDorm() {
    if (id_ == "dorm") return;

    // 宿舍主体房间：40 格宽 × 16 行。
    // 高度必须让「地图 + 标题 + 图例 + 提示 + 操作说明」全部塞进传统 cmd
    // 默认 25 行窗口，否则上方边框会被上滚出屏幕（曾出现「没上边框/窗悬空」）。
    //   空行1 + 标题1 + 空行1 + 地图16 + 空行1 + 图例1 + 站位1 + 数字键1 + 操作1
    //   = 25 行 —— 正好整除，不留滚动余量。
    const std::string S = "\xE3\x80\x80";   // 全角空格 U+3000
    const int W = 40;
    const int H = 16;

    std::vector<std::vector<std::string>> g(
        static_cast<std::size_t>(H),
        std::vector<std::string>(static_cast<std::size_t>(W), S));

    // 边框用汉字「墙」铺满（不用框线字符，避免终端半角/全角宽度不一致错位）。
    for (int x = 0; x < W; ++x) {
        g[0][static_cast<std::size_t>(x)] = "墙";
        g[H - 1][static_cast<std::size_t>(x)] = "墙";
    }
    for (int y = 0; y < H; ++y) {
        g[static_cast<std::size_t>(y)][0] = "墙";
        g[static_cast<std::size_t>(y)][W - 1] = "墙";
    }

    // 窗：上墙正中央开一格「窗」（可互动，但站不上去——它只画在墙里）。
    //
    // 门：只有两扇，而且**都要能走上去**（门的性质是出口，不是墙）：
    //     右墙下方 = 「杂物间的门」；
    //     下墙正中 = 正门（最下面那扇门）。
    //
    // 实现方式：把这两个坐标登记成「门格」（doorCells_）：
    //   · 地图数据里这一格写成空地 = 可以通行；
    //   · render() 画到这一格时改画「门」字，并且照样上紫色；
    //   · locAt() 照样返回「杂物间的门」/「正门」。
    // 这样地图看上去还是两扇门嵌在墙上，但玩家真的能走到门口、能寻路、能按数字键直达。
    // 旧版把「门」字直接写在墙线上，同时又禁止走「门」格，两扇门就成了永远够不着的死门。
    doorCells_.clear();
    doorCells_.push_back(std::make_pair(20, H - 1));    // 正门：下墙正中
    doorCells_.push_back(std::make_pair(32, H - 1));    // 杂物间的门：下墙偏右
    for (std::size_t i = 0; i < doorCells_.size(); ++i) {
        int dx = doorCells_[i].first;
        int dy = doorCells_[i].second;
        if (dy >= 0 && dy < H && dx >= 0 && dx < W) g[dy][dx] = S;
    }
    g[0][20] = "窗";     // 上墙正中只留窗

    auto put = [&](int x, int y, const std::string& c) {
        if (x >= 1 && x < W - 1 && y >= 1 && y < H - 1)
            g[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = c;
    };

    // ---- 床：3 列宽 × 4 行高，靠左上角 ----
    put(2, 2, "床"); put(3, 2, "床"); put(4, 2, "床");
    put(2, 3, "床"); put(3, 3, "床"); put(4, 3, "床");
    put(2, 4, "枕"); put(3, 4, "被"); put(4, 4, "床");
    put(2, 5, "床"); put(3, 5, "床"); put(4, 5, "床");

    // ---- 书桌区（中左偏上） ----
    put(9, 2, "桌"); put(10, 2, "桌");
    put(9, 3, "椅");
    put(11, 2, "灯");                        // 桌角油灯
    put(12, 2, "卷"); put(13, 2, "卷");      // 散落的文卷
    put(11, 3, "墨");                        // 砚台

    // ---- 右上大柜（3 列 × 3 行） ----
    put(26, 2, "柜"); put(27, 2, "柜"); put(28, 2, "柜");
    put(26, 3, "柜"); put(27, 3, "柜"); put(28, 3, "柜");
    put(26, 4, "柜"); put(27, 4, "柜"); put(28, 4, "柜");

    // ---- 右侧小柜（2 列 × 2 行） ----
    put(33, 2, "柜"); put(34, 2, "柜");
    put(33, 3, "柜"); put(34, 3, "柜");

    // ---- 中部：两摞竹简 / 文书 ----
    put(15, 6, "简"); put(16, 6, "简");
    put(15, 7, "简"); put(16, 7, "简");
    put(30, 6, "简"); put(30, 7, "简");

    // ---- 下半部：让空间不空，物品分散 ----
    put(8, 10, "絮");                 // 地上一撮棉絮
    put(24, 9, "破");                 // 掉下来的墙皮
    put(3, 13, "螭");                 // 存档点：双螭玉佩（左下角）
    put(14, 12, "尘");                // 地上的灰尘①（左下区域，离出生点不远）
    put(31, 11, "尘");                // 地上的灰尘②（右侧，门边上）

    tiles_ = g;

    // ---- 玩家出生点校正：必须是真正能站的地板 ----
    // setPlayer 会静默丢弃越界坐标（保留旧的 px_/py_），如果出生点落在墙里，
    // 玩家就会站在墙格上——画面上看着像「卡在墙里」，鼠标/寻路全错。
    // 这里从房间正中开始找最近的一块空地兜底。
    px_ = 20;
    py_ = 10;
    if (!isWalkable(px_, py_)) {
        bool fixed = false;
        for (int r = 1; r < H && !fixed; ++r) {
            for (int dy = -r; dy <= r && !fixed; ++dy) {
                for (int dx = -r; dx <= r && !fixed; ++dx) {
                    if (isWalkable(20 + dx, 10 + dy)) {
                        px_ = 20 + dx;
                        py_ = 10 + dy;
                        fixed = true;
                    }
                }
            }
        }
    }

    locByCell_.clear();
    locByCell_["床"] = "床";
    locByCell_["桌"] = "桌子";
    locByCell_["门"] = "门";
    locByCell_["窗"] = "窗";
    locByCell_["螭"] = "双螭玉佩";
    // 更多可互动的家具 / 物件（走近即触发查看）
    locByCell_["柜"] = "木柜";
    locByCell_["椅"] = "椅子";
    locByCell_["简"] = "书简";
    locByCell_["墨"] = "砚台";
    locByCell_["灯"] = "油灯";
    locByCell_["卷"] = "文卷";
    locByCell_["枕"] = "枕头";
    locByCell_["被"] = "棉被";
    locByCell_["破"] = "墙皮";
    locByCell_["絮"] = "棉絮";
    locByCell_["尘"] = "地上的灰尘";

    // 同一个字在墙上 / 地上是两件不同的东西：两扇门都是「门」字，要分开。
    // locByCell 只能按字查，所以精确位置的重载在这里单独登记。
    // **注意**：isInteractive / locAt 都走这张表，只要在这里登记过，那扇门就一定「可互动」。
    locAt_.clear();
    locAt_[std::make_pair(20, 0)] = "窗";              // 上墙正中
    locAt_[std::make_pair(20, H - 1)] = "正门";         // 下墙正中
    locAt_[std::make_pair(32, H - 1)] = "杂物间的门";   // 下墙偏右

    id_ = "dorm";
    title_ = "宿舍";
    night_ = false;
}

// ============================================================================
//  宿舍 · 深夜（布局完全照搬白天，只把整屏压暗）
//  深夜的剧情就在这张图上跑：四不遇从门口进来、走到床边、说话、出门离开。
//  格子、门、窗、图例、出生点都与白天一致——玩家看到的还是「自己那间宿舍」。
// ============================================================================
void Map::loadDormNight() {
    const int keepX = px_;
    const int keepY = py_;
    const bool had = (id_ == "dorm");
    id_.clear();          // 强制重建（白天那份可能是带图例的老版本）
    loadDorm();
    title_ = "宿舍 · 深夜";
    night_ = true;
    if (had) {            // 站的位置沿用，不把人从床边拉走
        if (isWalkable(keepX, keepY)) { px_ = keepX; py_ = keepY; }
    }
}

// ============================================================================
//  宿舍 · 清晨（第二天）
//  深夜那张图是整屏压暗的；天亮以后要把白天那张重新建出来，把暗屏去掉，
//  并在床脚补上那朵荼蘼——深夜里看到的只是梦，天亮时它才真的在地板上。
// ============================================================================
void Map::loadDormMorning() {
    id_.clear();          // 强制重建：刚跑完深夜，id_ 还停在 "dorm"
    loadDorm();
    title_ = "宿舍 · 清晨";
    night_ = false;
    // 从床上起来：站到床沿外那一格（床占 x 2~4 / y 2~5）。
    int sx = 6, sy = 5;
    if (!isWalkable(sx, sy)) { sx = 6; sy = 6; }
    if (!isWalkable(sx, sy)) { sx = 20; sy = 15; }   // 兜底：门内那块空地板
    px_ = sx;
    py_ = sy;
    bloomFlower();        // 床脚那朵荼蘼，天亮了才看得见
}

// ============================================================================
//  杂物房（杂物间）—— 从宿舍右墙那扇窄门进来的一间小屋
//  只有两个出口：来时那扇门（回宿舍），和右墙上的「里间的门」。
//  里间的门锁着，钥匙藏在铜匣里；铜匣是「字锁」，要解字谜（荼 / 靡 / 蘼），
//  按对了才弹开，拿到「铜钥匙」。钥匙不是捡的，是解出来的。
// ============================================================================
void Map::loadStoreroom() {
    if (id_ == "storeroom") return;

    const std::string S = "\xE3\x80\x80";   // 全角空格
    const int W = 32;
    const int H = 14;

    std::vector<std::vector<std::string>> g(
        static_cast<std::size_t>(H),
        std::vector<std::string>(static_cast<std::size_t>(W), S));

    for (int x = 0; x < W; ++x) {
        g[0][static_cast<std::size_t>(x)] = "墙";
        g[H - 1][static_cast<std::size_t>(x)] = "墙";
    }
    for (int y = 0; y < H; ++y) {
        g[static_cast<std::size_t>(y)][0] = "墙";
        g[static_cast<std::size_t>(y)][W - 1] = "墙";
    }

    doorCells_.clear();
    doorCells_.push_back(std::make_pair(16, H - 1));   // 来时的门：下墙正中
    doorCells_.push_back(std::make_pair(31, 6));       // 里间的门：右墙
    for (std::size_t i = 0; i < doorCells_.size(); ++i) {
        int dx = doorCells_[i].first;
        int dy = doorCells_[i].second;
        if (dy >= 0 && dy < H && dx >= 0 && dx < W) g[dy][dx] = S;
    }

    auto put = [&](int x, int y, const std::string& c) {
        if (x >= 1 && x < W - 1 && y >= 1 && y < H - 1)
            g[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = c;
    };

    // ---- 靠上墙的一排木架，堆着蒙灰的旧物 ----
    put(3, 2, "架"); put(4, 2, "架"); put(5, 2, "架");
    put(10, 2, "架"); put(11, 2, "架"); put(12, 2, "架"); put(13, 2, "架");
    put(22, 2, "架"); put(23, 2, "架");

    // ---- 地上的木箱 ----
    put(5, 8, "箱"); put(6, 8, "箱");
    put(5, 9, "箱");
    put(20, 10, "箱");

    // ---- 右下的旧柜 ----
    put(25, 9, "柜"); put(26, 9, "柜");
    put(25, 10, "柜"); put(26, 10, "柜");

    // ---- 铜匣（字锁）在屋子右侧 ----
    put(24, 6, "匣");

    // 这间屋子只是间普通的杂物间：没有账册、没有花、也没有故事，
    // 只有架子、箱子、柜子和一只铜匣——放东西的地方。
    put(12, 7, "箱");
    put(3, 10, "箱");
    put(16, 8, "架");

    tiles_ = g;

    px_ = 16;
    py_ = 12;
    if (!isWalkable(px_, py_)) {
        bool fixed = false;
        for (int r = 1; r < H && !fixed; ++r) {
            for (int dy = -r; dy <= r && !fixed; ++dy) {
                for (int dx = -r; dx <= r && !fixed; ++dx) {
                    if (isWalkable(16 + dx, 12 + dy)) {
                        px_ = 16 + dx;
                        py_ = 12 + dy;
                        fixed = true;
                    }
                }
            }
        }
    }

    locByCell_.clear();
    locByCell_["架"] = "木架";
    locByCell_["箱"] = "木箱";
    locByCell_["柜"] = "旧柜";
    locByCell_["匣"] = "铜匣";

    locAt_.clear();
    locAt_[std::make_pair(16, H - 1)] = "杂物间的门";
    locAt_[std::make_pair(31, 6)] = "里间的门";

    id_ = "storeroom";
    title_ = "杂物房";
    night_ = false;
}

// ============================================================================
//  天庭回廊（seg = 1..4）
//  一条会拐弯的长回廊，四段各一个形状：
//    · 西廊     —— 笔直的一段（40×7）
//    · 中廊     —— 走到一半向南折下去，尽头再往西摊开（L 形，40×12）
//    · 东廊     —— 先向东、再折向南、最后又折回向东（S 形，40×12）
//    · 困兽府前 —— 走到尽头摊开一片小广场，困兽府就在广场最里头（40×10）
//  做法：先把整张图填满「墙」，再把要走的格子「挖」成空地，
//  所以拐弯、死角、广场都只是「挖法」不同，走法还是同一套。
//
//  每段都有：
//    · 两头各一个拱门（青）：走上去就自动切到相邻那段，不用按回车；
//    · 两旁的墙上有神仙府邸的门（紫）：按回车才推得开；
//    · 一块「记事碑」（金）：站在上面按回车就能存档。
// ============================================================================
void Map::loadCorridor(int seg) {
    if (seg < 1) seg = 1;
    if (seg > 4) seg = 4;
    const std::string wantId = "corridor" + std::to_string(seg);
    if (id_ == wantId) return;          // 同一段：不重建（保住玩家站位）

    const int W = 40;
    int H = 7;
    if (seg == 2 || seg == 3) H = 12;
    else if (seg == 4) H = 10;

    const std::string S = "\xE3\x80\x80";      // 空地（全角空格）
    const std::string WALL = "\xE5\xA2\x99";   // 墙
    std::vector<std::vector<std::string> > g(
        static_cast<std::size_t>(H),
        std::vector<std::string>(static_cast<std::size_t>(W), WALL));

    const int keepX = px_;
    const int keepY = py_;
    const bool prevCorridor = (id_.compare(0, 8, "corridor") == 0);

    locByCell_.clear();     // 回廊不画图例
    locAt_.clear();
    doorCells_.clear();
    autoAt_.clear();

    // 挖出一块可走的空地（含端点）
    auto carve = [&](int x0, int y0, int x1, int y1) {
        for (int y = y0; y <= y1; ++y) {
            if (y < 0 || y >= H) continue;
            for (int x = x0; x <= x1; ++x) {
                if (x < 0 || x >= W) continue;
                g[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = S;
            }
        }
    };
    auto put = [&](int x, int y, const std::string& c) {
        if (x < 0 || x >= W || y < 0 || y >= H) return;
        g[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = c;
    };
    // 门：嵌在墙上，但可以走上去（门是出口，不是墙）
    auto door = [&](int x, int y, const std::string& name) {
        put(x, y, S);
        doorCells_.push_back(std::make_pair(x, y));
        locAt_[std::make_pair(x, y)] = name;
    };
    // 拱门：走上去立刻切图的格子
    auto arch = [&](int x, int y, const std::string& name) {
        put(x, y, S);
        locAt_[std::make_pair(x, y)] = name;
        autoAt_[std::make_pair(x, y)] = name;
    };
    // 窗：嵌在墙里，站不上去，站在旁边按回车看
    auto win = [&](int x, int y) {
        put(x, y, "窗");
        locAt_[std::make_pair(x, y)] = "窗";
    };
    // 柱子：挡路（isWallCell 认「柱」），把一进进的长廊隔开
    auto pillar = [&](int x, int y) { put(x, y, "柱"); };
    // 记事碑：这一段的存档点
    auto save = [&](int x, int y) {
        put(x, y, "碑");
        locAt_[std::make_pair(x, y)] = "记事碑";
    };

    if (seg == 1) {
        title_ = "三 · 天庭回廊 · 西廊";
        carve(1, 1, 38, 5);
        door(8, 0, "白泽府");
        door(26, 0, "獬豸府");
        door(14, 6, "杂物司");
        win(34, 0);
        pillar(6, 2);  pillar(6, 4);
        pillar(22, 2); pillar(22, 4);
        pillar(32, 2); pillar(32, 4);
        save(12, 1);
        arch(0, 3, "回廊西口");
        arch(39, 3, "回廊东口");
    } else if (seg == 2) {
        title_ = "回廊 · 中廊";
        carve(1, 1, 38, 4);      // 东西向的主廊
        carve(29, 4, 34, 10);    // 走到一半向南折下去的竖廊
        carve(24, 7, 34, 10);    // 尽头向西摊开的一段
        door(6, 0, "斗姆宫");
        door(18, 0, "月老祠");
        door(39, 2, "药王殿");
        door(35, 7, "织云坊");
        win(10, 0);
        pillar(14, 2); pillar(14, 4);
        pillar(26, 2); pillar(26, 4);
        save(8, 1);
        arch(0, 3, "回廊西口");
        arch(31, 11, "回廊南口");
    } else if (seg == 3) {
        title_ = "回廊 · 东廊";
        carve(1, 1, 22, 4);      // 上段：向东
        carve(18, 4, 22, 8);     // 中段：折向南
        carve(18, 8, 38, 10);    // 下段：再折回向东
        door(6, 0, "九尾府");
        door(14, 0, "文昌阁");
        door(28, 11, "鹿蜀府");
        win(36, 11);
        pillar(10, 2); pillar(10, 4);
        pillar(32, 10);
        save(4, 1);
        arch(0, 2, "回廊西口");
        arch(39, 9, "回廊东口");
    } else {
        title_ = "回廊 · 困兽府前";
        carve(1, 1, 38, 4);      // 主廊
        carve(26, 4, 38, 8);     // 尽头摊开的广场
        door(10, 0, "文书阁");
        door(39, 5, "困兽府");
        win(20, 0);
        pillar(16, 2); pillar(16, 4);
        pillar(32, 8);
        save(5, 1);
        arch(0, 2, "回廊西口");
        // 东头没有拱门：回廊到这里就到头了，再往前是困兽府的门
    }

    tiles_ = g;
    id_ = wantId;
    night_ = false;

    // 站位：从别的回廊走回来就留在原地；否则回到西头（脚本里会显式给坐标）
    if (prevCorridor && isWalkable(keepX, keepY)) {
        px_ = keepX;
        py_ = keepY;
    } else {
        px_ = 1;
        py_ = (seg == 1) ? 3 : 2;
    }
}

std::string Map::autoExitAt(int x, int y) const {
    std::map<std::pair<int, int>, std::string>::const_iterator it =
        autoAt_.find(std::make_pair(x, y));
    return (it != autoAt_.end()) ? it->second : std::string();
}

// ============================================================================
//  文书阁：四不相干活的地方
//  一进门就是一排排桌案，桌上摊着纸、压着笔山、搁着砚台；
//  两个同事埋头抄写，最里头那张桌边上站着上司。
//  格子含义：
//    墙  墙壁
//    ·   地板（全角空格）
//    桌  桌案（两张拼一案）      椅  凳子
//    笔  毛笔                    墨  砚台            纸  纸
//    简  摞起来的竹简            卷  摊开的文卷        灯  油灯
//    柜  木柜                    窗  窗                门  门（通回廊）
//    僚  同事（挡路，站在旁边按回车说话）
//    司  上司（同上）
//    相  玩家（四不相）
// ============================================================================
void Map::loadOffice() {
    if (id_ == "office") return;

    const int W = 40;
    const int H = 11;
    const std::string S = "\xE3\x80\x80";
    const std::string WALL = "\xE5\xA2\x99";
    std::vector<std::vector<std::string> > g(
        static_cast<std::size_t>(H),
        std::vector<std::string>(static_cast<std::size_t>(W), WALL));
    auto put = [&](int x, int y, const std::string& c) {
        if (x < 0 || x >= W || y < 0 || y >= H) return;
        g[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = c;
    };
    for (int y = 1; y <= 9; ++y)
        for (int x = 1; x <= 38; ++x) put(x, y, S);

    locByCell_.clear();
    locAt_.clear();
    doorCells_.clear();
    autoAt_.clear();

    // 摆一件「可以按回车」的东西：格子字 + 地点名
    auto prop = [&](int x, int y, const std::string& c, const std::string& name) {
        put(x, y, c);
        locAt_[std::make_pair(x, y)] = name;
    };
    auto desk = [&](int x, int y) { prop(x, y, "桌", "桌子"); };
    auto chair = [&](int x, int y) { prop(x, y, "椅", "椅子"); };

    // 上墙正中的窗；下墙的门（回廊那头）
    put(20, 0, "窗");
    locAt_[std::make_pair(20, 0)] = "窗";
    put(3, 10, "门");
    doorCells_.push_back(std::make_pair(3, 10));
    locAt_[std::make_pair(3, 10)] = "门";

    // 靠墙堆着的公文
    prop(5, 1, "简", "竹简");
    prop(6, 1, "卷", "文卷");

    // 第一张桌案：同事甲
    desk(8, 3);  desk(9, 3);
    chair(8, 4); chair(9, 4);
    prop(10, 3, "笔", "毛笔");
    prop(10, 4, "墨", "砚台");
    prop(11, 4, "纸", "纸");
    prop(9, 2, "僚", "同事");

    // 第二张桌案：同事乙
    desk(16, 3);  desk(17, 3);
    chair(16, 4); chair(17, 4);
    prop(18, 3, "笔", "毛笔");
    prop(18, 4, "墨", "砚台");
    prop(19, 4, "纸", "纸");
    prop(17, 2, "僚", "同事");

    // 第三张桌案：今天没人坐，纸上压着一方镇纸
    desk(24, 3);  desk(25, 3);
    chair(24, 4); chair(25, 4);
    prop(26, 4, "纸", "纸");

    prop(30, 3, "灯", "油灯");
    prop(36, 2, "柜", "木柜");

    // 最里头：上司的桌案
    desk(32, 7);
    desk(36, 7);
    chair(33, 8);
    chair(35, 8);
    prop(30, 7, "灯", "油灯");
    prop(34, 7, "司", "上司");

    tiles_ = g;
    id_ = "office";
    title_ = "四 · 文书阁";
    night_ = false;
    px_ = 5;
    py_ = 9;
}

// ============================================================================
//  深夜 · 床脚开花
//  四不遇走了以后，床脚那朵「荼蘼」才开出来。直接往 tiles_ 里补一格「花」，
//  它立刻是一个可站立、可按回车互动的地点（地点 id = 荼蘼花）。
//  只有深夜那张图会调用。
// ============================================================================
void Map::bloomFlower() {
    if (tiles_.empty()) return;
    const int fx = 5;
    const int fy = 5;                       // 床脚右侧那一格（床占 2~4 / 2~5）
    if (fy < 1 || fy >= height() - 1) return;
    if (fx < 1 || fx >= width() - 1) return;
    tiles_[static_cast<std::size_t>(fy)][static_cast<std::size_t>(fx)] = "花";
    locByCell_["花"] = "荼蘼花";
    locAt_[std::make_pair(fx, fy)] = "荼蘼花";
}

std::string Map::locAt(int x, int y) const {
    std::map<std::pair<int, int>, std::string>::const_iterator it =
        locAt_.find(std::make_pair(x, y));
    if (it != locAt_.end()) return it->second;
    return locationAt(x, y);
}
// ============================================================================
//  通用访问器
// ============================================================================
const std::string& Map::id() const { return id_; }
const std::string& Map::title() const { return title_; }

int Map::width() const {
    return tiles_.empty() ? 0 : static_cast<int>(tiles_[0].size());
}

int Map::height() const {
    return static_cast<int>(tiles_.size());
}

bool Map::inBounds(int x, int y) const {
    return y >= 0 && y < height() && x >= 0 && x < width();
}

static bool isWallCell(const std::string& cell) {
    return cell == "墙" || cell == "#" || cell == "┌" || cell == "─" || cell == "┐" ||
           cell == "│" || cell == "└" || cell == "┘" || cell == "┬" ||
           cell == "┴" || cell == "┆" || cell == "柱" ||   // 「柱」= 回廊的柱子，挡路
           cell == "僚" || cell == "司";                   // 同事 / 上司：人也是挡路的
}

bool Map::isWalkable(int x, int y) const {
    if (!inBounds(x, y)) return false;
    const std::string& c = tiles_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
    if (isWallCell(c)) return false;
    // 窗嵌在墙里（地板之外），玩家站不上去，只能在旁边按回车互动。
    if (c == "窗") return false;
    // 门**不再**一律算不可站立：它们虽然画在墙线上，但性质是「出口」，
    // 玩家应该能走到门前、按回车出去。以前这里把「门」也判成不可走，
    // 结果门被彻底封死——鼠标点不过去、寻路到不了、数字键直达也停不下来，
    // 看上去就是「门交互不了，也去不了」。
    // 现在只有「真正的墙」才挡路；门能不能走由格子性质决定（见 loadDorm 注释）。
    return true;
}

std::string Map::locationAt(int x, int y) const {
    if (!inBounds(x, y)) return "";
    const std::string& cell = tiles_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
    std::map<std::string, std::string>::const_iterator it = locByCell_.find(cell);
    if (it != locByCell_.end()) return it->second;
    return "";
}

std::string Map::cellAt(int x, int y) const {
    if (!inBounds(x, y)) return "";
    return tiles_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
}

bool Map::isInteractive(int x, int y) const {
    return !locAt(x, y).empty();
}

void Map::setPlayer(int x, int y) {
    if (inBounds(x, y)) {
        px_ = x;
        py_ = y;
    }
}

int Map::playerX() const { return px_; }
int Map::playerY() const { return py_; }

bool Map::movePlayer(int dx, int dy) {
    int nx = px_ + dx;
    int ny = py_ + dy;
    if (!isWalkable(nx, ny)) return false;
    px_ = nx;
    py_ = ny;
    return true;
}

std::string Map::playerLocation() const {
    return locAt(px_, py_);
}

// ---------------------------------------------------------------- BFS 寻路 --
std::vector<char> Map::pathTo(int tx, int ty) const {
    return pathFrom(px_, py_, tx, ty);
}

// 从任意起点 (sx,sy) 到 (tx,ty) 的路径。
// 地图角色（四不遇）必须用自己的位置当起点，不能借用玩家的 px_/py_，
// 否则他会按「玩家在哪儿」来算方向，看起来就是不动或者乱走。
std::vector<char> Map::pathFrom(int sx, int sy, int tx, int ty) const {
    std::vector<char> path;
    if (!inBounds(tx, ty)) return path;
    if (!isWalkable(tx, ty)) return path;
    if (!inBounds(sx, sy)) return path;
    if (tx == sx && ty == sy) return path;   // 已在目标上，无需移动

    const int W = width();
    const int H = height();
    // visited 记录每个格是否已访问；prev 记录「从哪个方向来到这里」。
    std::vector<std::vector<bool>> visited(
        static_cast<std::size_t>(H),
        std::vector<bool>(static_cast<std::size_t>(W), false));
    // 用 -1/0/1 表示来向：prev[y][x] 存「从 (x,y) 往回走的方向」。
    // 编码：0=上(U) 1=下(D) 2=左(L) 3=右(R)，-1=未访问。
    std::vector<std::vector<int>> prev(
        static_cast<std::size_t>(H),
        std::vector<int>(static_cast<std::size_t>(W), -1));

    // 简单的队列：BFS 从 (sx, sy) 出发
    std::vector<std::pair<int,int>> q;
    q.push_back(std::make_pair(sx, sy));
    visited[static_cast<std::size_t>(sy)][static_cast<std::size_t>(sx)] = true;

    const int dx[4] = {0, 0, -1, 1};
    const int dy[4] = {-1, 1, 0, 0};
    // 与 dx/dy 对应的「从当前格到此邻格」方向标记，供回溯用
    const int dir_to_prev[4] = {1, 0, 3, 2};  // 到上邻格=往下回；到右邻格=往左回…

    std::size_t head = 0;
    bool found = false;
    while (head < q.size() && !found) {
        int cx = q[head].first;
        int cy = q[head].second;
        ++head;
        for (int d = 0; d < 4; ++d) {
            int nx = cx + dx[d];
            int ny = cy + dy[d];
            if (!isWalkable(nx, ny)) continue;
            if (visited[static_cast<std::size_t>(ny)][static_cast<std::size_t>(nx)])
                continue;
            visited[static_cast<std::size_t>(ny)][static_cast<std::size_t>(nx)] = true;
            // 记录：从邻格 (nx,ny) 往回走的方向 = dir_to_prev[d]
            prev[static_cast<std::size_t>(ny)][static_cast<std::size_t>(nx)] = dir_to_prev[d];
            if (nx == tx && ny == ty) { found = true; break; }
            q.push_back(std::make_pair(nx, ny));
        }
    }
    if (!found) return path;

    // 从目标回溯到起点，收集方向，再反转。
    // prev[cy][cx] 编码的是「从这一格回到父格」的方向（0上 1下 2左 3右）。
    // 所以 p 既是回退方向，也是我们「前进到这一格」的反方向：
    //   p=0 回到父格要往上 → 反过来，从父格到这里是往下 'D'
    // 注意 step 与坐标位移必须一致，否则会朝反方向一路走出地图。
    int cx = tx, cy = ty;
    while (!(cx == sx && cy == sy)) {
        int p = prev[static_cast<std::size_t>(cy)][static_cast<std::size_t>(cx)];
        char step;
        switch (p) {
            case 0: step = 'D'; cy -= 1; break;   // 回退向上 → 前进向下
            case 1: step = 'U'; cy += 1; break;   // 回退向下 → 前进向上
            case 2: step = 'R'; cx -= 1; break;   // 回退向左 → 前进向右
            case 3: step = 'L'; cx += 1; break;   // 回退向右 → 前进向左
            default: return std::vector<char>();  // 异常，放弃
        }
        path.push_back(step);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// ============================================================================
//  渲染
// ============================================================================
void Map::render(Renderer& r, const std::string& highlightLoc) const {
    renderWith(r, highlightLoc, std::vector<MapActorView>());
}

void Map::renderWith(Renderer& r, const std::string& highlightLoc,
                     const std::vector<MapActorView>& views) const {
    for (int y = 0; y < height(); ++y) {
        std::string row;
        row.reserve(static_cast<std::size_t>(width()) * 12);
        for (int x = 0; x < width(); ++x) {
            // 站在这一格上的角色（同一格有多人时取最后一个）
            const MapActorView* who = nullptr;
            for (std::size_t i = 0; i < views.size(); ++i) {
                if (views[i].x == x && views[i].y == y && !views[i].hidden) who = &views[i];
            }
            // 角色和玩家同一格时，玩家优先（玩家是本尊，不能被别的字盖住）
            if (who && x == px_ && y == py_) who = nullptr;

            const std::string& cell = tiles_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
            // 门格：数据里是空地（可通行），但**画面上一律写成「门」字**，
            // 玩家站上去时反显，表示「你现在就站在这扇门上」。
            const bool isDoor = isDoorCell(x, y);
            const char* c = nullptr;
            std::string shown = isDoor ? std::string("\xE9\x97\xA8") : cell;   // 门
            if (who) {
                // 角色格：盖掉脚下的家具，显示「代表他的那个字」（遇 / 相）。
                // 角色字一律是占 2 列的汉字，和别的地图格一样宽，不会错位。
                shown = !who->glyph.empty() ? who->glyph
                                            : std::string(MAP_ACTOR_SIBUYU);
                c = who->color.empty() ? color::AMBER : who->color.c_str();
                if (c) row += c;
                row += "\033[1m";   // 加粗：在满屏汉字里一眼能看见他
                row += shown;
                row += color::RESET;
                continue;
            }
            if (x == px_ && y == py_) {
                if (isDoor) {
                    c = "\033[7m";   // 站在门上：反显的「门」字
                } else {
                    // 玩家站在「地点 / 家具」格上时，反显显示物件字（床/柜/桌椅），
                    // 而不是用「相」盖掉它——这样玩家一眼就能看出自己站在什么上面。
                    bool onProp = !isWallCell(cell) &&
                                  cell != "\xE3\x80\x80" &&   // 全角空格=空地
                                  cell != ".";
                    if (onProp) {
                        c = "\033[7m";   // 反显：玩家站位标记
                    } else {
                        shown = MAP_PLAYER_CHAR;
                        c = speaker::SIBUXIANG;
                    }
                }
            } else if (isDoor) {
                std::string loc = locAt(x, y);
                // 一扇门就是一扇门，永远描出来；当前目标门用金色强调。
                c = (loc == highlightLoc) ? color::GOLD : color::PURPLE;
            } else {
                std::string loc = locAt(x, y);
                if (cell == "僚") {
                    c = color::WHITE;    // 同事：一身白，伏在案上抄写
                } else if (cell == "司") {
                    c = color::PURPLE;   // 上司：深色官袍
                } else if (cell == "碑") {
                    c = color::GOLD;     // 记事碑：存档点，隔着一整条回廊也看得见
                } else if (isWallCell(cell)) {
                    c = color::GRAY;   // 满屏「墙」字，用暗灰避免喧宾夺主
                } else if (cell == "门") {
                    c = color::PURPLE; // 兼容：旧数据里直接写在墙上的门字
                } else if (cell == "窗") {
                    c = color::CYAN;   // 窗同上
                } else if (cell == "." || cell == " " || cell == "\xE3\x80\x80") {
                    c = nullptr;       // 地板（含全角空格）不染色
                } else if (!loc.empty()) {
                    c = (loc == highlightLoc) ? color::GOLD : color::CYAN;
                } else {
                    // 其他装饰（破、尘）
                    c = color::AMBER;
                }
            }
            if (c) row += c;
            row += shown;
            row += color::RESET;
        }
        r.printLine(row);
    }
}

bool Map::isDoorCell(int x, int y) const {
    for (std::size_t i = 0; i < doorCells_.size(); ++i) {
        if (doorCells_[i].first == x && doorCells_[i].second == y) return true;
    }
    return false;
}

// 在地图上额外画「角色」（深夜宿舍里的四不遇）。
// 直接复用 renderWith：角色那一格盖成角色字（「遇」），带自己的颜色并加粗。
// 以前这里是一整段复制粘贴的渲染代码，每次改地图配色都得改两遍、极易漂移，
// 现在只有一条渲染路径。
void Map::renderActors(Renderer& r, const std::vector<MapActorView>& views) const {
    renderWith(r, "", views);
}

bool Map::isNight() const { return night_; }

void Map::renderLegend(Renderer& r) const {
    // 图例按「家具 -> 小物 -> 建筑 -> 杂物 -> 存档点」的顺序排列，
    // 每个单元格字只取对应的地点名。两扇门是同一格字「门」，只能写一个，
    // 具体哪扇门在走到跟前时由状态行提示。
    static const char* const order[] = {
        "床", "桌", "椅", "灯", "卷", "墨", "柜", "简",
        "箱", "架", "窗", "门", "破", "絮", "尘", "螭"
    };
    const std::size_t orderCount = sizeof(order) / sizeof(order[0]);

    // 依次拼出「〔地点名〕」小片段
    std::vector<std::string> pieces;
    for (std::size_t i = 0; i < orderCount; ++i) {
        std::map<std::string, std::string>::const_iterator it =
            locByCell_.find(std::string(order[i]));
        if (it == locByCell_.end() || it->second.empty()) continue;
        pieces.push_back("〔" + it->second + "〕");
    }
    if (pieces.empty()) return;   // 没有可展示的地点，图例整行省略

    // 分行折排：每行最多 60 显示列，片段之间以全角空格分隔。
    // 行首缩进两个全角空格（4 列）不计入 60 列的片段宽度预算。
    const int kLineCols = 60;
    const std::string kSep = "　";
    const int kSepCols = Renderer::widthOf(kSep);

    std::string line;
    int lineW = 0;
    for (std::size_t i = 0; i < pieces.size(); ++i) {
        int w = Renderer::widthOf(pieces[i]);
        if (!line.empty() && lineW + kSepCols + w > kLineCols) {
            r.printLine("　　" + line, color::SILVER);
            line.clear();
            lineW = 0;
        }
        if (!line.empty()) {
            line += kSep;
            lineW += kSepCols;
        }
        line += pieces[i];
        lineW += w;
    }
    if (!line.empty()) r.printLine("　　" + line, color::SILVER);
}

}  // namespace game
