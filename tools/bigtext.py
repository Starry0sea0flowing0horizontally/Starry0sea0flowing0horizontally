# -*- coding: utf-8 -*-
"""把一句话渲染成终端「大字号」：用 ▀▄█ 半块字符，1 像素宽 = 1 列，2 像素高 = 1 行。
生成 src/bigtext.h，供结语页显示「我在看着你」这几个大字。"""
import io
from PIL import Image, ImageDraw, ImageFont

TEXT = "我在看着你"
FONT = r"C:\Windows\Fonts\msyhbd.ttc"
SIZE = 14           # 字号（像素）：14px -> 7 行高，5 个字约 70 列
THRESHOLD = 100     # 灰度阈值：低于这个值算「有笔画」

font = ImageFont.truetype(FONT, SIZE)
w, h = font.getsize(TEXT)
img = Image.new("L", (w, h + 4), 0)
ImageDraw.Draw(img).text((0, 0), TEXT, font=font, fill=255)
px = img.load()

rows = []
for y in range(0, h, 2):
    line = []
    for x in range(w):
        top = px[x, y] >= THRESHOLD
        bot = (y + 1 < h) and px[x, y + 1] >= THRESHOLD
        line.append("\u2588" if (top and bot) else
                    "\u2580" if top else
                    "\u2584" if bot else " ")
    rows.append("".join(line).rstrip())

# 不往控制台打（GBK 控制台印不了半块字符），直接看生成的 bigtext.h

out = ['// 自动生成：tools/bigtext.py（Pillow + 微软雅黑粗体 渲染，半块字符拼成）',
       '// 结语页那句红字的大字号版本，一行一个字符串；只有 game.cpp 会 include。',
       '#pragma once',
       '',
       'namespace bigtext {',
       '',
       'constexpr const char* const WATCHING[] = {']
for line in rows:
    out.append('    "%s",' % line.replace("\\", "\\\\").replace('"', '\\"'))
out += ['};', '',
        '}  // namespace bigtext', '']
io.open(r"E:\花至荼蘼\04.游戏程序\src\bigtext.h", "w",
        encoding="utf-8", newline="\n").write("\n".join(out))
print("wrote src/bigtext.h")
