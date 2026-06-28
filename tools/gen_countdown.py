from PIL import Image, ImageDraw, ImageFont
import os, sys

LW, LH = 296, 128  # landscape after 90° rotation
BW = 128

BLACK = 0
WHITE = 1

font_path = r"C:\Windows\Fonts\msyhbd.ttc"
if not os.path.exists(font_path):
    font_path = r"C:\Windows\Fonts\msyh.ttc"

top_text = "粤S65XP7"

# Find font size: fit 92% width
lo, hi = 20, 60
best = 24
while lo <= hi:
    mid = (lo + hi) // 2
    f = ImageFont.truetype(font_path, size=mid)
    w = f.getbbox(top_text)[2] - f.getbbox(top_text)[0]
    if w <= LW * 0.92:
        best = mid
        lo = mid + 1
    else:
        hi = mid - 1

font = ImageFont.truetype(font_path, size=best)
tw, th = font.getbbox(top_text)[2] - font.getbbox(top_text)[0], \
        font.getbbox(top_text)[3] - font.getbbox(top_text)[1]

# Top text centered, y=2
top_y = 2
tx = (LW - tw) // 2

img = Image.new('RGB', (LW, LH), (255, 255, 255))
draw = ImageDraw.Draw(img)
draw.text((tx, top_y), top_text, font=font, fill=(0, 0, 0))

# Encode to 4-color buffer (128x296, 9472 bytes)
buf = bytearray(BW // 4 * 296)
for ly in range(LH):
    for lx in range(LW):
        r, g, b = img.getpixel((lx, ly))
        color = BLACK if r < 100 and g < 100 and b < 100 else WHITE
        b_x = 127 - ly
        b_y = lx
        byte_idx = b_y * 32 + (b_x // 4)
        bit_shift = (3 - (b_x % 4)) * 2
        buf[byte_idx] |= (color << bit_shift)

print(f"// 顶部文字: '{top_text}', 字号={best}px, 文字区域 y=0..{top_y+th}")
print(f"// 预渲染四色位图, 128x296, 9472 字节")
print(f"static const unsigned char TopImage[9472] = {{")
for i in range(0, len(buf), 16):
    row = buf[i:i+16]
    h = ",".join(f"0x{b:02X}" for b in row)
    print(f"    {h}," if i + 16 < len(buf) else f"    {h}")
print("};")
print(f"#define TOP_TEXT_BOTTOM {top_y + th}  // 顶部文字底部 Y 坐标(横屏)")

# 同时写入文件，避免 shell 重定向导致的编码问题
header_path = os.path.join(os.path.dirname(__file__), "..", "main", "top_banner.h")
with open(header_path, "w", encoding="utf-8") as f:
    f.write(f"// 顶部文字: '{top_text}', 字号={best}px, 文字区域 y=0..{top_y+th}\n")
    f.write(f"// 预渲染四色位图, 128x296, 9472 字节\n")
    f.write(f"static const unsigned char TopImage[9472] = {{\n")
    for i in range(0, len(buf), 16):
        row = buf[i:i+16]
        h = ",".join(f"0x{b:02X}" for b in row)
        f.write(f"    {h},\n" if i + 16 < len(buf) else f"    {h}\n")
    f.write("};\n")
    f.write(f"#define TOP_TEXT_BOTTOM {top_y + th}  // 顶部文字底部 Y 坐标(横屏)\n")
print(f"// 已写入 {header_path}", file=sys.stderr)
