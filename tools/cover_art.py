# -*- coding: utf-8 -*-
"""把开场插画转成终端字符画（上半块 U+2580 + 24bit 真色彩），生成 src/cover_art.h。

用法:  python cover_art.py [源图] [窄列数] [宽列数]

  源图默认 assets/images/prologue_cover.png
  生成三套尺寸，各司其职：
    Title  —— 标题画面里、标题「花至荼蘼」正下方的小图（竖直空间有限）
    Small  —— 80 列窗口的通用配图
    Large  —— 宽终端的通用配图
  另外输出放大预览图 assets/images/prologue_cover_preview.png

原理：一个字符格 = 上下两个像素 —— 前景色画 ▀ 的上半，背景色填同一格的下半。
  这样 N 列 × R 行 = N×2R 个像素。终端字符高≈宽×2，用小写 ▀ 的上下两色正好贴合。
清晰度提升：
  1) 降采样前先 UnsharpMask 锐化，边缘不会被缩放抹糊；
  2) 用 LANCZOS 高质量缩略（比 BOX 更锐）；
  3) 「墨线保留」降采样：按最暗/均值压低亮度，细线留得住。
"""
import os
import sys

import numpy as np
from PIL import Image, ImageEnhance, ImageFilter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC_IMG = os.path.join(ROOT, "assets", "images", "prologue_cover.png")
HEADER = os.path.join(ROOT, "src", "cover_art.h")
PREVIEW = os.path.join(ROOT, "assets", "images", "prologue_cover_preview.png")

TOP = "\u2580"          # ▀
RESET = "\x1b[0m"
INK = 0.72              # 墨线保留强度：0 关闭，1 完全按局部最暗值压
CONTRAST = 1.18
SHARPEN = 90            # UnsharpMask 百分比，越大越锐
SHARPEN_RADIUS = 2

try:
    RS = Image.Resampling.LANCZOS
except AttributeError:          # pragma: no cover
    RS = Image.LANCZOS


def content_box(im):
    """找出画面主体的包围盒（忽略四周的纯色底）。"""
    a = np.asarray(im).astype(np.int16)
    border = np.concatenate([a[0], a[-1], a[:, 0], a[:, -1]])
    bg = np.median(border, axis=0)
    mask = (np.abs(a - bg).max(axis=2) > 28)
    ys, xs = np.nonzero(mask)
    if len(ys) == 0:
        return 0, 0, im.width, im.height
    return int(xs.min()), int(ys.min()), int(xs.max()) + 1, int(ys.max()) + 1


def fit(im, cols, rows):
    """把源图裁/缩成 cols x (rows*2) 像素，保持比例，优先保留主体。"""
    w0, h0 = im.size
    pw, ph = cols, rows * 2
    # 目标画布宽高比
    ratio = pw / float(ph)
    src_ratio = w0 / float(h0)
    if abs(src_ratio - ratio) < 1e-3:
        sub = im
    elif src_ratio > ratio:
        # 源图偏宽：裁左右到同比例
        new_w = int(round(h0 * ratio))
        x0 = (w0 - new_w) // 2
        sub = im.crop((x0, 0, x0 + new_w, h0))
    else:
        # 源图偏高：优先按主体居中裁上下
        new_h = int(round(w0 / ratio))
        _, y0c, _, y1c = content_box(im)
        y0 = (y0c + y1c) // 2 - new_h // 2
        y0 = max(0, min(y0, h0 - new_h))
        sub = im.crop((0, y0, w0, y0 + new_h))
    return sub.resize((pw, ph), RS)


