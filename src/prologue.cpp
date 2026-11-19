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
    b.setFlag("item_fluff_gray", "一团灰色的绒毛");
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
    b.say("四不相", "花至……什么？", speaker::SIBUXIANG);
    b.choice("要按下哪个字？", {
        {"荼", "storeroom_box_cha"},
        {"靡", "storeroom_box_mi"},
        {"蘼", "storeroom_box_li"},
    });

    // 按错：字位卡住，弹不动。
    b.label("storeroom_box_cha");
    b.narr("他把「荼」按了下去。「嗒」一声，字位陷进去半分，又慢慢弹了回来。");
    b.say("四不相", "不对……只有一个是按得下去的。", speaker::SIBUXIANG);
    b.jump("storeroom_box");

    b.label("storeroom_box_mi");
    b.narr("他把「靡」按了下去。匣子里响了一声，像什么被拧歪了，字位纹丝不动地卡在那儿。");
    b.say("四不相", "也不是这个。那个「靡」，是靡靡之音的靡……", speaker::SIBUXIANG);
    b.jump("storeroom_box");

    // 按对：花至荼蘼。开匣，得到铜钥匙。
    b.label("storeroom_box_li");
    b.narr("他把「蘼」按了下去。");
    b.narr("「嗒。」字位到底了。匣子里传来一连串很轻的机括声，一层一层往里退。匣盖弹开。");
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
        sy.steps.push_back(ScriptBuilder::actNarr("一只灰白色的幼崽从缝里挤进来——琥珀色的眼睛在黑暗中亮了一瞬，又暗下去，像怕自己的光吵醒了床上的白麒麟。"));
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
        sy.steps.push_back(ScriptBuilder::actNarr("风从窗缝里灌进来，吹动他缺了一点的尾巴尖。他缩了一下，把自己蜷得更小了一点，像一朵不敢开大的花。"));
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
    // 这两段不进地图，只留在屏上做开场；进地图后它们会被搬到地图下面的字幕栏里，
    // 所以返回地图时也会重新显示——玩家一睁眼就知道「要迟到了」。
    b.narr("闹钟不合时宜地响起。四不相猛地坐起，浑身一颤。");
    b.narr("窗外天已经大亮了。晨光从破纸洞里灌进来，在他纯白色的绒毛上铺了一层惨淡的薄亮。");
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
    //  · 每段两头各开一个拱门，**走上去就换段**，不用按回车；
    //  · 两旁的墙上立着神仙府邸，要按回车才推得开；
    //  · 每段都立着一块「记事碑」，站在上面按回车就能存档；
    //  · 一进中廊就撞上四不遇：那一段玩家**完全不能操作**，脚自己往前撞，
    //    台词全部念完才把操控权还回来；
    //  · 开场那几句不在地图之前吞掉，而是逐句挂在地图**下方**（见 game.cpp 里
    //    「不出场、只说话」的角色）。
    // =====================================================================

    // 回廊每一段各有一份出口表。地点名必须和 map.cpp 里登记的完全一致，
    // 差一个字就点不动。拱门（回廊西口 / 回廊东口 / 回廊南口）是「自动门」：
    // 走上去就换段，不用按回车。记事碑是存档点。
    std::vector<MapExit> c1 = {
        {"回廊西口", "corridor1_west"},
        {"回廊东口", "corridor2_start"},
        {"白泽府", "mansion_baize"},
        {"獬豸府", "mansion_xiezhi"},
        {"杂物司", "mansion_zawu"},
        {"记事碑", "corridor1_save"},
        {"窗", "corridor1_window"},
    };
    std::vector<MapExit> c2 = {
        {"回廊西口", "corridor1_east"},
        {"回廊南口", "corridor3_start"},
        {"斗姆宫", "mansion_doumu"},
        {"月老祠", "mansion_yuelao"},
        {"药王殿", "mansion_yaowang"},
        {"织云坊", "mansion_zhiyun"},
        {"记事碑", "corridor2_save"},
        {"窗", "corridor2_window"},
    };
    std::vector<MapExit> c3 = {
        {"回廊西口", "corridor2_east"},
        {"回廊东口", "corridor4_start"},
        {"九尾府", "mansion_jiuwei"},
        {"文昌阁", "mansion_wenchang"},
        {"鹿蜀府", "mansion_lushu"},
        {"记事碑", "corridor3_save"},
        {"窗", "corridor3_window"},
    };
    std::vector<MapExit> c4 = {
        {"回廊西口", "corridor3_east"},
        {"文书阁", "office"},
        {"困兽府", "mansion_kunshou"},
        {"记事碑", "corridor4_save"},
        {"窗", "corridor4_window"},
    };

    // ---- 进回廊：先让地图立起来，旁白挂在地图下面一句一句说 ----
    b.label("corridor_intro");
    b.branch("corridor_intro_seen", "1", "corridor1_start");
    b.setFlag("corridor_intro_seen", "1");
    {
        MapActor nar;
        nar.id = "narrator";
        nar.name = "旁白";
        nar.x = 1;
        nar.y = 3;
        nar.unseen = true;   // 不画在地图上，只负责把话挂到地图下方
        nar.steps.push_back(ScriptBuilder::actNarr(
            "天庭的走廊很长。晨光从高大的窗棂间漏下来，在地砖上切成一段一段的光格，像一道一道看不见的栅栏。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "四不相叼着公文一路狂奔，后蹄在光洁的地砖上打滑。文书阁在回廊最里头——要穿过西廊、中廊、东廊，再拐到困兽府前那一段。"));
        nar.steps.push_back(ScriptBuilder::actSay("四不相", "借过……借过……要迟到了……",
                                                 speaker::SIBUXIANG));
        nar.steps.push_back(ScriptBuilder::actFinish());

        std::vector<MapActor> actors;
        actors.push_back(nar);
        b.mapWithActors("corridor1", 2, 3, c1, actors);
    }

    // ---- 西廊（笔直的一段）----
    b.label("corridor1_start");
    b.map("corridor1", 2, 3, c1);
    b.label("corridor1_back");
    b.map("corridor1", -1, -1, c1);       // 负数坐标 = 留在原地，看完东西回到门前
    b.label("corridor1_east");
    b.map("corridor1", 38, 3, c1);        // 从中廊往西折返：落在西廊东头

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

    // ---- 中廊（走到一半向南折下去）：一进来就撞上四不遇 ----
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
        yu.steps.push_back(ScriptBuilder::actMove(38, 3, 150));
        yu.steps.push_back(ScriptBuilder::actDisappear());
        yu.steps.push_back(ScriptBuilder::actFinish());

        std::vector<MapActor> actors;
        actors.push_back(yu);
        b.mapWithActors("corridor2", 2, 3, c2, actors);
    }
    b.setFlag("met_sibuyu", "1");
    b.narr("四不相抱着公文往南边跑。跑过光与暗的分界，跑过一根一根廊柱。他没有回头。");
    b.narr("他觉得自己应该回头。可他没有。他不知道自己错过了什么。");
    b.narr("身后的回廊里，那个身影蹲了下来。地上有一根白色的绒毛，是四不相刚才蹭掉的，还带着一点晨光的温度。");
    b.narr("他捡起来，贴在脸颊上，轻轻蹭了一下，像碰一个隔了一万年的温度。然后他闭上了眼睛。");
    b.narr("再睁眼时，一滴清泪从眼眶里滑出来，砸在地砖上，绽成一朵极小的荼蘼花。");
    b.say("典狱长", "……你跑什么。我又不会罚你。", speaker::SIBUYU);
    b.narr("他把那朵花和那根绒毛一起收进袖口，爪尖攥得那么紧，仿佛一松开，那根绒毛也会像四不相一样，从他爪心里跑掉。");
    b.say("典狱长", "……杂物司……不能让你去。", speaker::SIBUYU);
    b.jump("corridor2_after");

    b.label("corridor2_plain");
    b.map("corridor2", 2, 3, c2);
    b.label("corridor2_back");
    b.map("corridor2", -1, -1, c2);
    b.label("corridor2_after");
    b.map("corridor2", -1, -1, c2);
    b.label("corridor2_east");
    b.map("corridor2", 31, 10, c2);       // 从东廊折返：落在南口前

    b.label("corridor2_save");
    b.savePrompt();
    b.jump("corridor2_back");

    b.label("corridor2_window");
    b.narr("窗半开着，风灌进来，把廊下的灯笼吹得贴到了墙上。");
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

    // ---- 东廊（S 形：向东、折向南、再折回向东）----
    b.label("corridor3_start");
    b.map("corridor3", 2, 2, c3);
    b.label("corridor3_back");
    b.map("corridor3", -1, -1, c3);
    b.label("corridor3_east");
    b.map("corridor3", 38, 9, c3);        // 从困兽府前折返：落在东廊东头

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

    b.label("mansion_kunshou");
    b.narr("困兽府的门是黑的，比回廊里别的门都黑，黑得连门环都看不见。");
    b.narr("门前没有灯，也没有人。门上没有封条，也没有名字——只刻着一个字：「封」。");
    b.say("四不相", "……典狱长就住在这里面？", speaker::SIBUXIANG);
    b.narr("他的脚步在门前慢了半拍。他不知道自己为什么慢。");
    b.jump("corridor4_back");

    // =====================================================================
    //  四、文书阁：四不相干活的地方（一个能走的地图）
    //  一屋子桌案，同事伏在案上抄写，最里头站着上司。
    //  进门的几句旁白挂在地图下方；跟谁说话、看什么东西都按回车。
    // =====================================================================
    std::vector<MapExit> officeExits = {
        {"上司", "office_boss"},
        {"同事", "office_colleague"},
        {"桌子", "office_desk"},
        {"椅子", "office_chair"},
        {"毛笔", "office_brush"},
        {"砚台", "office_ink"},
        {"门", "corridor4_back"},
        {"窗", "office_window"},
        {"纸", "office_paper"},
        {"竹简", "office_bamboo"},
        {"文卷", "office_scroll"},
        {"油灯", "office_lamp"},
        {"木柜", "office_cabinet"},
    };
    b.label("office");
    b.branch("office_intro_seen", "1", "office_reenter");
    b.setFlag("office_intro_seen", "1");
    {
        MapActor nar;
        nar.id = "narrator";
        nar.name = "旁白";
        nar.x = 3;
        nar.y = 9;
        nar.unseen = true;
        nar.steps.push_back(ScriptBuilder::actNarr(
            "文书阁里一股旧纸、陈墨和人身上的暖气。天光从高窗上下来，落在两排桌案之间，浮着一层细灰。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "两个同事已经伏在案上抄了半日，笔尖沙沙地响，谁也没抬头。"));
        nar.steps.push_back(ScriptBuilder::actNarr(
            "最里头那张桌边上站着一个人，官袍沉沉地垂着，正低头看手里的一卷竹简。"));
        nar.steps.push_back(ScriptBuilder::actFinish());

        std::vector<MapActor> actors;
        actors.push_back(nar);
        b.mapWithActors("office", 5, 9, officeExits, actors);
    }

    b.label("office_reenter");
    b.map("office", 5, 9, officeExits);

    b.label("office_plain");
    b.map("office", -1, -1, officeExits);

    b.label("office_colleague");
    b.narr("隔壁案的同事抬起头，笔尖还在滴墨，在纸上洇出一个圆点。");
    b.say("同事", "四不相？你脸怎么这么白。", speaker::SHANGSI);
    b.say("同事", "一大早的，撞见谁了？", speaker::SHANGSI);
    b.say("四不相", "……没、没有。走路急。", speaker::SIBUXIANG);
    b.narr("同事「哦」了一声，低头继续抄。抄了两笔，又想起什么似的补了一句。");
    b.say("同事", "上司一早就来了。在你那张桌子边上站了半天，又回里头去了。", speaker::SHANGSI);
    b.narr("四不相的心往下一沉。");
    b.jump("office_plain");

    b.label("office_desk");
    b.narr("昨天没抄完的文卷还摊在桌上，纸边被他自己的爪印蹭得发黑。");
    b.narr("笔架上的笔没洗，墨干在笔尖上，硬得像一根小骨头。");
    b.jump("office_plain");

    b.label("office_chair");
    b.narr("凳子少了一条腿，底下垫着一块叠了三次的旧纸。坐上去会晃，但还能坐。");
    b.jump("office_plain");

    b.label("office_brush");
    b.narr("笔杆上有一圈牙印——是他自己咬的。抄到眼睛发花的时候，他就咬笔杆。");
    b.jump("office_plain");

    b.label("office_ink");
    b.narr("砚池里的墨是新磨的，凉。磨墨的人很用力，池边溅出来几点。");
    b.jump("office_plain");

    b.label("office_paper");
    b.narr("一沓空白纸，纸角裁得齐整。用了半沓——今天还有半沓要填满。");
    b.jump("office_plain");

    b.label("office_bamboo");
    b.narr("竹简按日子码成一摞，最上面那卷是今天早会上要交的。");
    b.narr("他伸手把它抽出来，又塞了回去——先跟里头那位说清楚，再说别的。");
    b.jump("office_plain");

    b.label("office_scroll");
    b.narr("摊开的文卷上有一行抄了一半的字，墨迹断在半句上。昨天就是抄到这儿被打断的。");
    b.jump("office_plain");

    b.label("office_lamp");
    b.narr("灯还亮着，灯芯剪过，火苗很稳。这一屋子的白天，都要靠它撑到天黑。");
    b.jump("office_plain");

    b.label("office_cabinet");
    b.narr("木柜里是一格一格的档卷，标签上写着年份。最早的那一格，灰厚得能写字。");
    b.jump("office_plain");

    b.label("office_window");
    b.narr("窗外是回廊的廊顶，灰白色的一大片，看不见天。");
    b.jump("office_plain");

    // ---- 走到上司跟前，按回车说话：四、文书阁的正戏从这儿开始 ----
    b.label("office_boss");

    // =====================================================================
    //  四、文书阁（含玩家内心选择）
    // =====================================================================
    b.scene("四 · 文书阁", "scene_office");
    b.narr("四不相抱着公文，一步一步挪到最里头那张桌前。");
    b.narr("上司手里捏着一卷竹简，不紧不慢地翻着。听见脚步声，他抬了一下眼皮，嘴角微微弯了一下——那种「我就知道你会迟到」的笑，浮在脸上。");
    b.say("上司", "又踩点。", speaker::SHANGSI);
    b.say("四不相", "对……对不起……我路上撞到了人……", speaker::SIBUXIANG);
    b.say("上司", "撞到人了？", speaker::SHANGSI);
    b.narr("上司挑了一下眉，目光落在四不相身上——毛发乱糟糟的，爪尖缺了一小块。他忽然笑了一下。");
    b.say("上司", "撞到谁了？这么大阵仗。", speaker::SHANGSI);
    b.say("四不相", "……困兽府的、典狱长大人……", speaker::SIBUXIANG);
    b.narr("上司的笑容僵了一瞬。他手里的竹简放下来了，眼神从「敷衍」变成了「审慎」。");
    b.say("上司", "典狱长？你撞到四不遇了？", speaker::SHANGSI);
    b.say("四不相", "……是。", speaker::SIBUXIANG);
    b.narr("上司沉默了一会儿。他重新打量了一下这只白毛麒麟——正缩着脖子，像一只等着被踹的旧棉絮。");
    b.narr("典狱长都没说什么，他再追究，倒显得自己比典狱长还大牌了。");
    b.say("上司", "行了。下次再迟到，自己去交调任文书。不用等我来批。", speaker::SHANGSI);
    b.narr("他转身走了。四不相站在原地，爪尖还抱着那摞竹简，前爪边缘渗着一丝极淡的暗金色——是刚才撞到典狱长时蹭出来的。");
    b.narr("他低头看着那一丝颜色，愣了一下。");
    b.say("四不相", "……暗金色的血？", speaker::SIBUXIANG);
    b.narr("他举起那只爪尖，对着窗漏进来的光看了看。暗金色的，很淡，像混进白色绒毛里的一线将熄未熄的余烬。");
    b.narr("他翻来覆去看了好一会儿，想不起来自己怎么会流这种颜色的血。");
    b.narr("最后，他把爪尖那一点暗金色擦掉了。他不知道自己为什么擦掉它。他只是觉得，留着那一点颜色，心口会更疼。");
    b.narr("他拿起笔，蘸墨，低头抄写。笔尖落在纸上的时候，他停了一下。");
    b.narr("他想起那双眼睛。琥珀色的。整个天庭都知道的典狱长，手段狠辣、不近人情。可那双眼睛看他的时候，为什么像在求他？");
    b.say("四不相", "……他为什么那样看我？", speaker::SIBUXIANG);
    b.narr("他问完愣了一下。他只是一个抄文书的，连迟到都要被罚的那种。典狱长怎么看他，跟他有什么关系？");
    b.narr("可写了两个字，笔又停了。他忽然发现，自己在回想那个声音——「你走吧，我不怪你。」");
    b.narr("那个声音很轻，像怕压碎什么。可他为什么要怕压碎我？");
    b.narr("他把笔搁下，站起来，走到窗边，看着窗纸上破碎的光。风从破了的窗纸缝里渗进来，凉凉的，扑在他脸上。");
    b.say("四不相", "……我们见过吗？", speaker::SIBUXIANG);
    b.narr("没有人回答他。他站在窗边，看着灰白色的天光从云缝里漏下来，落在窗台的石沿上。");
    b.narr("他的胸口又酸了一下，又空了一下，像有什么东西正被一只看不见的手从里面掏走，一点一点地，掏了一万年。");

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
    b.narr("他站起来，转身走回困兽府的方向。缺了一点的尾巴尖在风里晃了晃，像一根快要燃尽的香，还在撑着最后一寸不肯灭。");

    // ---- 回到值守室（回廊这条路今天已经跑过了，这里不再让他走一遍） ----
    b.jump("duty_night");
    b.label("duty_night");

    // =====================================================================
    //  五、夜晚，值守室
    // =====================================================================
    b.scene("五 · 夜晚，值守室", "scene_room_night2");
    b.narr("天色彻底暗下来。天庭的夜，是没有星也没有月的黑，暗沉沉地压在所有屋顶上，像一口倒扣的锅。");
    b.narr("四不相拖着步子走回值守室。下午的活没干完，上司临走前把一摞没抄完的文卷摔在他桌上——「明天早会前交齐。」");
    b.narr("他抄到掌灯时分才收笔，爪尖磨得发红。低头一看，那道暗金色的痕迹已经没了，像从没存在过一样。");
    b.narr("可他总觉得，那道痕迹还在，在他的骨头里。他擦不掉。");
    b.narr("他推开门，屋子里冷得像冰窖。窗纸破的三个洞还在漏风，他懒得补，裹紧棉絮坐在床边，发了好一会儿呆。");
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
    // =====================================================================
    b.scene("六 · 深夜，值守室", "scene_room_door");
    b.narr("门被轻轻拉开一条缝。没有声音。像一只爪子用了一万年的熟练度，让门轴连一声呻吟都没有发出来。");
    b.narr("一只灰白色的幼崽从缝里挤进来。琥珀色的眼睛在黑暗中亮了一瞬，又暗下去，像怕自己的光吵醒了床上的白麒麟。");
    b.narr("他很小，小到可以藏在桌腿的阴影里。他踮着脚走到床边，爪尖搭在床沿上，没有碰四不相。");
    b.say("灰白色幼崽", "哥……我来看你了……今天你在走廊上撞到我了……你跟我说对不起……你说「我还有人要等」……你在等谁啊……", speaker::SIBUYU);
    b.narr("他的声音很轻，轻到像在哄一个睡着的人。他等了一会儿，四不相没有醒。");
    b.say("灰白色幼崽", "你在等我吗？你是不是在等我？你是不是……已经忘了你在等我？", speaker::SIBUYU);
    b.narr("然后他停住了。因为四不相在梦里翻了个身，眉头皱了一下——他听见了什么，极远极远的声音，像从雪地里传来的。");
    b.narr("四不遇的爪尖颤了一下。他张了张嘴，没来得及缩回爪子——四不相醒了。");
    b.narr("四不相睁开眼，看见床边蹲着一只灰白色的幼崽。他先是迷茫，然后认出了那双眼睛——琥珀色的，和白天走廊里那只兽的眼睛一模一样。");
    b.narr("他坐起来，往下缩了缩，爪尖攥紧了棉絮边缘，把被子往上拉了拉。");
    b.say("四不相", "典、典狱长大人……您怎么来这儿了？这里不是您该待的地方……您回去吧……", speaker::SIBUXIANG);
    b.narr("幼崽愣住了。琥珀色的眼睛亮了一下——像一盏在风里摇了很久的灯，忽然被人拢住了——然后暗下去了。");
    b.narr("他站了很久。他的爪尖还搭在床沿上。他是想碰他的。他想了很久了。可他不敢。");
    b.say("灰白色幼崽", "哥……你不记得我了？", speaker::SIBUYU);
    b.narr("他说这句话的时候，声音很轻，轻到像他自己都不信会有人回答。可他还是问了。他每次都会问。他每次都知道答案。");
    b.narr("他脚边的阴影里，一朵极小的荼蘼花正悄悄地、无声地卷起花瓣，像在替他把没能喊出来的那一句吞回去。");
    b.say("四不相", "我……没有弟弟。", speaker::SIBUXIANG);
    b.narr("他说得很平静。平静得像一把封了鞘的刀。说完之后，他自己都觉得，那句话像从很远的、被洗干净的地方传来的回声。");
    b.narr("他看着那只幼崽——他看见他琥珀色的眼睛暗了一瞬，像一颗快要碎掉的琥珀，撑着一道裂缝，没有彻底裂开。");
    b.narr("可他看出来了。那一道裂缝，是最后一次了。");
    b.say("灰白色幼崽", "……嗯。我知道了。", speaker::SIBUYU);
    b.narr("他转身，一步一步走向门口。他的尾巴拖在地上，缺了那一截的地方在轻轻颤着。");
    b.narr("他走到门边的时候，爪尖搭在门框上，停住了。他没有回头。他在等一个声音。一个他等了一万年的声音。");
    b.say("四不相", "……你叫什么名字？", speaker::SIBUXIANG);
    b.narr("幼崽在门缝的光里停住了，没有回头。");
    b.say("灰白色幼崽", "……四不遇。", speaker::SIBUYU);
    b.say("四不相", "四不遇……好名字。我好像……在梦里听过。", speaker::SIBUXIANG);
    b.narr("幼崽的爪尖抠进木框里，木屑落了一地。他的琥珀色眼睛在黑暗里亮了一瞬——然后，那道裂缝终于裂开了。");
    b.narr("没有声音。没有碎响。只是有什么东西，在他心里彻底断开了。");
    b.say("灰白色幼崽", "……嗯。你听过。", speaker::SIBUYU);
    b.narr("他推开门，走了出去。门关上了。他在门外站了很久，极轻的，像是在等什么。");
    b.narr("门里面安安静静的，像没有人来过。");
    b.narr("四不相坐在床上，低头看着自己的爪心。那里不知什么时候多了一根灰白色的绒毛。");
    b.say("四不相", "四不遇……我好像真的……在梦里听过。", speaker::SIBUXIANG);
    b.narr("他低下头，把脸埋进爪心里。风从破纸洞里灌进来，呜呜地响，像有谁在很远的地方替他回答了一个问题——但那个回答，他听不见了。");
    b.narr("他不知道自己为什么想哭。他只是觉得，那只幼崽转身的时候，自己的心口缺了一块。");
    b.narr("他躺下去，蜷进棉絮里，闭上眼睛。黑暗淹没了整个房间。他睡着了。");
    b.narr("这一次，他睡着的时候，有一粒淡金色的光从门缝里渗进来，落在他眉心。他没有醒。");
    b.narr("光渗进他的梦里，把他拉向未知的方向。");

    // =====================================================================
    //  七、梦中，遇龙雪山
    // =====================================================================
    b.scene("七 · 梦中，遇龙雪山", "scene_dream_snow");
    b.narr("他梦见自己站在一片白茫茫的雪地上。风很大，雪粒打在脸上，像碎了的冰。");
    b.narr("远处，一只灰白色的幼崽正在跑——跑得很慢，跌跌撞撞的，像是在等他。");
    b.narr("四不相追上去。爪子陷进雪里，拔出来又陷进去，可他不想停。");
    b.narr("他不知道自己为什么跑。他只是觉得——如果这次追不到，就再也不会有下一次了。");
    b.say("四不相", "……你等等……你等等我……", speaker::SIBUXIANG);
    b.narr("他跑近了。那只灰白色的幼崽就在前面几步远的地方，缺了一截的尾巴尖在风里晃着，像一根快要燃尽的香。");
    b.narr("四不相伸出爪子，指尖快要碰到那截尾巴了——");
    b.narr("幼崽停下来，回过头。赤红色的眼睛在雪光里亮着，像一盏快要燃尽的灯，里面全是水光。");
    b.say("灰白色幼崽", "你追到我了。", speaker::YOULING);
    b.narr("四不相张了张嘴。他想问：你是谁？你叫什么？我们见过吗？可他一个字也没说出来。");
    b.narr("他看着那只幼崽——灰白色的毛发被雪弄得湿漉漉的，赤红色的眼睛亮晶晶的，蓬松的尾巴尖在身后轻轻晃着。");
    b.narr("他的胸口忽然疼了一下。很深。像有什么东西，一直埋在那里，终于被挖出来了——但他不知道那是什么。");
    b.narr("白光从幼崽身后涌过来。很慢，像水漫过堤坝，漫过四不相的蹄踝，漫过他的膝盖，漫过他的腰腹。");
    b.narr("他没有躲。他看见那只幼崽在光里看着他，嘴角弯了一下——很轻，轻到像怕吓到他——像一朵开了一万年、终于等到开花那天的花。");
    b.say("灰白色幼崽", "……进来吧。我都给你留着呢。", speaker::YOULING);
    b.narr("白光吞没了一切。雪地、风、赤红色的眼睛、那截缺了的尾巴尖——全部消失了。");
    b.narr("四不相觉得自己沉进了一片很暖很暖的光里，像沉进了一场，被等了一万年的梦里。");

    // =====================================================================
    //  序章 · 完
    // =====================================================================
    b.chapterEnd("序章 · 完", "故事，才刚刚开始。\n\n第一章「童年记忆篇」，敬请期待。");

    return b.done();
}

}  // namespace game
