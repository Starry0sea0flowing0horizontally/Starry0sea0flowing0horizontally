// ============================================================================
//  花至荼蘼 —— 序章「咫尺天涯」剧情脚本
//  基于原作《有兽焉》的同人原创剧情
// ============================================================================
#include "prologue.h"

#include "config.h"

namespace game {

Script buildPrologue() {
    ScriptBuilder b;

    // =====================================================================
    //  开篇 · 四不相第一人称自白
    // =====================================================================
    b.scene("序章 · 咫尺天涯", "");
    b.narr("我叫四不相，是一只白毛麒麟。", color::WHITE);
    b.narr("目前在天庭抄文书，不算累，也不算好。", color::SILVER);
    b.narr("像一件用了太久的旧器皿，连自己什么时候碎的都不知道。", color::SILVER);
    b.narr("说是上班，其实是在一个冷清的部门抄写文书。", color::SILVER);
    b.narr("上司说字要工整，我就工整。同僚把卷子推过来，我就接。", color::SILVER);
    b.narr("没什么好抱怨的，大家都这样。", color::SILVER);
    b.narr("日日夜夜的重复，我的心早已失去了波澜。", color::SILVER);
    b.narr("有时抄到一半停下来，看着自己的指尖，想—我这是在做什么。", color::SILVER);
    b.narr("然后低头，继续抄。", color::SILVER);
    b.narr("反正想也没有用。", color::SILVER);
    b.narr("同僚们偶尔闲聊，说谁被调走了，说谁再没回来。", color::SILVER);
    b.narr("我听着，偶尔点一下头，然后低头继续抄。", color::SILVER);
    b.narr("不问。不打听。不关我的事。", color::SILVER);
    b.narr("还有不定期的体检。路不远，我走得很慢。", color::SILVER);
    b.narr("走快走慢都一样。", color::SILVER);
    b.narr("屋子很白，椅子是冷的，光照下来，我坐着。", color::SILVER);
    b.narr("问什么答什么，答不上来的就不答。", color::SILVER);
    b.narr("出来的时候头有点晕，周围的一切还是原样。", color::SILVER);
    b.narr("我走回去，坐下，蘸墨，继续写。", color::SILVER);
    b.narr("体检是规矩，规矩就该守。", color::SILVER);
    b.narr("天庭定的事，总有它的道理。我不懂，也不需要懂。", color::SILVER);
    b.narr("守规矩的人不会被为难。", color::SILVER);
    b.narr("我不惹事，不偷懒，不该问的不问——", color::SILVER);
    b.narr("这样就不会被调去更冷清的地方。", color::SILVER);
    b.narr("我走回去，坐下，蘸墨，继续写。", color::SILVER);
    b.narr("忘了什么也不重要——", color::SILVER);
    b.narr("该记的早晚会记起来，不该记的，本来就不该留着。", color::SILVER);
    b.narr("杂物司。听说那里的人去了就没回来。", color::SILVER);
    b.narr("我不去那里。我按时上班，按时抄完，按时睡觉。", color::SILVER);
    b.narr("没事的，只要我一直这样，就不会有事。", color::SILVER);
    b.narr("每天都是一样的，每天都没有意外，这很安全。", color::SILVER);
    b.narr("之前的事？记得的也没用。", color::SILVER);
    b.narr("有用的自然不会忘，忘了的大概是不需要的。", color::SILVER);
    b.narr("我不去想那些了。", color::SILVER);
    b.narr("想多了容易出错，出错就要被调走——我不能去杂物司。", color::SILVER);
    b.narr("最近都在做噩梦。梦里大雪，雪里似乎有影子在追我。", color::SILVER);
    b.narr("醒来的时候喘着气，爪心是湿的，像攥过什么东西又松开了。", color::SILVER);
    b.narr("我不去想那是什么。梦就是梦。", color::SILVER);
    b.narr("醒了就好了，醒了就可以去抄文书了。", color::SILVER);
    b.narr("今天又加班到深夜。上司说这些明天要交。", color::SILVER);
    b.narr("我抄到前爪发酸，墨快干了也没停。", color::SILVER);
    b.narr("窗外的光早没了，屋子里只有灯芯那一点暗黄。", color::SILVER);
    b.narr("我低头写，一行接一行，一个字接一个字。", color::SILVER);
    b.narr("窗外的风又灌进来了，凉凉的，扑在脸上。", color::SILVER);
    b.narr("我坐了一会儿，在想明天会不会又有一摞新的。", color::SILVER);
    b.narr("想也没用。有就有，没有就没有。", color::SILVER);
    b.narr("今天又加班到深夜，我一点也不困，先睡吧。", color::SILVER);

    // =====================================================================
    //  宿舍探索（汉字地图）：先逛一逛这间破宿舍
    // =====================================================================
    // 门是离开点；床、桌、窗可查看；左下角双螭玉佩为存档点；柜/椅/简/墨/灯等均可互动。
    b.map("dorm", 18, 9, {
        {"床", "dorm_bed"}, {"桌子", "dorm_desk"}, {"窗", "dorm_window"},
        {"正门", "dorm_door"}, {"杂物间的门", "dorm_storeroom"},
        {"双螭玉佩", "dorm_save"},
        {"木柜", "dorm_cabinet"}, {"椅子", "dorm_chair"}, {"书简", "dorm_bamboo"},
        {"砚台", "dorm_ink"}, {"油灯", "dorm_lamp"}, {"文卷", "dorm_scroll"},
        {"枕头", "dorm_pillow"}, {"棉被", "dorm_quilt"}, {"墙皮", "dorm_plaster"},
        {"棉絮", "dorm_fluff"}, {"地上的灰尘", "dorm_dust"},
    });

    // ---- 上床睡觉（主线入口：睡觉 → 门无声开 → 灰白幼崽深夜到访） ----
    b.label("dorm_bed");
    b.branch("time", "清晨", "morning_bed");
    b.say("四不相", "累死了……终于可以睡觉了……", speaker::SIBUXIANG);
    b.narr("他连外袍都懒得脱，一头栽进那床薄薄的棉絮里。木板「吱呀」响了一声。");
    b.narr("周围落针可闻，只有几缕光从窗缝里透进来。");
    b.narr("那点念头还悬在半空，眼皮已沉得像灌了铅。刚沾到枕头，他就没了声息。");
    b.narr("呼吸渐渐平稳下来，变成均匀的、浅浅的起伏。窗纸破了三个洞，风从洞里灌进来，吹动他肩上的黄色绒毛。");
    b.jump("sushe_night");

    // 清晨再看床：已经天亮了，睡不回去，也不能把深夜那段重演一遍。
    b.label("morning_bed");
    b.narr("床上只剩一摊没叠的棉被，枕头还凹着一块。刚才就是从这儿弹起来的。");
    b.say("四不相", "现在哪有工夫躺回去……要迟到了。", speaker::SIBUXIANG);
    b.jump("dorm_back");

    // ---- 查看桌子 ----
    b.label("dorm_desk");
    b.narr("一张旧木桌，桌面裂着几道口子，墨渍和烛泪糊成一片。");
    b.narr("散着几卷抄了一半的文书，字迹工整得像是刻上去的。");
    b.narr("桌角搁着一盏暗黄的油灯，灯芯只有豆大一点，将灭未灭。");
    b.jump("dorm_back");

    // ---- 打开窗 ----
    b.label("dorm_window");
    b.narr("窗子糊着纸，早破了几个洞。四不相伸手推了一下，窗扇「咯吱」一声向外敞开。");
    b.narr("凉风灌进来，吹得他肩上的绒毛一抖。他缩了缩脖子。");
    b.jump("dorm_back");

    // ---- 更多可互动小物件（走近即触发查看）----
    b.label("dorm_cabinet");
    b.narr("一口老旧的木柜，柜门半掩着，里头空荡荡的，飘出一股陈年的樟木味。");
    b.narr("他伸手在柜底摸了摸，什么也没摸到，只有几粒灰。");
    b.jump("dorm_back");

    b.label("dorm_chair");
    b.narr("一把缺了腿的椅子，靠墙歪着。四不相扶了扶，它晃了晃，又歪了回去。");
    b.jump("dorm_back");

    b.label("dorm_bamboo");
    b.narr("几摞堆得整整齐齐的竹简，编绳都松了。他抽出一卷翻看，是前几日没抄完的文书。");
    b.narr("字写得工工整整，像是一个字一个字刻上去的。");
    b.jump("dorm_back");

    b.label("dorm_ink");
    b.narr("一方旧砚台，砚池里凝着半汪干涸的墨。他用指腹蹭了蹭，指头上留下一点洗不掉的青。");
    b.jump("dorm_back");

    b.label("dorm_lamp");
    b.narr("一盏油灯，灯芯烧得只剩一点。他挑了挑灯芯，火光晃了晃，又暗了下去。");
    b.jump("dorm_back");

    b.label("dorm_scroll");
    b.narr("几卷散落的文卷，边角卷着，沾着几点淡黄的烛泪。");
    b.jump("dorm_back");

    b.label("dorm_pillow");
    b.narr("一只瘪塌塌的枕头，棉花都跑偏了。他伸手按了按，软软的，带着点自己的体温。");
    b.jump("dorm_back");

    b.label("dorm_quilt");
    b.narr("一床薄薄的棉被，已经洗得发白。他拉起来掸了掸，扬起一点细小的灰。");
    b.jump("dorm_back");

    b.label("dorm_plaster");
    b.narr("墙上掉下一块墙皮，露着下面灰扑扑的土。他伸手摸了摸，凉凉的、粗糙的。");
    b.jump("dorm_back");

    b.label("dorm_fluff");
    b.narr("地上一小撮棉絮，不知从哪儿飘来的。他弯腰捡起来，放在指尖，一吹就不见了。");
    b.jump("dorm_back");

    // ---- 回到宿舍继续逛（保持互动前的位置，待在物件附近） ----
    // mapStartX/mapStartY 为负数时沿用玩家上一次站的位置，不再拉回房间中央。
    // 这张图白天、夜里、清晨都用；天亮了（时间=清晨）就改走清晨那张图和清晨的台词，
    // 否则一觉醒来看床，又会把「栽进床里睡着」重演一遍。
    // map.cpp 里另有兜底：凡是按时间改走的地图，清晨都会被转到 dorm_morning。
    b.label("dorm_back");
    b.branch("time", "清晨", "morning_back");
    b.map("dorm", -1, -1, {
        {"床", "dorm_bed"}, {"桌子", "dorm_desk"}, {"窗", "dorm_window"},
        {"正门", "dorm_door"}, {"杂物间的门", "dorm_storeroom"},
        {"双螭玉佩", "dorm_save"},
        {"木柜", "dorm_cabinet"}, {"椅子", "dorm_chair"}, {"书简", "dorm_bamboo"},
        {"砚台", "dorm_ink"}, {"油灯", "dorm_lamp"}, {"文卷", "dorm_scroll"},
        {"枕头", "dorm_pillow"}, {"棉被", "dorm_quilt"}, {"墙皮", "dorm_plaster"},
        {"棉絮", "dorm_fluff"}, {"地上的灰尘", "dorm_dust"},
    });

    // ---- 地上的灰尘（两处，同一条脚本） ----
    // 第一次互动：什么都没有；再次互动：灰尘里混着一团灰色的毛 -> 得到道具。
    // 拿到之后再来，就是「已经翻过了」。用 flag 记录状态，可随存档保存。
    // 绒毛是「重要物品」，只能得到一次——已经拿到了就永远不会再翻出来。
    b.label("dorm_dust");
    b.branch("dust", "已得", "dorm_dust_got");
    b.branch("dust", "已翻", "dorm_dust_again");
    b.narr("地上积了一层薄薄的灰尘，被踩得东一块西一块。");
    b.setFlag("dust", "已翻");
    b.jump("dorm_back");

    b.label("dorm_dust_again");
    b.narr("四不相又蹲下去，拨开那层灰。");
    b.narr("灰尘里似乎混着一团灰色的毛。");
    b.narr("他用爪尖把它挑了出来——小小一团，软得几乎没分量，灰扑扑的，边缘还沾着细灰。");
    b.narr("不知为什么，他把那团毛收进了袖口，没有扔。");
    b.setFlag("dust", "已得");
    b.item("一团灰色的绒毛", "一团软得几乎没有分量的灰色绒毛。");
    b.jump("dorm_back");

    b.label("dorm_dust_got");
    b.narr("地上的灰尘。除了灰，什么也没有了。");
    b.jump("dorm_back");

    // ---- 存档点：双螭玉佩（左下角） ----
    b.label("dorm_save");
    b.narr("一枚掌心大小的白玉佩，双螭蟠曲相对，各衔一粒青玉小珠，螭尾交缠成环。");
    b.narr("传说黄帝铸鼎时，有双螭守鼎，鼎成后化入玉中。旅人将双螭首相对，当前一刻便封入玉心；再相对，即可归返彼时。");
    b.savePrompt();
    b.jump("dorm_back");

    // ---- 开门（有选项） ----
    b.label("dorm_door");
    b.narr("门轴坏了很久，轻轻一碰就吱呀地响。");
    b.choice("要打开门吗？", {
        {"打开门", "dorm_door_open"},
        {"还是算了", "dorm_back"},
    });

    // 选「打开门」：门从外面锁着，打不开，只能作罢回宿舍。
    // 只有上床睡觉才会进入「深夜·宿舍」剧情。
    b.label("dorm_door_open");
    b.narr("四不相拉了拉门，门纹丝不动——外头像是被什么抵住了，又像是锁着。");
    b.narr("他试着推了推，门只是「吱呀」一声，又落回原位。今晚……是出不去这间屋子了。");
    b.say("四不相", "……算了，还是睡吧，明天还得早起呢。", speaker::SIBUXIANG);
    b.jump("dorm_back");

    // ---- 右墙上的杂物间的门 ----
    // 杂物房地图暂不做，这里只把「门」做出来：能看、能摸、能认出来。
    b.label("dorm_storeroom");
    b.narr("右墙上另开着一扇窄门，比正门矮半头，门板要薄些，颜色也更旧。");
    b.narr("门缝底下塞着一块卷起来的粗布，把缝堵得严严实实，透不进一丝风。");
    b.choice("要看看这扇门吗？", {
        {"摸一摸门板", "dorm_storeroom_touch"},
        {"试试能不能弄开", "dorm_storeroom_open"},
        {"算了", "dorm_back"},
    });

    b.label("dorm_storeroom_touch");
    b.narr("他抬手摸了摸。门板上落了一层手上的油灰，指腹抹过去，留下四道浅浅的印子。");
    b.narr("门把手是生了锈的铁环，冰凉。他试着转了转——纹丝不动，锁着。");
    b.narr("门上挂着一块掉了漆的木牌，字迹只剩半边：「……物」。");
    b.say("四不相", "杂物间啊。锁着。", speaker::SIBUXIANG);
    b.narr("他缩回爪尖，忽然有些好奇：那间小小的、锁着的屋子，里头会堆着什么？");
    b.narr("——不过今晚不是想这个的时候。明天还要早起。");
    b.jump("dorm_back");

    // ---- 杂物房（一整张地图：补给 + 钥匙） ----
    // 铁环锈死了，但老锁的锁舌已经裂了——用力一拽能崩开。
    // 进去以后是一间 32×14 的普通杂物间：木架、木箱、旧柜、铜匣。
    // 这里没有故事、没有账册：只有回血的药、回 TP 的香，和一把要解出来的钥匙。
    // 里间的门锁着，钥匙在铜匣里；铜匣没有锁孔，只有三个字位，
    // 刻着一行「花至＿＿，春事了」，按对「荼蘼」的「蘼」才会弹开。
    b.label("dorm_storeroom_open");
    b.narr("他两只爪子攥住铁环，整个人往后一坠——「咔」，锁舌里裂掉的老锈崩出来一点。");
    b.narr("门开了。一股陈年灰尘混着旧木头的味道涌出来，凉得像从地底下冒上来的。");
    b.say("四不相", "……就、就进去看一眼。看一眼就出来。", speaker::SIBUXIANG);
    b.jump("storeroom_enter");

    b.label("storeroom_enter");
    // 出生点传 -1：互动完回到这张图时留在原地，不要把人送回门口。
    // 第一次进来时 id_ 还是 "dorm"，房间会重建，默认就站在门下（16,12）。
    // 出口指向 dorm_back：天黑了回夜里那间宿舍，天亮了自动回清晨那间。
    b.map("storeroom", -1, -1, {
        {"杂物间的门", "dorm_back"},
        {"木架", "storeroom_shelf"},
        {"木箱", "storeroom_crate"},
        {"旧柜", "storeroom_cabinet"},
        {"铜匣", "storeroom_box"},
        {"里间的门", "storeroom_inner"},
    });

    // 一间普通的杂物间：没有故事，只有东西——补给，和一把要解出来的钥匙。
    // 每样东西只能拿一次（拿过记 flag，再来就是空的了）。

    // ---- 木架：药罐 ----
    b.label("storeroom_shelf");
    b.branch("storeroom_shelf_got", "1", "storeroom_shelf_empty");
    b.narr("木架最里层塞着几只没贴签的陶罐。他揭开一只——是药粉，还有半罐子。");
    b.setFlag("storeroom_shelf_got", "1");
    b.item("回春散", "粗陶罐里的白药粉。敷上伤口能恢复体力。（HP +40）");
    b.setFlag("hp_sibuxiang", "100");
    b.jump("storeroom_enter");

    b.label("storeroom_shelf_empty");
    b.narr("剩下的罐子都是空的，只有一股陈年的药味。");
    b.jump("storeroom_enter");

    // ---- 木箱：香 ----
    b.label("storeroom_crate");
    b.branch("storeroom_crate_got", "1", "storeroom_crate_empty");
    b.narr("他掀开箱盖。里面是半箱干草，草里埋着一只油纸包，包着几炷香。");
    b.setFlag("storeroom_crate_got", "1");
    b.item("凝神香", "一炷就够静一炷香的时间。点着能收拢散掉的灵力。（TP +30）");
    b.setFlag("tp_sibuxiang", "30");
    b.jump("storeroom_enter");

    b.label("storeroom_crate_empty");
    b.narr("箱子里只剩下干草了。");
    b.jump("storeroom_enter");

    // ---- 旧柜：柜子里也有 ----
    b.label("storeroom_cabinet");
    b.branch("storeroom_cabinet_got", "1", "storeroom_cabinet_empty");
    b.narr("旧柜的柜门一拉就开，铰链哑哑地响。柜子里分了三层，东西摆得整整齐齐。");
    b.narr("上层是一小袋蜜渍梅，中层一只扁瓷瓶，下层压着一块干粮。");
    b.setFlag("storeroom_cabinet_got", "1");
    b.item("蜜渍梅", "甜得发腻的梅子，含着能压住心慌。（HP +15）");
    b.setFlag("hp_sibuxiang", "100");
    b.item("安神汤", "瓷瓶里温着的汤药，一口下去，手脚都不抖了。（TP +20）");
    b.setFlag("tp_sibuxiang", "30");
    b.jump("storeroom_enter");

    b.label("storeroom_cabinet_empty");
    b.narr("柜子空了，只剩下一层薄薄的灰。");
    b.jump("storeroom_enter");

    // ---- 铜匣：三个字位的字锁（关键道具：铜钥匙） ----
    b.label("storeroom_box");
    b.branch("storeroom_key", "已得", "storeroom_box_got");
    b.narr("一只巴掌大的黄铜匣子，匣面上没有锁孔，只有三个凹进去的字位。匣侧刻着一行小字：");
    b.narr("「花至＿＿，春事了。」");
    b.branch("storeroom_cha", "1", "storeroom_box_half");
    b.say("四不相", "花至……什么？", speaker::SIBUXIANG);
    b.choice("要按下哪个字？", {
        {"荼", "storeroom_box_cha"},
        {"靡", "storeroom_box_mi"},
        {"蘼", "storeroom_box_li"},
    });

    // 第一格：先按「荼」。这个字是按得下去的，按下去就不弹回来了。
    b.label("storeroom_box_cha");
    b.branch("storeroom_cha", "1", "storeroom_box_half");
    b.narr("他把「荼」按了下去。「嗒」一声，字位陷进去半分——这一回没有弹回来，卡住了。");
    b.say("四不相", "……按得下去。还差一个字。", speaker::SIBUXIANG);
    b.setFlag("storeroom_cha", "1");
    b.jump("storeroom_box");

    // 「荼」已经按下了：剩下那两个字里再选一个。
    b.label("storeroom_box_half");
    b.narr("「荼」那一格已经陷下去，卡住了。匣侧那行小字还空着最后半格。");
    b.choice("再按哪一个？", {
        {"靡", "storeroom_box_mi"},
        {"蘼", "storeroom_box_li"},
    });

    b.label("storeroom_box_mi");
    b.narr("他把「靡」按了下去。匣子里响了一声，像什么被拧歪了，字位纹丝不动地卡在那儿。");
    b.say("四不相", "也不是这个。那个「靡」，是靡靡之音的靡……", speaker::SIBUXIANG);
    b.jump("storeroom_box");

    // 「蘼」：得先按下「荼」，两个都按下才开匣。
    b.label("storeroom_box_li");
    b.branch("storeroom_cha", "1", "storeroom_box_li_ok");
    b.narr("他把「蘼」按了下去。字位只陷下去一点，又慢慢弹了回来，匣子一动不动。");
    b.say("四不相", "……不对。是还差一格。", speaker::SIBUXIANG);
    b.jump("storeroom_box");

    // 两格都对：花至荼蘼。开匣，得到铜钥匙。
    b.label("storeroom_box_li_ok");
    b.narr("他把「蘼」按了下去。");
    b.narr("「嗒。」两格字位一起到底。匣子里传来一连串很轻的机括声，一层一层往里退。匣盖弹开。");
    b.narr("里面铺着一层发黄的丝绒，丝绒上躺着一把小小的铜钥匙。");
    b.say("四不相", "荼蘼……「花至荼蘼」，是春天要尽了的意思。", speaker::SIBUXIANG);
    b.setFlag("storeroom_lock", "解开");
    b.setFlag("storeroom_key", "已得");
    b.item("铜钥匙", "一把黄铜小钥匙，锁孔是「品」字形的。齿上还留着开匣时蹭下的新亮。");
    b.jump("storeroom_enter");

    b.label("storeroom_box_got");
    b.narr("铜匣空着，丝绒上只剩一个钥匙形状的浅印。");
    b.jump("storeroom_enter");

    // ---- 里间的门：要铜钥匙 ----
    b.label("storeroom_inner");
    b.branch("storeroom_key", "已得", "storeroom_inner_open");
    b.narr("右墙上还有一扇更窄的门，门板上钉着一把黄铜小锁。锁孔是「品」字形的。");
    b.say("四不相", "……得先找到能开这把锁的东西。", speaker::SIBUXIANG);
    b.jump("storeroom_enter");

    b.label("storeroom_inner_open");
    b.branch("storeroom_inner_got", "1", "storeroom_inner_empty");
    b.narr("铜钥匙插进去，只转了半圈，锁就开了。里间很小，堆着几只没封口的旧木匣。");
    b.narr("匣子里是留着的东西：一包伤药、一只暖手的铜炉，还有一吊钱。");
    b.setFlag("storeroom_inner_got", "1");
    b.item("金创药", "细布包着的黑色药膏，专治蹄子蹭破的伤。（HP +60）");
    b.setFlag("hp_sibuxiang", "100");
    b.item("铜手炉", "炉里还有一点余温。捧着它，冷得发抖的手会停下来。（TP +40）");
    b.setFlag("tp_sibuxiang", "40");
    b.jump("storeroom_enter");

    b.label("storeroom_inner_empty");
    b.narr("里间的木匣都空了。他退出来，把门重新掩上。");
    b.jump("storeroom_enter");

    // ---- 深夜，宿舍（睡觉 → 门无声开 → 灰白幼崽） ----
    // 「一、深夜，宿舍」这一段是**在地图里跑的**：
    //  · 白天那段收尾旁白一次播完（不逐句按回车），直接进深夜宿舍；
    //  · 四不遇作为地图上的一个角色出现：他自己走动、照着台词说话，
    //    绕开玩家走到床边停顿、说那两句，再走回门边离开；
    //  · 他说完「他擦了擦眼泪，离开了。」才真正从地图上消失，然后进清晨。
    // 玩家可以一边看一边在旁边走动（他自己会绕开玩家）。
    //
    // 走位说明（床在左上角：x 2~4 / y 2~5，枕头在 (2,4)、棉被在 (3,4)）：
    //   进门 (20,15) → 往里走一段 → 停在门口「哥……」 →
    //   一直走到床前 (5,4)（枕头正右边那一格）站住，
    //   剩下的台词**全部站在这儿对着床上的人说完**，不再乱走。
    // 以前是走到 (20,8) 就停在房间正中、离床很远，说「他站在床边」时
    // 人却站在房间中间，看着像发呆不动。
    b.label("sushe_night");
    {
        MapActor sy;
        sy.id = "sibuyu";
        sy.name = "四不遇";
        sy.glyph = "遇";
        sy.color = speaker::SIBUYU;
        sy.x = 20;
        sy.y = 15;
        sy.steps.push_back(ScriptBuilder::actNarr("门被轻轻拉开一条缝。没有声音。"));
        sy.steps.push_back(ScriptBuilder::actNarr("那扇门轴坏了很久了，开的时候总会吱呀一声。可它今天没有响。"));
        sy.steps.push_back(ScriptBuilder::actNarr("像一只爪子用了一万年的熟练度，让门轴连一声呻吟都没敢发出来。"));
        sy.steps.push_back(ScriptBuilder::actNarr("一只灰白色的幼崽从缝里挤进来——琥珀色的眼睛在黑暗中亮了一瞬，又暗下去，像怕自己的光吵醒了床上的白麒麟。他额上那对小角好好地立着，尾巴也是完完整整的一条。"));
        sy.steps.push_back(ScriptBuilder::actMove(20, 12));
        sy.steps.push_back(ScriptBuilder::actSay("四不遇", "哥……", speaker::SIBUYU));
        // 走到床前站住（(5,4) = 枕头右边一格），从这里开始不再走开。
        sy.steps.push_back(ScriptBuilder::actMove(5, 4, 180));
        sy.steps.push_back(ScriptBuilder::actNarr("他站在床边，很小，小到只够得着床沿。他伸出爪子，把四不相踢到地上的棉絮拾起来，抖了抖，重新盖在四不相身上。"));
        sy.steps.push_back(ScriptBuilder::actNarr("动作很轻，轻到像在给一朵快要谢了的花培土。他的爪尖没有碰到四不相。一次都没有。"));
        sy.steps.push_back(ScriptBuilder::actNarr("像怕自己一碰，那朵花就彻底谢了。"));
        sy.steps.push_back(ScriptBuilder::actSay("四不遇",
            "哥……今晚……梦里相见……你能不能不要再拒绝我……能不能……别跑……能不能想起来啊……",
            speaker::SIBUYU));
        sy.steps.push_back(ScriptBuilder::actNarr("他就蹲在床边，爪尖搭在床沿上，没有碰四不相。琥珀色的眼睛看着他，看得很深，像在看一个被锁在很深很深地方的人。"));
        sy.steps.push_back(ScriptBuilder::actNarr("他伸手够不到，他喊他听不见。他站在外面等了一万年，里面的门一直没有开。"));
        sy.steps.push_back(ScriptBuilder::actNarr("风从窗缝里灌进来，吹动他尾巴尖上那撮软毛。他缩了一下，把自己蜷得更小了一点，像一朵不敢开大的花。"));
        sy.steps.push_back(ScriptBuilder::actNarr("他擦了擦眼泪，离开了。"));
        sy.steps.push_back(ScriptBuilder::actMove(20, 12));
        sy.steps.push_back(ScriptBuilder::actMove(20, 15, 180));
        sy.steps.push_back(ScriptBuilder::actDisappear());
        sy.steps.push_back(ScriptBuilder::actNarr("门关上了。屋子里又暗下来，只剩下他一个人，和一屋子睡着的旧家具。"));
        sy.steps.push_back(ScriptBuilder::actNarr("他不知道自己在那儿站了多久，也不知道自己是什么时候又睡着的。梦像水一样漫上来，把一切都盖住了。"));
        sy.steps.push_back(ScriptBuilder::actFinish());

        std::vector<MapActor> actors;
        actors.push_back(sy);
        // 玩家的出口：床上 → 清晨。深夜这一段四不相是睡着的，动不了，
        // 也不该动——这是他的梦，只有四不遇在动。演完自动接「二、清晨」。
        b.mapWithActors("dorm_night", -1, -1, {{"床", "dorm_wake"}}, actors);
    }


    // =====================================================================
    //  二、第二天清晨，宿舍
    // =====================================================================
    // 顺序很重要：先在地图**之前**把「醒来」演完（旁白 + 四不相那句台词），
    // 全部按回车读完，**最后**才进 dorm_morning 地图。地图是给玩家操作的，
    // 剧情不在那张图上跑。
    // 昨夜床边那只灰白幼崽、那朵开在梦里的花，都要等天亮才有答案。
    b.label("dorm_wake");
    b.cls();
    b.scene("二 · 清晨，宿舍", "scene_room_morning");
    // 「闹钟响起」和「窗外天已经大亮了」这两句不再在进地图前念一遍——
    // 进 dorm_morning 以后它们会自动挂到地图下方的字幕栏里（见 game.cpp 的
    // morning_intro）。这里只留四不相那一句，免得同一段说明出现两遍。
    b.say("四不相", "要迟到了，得赶快过去", speaker::SIBUXIANG);
    b.setFlag("time", "清晨");   // 天亮了：这时候再去看床，不会再睡回去

    // 清晨宿舍的出口表只有这一份：地图上的每一个地点名都必须在这里出现，
    // 名字写错了就点不动（「打不开」「出不去」多半是这里对不上）。
    std::vector<MapExit> morningExits = {
        {"正门", "morning_door"}, {"杂物间的门", "morning_storeroom"},
        {"荼蘼花", "morning_flower"}, {"双螭玉佩", "morning_save"},
        {"桌子", "morning_desk"}, {"窗", "morning_window"},
        {"木柜", "morning_cabinet"}, {"椅子", "morning_chair"}, {"书简", "morning_bamboo"},
        {"砚台", "morning_ink"}, {"油灯", "morning_lamp"}, {"文卷", "morning_scroll"},
        {"枕头", "morning_bed_view"}, {"棉被", "morning_quilt"}, {"墙皮", "morning_desk"},
        {"棉絮", "morning_quilt"}, {"地上的灰尘", "morning_dust"},
    };
    // 天亮后的宿舍：从这里开始交回操控权（负数坐标 = 沿用玩家位置，没走过就按地图默认出生点）
    b.label("sushe_scene");
    b.map("dorm_morning", -1, -1, morningExits);

    // ---- 床脚那朵花：天亮以后，它才真的在地板上 ----
    b.label("morning_flower");
    b.narr("他低头看了看自己的爪尖——昨夜是不是攥过什么，他记不清了。");
    b.narr("床脚的地板上，开着一朵荼蘼。");
    b.narr("花瓣是极淡的白，白到几乎透明，花心一点琥珀色的蕊——在这间满是旧纸和灰尘的屋子里，干净得不像真的。");
    b.say("四不相", "……荼蘼？哪来的荼蘼？", speaker::SIBUXIANG);
    b.narr("他俯身碰了碰。花瓣凉得像水，一碰就抖了一下，抖落一层极细的粉。");
    b.narr("荼蘼是春天最后开的花。开到荼蘼，春天就尽了。");
    b.narr("他蹲在那儿看了很久，最后什么也没敢动，只是把公文捡起来，拍掉灰。指尖上留了一点凉，很久都没有散。");
    b.jump("morning_back");

    // ---- 出门：抓公文、绊门槛、一路狂奔 ----
    b.label("morning_door");
    b.narr("他抓起桌上的公文，爪尖都在抖，一把拉开了正门。");
    b.narr("跑出门的时候，后蹄被门槛绊了一下，左爪蹭掉了一小块，他没空看。");
    b.narr("他一路狂奔。他不知道自己在逃什么。他只是觉得，不能停下来。一停下来，就会被什么东西追上。");
    b.jump("corridor_intro");

    // 清晨在宿舍里查看东西：看完回到清晨那张图（不是昨夜那张）。
    b.label("morning_back");
    b.map("dorm_morning", -1, -1, morningExits);

    // 清晨的物件各自一套台词——天亮了，说的话也不能和昨晚一样。
    b.label("morning_bed_view");
    b.narr("床上只剩一摊没叠的棉被，枕头还凹着一块。刚才就是从这儿弹起来的。");
    b.say("四不相", "现在哪有工夫躺回去……要迟到了。", speaker::SIBUXIANG);
    b.jump("morning_back");

    b.label("morning_desk");
    b.narr("公文散了一桌子，昨夜抄到一半的那几卷还摊着。墨已经干在砚池底了。");
    b.say("四不相", "……这些今天要交的。", speaker::SIBUXIANG);
    b.narr("他把散开的竹简一卷卷收拢，爪尖抖得厉害。");
    b.jump("morning_back");

    b.label("morning_window");
    b.narr("晨光从破纸洞里直直地灌进来，晃得他眯起眼。");
    b.say("四不相", "天都这么亮了……快走快走。", speaker::SIBUXIANG);
    b.jump("morning_back");

    b.label("morning_cabinet");
    b.narr("木柜里还是空的，樟木味散了一夜淡了不少。");
    b.jump("morning_back");

    b.label("morning_chair");
    b.narr("椅子还歪在墙边。昨夜它就是这么歪着的，今天也是。");
    b.jump("morning_back");

    b.label("morning_bamboo");
    b.narr("竹简摞得整整齐齐。他抽出最上面一卷，又塞了回去——没时间看了。");
    b.jump("morning_back");

    b.label("morning_ink");
    b.narr("砚池里的墨干成了硬块，得重新磨。他把砚台往旁边一推。");
    b.jump("morning_back");

    b.label("morning_lamp");
    b.narr("灯早就灭了，灯芯焦黑。昨夜他好像点到了很晚。");
    b.jump("morning_back");

    b.label("morning_scroll");
    b.narr("散落的文卷还在地上，他弯腰捡起两卷，剩下的来不及了。");
    b.jump("morning_back");

    b.label("morning_quilt");
    b.narr("棉被堆成一团，他本想叠，手伸到一半又收回去——算了。");
    b.jump("morning_back");

    b.label("morning_dust");
    // 灰尘只有一处脚本：昨夜没翻出绒毛的，清晨还在这里；翻过了就是一片空灰。
    b.jump("dorm_dust");

    b.label("morning_save");
    b.jump("dorm_save");

    // ---- 右墙那扇杂物间的门：昨天锁死，今早还在，可以趁现在进去 ----
    b.label("morning_storeroom");
    b.narr("右墙上那扇窄门还在，门缝下那块粗布也还堵着。");
    b.choice("要进去看看吗？", {
        {"弄开它", "dorm_storeroom_open"},
        {"算了，要迟到了", "morning_back"},
    });

    // =====================================================================
    //  三、天庭，回廊
    //  一条会拐弯的四段长回廊：西廊 → 中廊（向南折）→ 东廊（S 形）→ 困兽府前。
    //  · 每段都**不止一个出口**：东头、北头、南头、屋顶都开着拱门，
    //    走上去就换段，不用按回车——走错了就得绕一圈，回廊本来就是这样；
    //  · 两旁的墙上立着神仙府邸，要按回车才推得开；
    //  · 回廊本身也不是一条空走廊：廊柱、花坛、池水、水渠、石阶、高台、
    //    铜鼎、屏风，该绕的地方就得绕，该看的东西凑近了能看；
    //  · 每段都立着一块「记事碑」，站在上面按回车就能存档；
    //  · 一进中廊就撞上四不遇：那一段玩家**完全不能操作**，脚自己往前撞，
    //    台词全部念完才把操控权还回来；
    //  · 开场那几句不在地图之前吞掉，而是逐句挂在地图**下方**（见 game.cpp 里
    //    「不出场、只说话」的角色）。
    // =====================================================================

    // 回廊每一段各有一份出口表。地点名必须和 map.cpp 里登记的完全一致，
    // 差一个字就点不动。拱门是「自动门」：走上去就换段，不用按回车。
    // 记事碑是存档点；窗 / 花坛 / 铜鼎是「凑近了按回车看一眼」的地方。
    std::vector<MapExit> c1 = {
        {"回廊西口", "corridor1_west"},
        {"回廊东口", "corridor2_start"},
        {"回廊北口", "corridor2_start"},
        {"白泽府", "mansion_baize"},
        {"獬豸府", "mansion_xiezhi"},
        {"杂物司", "mansion_zawu"},
        {"记事碑", "corridor1_save"},
        {"窗", "corridor1_window"},
        {"花坛", "corridor1_flower"},
    };
    std::vector<MapExit> c2 = {
        {"回廊西口", "corridor1_east"},
        {"回廊北口", "corridor1_north"},
        {"回廊南口", "corridor3_start"},
        {"回廊东门", "corridor3_start"},
        {"斗姆宫", "mansion_doumu"},
        {"月老祠", "mansion_yuelao"},
        {"药王殿", "mansion_yaowang"},
        {"织云坊", "mansion_zhiyun"},
        {"记事碑", "corridor2_save"},
    };
    std::vector<MapExit> c3 = {
        {"回廊西口", "corridor2_east"},
        {"回廊北口", "corridor2_north"},
        {"回廊南口", "corridor4_start"},
        {"回廊东口", "corridor4_start"},
        {"九尾府", "mansion_jiuwei"},
        {"文昌阁", "mansion_wenchang"},
        {"鹿蜀府", "mansion_lushu"},
        {"记事碑", "corridor3_save"},
        {"窗", "corridor3_window"},
    };
    std::vector<MapExit> c4 = {
        {"回廊西口", "corridor3_east"},
        {"回廊北口", "corridor3_north"},
        {"文书阁", "office"},
        {"困兽府", "mansion_kunshou"},
        {"记事碑", "corridor4_save"},
        {"窗", "corridor4_window"},
        {"铜鼎", "corridor4_ding"},
    };

    // ---- 进回廊：先让地图立起来，旁白挂在地图下面一句一句说 ----
    b.label("corridor_intro");
    b.branch("corridor_intro_seen", "1", "corridor1_start");
    b.setFlag("corridor_intro_seen", "1");
    {
        MapActor nar;
        nar.id = "narrator";
        nar.name = "旁白";
        nar.x = 2;
        nar.y = 5;
        nar.unseen = true;   // 不画在地图上，只负责把话挂到地图下方
        nar.steps.push_back(ScriptBuilder::actNarr(
            "天庭的走廊很长。晨光从高大的窗棂间漏下来，在地砖上切成一段一段的光格，像一道一道看不见的栅栏。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "两排廊柱高得快顶到梁上。柱脚边是花坛、铜鼎、半人高的屏风，廊下还凿着一条水渠，水声细得像有人在很远的地方翻书。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "这条回廊岔口很多：东头、北头、南头都开着拱门，走错一个就得绕着天庭多跑半个圈。"));
        nar.steps.push_back(ScriptBuilder::actSay("四不相", "借过……借过……要迟到了……",
                                                 speaker::SIBUXIANG));
        nar.steps.push_back(ScriptBuilder::actFinish());

        std::vector<MapActor> actors;
        actors.push_back(nar);
        b.mapWithActors("corridor1", 2, 5, c1, actors);
    }

    // ---- 西廊（两排廊柱、花坛、一池静水，三个拱门）----
    b.label("corridor1_start");
    b.map("corridor1", 2, 5, c1);
    b.label("corridor1_back");
    b.map("corridor1", -1, -1, c1);       // 负数坐标 = 留在原地，看完东西回到门前
    b.label("corridor1_east");
    b.map("corridor1", 38, 5, c1);        // 从中廊往西折返：落在西廊东头
    b.label("corridor1_north");
    b.map("corridor1", 33, 1, c1);        // 从中廊北口绕回来：落在西廊北阶底下

    b.label("corridor1_west");
    b.narr("回廊在这里拐下去，通往宿舍那一头。楼梯上落着一层薄灰，连个脚印都没有。");
    b.say("四不相", "……现在折回去，只会更迟。", speaker::SIBUXIANG);
    b.jump("corridor1_back");

    b.label("corridor1_save");
    b.savePrompt();
    b.jump("corridor1_back");

    b.label("corridor1_window");
    b.narr("窗外的云压得很低，一片一片，像没抄完的纸。");
    b.jump("corridor1_back");

    b.label("corridor1_flower");
    b.narr("花坛里种着一栏白花，开得很整齐，整齐得不像自己长出来的。");
    b.narr("四不相认得这花。春天最后开的那种。可他一时想不起名字，只觉得鼻尖发酸。");
    b.jump("corridor1_back");

    b.label("mansion_baize");
    b.narr("白泽府的门虚掩着，里面传出翻书的声音，一页一页，慢得像在数云。");
    b.narr("门口立着一块木牌，字很旧了：「有问必答，无问勿扰」。");
    b.say("四不相", "……我倒是想问。可我问什么呢。", speaker::SIBUXIANG);
    b.jump("corridor1_back");

    b.label("mansion_xiezhi");
    b.narr("獬豸府门前摆着一只铜鼎，鼎里插着三炷没点过的香。");
    b.narr("门上钉着一张判词，墨还很新：「是非曲直，一触即知」。");
    b.say("四不相", "……这个我可不敢碰。", speaker::SIBUXIANG);
    b.jump("corridor1_back");

    b.label("mansion_zawu");
    b.narr("杂物司的门上贴着封条，黄纸，红印已经褪成了粉色。门缝里透出旧纸和霉味。");
    b.say("四不相", "杂物司……", speaker::SIBUXIANG);
    b.narr("他心里莫名咯噔了一下，像有人在他看不见的地方，替他把这三个字念了一遍。");
    b.narr("可他从来没来过这儿。他连这个名字都是刚才才看见的。");
    b.jump("corridor1_back");

    // ---- 中廊（向东、再向南折下去，院心一畦花坛）：一进来就撞上四不遇 ----
    b.label("corridor2_start");
    b.branch("met_sibuyu", "1", "corridor2_plain");
    {
        // 这一整段玩家不能操作：方向键、WASD、鼠标点击全都不响应，
        // 只有空格 / 回车能翻台词。四不相的脚由剧本一步一步往前挪，
        // 四不遇从东头迎面走来，两个人撞在一起，台词念完才把操控还回去。
        MapActor yu;
        yu.id = "sibuyu";
        yu.name = "四不遇";
        yu.glyph = "遇";
        yu.color = speaker::SIBUYU;
        yu.x = 34;
        yu.y = 3;
        yu.steps.push_back(ScriptBuilder::actWait(220));
        yu.steps.push_back(ScriptBuilder::actNarr(
            "回廊那头先响起来的不是脚步声，是披风擦过地砖的声音。很轻，慢得不像在赶路。"));
        yu.steps.push_back(ScriptBuilder::actMove(30, 3, 170));
        yu.steps.push_back(ScriptBuilder::actPMove(21, 3, 80));   // 四不相低头猛跑
        yu.steps.push_back(ScriptBuilder::actMove(22, 3, 150));   // 正好走到跟前
        yu.steps.push_back(ScriptBuilder::actNarr(
            "四不相刹不住。连麒麟带公文，结结实实地撞了上去。竹简散了一地，有几卷滚出去老远。"));
        yu.steps.push_back(ScriptBuilder::actSay("四不相",
            "对不起对不起对不起……我不是故意的……我马上收拾……", speaker::SIBUXIANG));
        yu.steps.push_back(ScriptBuilder::actNarr(
            "他跪下去，头埋得极低，爪尖慌乱地捡着散落的竹简。他不敢抬头。"));
        yu.steps.push_back(ScriptBuilder::actNarr(
            "可他瞥见了那件披风——黑的，上面绣着暗红色的凶兽纹。以及，侧脸上那个画着眯眯眼的面具。"));
        yu.steps.push_back(ScriptBuilder::actNarr(
            "四不相的脑子「嗡」了一声。典狱长。他撞到的是困兽府那位典狱长。"));
        yu.steps.push_back(ScriptBuilder::actSay("四不相",
            "典狱长大人……对不起……求您别罚我……我今天已经迟了……我不能再被调走了……",
            speaker::SIBUXIANG));
        yu.steps.push_back(ScriptBuilder::actNarr(
            "那个身影没有说话。四不相听见风从回廊尽头灌过来，吹动那件披风的边缘。"));
        yu.steps.push_back(ScriptBuilder::actSay("典狱长", "……你走吧。我不怪你。",
                                                 speaker::SIBUYU));
        yu.steps.push_back(ScriptBuilder::actNarr(
            "四不相的爪子停了一下。那个声音，他好像在哪里听过——像雪落在爪心里，凉一下，就化了。"));
        yu.steps.push_back(ScriptBuilder::actNarr(
            "他抬了一下眼，就一下。看见了一双琥珀色的眼睛，很静，很暗，像一颗被月光泡了一万年的石头。"));
        yu.steps.push_back(ScriptBuilder::actNarr(
            "那双眼睛也在看他，看得很深，像在辨认一个被遗忘了很久的名字。"));
        yu.steps.push_back(ScriptBuilder::actSay("四不相",
            "谢、谢谢典狱长大人……我……我先走了……", speaker::SIBUXIANG));
        yu.steps.push_back(ScriptBuilder::actNarr(
            "四不相抱着公文往南边跑。跑过光与暗的分界，跑过一根一根廊柱。他没有回头。"));
        yu.steps.push_back(ScriptBuilder::actNarr(
            "他觉得自己应该回头。可他没有。他不知道自己错过了什么。"));
        yu.steps.push_back(ScriptBuilder::actMove(38, 3, 150));
        yu.steps.push_back(ScriptBuilder::actDisappear());
        yu.steps.push_back(ScriptBuilder::actNarr(
            "身后的回廊里，那个身影蹲了下来。地上有一根白色的绒毛，是四不相刚才蹭掉的，还带着一点晨光的温度。"));
        yu.steps.push_back(ScriptBuilder::actNarr(
            "他捡起来，贴在脸颊上，轻轻蹭了一下，像碰一个隔了一万年的温度。然后他闭上了眼睛。"));
        yu.steps.push_back(ScriptBuilder::actNarr(
            "再睁眼时，一滴清泪从眼眶里滑出来，砸在地砖上，绽成一朵极小的荼蘼花。"));
        yu.steps.push_back(ScriptBuilder::actSay("典狱长", "……你跑什么。我又不会罚你。",
                                                speaker::SIBUYU));
        yu.steps.push_back(ScriptBuilder::actNarr(
            "他把那朵花和那根绒毛一起收进袖口，爪尖攥得那么紧，仿佛一松开，那根绒毛也会像四不相一样，从他爪心里跑掉。"));
        yu.steps.push_back(ScriptBuilder::actSay("典狱长", "……杂物司……不能让你去。",
                                                speaker::SIBUYU));
        yu.steps.push_back(ScriptBuilder::actFinish());

        std::vector<MapActor> actors;
        actors.push_back(yu);
        b.mapWithActors("corridor2", 2, 3, c2, actors);
    }
    // 演完了：操控权在这里交回给玩家（见 corridor2_after）
    b.setFlag("met_sibuyu", "1");
    b.jump("corridor2_after");

    b.label("corridor2_plain");
    b.map("corridor2", 2, 3, c2);
    b.label("corridor2_back");
    b.map("corridor2", -1, -1, c2);
    b.label("corridor2_after");
    b.map("corridor2", -1, -1, c2);
    b.label("corridor2_east");
    b.map("corridor2", 8, 11, c2);        // 从东廊往西折返：落在南口前的大院里
    b.label("corridor2_north");
    b.map("corridor2", 33, 1, c2);        // 从东廊北口绕回来：落在中廊北拱门下

    b.label("corridor2_save");
    b.savePrompt();
    b.jump("corridor2_back");

    b.label("mansion_doumu");
    b.narr("斗姆宫的檐下垂着一圈星图，二十八宿一格一格地排着，其中有一颗是暗的。");
    b.narr("四不相数到第十七颗就数不下去了——他不认得那些名字。");
    b.jump("corridor2_back");

    b.label("mansion_yuelao");
    b.narr("月老祠的红线从门缝里垂出来，垂了一地，红得晃眼。");
    b.narr("四不相绕着走，怕踩到。可走到一半，他发现有一根线的两头，都断着。");
    b.jump("corridor2_back");

    b.label("mansion_yaowang");
    b.narr("药王殿里飘着苦味。殿门半开，门槛上搁着一只铜盆，盆里泡着几根叫不出名字的草。");
    b.narr("四不相的鼻子皱了皱，忽然觉得后蹄上那块擦伤不那么疼了。");
    b.jump("corridor2_back");

    b.label("mansion_zhiyun");
    b.narr("织云坊的门口挂着半匹没织完的云，白得像刚洗过的绢。");
    b.narr("风一吹，云就丝丝缕缕地散开，散到一半又被谁的手收了回去。");
    b.jump("corridor2_back");

    // ---- 东廊（S 形：向东、折向南、再折回向东；廊心有石阶、廊下有水渠）----
    b.label("corridor3_start");
    b.map("corridor3", 2, 3, c3);
    b.label("corridor3_back");
    b.map("corridor3", -1, -1, c3);
    b.label("corridor3_east");
    b.map("corridor3", 38, 10, c3);       // 从困兽府前折返：落在东廊东头
    b.label("corridor3_north");
    b.map("corridor3", 22, 1, c3);        // 从困兽府前绕北口回来：落在东廊北头

    b.label("corridor3_save");
    b.savePrompt();
    b.jump("corridor3_back");

    b.label("corridor3_window");
    b.narr("这扇窗的纸是新糊的，糊得歪，边上还留着一道没抹平的手指印。");
    b.jump("corridor3_back");

    b.label("mansion_jiuwei");
    b.narr("九尾府的门关得很严，门环上缠着一绺白毛，不知是谁留下的。");
    b.narr("四不相踮脚看了看，又低头看看自己的尾巴——不是他的毛。他的毛短，这绺毛长，软，还带着一点凉。");
    b.jump("corridor3_back");

    b.label("mansion_wenchang");
    b.narr("文昌阁的檐角挂着风铃，风铃下面压着一叠文书，被雨打湿又晒干，纸边全卷了起来。");
    b.narr("这里不是文书阁。名字像，门也不是。他站在门口看了两眼，又走了。");
    b.jump("corridor3_back");

    b.label("mansion_lushu");
    b.narr("鹿蜀府的门半开着，里面有人在唱一支很老的调子，唱到一半忽然停了。");
    b.narr("停下来的那一瞬间，四不相听见了自己的脚步声，响得吓人。");
    b.jump("corridor3_back");

    // ---- 困兽府前（回廊的尽头，摊开一片小广场）----
    b.label("corridor4_start");
    b.map("corridor4", 2, 2, c4);
    b.label("corridor4_back");
    b.map("corridor4", -1, -1, c4);

    b.label("corridor4_save");
    b.savePrompt();
    b.jump("corridor4_back");

    b.label("corridor4_window");
    b.narr("窗棂上积着灰。从这个角度看出去，正好能看见困兽府的屋脊，黑压压的一条。");
    b.jump("corridor4_back");

    b.label("corridor4_ding");
    b.narr("广场角上立着一只半人高的铜鼎，鼎身刻着缠在一处的鳞和爪，刻得太深，摸上去硌手。");
    b.narr("鼎里没有香，只有一层积了很多年的灰。四不相看了两眼，缩回了手。");
    b.jump("corridor4_back");

    b.label("mansion_kunshou");
    b.narr("一座紧闭着的府邸矗立着，匾额上，“困兽府”几个苍劲有力的字跃然其上，雕花的窗棂黯淡。");
    b.narr("门不是黑的，只是比较暗，暗得连门环都看不太清。");
    b.narr("门前挂着灯，暖黄的一小团，风一吹就轻轻地晃——像是……等着什么人回来，好一起进去。");
    b.say("四不相", "……典狱长就住在这里面？", speaker::SIBUXIANG);
    b.narr("他的脚步在门前慢了半拍。他不知道自己为什么慢。");
    b.jump("corridor4_back");

    // =====================================================================
    //  四、文书阁：四不相干活的地方（一个能走的地图，40×16 的一大间）
    //  一进门，最里头那个上司就放下竹简走了过来——这一段玩家**不能操作**，
    //  方向盘、WASD、鼠标点击全部不响应，只有空格 / 回车能翻台词；
    //  台词全部念完、他转身走回桌后，操控权才交回来。
    //  之后可以在屋里自由走动：跟同事打听、翻自己桌上的东西、从门出去。
    // =====================================================================
    std::vector<MapExit> officeExits = {
        {"上司", "office_boss_again"},
        {"同事", "office_colleague"},
        {"座位", "office_desk"},
        {"笔墨", "office_ink"},
        {"案卷", "office_scroll"},
        {"油灯", "office_lamp"},
        {"窗", "office_window"},
        {"记事碑", "office_save"},
        {"门", "office_leave"},
    };
    b.label("office");
    b.branch("office_intro_seen", "1", "office_reenter");
    b.setFlag("office_intro_seen", "1");
    {
        // 上司：从最里头那张桌后一路走过来，说完再走回去。
        // 他的出场格就是桌后那一格——地图上先不摆「司」，免得画成两个人。
        MapActor boss;
        boss.id = "shangsi";
        boss.name = "上司";
        boss.glyph = "司";
        boss.color = speaker::SHANGSI;
        boss.x = 34;
        boss.y = 2;
        boss.steps.push_back(ScriptBuilder::actNarr(
            "文书阁的门在身后合上，一股旧纸、陈墨和许多人身上的暖气扑面而来。"));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "屋里比他想的还要满：两排长案，伏在案上抄写的人一个挨着一个，笔尖沙沙地响，谁也没抬头。"));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "最里头那张桌后的人放下竹简，绕过桌角，一步一步朝他走了过来。"));
        boss.steps.push_back(ScriptBuilder::actMove(4, 13, 90));   // 一路走到门口，停在四不相跟前
        boss.steps.push_back(ScriptBuilder::actNarr(
            "四不相抱着公文，站在门边，一格也不敢挪。"));
        boss.steps.push_back(ScriptBuilder::actSay("上司", "又踩点。", speaker::SHANGSI));
        boss.steps.push_back(ScriptBuilder::actSay("四不相",
            "对……对不起……我路上撞到了人……", speaker::SIBUXIANG));
        boss.steps.push_back(ScriptBuilder::actSay("上司", "撞到人了？", speaker::SHANGSI));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "上司挑了一下眉，目光落在四不相身上——毛发乱糟糟的，爪尖缺了一小块。他忽然笑了一下。"));
        boss.steps.push_back(ScriptBuilder::actSay("上司", "撞到谁了？这么大阵仗。", speaker::SHANGSI));
        boss.steps.push_back(ScriptBuilder::actSay("四不相",
            "……困兽府的、典狱长大人……", speaker::SIBUXIANG));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "上司的笑容僵了一瞬。他手里的竹简放下来了，眼神从「敷衍」变成了「审慎」。"));
        boss.steps.push_back(ScriptBuilder::actSay("上司", "典狱长？你撞到四不遇了？", speaker::SHANGSI));
        boss.steps.push_back(ScriptBuilder::actSay("四不相", "……是。", speaker::SIBUXIANG));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "上司沉默了一会儿。他重新打量了一下这只白毛麒麟——正缩着脖子，像一只等着被踹的旧棉絮。"));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "典狱长都没说什么，他再追究，倒显得自己比典狱长还大牌了。"));
        boss.steps.push_back(ScriptBuilder::actSay("上司",
            "行了。下次再迟到，自己去交调任文书。不用等我来批。", speaker::SHANGSI));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "他转身走回最里头，竹简重新拿起来，像什么都没发生过。"));
        boss.steps.push_back(ScriptBuilder::actMove(34, 2, 120));   // 说完再转身走回最里头
        // ---- 上司走了：剩下的都是四不相自己的心思 ----
        // 这一段不再 cls 掉地图：台词全部挂在这张地图的**下方**字幕条里，
        // 玩家一边看着文书阁（上司已经回到最里头那张桌后），一边按空格往下读。
        boss.steps.push_back(ScriptBuilder::actNarr(
            "四不相站在原地，爪尖还抱着那摞竹简，前爪边缘渗着一丝极淡的暗金色——是刚才撞到典狱长时蹭出来的。"));
        boss.steps.push_back(ScriptBuilder::actNarr("他低头看着那一丝颜色，愣了一下。"));
        boss.steps.push_back(ScriptBuilder::actSay("四不相", "……暗金色的血？", speaker::SIBUXIANG));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "他举起那只爪尖，对着窗漏进来的光看了看。暗金色的，很淡，像混进白色绒毛里的一线将熄未熄的余烬。"));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "他翻来覆去看了好一会儿，想不起来自己怎么会流这种颜色的血。"));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "最后，他把爪尖那一点暗金色擦掉了。他不知道自己为什么擦掉它。他只是觉得，留着那一点颜色，心口会更疼。"));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "他拿起笔，蘸墨，低头抄写。笔尖落在纸上的时候，他停了一下。"));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "他想起那双眼睛。琥珀色的。整个天庭都知道的典狱长，手段狠辣、不近人情。可那双眼睛看他的时候，为什么像在求他？"));
        boss.steps.push_back(ScriptBuilder::actSay("四不相", "……他为什么那样看我？", speaker::SIBUXIANG));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "他问完愣了一下。他只是一个抄文书的，连迟到都要被罚的那种。典狱长怎么看他，跟他有什么关系？"));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "可写了两个字，笔又停了。他忽然发现，自己在回想那个声音——「你走吧，我不怪你。」"));
        boss.steps.push_back(ScriptBuilder::actNarr("那个声音很轻，像怕压碎什么。可他为什么要怕压碎我？"));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "他把笔搁下，站起来，走到窗边，看着窗纸上破碎的光。风从破了的窗纸缝里渗进来，凉凉的，扑在他脸上。"));
        boss.steps.push_back(ScriptBuilder::actSay("四不相", "……我们见过吗？", speaker::SIBUXIANG));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "没有人回答他。他站在窗边，看着灰白色的天光从云缝里漏下来，落在窗台的石沿上。"));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "他的胸口又酸了一下，又空了一下，像有什么东西正被一只看不见的手从里面掏走，一点一点地，掏了一万年。"));
        boss.steps.push_back(ScriptBuilder::actFinish());

        std::vector<MapActor> actors;
        actors.push_back(boss);
        b.mapWithActors("office", 4, 13, officeExits, actors);
    }

    // ---- 内心选择（演示选择 / 变量 / 分支，为后续「温茶」结局铺垫） ----
    b.choice("你的心口泛着说不清的酸涩。这一刻，你想——", {
        {"我……好像在哪里听过那个声音。", "hui_xiang_yes"},
        {"不，别想了。被记住，是一件危险的事。", "hui_xiang_no"},
    });

    b.label("hui_xiang_yes");
    b.setFlag("hui_xiang", "回想");
    b.narr("那个声音，像雪落在爪心里。你越是想抓住，它化得越快，可凉的那一下，是真的。", color::SILVER);
    b.jump("hui_xiang_end");

    b.label("hui_xiang_no");
    b.setFlag("hui_xiang", "压抑");
    b.narr("你强迫自己把那个声音按下去。被记住是危险的——一旦有人记住你，那个人就会替你承受些什么。", color::SILVER);
    b.jump("hui_xiang_end");

    b.label("hui_xiang_end");
    b.narr("他转身，回到桌前坐下，继续抄写。");
    b.narr("而在文书阁门外很远的地方，一只灰白色的幼崽蹲在廊柱的阴影里，琥珀色的眼睛看着他所在的那扇窗户。");
    b.narr("他一直在看窗户边那道瘦弱的影子——看他停下笔、站起来、走到窗边、开口、沉默、然后回去坐下。");
    b.narr("他看了一整段沉默，像看一颗流星划过夜空。他不知道，那是最后一眼。");
    b.say("灰白色幼崽", "……见过。你梦过我。你捏过我。你等过我。我……也等过你。", speaker::SIBUYU);
    b.narr("他把绒毛贴在胸口，闭上了眼睛。再睁开时，琥珀色的光在眼底闪了一下，又暗下去了。");
    b.narr("他站起来，转身走回困兽府的方向。尾巴在风里垂着，完整、安静，像一根快要燃尽的香，还在撑着最后一寸不肯灭。");

    // ---- 回到文书阁：正戏演完，操控权交回来，可以走一圈再出门 ----
    b.jump("office_free");

    b.label("office_free");
    b.map("office", 4, 13, officeExits);

    b.label("office_plain");      // 看完东西 / 说完话，回到原地
    b.map("office", -1, -1, officeExits);

    b.label("office_reenter");    // 从回廊再进来（开场那一趟已经演过了）
    b.map("office", 4, 13, officeExits);

    b.label("office_leave");      // 想走：文卷还没抄完，他自己又回了桌前
    b.narr("他摸到门边，爪尖搭在门上，回头看了一眼自己那张桌子——昨天没抄完的文卷还摊在那儿，笔也没洗。");
    b.narr("「明天早会前交齐。」他不敢就这么走出去。");
    b.jump("office_seat");

    // ---- 坐下抄到天黑（全程挂在地图下方）----
    // walkBack=true ：从门口被「明天早会前交齐」逼回来，他自己一路走回案边
    //                 （这一段玩家本来就不能操作，走位由剧本给）；
    // walkBack=false：玩家自己已经走到案边、按了回车，就地坐下，绝不再演一遍
    //                 「四不相自己走到属于他的桌旁」。
    auto eveningMontage = [&](bool walkBack) {
        MapActor nar;
        nar.id = "narrator";
        nar.name = "旁白";
        nar.x = 29;
        nar.y = 4;
        nar.unseen = true;
        if (walkBack) nar.steps.push_back(ScriptBuilder::actPMove(29, 4, 90));   // 只有这一种情形才让他自己走
        nar.steps.push_back(ScriptBuilder::actNarr(
            "他把凳子摆正，坐下来，文卷铺开，砚台挪到手边，笔尖在砚池里转了两圈，落纸。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "一整个下午，文书阁里只有笔尖擦过纸面的声音。抄错一个字，整页重来；抄到眼花，就抬头看一眼窗。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "窗上的光从这一格挪到那一格，又一点一点淡下去。他抄完一卷，摞好，再抽下一卷。"));
        nar.steps.push_back(ScriptBuilder::actFinish());

        // 上司还坐在最里头那张桌后（这一段不画「司」那一格，由角色顶上）。
        MapActor boss;
        boss.id = "shangsi";
        boss.name = "上司";
        boss.glyph = "司";
        boss.color = speaker::SHANGSI;
        boss.x = 34;
        boss.y = 2;
        boss.steps.push_back(ScriptBuilder::actWait(1));   // 一直在桌后坐着（这一步什么都不做，只是把他摆在桌上）

        std::vector<MapActor> actors;
        actors.push_back(nar);
        actors.push_back(boss);
        b.mapWithActors("office", walkBack ? 4 : -1, walkBack ? 13 : -1, officeExits, actors);
    };

    b.label("office_sit");        // 人已经站在案边了：直接坐下
    eveningMontage(false);
    b.jump("office_dusk");

    b.label("office_seat");       // 想偷偷走掉，被逼回来：先走回案边
    eveningMontage(true);
    b.jump("office_dusk");

    // ---- 入夜：同僚一个个走了，上司也走了，他吹灯出门 ----
    b.label("office_dusk");
    {
        MapActor boss;
        boss.id = "shangsi";
        boss.name = "上司";
        boss.glyph = "司";
        boss.color = speaker::SHANGSI;
        boss.x = 34;
        boss.y = 2;
        boss.steps.push_back(ScriptBuilder::actNarr(
            "天色暗下来了。案上的字开始看不清，有人咳了一声，屋里的人便一个接一个收了笔。"));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "「收了吧。」最里头那张桌后的人合上竹简，站起来。僚属们依次起身，把竹简摞好，向最里头行了个礼，出去了。"));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "一屋子的沙沙声忽然没了，空得能听见窗外风过廊檐。"));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "上司走到他桌边，停了一下。四不相把头埋得更低。"));
        boss.steps.push_back(ScriptBuilder::actSay("上司",
            "灯留一盏。别把墨洒在文卷上。", speaker::SHANGSI));
        boss.steps.push_back(ScriptBuilder::actSay("四不相", "……是。", speaker::SIBUXIANG));
        boss.steps.push_back(ScriptBuilder::actMove(4, 13, 95));   // 上司从桌边走到门口
        boss.steps.push_back(ScriptBuilder::actDisappear());
        boss.steps.push_back(ScriptBuilder::actNarr(
            "门在他身后合上。文书阁里只剩下四不相一个人，和一盏灯。"));
        boss.steps.push_back(ScriptBuilder::actNarr(
            "他把抄好的文卷摞齐，压上砚台，站起来，吹熄了灯。"));
        boss.steps.push_back(ScriptBuilder::actPMove(3, 13, 90));   // 四不相自己走到门边
        boss.steps.push_back(ScriptBuilder::actNarr(
            "回廊里的灯一盏一盏亮起来，天已经黑了。他拉上门，沿着廊下慢慢往回走。"));
        boss.steps.push_back(ScriptBuilder::actFinish());

        std::vector<MapActor> actors;
        // 一屋子的同事：说走就走。桌边那八个「僚」在入夜这张图上本来就不画了
        // （见 Map::loadOffice 的 clerksPresent），改由角色顶上——
        // 一个一个收起笔、起身、走出画外，走完桌边一个人都不剩。
        static const int clerkX[8] = {5, 11, 17, 23, 5, 11, 17, 23};
        static const int clerkY[8] = {2, 2, 2, 2, 7, 7, 7, 7};
        for (int i = 0; i < 8; ++i) {
            MapActor c;
            c.id = "colleague" + std::to_string(i);
            c.name = "同事";
            c.glyph = "僚";
            c.color = color::WHITE;                 // 同事：一身白
            c.x = clerkX[i];
            c.y = clerkY[i];
            c.steps.push_back(ScriptBuilder::actWait(400 + i * 260));   // 一个接一个起身
            c.steps.push_back(ScriptBuilder::actDisappear());           // 收拾好，出去了
            actors.push_back(c);
        }
        actors.push_back(boss);
        b.mapWithActors("office_dusk", 29, 4, officeExits, actors);
    }

    // ---- 出门：回廊里天已经黑了，走回值守室那扇门再进去 ----
    b.jump("duty_corridor");

    b.label("office_boss_again");
    b.narr("上司头也没抬，笔尖在纸上顿了一下。");
    b.say("上司", "还站着？文书自己会抄完吗。", speaker::SHANGSI);
    b.say("四不相", "……是。", speaker::SIBUXIANG);
    b.jump("office_plain");

    b.label("office_colleague");
    b.narr("隔壁案的同事抬起头，笔尖还在滴墨，在纸上洇出一个圆点。");
    b.say("同事", "四不相？你脸怎么这么白。", speaker::SHANGSI);
    b.say("同事", "一大早的，撞见谁了？", speaker::SHANGSI);
    b.say("四不相", "……没、没有。走路急。", speaker::SIBUXIANG);
    b.narr("同事「哦」了一声，低头继续抄。抄了两笔，又想起什么似的补了一句。");
    b.say("同事", "上司今天心情倒是好。刚走过去那一趟，回来居然没摔竹简。", speaker::SHANGSI);
    b.narr("四不相的心往下一沉。");
    b.jump("office_plain");

    b.label("office_desk");       // 他自己的座位（金色案子「案」+「席」）
    b.narr("这张金色的是他自己的案子。一屋子几十张桌，只有这一张描了金——他坐下，别人就知道这儿有人。");
    b.narr("昨天没抄完的文卷还摊在案上，纸边被他自己的爪印蹭得发黑；笔也没洗，墨干在笔尖上，硬得像一根小骨头。");
    b.jump("office_sit");

    b.label("office_ink");
    b.narr("砚池里的墨是新磨的，凉。磨墨的人很用力，池边溅出来几点。");
    b.narr("一沓空白纸，纸角裁得齐整。用了半沓——今天还有半沓要填满。");
    b.jump("office_plain");

    b.label("office_scroll");
    b.narr("竹简按日子码成一摞，最上面那卷是今天早会上要交的。");
    b.narr("摊开的文卷上有一行抄了一半的字，墨迹断在半句上。昨天就是抄到这儿被打断的。");
    b.jump("office_plain");

    b.label("office_lamp");
    b.narr("灯还亮着，灯芯剪过，火苗很稳。这一屋子的白天，都要靠它撑到天黑。");
    b.jump("office_plain");

    // 1.0.0 存档锚点；保留无旁白跳转以兼容旧档。
    b.label("office_cabinet");
    b.jump("office_plain");

    b.label("office_save");       // 记事碑：文书阁的存档点
    b.savePrompt();
    b.jump("office_plain");

    b.label("office_window");
    b.narr("窗外是回廊的廊顶，灰白色的一大片，看不见天。");
    b.narr("窗纸有一处破了，风从那儿渗进来，吹得案上的纸角一动一动。");
    b.jump("office_plain");

    // =====================================================================
    //  回廊 · 入夜（困兽府前那一段的夜版）
    //  从文书阁出来天已经黑了。这一段回廊的北墙上多开一扇「值守室」的门，
    //  走上去按回车才进得去——值守室是自己一间屋，不从文书阁里穿过去。
    // =====================================================================
    std::vector<MapExit> c4n = {
        {"值守室", "duty_door_in"},
        {"文书阁", "duty_office_locked"},
        {"记事碑", "duty_corridor_save"},
        {"窗", "duty_corridor_window"},
        {"铜鼎", "duty_corridor_ding"},
        {"回廊西口", "duty_arch_west"},
        {"回廊北口", "duty_arch_north"},
    };

    b.label("duty_corridor");
    b.map("corridor4n", 10, 1, c4n);      // 拉上文书阁的门，站在门口
    b.label("duty_corridor_back");
    b.map("corridor4n", -1, -1, c4n);

    b.label("duty_corridor_save");
    b.savePrompt();
    b.jump("duty_corridor_back");

    b.label("duty_corridor_window");
    b.narr("廊下的灯一盏一盏亮着，把地砖照成一条一条的灰金色。窗纸上映着灯的影子，风一过就晃。");
    b.jump("duty_corridor_back");

    b.label("duty_corridor_ding");
    b.narr("铜鼎里的灰今夜没有动。他把手揣进袖子里，快步走了过去。");
    b.jump("duty_corridor_back");

    b.label("duty_office_locked");
    b.narr("文书阁的灯已经吹了，门上落了闩。明天早会前要交的那一摞，还压在砚台下头。");
    b.jump("duty_corridor_back");

    b.label("duty_arch_west");
    b.narr("西廊那头没有点灯，黑得看不见地砖。往那边去是回廊的另一头，绕回来只会更晚。");
    b.jump("duty_corridor_back");

    b.label("duty_arch_north");
    b.narr("北边的拱门下站着风。过了这道门是中廊，离家越来越远——他今天已经绕过一次了。");
    b.jump("duty_corridor_back");

    // ---- 推开值守室的门：进屋 ----
    // 进屋只念三句交代，然后**把操控交给你**：自己走回自己的桌前，
    // 在「桌子」上按回车才触发抄写那段动画（见本文件后面的 duty_desk）。
    b.label("duty_door_in");
    b.branch("duty_intro_seen", "1", "duty_free");
    b.setFlag("duty_intro_seen", "1");

    b.label("duty_night");

    // =====================================================================
    //  五、夜晚，值守室
    //  这里是一张真的地图（夜间版）：进屋以后就归你操控——可以在屋里走一圈、
    //  在记事碑前存档；走到桌前按回车开始抄写；也可以直接走到床上睡下。
    // =====================================================================
    b.scene("五 · 夜晚，值守室", "scene_room_night2");
    b.narr("天庭的夜是没有星也没有月的黑，暗沉沉地压在所有屋顶上，像一口倒扣的锅。");
    b.narr("值守室的门推开时「吱呀」响了一声。屋里冷得像冰窖，窗纸上破着的三个洞还在漏风，呜呜地响。");
    b.narr("下午的活到底没干完。上司临走把那摞没抄完的文卷摔在他桌上——「明天早会前交齐。」");

    // 出口表压在 9 项以内（数字键 1~9 才够用）
    std::vector<MapExit> dutyExits = {
        {"床", "duty_sleep"},
        {"桌子", "duty_desk"},
        {"笔墨", "duty_ink"},
        {"案卷", "duty_scroll"},
        {"油灯", "duty_lamp"},
        {"窗", "duty_window"},
        {"棉絮", "duty_fluff"},
        {"记事碑", "duty_save"},
        {"门", "duty_door"},
    };

    // ---- 屋里自由走动：从门口进来，接下来怎么走、什么时候坐到桌前，都由你决定 ----
    b.label("duty_free");
    b.map("dutyroom", 3, 12, dutyExits);

    b.label("duty_back");
    b.map("dutyroom", -1, -1, dutyExits);

    b.label("duty_save");
    b.savePrompt();
    b.jump("duty_back");

    // ---- 桌前抄写：在「桌子」上按回车以后，是一段**不能操控**的演出 ----
    //  坐下 → 挑灯（灯在「灯」「火」之间一明一暗）→ 一张一张抄 → 抄完
    //  自己走到床边 → 爬上床、裹进棉絮里睡着。演完从「床」这个出口
    //  自动接上「六 · 深夜，值守室」（四不遇进门那场）。
    b.label("duty_desk");
    {
        MapActor nar;
        nar.id = "narrator";
        nar.name = "旁白";
        nar.unseen = true;
        nar.x = 1;
        nar.y = 1;
        nar.steps.push_back(ScriptBuilder::actPMove(13, 3, 120));   // 先自己走到桌前
        nar.steps.push_back(ScriptBuilder::actNarr(
            "桌上摊着昨天没抄完的文卷，边上压着一块缺了角的砚台。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "他把袖子往上卷了一道，坐下，把最上面那一卷在灯下摊平。"));
        // 挑灯：灯芯一明一暗（灯 ↔ 火）
        nar.steps.push_back(ScriptBuilder::actTile(16, 2, "火", 240));
        nar.steps.push_back(ScriptBuilder::actTile(16, 2, "灯", 190));
        nar.steps.push_back(ScriptBuilder::actTile(16, 2, "火", 230));
        nar.steps.push_back(ScriptBuilder::actTile(16, 2, "灯", 200));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "墨是现磨的。笔尖落下去，屋里就只剩笔在纸上走的沙沙声，和破窗洞里的呜呜风声。"));
        nar.steps.push_back(ScriptBuilder::actTile(16, 2, "火", 250));
        nar.steps.push_back(ScriptBuilder::actTile(16, 2, "灯", 190));
        nar.steps.push_back(ScriptBuilder::actTile(16, 2, "火", 240));
        nar.steps.push_back(ScriptBuilder::actTile(16, 2, "灯", 190));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "一张一张，抄到掌灯时分。爪尖磨得发红，纸边被他的爪印蹭得发黑。"));
        nar.steps.push_back(ScriptBuilder::actTile(16, 2, "火", 250));
        nar.steps.push_back(ScriptBuilder::actTile(16, 2, "灯", 200));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "最后一笔落下。他把笔搁回砚边，长长地呼出一口气。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "抄到这会儿，眼皮开始打架了。他抬手揉了揉眼睛，笔在指间停了一会儿，停得比平常久。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "低头一看，白天那道暗金色的痕迹已经没了，像从没存在过一样。可他总觉得，它还在，在他的骨头里。"));
        nar.steps.push_back(ScriptBuilder::actPMove(2, 5, 110));   // 抄完自己走到床边
        nar.steps.push_back(ScriptBuilder::actNarr(
            "他爬上床，脱了外袍，裹进那床旧棉絮里。棉絮有一股陈年的潮味，怎么晒都晒不干。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "他睡得太熟了，没有听见那扇门被推开的声音。"));
        nar.steps.push_back(ScriptBuilder::actFinish());

        // 这张演出图的「床」不再是「睡下」，而是接着演深夜那一场
        std::vector<MapExit> copyExits;
        for (std::size_t i = 0; i < dutyExits.size(); ++i) {
            if (dutyExits[i].location == "床") copyExits.push_back({"床", "duty_deepnight"});
            else copyExits.push_back(dutyExits[i]);
        }

        std::vector<MapActor> actors;
        actors.push_back(nar);
        b.mapWithActors("dutyroom", -1, -1, copyExits, actors);
    }
    b.jump("duty_deepnight");

    b.label("duty_ink");
    b.narr("砚池里的墨已经干了，干成一层壳。他用指甲刮了一下，刮下来一点黑灰。");
    b.jump("duty_back");

    b.label("duty_scroll");
    b.narr("竹简按日子码在桌角，最上面那卷还差半句。笔搁在砚边，笔尖朝上，像一根小小的骨头。");
    b.jump("duty_back");

    b.label("duty_lamp");
    b.narr("油灯里的油只剩一个底。火苗很小，风一过就歪一下，把影子在墙上推来推去。");
    b.jump("duty_back");

    b.label("duty_window");
    b.narr("窗纸上破着的三个洞，一大两小。风从洞里钻进来，屋里就跟着响三声。");
    b.narr("他伸手想把洞捏住，指尖伸到一半又放下了。补不补，夜里都一样冷。");
    b.jump("duty_back");

    b.label("duty_fluff");
    b.narr("墙角那一撮旧棉絮已经发黄了，一碰就散，像一小堆落了雪的灰。");
    b.jump("duty_back");

    b.label("duty_door");
    b.narr("门是他自己带上的。门轴叫了一声，现在安静了。");
    b.narr("他把门闩推上去——今夜外面再大的风，也别想把它吹开。");
    b.jump("duty_back");

    // ---- 睡下：躺进棉絮里，接上后面那场梦 ----
    b.label("duty_sleep");
    b.narr("他爬上床，脱了外袍，裹进那床旧棉絮里。棉絮有一股陈年的潮味，怎么晒都晒不干。");
    b.narr("他在想白天那只兽。典狱长，整个天庭都怕的典狱长，为什么要用那种眼神看他？");
    b.say("四不相", "……四不遇。", speaker::SIBUXIANG);
    b.narr("他念出这个名字的时候顿了一下。典狱长的名字。他以前从来不敢想，也从来没记住过。");
    b.narr("可白天从上司嘴里听见的那一瞬间，这个名字就扎进他脑子里了，像一根刺，拔不出来。");
    b.say("四不相", "四不遇……我好像在哪里听过。", speaker::SIBUXIANG);

    // ---- 根据内心选择，呈现不同的回想（演示分支） ----
    b.branch("hui_xiang", "回想", "wuye_huixiang");
    b.narr("他本能地把这个名字推远。他不知道自己在怕什么，只是那堵墙还在，里面有东西在动、在挠、在喊他——他不想听清。", color::SILVER);
    b.jump("wuye_after");
    b.label("wuye_huixiang");
    b.narr("这个名字在他舌尖滚过的时候，他整个身体都像被什么东西接通了，很疼，但不陌生。他追着那种感觉往里走，却撞在一堵墙上。", color::SILVER);
    b.label("wuye_after");

    b.narr("他想了很久。想不起来。那堵墙还在，里面有什么东西在动，在挠，在喊他——他听不清。");
    b.narr("他攥紧爪尖，忽然觉得累极了。不是加班的累，是一种更深的、从骨头缝里往外渗的累。");
    b.narr("像走了很远很远的路，回头看，才发现自己一直在原地转圈。");
    b.narr("他倒下去，蜷进棉絮里，闭上眼睛。");
    b.narr("黑暗淹没了整个房间。只有那三个破洞还在漏风，呜呜地响，像有谁在很远的地方哭。");
    b.narr("他睡得太熟了，没有听见那扇门被推开的声音。");

    // ---- 深夜值守室门开（直接进，不再走回廊地图） ----
    b.jump("duty_deepnight");
    b.label("duty_deepnight");

    // =====================================================================
    //  六、深夜，值守室门开
    //  这一段是**地图里的动画**（不能操控）：
    //   四不遇从门口进来 → 贴着桌腿的阴影踮脚走到床边蹲下 →
    //   对着床上说话 → 四不相被惊醒、坐起来 → 两个人的对话 →
    //   他转身走回门口、推门出去 → 剩下的话由四不相自己坐在床上说完。
    //  演完从「床」这个出口自动接「七」。
    // =====================================================================
    b.scene("六 · 深夜，值守室", "scene_room_door");
    {
        MapActor sy;
        sy.id = "sibuyu";
        sy.name = "四不遇";
        sy.glyph = "遇";
        sy.color = speaker::SIBUYU;
        sy.x = 3;
        sy.y = 12;                                  // 从门口那条缝里进来
        sy.steps.push_back(ScriptBuilder::actNarr("门被轻轻拉开一条缝。没有声音。像一只爪子用了一万年的熟练度，让门轴连一声呻吟都没有发出来。"));
        sy.steps.push_back(ScriptBuilder::actMove(3, 10));      // 从缝里挤进来
        sy.steps.push_back(ScriptBuilder::actNarr("一只灰白色的幼崽从缝里挤进来。琥珀色的眼睛在黑暗中亮了一瞬，又暗下去，像怕自己的光吵醒了床上的白麒麟。"));
        sy.steps.push_back(ScriptBuilder::actMove(5, 8, 180));  // 藏在桌腿的阴影里往里走
        sy.steps.push_back(ScriptBuilder::actMove(5, 4, 160));  // 踮着脚走到床边，蹲下
        sy.steps.push_back(ScriptBuilder::actNarr("他很小，小到可以藏在桌腿的阴影里。他踮着脚走到床边，爪尖搭在床沿上，没有碰四不相。"));
        sy.steps.push_back(ScriptBuilder::actSay("四不遇",
            "哥……我来看你了……今天你在走廊上撞到我了……你跟我说对不起……你说「我还有人要等」……你在等谁啊……",
            speaker::SIBUYU));
        sy.steps.push_back(ScriptBuilder::actNarr("他的声音很轻，轻到像在哄一个睡着的人。他等了一会儿，四不相没有醒。"));
        sy.steps.push_back(ScriptBuilder::actSay("四不遇",
            "你在等我吗？你是不是在等我？你是不是……已经忘了你在等我？", speaker::SIBUYU));
        sy.steps.push_back(ScriptBuilder::actNarr("然后他停住了。因为四不相在梦里翻了个身，眉头皱了一下——他听见了什么，极远极远的声音，像从雪地里传来的。"));
        sy.steps.push_back(ScriptBuilder::actNarr("四不遇的爪尖颤了一下。他张了张嘴，没来得及缩回爪子——四不相醒了。"));
        sy.steps.push_back(ScriptBuilder::actPMove(3, 5, 170));  // 白麒麟在被子里翻了个身，坐起来
        sy.steps.push_back(ScriptBuilder::actNarr("四不相睁开眼，看见床边蹲着一只灰白色的幼崽。他先是迷茫，然后认出了那双眼睛——琥珀色的，和白天走廊里那只兽的眼睛一模一样。"));
        sy.steps.push_back(ScriptBuilder::actNarr("他坐起来，往下缩了缩，爪尖攥紧了棉絮边缘，把被子往上拉了拉。"));
        sy.steps.push_back(ScriptBuilder::actSay("四不相",
            "典、典狱长大人……您怎么来这儿了？这里不是您该待的地方……您回去吧……", speaker::SIBUXIANG));
        sy.steps.push_back(ScriptBuilder::actNarr("幼崽愣住了。琥珀色的眼睛亮了一下——像一盏在风里摇了很久的灯，忽然被人拢住了——然后暗下去了。"));
        sy.steps.push_back(ScriptBuilder::actNarr("他站了很久。他的爪尖还搭在床沿上。他是想碰他的。他想了很久了。可他不敢。"));
        sy.steps.push_back(ScriptBuilder::actSay("四不遇", "哥……你不记得我了？", speaker::SIBUYU));
        sy.steps.push_back(ScriptBuilder::actNarr("他说这句话的时候，声音很轻，轻到像他自己都不信会有人回答。可他还是问了。他每次都会问。他每次都知道答案。"));
        sy.steps.push_back(ScriptBuilder::actNarr("他脚边的阴影里，一朵极小的荼蘼花正悄悄地、无声地卷起花瓣，像在替他把没能喊出来的那一句吞回去。"));
        sy.steps.push_back(ScriptBuilder::actSay("四不相", "我……没有弟弟。", speaker::SIBUXIANG));
        sy.steps.push_back(ScriptBuilder::actNarr("他说得很平静。平静得像一把封了鞘的刀。说完之后，他自己都觉得，那句话像从很远的、被洗干净的地方传来的回声。"));
        sy.steps.push_back(ScriptBuilder::actNarr("他看着那只幼崽——他看见他琥珀色的眼睛暗了一瞬，像一颗快要碎掉的琥珀，撑着一道裂缝，没有彻底裂开。"));
        sy.steps.push_back(ScriptBuilder::actNarr("可他看出来了。那一道裂缝，是最后一次了。"));
        sy.steps.push_back(ScriptBuilder::actSay("四不遇", "……嗯。我知道了。", speaker::SIBUYU));
        sy.steps.push_back(ScriptBuilder::actNarr("他转身，一步一步走向门口。尾巴垂在地上，一路轻轻地颤着——是完完整整的一条，一截也不缺。"));
        sy.steps.push_back(ScriptBuilder::actMove(3, 10, 200));  // 一步一步走回门边
        sy.steps.push_back(ScriptBuilder::actNarr("他走到门边的时候，爪尖搭在门框上，停住了。他没有回头。他在等一个声音。一个他等了一万年的声音。"));
        sy.steps.push_back(ScriptBuilder::actSay("四不相", "……你叫什么名字？", speaker::SIBUXIANG));
        sy.steps.push_back(ScriptBuilder::actNarr("幼崽在门缝的光里停住了，没有回头。"));
        sy.steps.push_back(ScriptBuilder::actSay("四不遇", "……四不遇。", speaker::SIBUYU));
        sy.steps.push_back(ScriptBuilder::actSay("四不相", "四不遇……好名字。我好像……在梦里听过。", speaker::SIBUXIANG));
        sy.steps.push_back(ScriptBuilder::actNarr("幼崽的爪尖抠进木框里，木屑落了一地。他的琥珀色眼睛在黑暗里亮了一瞬——然后，那道裂缝终于裂开了。"));
        sy.steps.push_back(ScriptBuilder::actNarr("没有声音。没有碎响。只是有什么东西，在他心里彻底断开了。"));
        sy.steps.push_back(ScriptBuilder::actSay("四不遇", "……嗯。你听过。", speaker::SIBUYU));
        sy.steps.push_back(ScriptBuilder::actNarr("他推开门，走了出去。门关上了。他在门外站了很久，极轻的，像是在等什么。"));
        sy.steps.push_back(ScriptBuilder::actMove(3, 12, 220));  // 跨出门外
        sy.steps.push_back(ScriptBuilder::actDisappear());
        sy.steps.push_back(ScriptBuilder::actNarr("门里面安安静静的，像没有人来过。"));
        sy.steps.push_back(ScriptBuilder::actNarr("四不相坐在床上，低头看着自己的爪心。那里不知什么时候多了一根灰白色的绒毛。"));
        sy.steps.push_back(ScriptBuilder::actSay("四不相", "四不遇……我好像真的……在梦里听过。", speaker::SIBUXIANG));
        sy.steps.push_back(ScriptBuilder::actNarr("他低下头，把脸埋进爪心里。风从破纸洞里灌进来，呜呜地响，像有谁在很远的地方替他回答了一个问题——但那个回答，他听不见了。"));
        sy.steps.push_back(ScriptBuilder::actNarr("他不知道自己为什么想哭。他只是觉得，那只幼崽转身的时候，自己的心口缺了一块。"));
        sy.steps.push_back(ScriptBuilder::actNarr("他躺下去，蜷进棉絮里，闭上眼睛。黑暗淹没了整个房间。他睡着了。"));
        sy.steps.push_back(ScriptBuilder::actNarr("这一次，他睡着的时候，有一粒淡金色的光从门缝里渗进来，落在他眉心。他没有醒。"));
        sy.steps.push_back(ScriptBuilder::actNarr("光渗进他的梦里，把他拉向未知的方向。"));
        sy.steps.push_back(ScriptBuilder::actFinish());

        std::vector<MapActor> actors;
        actors.push_back(sy);
        // 演完从「床」这个出口接下一天（七 · 梦中）
        b.mapWithActors("dutyroom", -1, -1, {{"床", "duty_dream"}}, actors,
                        "六 · 深夜，值守室");
    }

    b.label("duty_dream");

    // =====================================================================
    //  浮梦 · 四不相梦里那条路（序章 七 — 十）
    //  一进入浮梦，看到的就是遇龙雪山；再往里是同游川、梦栖林，
    //  小径尽头是一座农家小院。四段全是**真的地图**，文字都挂在地图下方，
    //  不再清屏打黑底旁白。
    //    七 · 遇龙雪山 —— 天光把黄昏和黎明揉在一起，雪里开着忆棠（白花）
    //    八 · 同游川   —— 河底流着孟极和麒麟的回忆
    //    九 · 梦栖林   —— 林中一道白色的余影（孟极烬光）
    //    十 · 农家小院 —— 小径尽头的院子
    // =====================================================================
    std::vector<MapExit> dreamExits1 = {
        {"忆棠", "dream1_tang"},
        {"山径", "dream_river_in"},
        {"记事碑", "dream1_save"},
    };
    std::vector<MapExit> dreamExits2 = {
        {"忆棠", "dream2_tang"},
        {"同游川", "dream2_water"},
        {"浅滩", "dream2_ford"},
        {"川径", "dream_forest_in"},
        {"记事碑", "dream2_save"},
    };
    std::vector<MapExit> dreamExits3 = {
        {"林径", "dream_yard_in"},
        {"记事碑", "dream3_save"},
    };
    std::vector<MapExit> dreamExits4 = {
        {"屋门", "dream_end"},
        {"井", "dream4_well"},
        {"石桌", "dream4_stone"},
        {"院里的树", "dream4_tree"},
        {"记事碑", "dream4_save"},
    };

    // ---------------------------------------------------------------- 七 --
    b.scene("七 · 浮梦，遇龙雪山", "scene_dream_snow");
    {
        MapActor nar;
        nar.id = "narrator";
        nar.name = "旁白";
        nar.unseen = true;      // 不画在地图上，只把话挂到地图下方
        nar.x = 20;
        nar.y = 11;
        nar.steps.push_back(ScriptBuilder::actNarr(
            "他睁开眼，脚下是一片雪。没有风，也不冷——梦里的雪不冷。"));
        // 天光会动：雪面上先亮起几块光，再一句一句铺开（这就是雪山那段的小动画）
        nar.steps.push_back(ScriptBuilder::actTile(9, 6, "光", 200));
        nar.steps.push_back(ScriptBuilder::actTile(30, 8, "光", 200));
        nar.steps.push_back(ScriptBuilder::actTile(12, 3, "光", 220));
        nar.steps.push_back(ScriptBuilder::actTile(34, 10, "光", 220));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "抬头是遇龙雪山。天光像有人用最温柔的力度，把黄昏和黎明揉在了一起：一半橘，一半青白，铺在雪面上，一直铺到天边。"));
        nar.steps.push_back(ScriptBuilder::actTile(7, 9, "光", 200));
        nar.steps.push_back(ScriptBuilder::actTile(26, 6, "光", 200));
        nar.steps.push_back(ScriptBuilder::actTile(17, 9, "光", 220));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "雪里长着一种树。没有叶子，只开白花，一朵一朵亮得像点着的灯。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "他认得它们——忆棠。只在梦里开花的树。白花落在哪儿，哪儿就记起一件事。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "一条小径从脚下往前伸，绕过半座雪山，最后钻进山壁上一道唯一的缺口里。他顺着它走。"));
        nar.steps.push_back(ScriptBuilder::actFinish());

        std::vector<MapActor> actors;
        actors.push_back(nar);
        b.mapWithActors("dream1", -1, -1, dreamExits1, actors, "七 · 浮梦，遇龙雪山");
    }

    b.label("dream1_free");       // 醒来以后这张图就归你走
    b.map("dream1", -1, -1, dreamExits1);

    b.label("dream1_tang");
    b.narr("一朵白花落在他爪尖上，凉了一下，就化了。");
    b.narr("他忽然想起一句很旧的话：开到荼蘼，春天就尽了。可这树开的是春天以前的花——它开的时候，春天还没来。");
    b.jump("dream1_free");

    b.label("dream1_save");
    b.savePrompt();
    b.jump("dream1_free");

    // ---------------------------------------------------------------- 八 --
    b.label("dream_river_in");
    b.scene("八 · 同游川", "scene_dream_river");
    {
        MapActor nar;
        nar.id = "narrator";
        nar.name = "旁白";
        nar.unseen = true;
        nar.x = 20;
        nar.y = 11;
        nar.steps.push_back(ScriptBuilder::actNarr(
            "雪到这里就到头了。再往前，是一条横着流过去的河。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "水很浅，也很清，清得能看见河底——可河底流的不是沙，是梦。一丝一丝的，亮的，从上游淌下去。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "他蹲下来看了很久。他看清了两道影子：一头灰白色的兽，还有一头麒麟，一前一后地走在河里，走了很久很久。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "他不知道他们是谁。可他看着看着，眼睛就热了。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "河中间有一处浅滩。他踩着水蹚过去，水只没到蹄踝；上了北岸，小径又接着往山里走，一直通到山脊上那个缺口。"));
        nar.steps.push_back(ScriptBuilder::actFinish());

        std::vector<MapActor> actors;
        actors.push_back(nar);
        b.mapWithActors("dream2", -1, -1, dreamExits2, actors, "八 · 同游川");
    }

    b.label("dream2_free");
    b.map("dream2", -1, -1, dreamExits2);

    b.label("dream2_water");
    b.narr("河水从爪边淌过去，一丝一丝的亮光绕着他的指头转了一圈，又走了。");
    b.narr("水底那两道影子还在走。一头灰白色的兽，一头麒麟，一前一后，谁也不说话，可谁也不肯走到对方前头去。");
    b.narr("他伸出手，影子碎了；手一收回来，影子又合上了，还是那样一走一辈子地走。");
    b.jump("dream2_free");

    b.label("dream2_tang");
    b.narr("岸上也开着忆棠。这里的白花开得比雪山上密，一朵挨着一朵，把半边天都照亮了。");
    b.narr("有一朵花里，停着一句他没听清的话。他把耳朵凑过去，花就谢了。");
    b.jump("dream2_free");

    b.label("dream2_ford");
    b.narr("浅滩底下铺着细细的白石，踩上去不冷。水流过去，把石上的光洗得更亮了。");
    b.jump("dream2_free");

    b.label("dream2_save");
    b.savePrompt();
    b.jump("dream2_free");

    // ---------------------------------------------------------------- 九 --
    b.label("dream_forest_in");
    b.scene("九 · 梦栖林", "scene_dream_forest");
    {
        MapActor nar;
        nar.id = "narrator";
        nar.name = "旁白";
        nar.unseen = true;
        nar.x = 20;
        nar.y = 11;
        nar.steps.push_back(ScriptBuilder::actNarr(
            "过了河，雪没有了。树密起来，密得连天光都筛成一小块一小块，落在地上像碎掉的水。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "这地方他没来过。可他一进来就知道它叫什么——梦栖林。梦里的人和梦里的事，都歇在这儿。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "林子只有一条小径。小径弯来弯去，弯到最里面，就看见一道白色的影子。"));
        nar.steps.push_back(ScriptBuilder::actFinish());

        std::vector<MapActor> actors;
        actors.push_back(nar);
        b.mapWithActors("dream3", -1, -1, dreamExits3, actors, "九 · 梦栖林");
    }
    {
        // 那道白色的余影：烬光留下的一小半分魂，只为陪着四不遇、让他开心一点。
        MapActor ying;
        ying.id = "jingguang";
        ying.name = "白色余影";
        ying.glyph = "影";
        ying.color = color::WHITE;
        ying.x = 27;
        ying.y = 2;
        ying.steps.push_back(ScriptBuilder::actWait(260));
        ying.steps.push_back(ScriptBuilder::actMove(20, 2, 190));
        ying.steps.push_back(ScriptBuilder::actMove(20, 5, 170));
        ying.steps.push_back(ScriptBuilder::actNarr(
            "那道影子飘过来了。四只爪子离地半寸，白得透明——一头兽的轮廓。看不清它原来的样子，只有一双眼睛还留着颜色：雨过天晴的那种青，最深处一点金。"));
        ying.steps.push_back(ScriptBuilder::actSay("白色余影", "……你来了。", color::WHITE));
        ying.steps.push_back(ScriptBuilder::actNarr(
            "这双眼睛他认得。梦里梦外，天底下只此一双。"));
        ying.steps.push_back(ScriptBuilder::actSay("白色余影",
            "别皱眉。我留在这儿，不是为了看你皱眉的——也不是为了你。", color::WHITE));
        ying.steps.push_back(ScriptBuilder::actNarr(
            "影子笑了一下。它没有脸，可它就是笑了一下。"));
        ying.steps.push_back(ScriptBuilder::actSay("白色余影",
            "我是在等他。四不遇。", color::WHITE));
        ying.steps.push_back(ScriptBuilder::actNarr(
            "烬光走之前，把自己剩下的那一点撕了一小半下来，搁在这片林子里。不是给谁陪葬，是给那孩子做伴——他一个人走夜路走得太久了，梦里也不肯让人陪。"));
        ying.steps.push_back(ScriptBuilder::actNarr(
            "孟极的先祖叫「光」，烬光是那一脉的直孙。名字是从「终焉余烬」里取的两个字：烬是烧到最后的，光是不肯灭的。"));
        ying.steps.push_back(ScriptBuilder::actSay("白色余影",
            "他舍不得他。这么简单的一件事，天庭里没有一个人看得懂。", color::WHITE));
        ying.steps.push_back(ScriptBuilder::actSay("白色余影",
            "他笑一回，我就亮一回；他哪晚不走夜路，我就哪晚歇着。", color::WHITE));
        ying.steps.push_back(ScriptBuilder::actSay("白色余影",
            "所以别问我叫什么。你只要记得——他一个人走的那条夜路，边上多了一盏灯。", color::WHITE));
        ying.steps.push_back(ScriptBuilder::actNarr(
            "四不相抬起爪，想去碰那双眼睛。影子往回缩了缩，又停住了。"));
        ying.steps.push_back(ScriptBuilder::actSay("白色余影",
            "碰不着了。可我不会散——他留得比你想的多。", color::WHITE));
        ying.steps.push_back(ScriptBuilder::actNarr(
            "影子转过头，朝林子东边低低地叫了一声——那里有一条更细的小径，尽头亮着一盏灯。"));
        ying.steps.push_back(ScriptBuilder::actSay("白色余影",
            "沿着小径走。他在等你，等了很多年了。", color::WHITE));
        ying.steps.push_back(ScriptBuilder::actSay("白色余影",
            "回头再来看我。我就在这儿。", color::WHITE));
        ying.steps.push_back(ScriptBuilder::actNarr(
            "他再回头时，那影子已经散成一小片雪，落在他的爪尖上，凉了一下，却没有化。"));
        ying.steps.push_back(ScriptBuilder::actDisappear());
        ying.steps.push_back(ScriptBuilder::actFinish());

        std::vector<MapActor> actors;
        actors.push_back(ying);
        b.mapWithActors("dream3", -1, -1, dreamExits3, actors, "九 · 梦栖林");
    }

    b.label("dream3_free");
    b.map("dream3", -1, -1, dreamExits3);

    b.label("dream3_save");
    b.savePrompt();
    b.jump("dream3_free");

    // ---------------------------------------------------------------- 十 --
    b.label("dream_yard_in");
    b.scene("十 · 农家小院", "scene_dream_yard");
    {
        MapActor nar;
        nar.id = "narrator";
        nar.name = "旁白";
        nar.unseen = true;
        nar.x = 20;
        nar.y = 11;
        nar.steps.push_back(ScriptBuilder::actNarr(
            "小径的尽头是一道矮篱。篱后是一间屋，屋顶压着雪，窗里透出一点暖黄的光。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "院门虚掩着。井台上放着一只瓢，瓢里的水结了一层薄冰。晾架上挂着两条洗过的布，冻得硬邦邦的。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "这是他梦里来过无数次的地方。他知道推开门会看见什么——可他还是站在院子里，站了一会儿。"));
        nar.steps.push_back(ScriptBuilder::actFinish());

        std::vector<MapActor> actors;
        actors.push_back(nar);
        b.mapWithActors("dream4", -1, -1, dreamExits4, actors, "十 · 农家小院");
    }

    b.label("dream4_free");
    b.map("dream4", -1, -1, dreamExits4);

    b.label("dream4_well");
    b.narr("井水结了冰。冰面上映着窗里的那点黄光，一晃一晃的。");
    b.jump("dream4_free");

    b.label("dream4_stone");
    b.narr("石桌上摆着两只碗，两双筷子。一只碗扣着，一只碗敞着——敞着的那只里，饭还冒着气。");
    b.jump("dream4_free");

    b.label("dream4_tree");
    b.narr("院角那棵树也没有叶子，可它不开白花。它结着一种小小的、青色的果子，硬得像石头。");
    b.narr("这棵树他记得。它一直在。它比这间屋子还老。");
    b.jump("dream4_free");

    b.label("dream4_save");
    b.savePrompt();
    b.jump("dream4_free");

    // ---- 推门：梦的尽头 ----
    b.label("dream_end");
    {
        MapActor nar;
        nar.id = "narrator";
        nar.name = "旁白";
        nar.unseen = true;
        nar.x = 20;
        nar.y = 11;
        nar.steps.push_back(ScriptBuilder::actNarr(
            "他把手按在门上。门没有闩，一推就开了。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "屋里烧着一盆炭，火不旺，却把整间屋子烘得暖烘烘的。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "桌子那头，一只灰白色的幼崽正坐着。琥珀色的眼睛在暖光里亮着，像一盏快要燃尽的灯，里面全是水光。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "额上一对小角立在暖光里，干干净净的，一点也没淡。尾巴在身后轻轻地晃，完整的一条，尖上那撮软毛扫着地。"));
        nar.steps.push_back(ScriptBuilder::actSay("灰白色幼崽", "你追到我了。", speaker::YOULING));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "四不相张了张嘴。他想问：你是谁？你叫什么？我们见过吗？可他一个字也没说出来。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "他的胸口忽然疼了一下。很深。像有什么东西一直埋在那里，终于被挖出来了——但他不知道那是什么。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "白光从幼崽身后涌过来。很慢，像水漫过堤坝，漫过四不相的蹄踝，漫过他的膝盖，漫过他的腰腹。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "他没有躲。他看见那只幼崽在光里看着他，嘴角弯了一下——很轻，轻到像怕吓到他。"));
        nar.steps.push_back(ScriptBuilder::actSay("灰白色幼崽",
            "……进来吧。我都给你留着呢。", speaker::YOULING));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "白光吞没了一切。雪地、天光、忆棠、那条小径——全部消失了。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "四不相觉得自己沉进了一片很暖很暖的光里，像沉进了一场，被等了一万年的梦里。"));
        nar.steps.push_back(ScriptBuilder::actFinish());

        std::vector<MapActor> actors;
        actors.push_back(nar);
        b.mapWithActors("dream4", -1, -1, dreamExits4, actors, "十 · 农家小院");
    }

    // =====================================================================
    //  序章 · 完
    // =====================================================================
    // 结语之后接一段「感谢游玩」：正文逐行居中，见 Game::renderChapterEnd。
    // 最后那一句红字（我在看着你······）和署名由渲染端补，不写在这里。
    b.chapterEnd("序章 · 完",
                 "故事，才刚刚开始。\n"
                 "第一章「童年记忆篇」，敬请期待。\n"
                 "\n"
                 "—— 感谢游玩 ——\n"
                 "\n"
                 "一个序章，写了很久很久。\n"
                 "从我当初想出四不遇、想出这个同人故事，\n"
                 "到一段段 c++ 代码，一个浮梦，一个卑微的四不相。\n"
                 "我没想过要重写，也没想过要重来，我也崩溃过，\n"
                 "特别是深陷在一行行报错的代码、实现不了自己想要的效果的时候。\n"
                 "可每次想停，我都会告诉自己：还有人在等我，\n"
                 "死不相遇的故事不能就此终结。\n"
                 "我知道我比不上《四叶苜蓿》，但，这也凝聚了我小小的心血。\n"
                 "你们的每一次游玩、每一句话、每一条建议，\n"
                 "都是我继续更新的动力。\n"
                 "我会把四不遇一直写下去，写到，他们相遇，\n"
                 "让那两个孩子，真正在一起。\n"
                 "感谢一切给予我帮助的人！");

    return b.done();
}

}  // namespace game