def render(im, cols, rows):
    """返回 (每行一个带颜色转义的字符串, 实际像素矩阵)"""
    small = fit(im, cols, rows)
    px = np.asarray(small).astype(np.float32)

    # 墨线保留：按每个目标像素覆盖区域的「最暗值 / 平均值」压低亮度，细线才留得住
    w0, h0 = im.size
    g = np.asarray(im.convert("L")).astype(np.float32) / 255.0
    xs = np.linspace(0, w0, cols + 1).astype(int)
    ys = np.linspace(0, h0, rows * 2 + 1).astype(int)
    xs[-1], ys[-1] = w0, h0
    gmin = np.minimum.reduceat(np.minimum.reduceat(g, ys[:-1], axis=0), xs[:-1], axis=1)
    gmean = px.mean(axis=2) / 255.0
    f = np.clip(1.0 - INK + INK * gmin / np.maximum(gmean, 1e-3), 0.25, 1.3)[:, :, None]
    px = np.clip(px * f, 0, 255).astype(np.uint8)

    lines = []
    for r in range(rows):
        top, bot = px[2 * r], px[2 * r + 1]
        out, last_fg, last_bg = [], None, None
        for c in range(cols):
            fg = tuple(int(v) for v in top[c])
            bg = tuple(int(v) for v in bot[c])
            if fg != last_fg:
                out.append("\x1b[38;2;%d;%d;%dm" % fg)
                last_fg = fg
            if bg != last_bg:                     # 同色不重复发转义，行能短一半
                out.append("\x1b[48;2;%d;%d;%dm" % bg)
                last_bg = bg
            out.append(TOP)
        out.append(RESET)
        lines.append("".join(out))
    return lines, px


def cstr(s):
    """变成纯 ASCII 的 C++ 字面量：源码里不出现任何非 ASCII 字节。"""
    s = s.replace("\\", "\\\\").replace("\"", "\\\"")
    s = s.replace("\x1b", "\\x1b")
    s = s.replace(TOP, "\\xe2\\x96\\x80")
    return '"' + s + '"'


def main():
    args = sys.argv[1:]
    src = args[0] if len(args) > 0 else SRC_IMG
    narrow = int(args[1]) if len(args) > 1 else 76
    wide = int(args[2]) if len(args) > 2 else 92

    im = Image.open(src).convert("RGB")
    im = ImageEnhance.Contrast(im).enhance(CONTRAST)
    if SHARPEN > 0:
        im = im.filter(ImageFilter.UnsharpMask(radius=SHARPEN_RADIUS,
                                               percent=SHARPEN, threshold=2))
    print("source %s  %dx%d" % (os.path.basename(src), im.width, im.height))

    # Title 尺寸：标题画面下方空间有限，用小尺寸；Small/Large 供通用配图
    variants = []
    for name, cols, rows in (
            ("Title", 76, 14),
            ("Small", narrow, 24),
            ("Large", wide, 34)):
        lines, px = render(im, cols, rows)
        variants.append((name, cols, rows, lines))
        if name == "Large":
            Image.fromarray(px, "RGB").resize((cols * 8, rows * 16), Image.NEAREST).save(PREVIEW)
        print("%-6s %2dx%-2d  %d 行" % (name, cols, rows, len(lines)))

    out = ["// 自动生成，勿手改：python tools/cover_art.py",
           "// 开场插画：上半块字符 U+2580 + 24bit 真色彩，底色上两只对视的麒麟。",
           "// Title 用于标题画面；Small/Large 供通用配图（按终端宽度选择）。",
           "#ifndef COVER_ART_H",
           "#define COVER_ART_H",
           "",
           "namespace cover_art {",
           ""]
    for name, cols, rows, lines in variants:
        out.append("static const int k%sWidth = %d;" % (name, cols))
        out.append("static const int k%sHeight = %d;" % (name, rows))
        out.append("static const char* const k%s[] = {" % name)
        out.extend("    " + cstr(ln) + "," for ln in lines)
        out.append("};")
        out.append("")
    out += ["}  // namespace cover_art", "", "#endif  // COVER_ART_H"]

    with open(HEADER, "w", encoding="utf-8", newline="\n") as fh:
        fh.write("\n".join(out) + "\n")
    print("wrote src/cover_art.h  (%d KB)" % (os.path.getsize(HEADER) // 1024))


if __name__ == "__main__":
    main()
