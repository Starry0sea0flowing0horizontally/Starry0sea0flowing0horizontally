// ============================================================================
//  花至荼蘼 —— 可走动探索地图（终端字符俯视图）
// ============================================================================
#pragma once

#include <map>
#include <string>
#include <vector>

#include <utility>

#include "renderer.h"

namespace game {

// 一张可走动的地图：字符网格 + 玩家位置 + 地点标记。
// 地点标记（如 'A'）对应一个地点 id（如 "宿舍"），
// 玩家走到标记上并按回车时，由上层（Game）决定是否触发对应剧情。
class Map {
public:
    Map();

    // 构建默认的「天庭」地图（幂等：已加载则直接返回）
    void loadDefaultTianting();
    // 构建「宿舍」地图（汉字图标 + 框线风格，破旧感）
    void loadDorm();
    // 同一间宿舍的「深夜」版本：布局一模一样，只是满屋暗下来（整屏压暗）。
    // 深夜宿舍的剧情（四不遇进来、说话、离开）就在这张图上跑。
    void loadDormNight();
    // 第二天清晨：白天宿舍重建（去掉深夜的压暗），玩家从床边醒来，床脚开着荼蘼。
    void loadDormMorning();
    // 「杂物房」地图：宿舍右墙那扇窄门后面的一间小屋。
    // 出口只有两个：来时的门（回宿舍）和右墙的「里间的门」（上锁，要铜钥匙）。
    void loadStoreroom();
    // 深夜宿舍专用：四不遇走后，床脚开出一朵荼蘼（往地形里补一格「花」）。
    void bloomFlower();

    // ---- 天庭回廊（seg = 1..4）----
    // 一条会拐弯的长回廊，切成四段：西廊 → 中廊（向南折）→ 东廊（S 形）→ 困兽府前。
    // 每段两头各开一个拱门，走上去（不用按回车）就切到相邻那段；
    // 两旁的墙上开着神仙府邸的门，要按回车才推得开；每段都立着一块「记事碑」（存档点）。
    void loadCorridor(int seg);
    // 「走到就切图」的格子（回廊两头的拱门）。
    // 返回该格登记的地点名，不是这种格子返回 ""。
    std::string autoExitAt(int x, int y) const;

    // 「文书阁」：四不相干活的地方——三排桌案、八个伏案的同事、笔墨纸砚、
    // 竹简文卷、油灯木柜，最里头是上司的桌（40×16，一大间）。
    // bossPresent=false 时不摆上司那一格：开场演出里上司是「走过来的地图角色」，
    // 他站在那儿会被画成两个人（桌后一个 + 走过来的一个）。
    void loadOffice(bool bossPresent = true);

    // 已加载的地图 id / 显示标题（用于抬头与图例）
    const std::string& id() const;
    const std::string& title() const;

    int width() const;
    int height() const;

    bool inBounds(int x, int y) const;
    bool isWalkable(int x, int y) const;
    std::string locationAt(int x, int y) const;   // 地点 id，非地点返回 ""
    // 同上，但优先看「按坐标登记」的地点表——同一个字在不同位置可以是不同的东西
    // （宿舍墙上的两扇门都是「门」字，但要分成 杂物间的门 / 正门）。
    std::string locAt(int x, int y) const;
    std::string cellAt(int x, int y) const;       // 该格原始字符（越界返回 ""）
    // 是不是「门格」：数据上是空地（可以走上去），画面上画成「门」字。
    // 门 = 出口，必须能走到门前按回车，不能像墙一样把人挡在外面。
    bool isDoorCell(int x, int y) const;
    // 该格是否是一个「可进入的地点」（门/窗/床/桌/存档点等），
    // 用于鼠标点击时直接判断「点到物体就过去互动」，无需先走近再按回车。
    bool isInteractive(int x, int y) const;

    void setPlayer(int x, int y);
    int playerX() const;
    int playerY() const;
    bool movePlayer(int dx, int dy);              // 尝试移动，返回是否成功
    std::string playerLocation() const;           // 当前脚下地点 id，无则 ""

    // BFS 最短路径：从玩家当前位置走到 (tx,ty)，返回每一步方向序列。
    // 方向字符：'U'上 'D'下 'L'左 'R'右。目标不可达或非目标格时返回空。
    std::vector<char> pathTo(int tx, int ty) const;
    // 同上，但起点由调用者指定（地图角色从自己的格子出发时用这个）
    std::vector<char> pathFrom(int sx, int sy, int tx, int ty) const;

    // 绘制网格；highlightLoc 的地点标记高亮显示（作为当前目标）
    void render(Renderer& r, const std::string& highlightLoc) const;
    // 绘制网格 + 若干「会动的角色」（深夜宿舍里的四不遇等）
    struct MapActorView {
        int x = 0;
        int y = 0;
        std::string glyph;      // 地图上的一个字
        std::string color;      // ANSI 颜色
        bool hidden = false;    // 已离场
    };
    void renderActors(Renderer& r, const std::vector<MapActorView>& views) const;
    // 唯一的渲染实现：地图 + 角色一起画（render 就是它传空角色表）。
    void renderWith(Renderer& r, const std::string& highlightLoc,
                    const std::vector<MapActorView>& views) const;
    // 画出图例（家具 / 门 / 窗……）
    void renderLegend(Renderer& r) const;
    // 是否是「深夜」版本（整屏压暗）
    bool isNight() const;

private:
    // 每格一个字符串（可能是单字符 '.'/'#'/'A'，也可能是多字节汉字"床"等）
    std::vector<std::vector<std::string>> tiles_;
    std::map<std::string, std::string> locByCell_;  // 格子内容 -> 地点名
    std::map<std::pair<int, int>, std::string> locAt_;  // (x,y) -> 地点名（优先）
    // 门格坐标：数据上是空地（可通行），画面上画成「门」字。
    // 门是出口，必须能走上去；像墙一样挡人就成了「永远够不着的死门」。
    std::vector<std::pair<int, int>> doorCells_;
    // 自动出口格：(x,y) -> 地点名。走上去就立刻切图（回廊两头的拱门）。
    std::map<std::pair<int, int>, std::string> autoAt_;
    std::string id_;                          // 当前地图 id
    std::string title_;                       // 地图显示标题
    int px_ = 0;                              // 玩家列坐标 x
    int py_ = 0;                              // 玩家行坐标 y
    bool night_ = false;                      // 是否是「深夜」版本（整屏压暗）
    bool bossHere_ = true;                    // 文书阁：上司现在在不在桌后那个人
};

}  // namespace game
