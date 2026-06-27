from PIL import Image, ImageDraw, ImageFont
import os, sys, math

# 屏幕旋转后 296x128，文字占 80% 宽度 ≈ 237px
TARGET_W = 240
TARGET_H = 90  # 高度约 70% 屏幕高度

text = "粤S65XP7"

# 找到最好的中文字体
font_paths = [
    r"C:\Windows\Fonts\msyhbd.ttc",   # 微软雅黑粗体(优先)
    r"C:\Windows\Fonts\msyh.ttc",
    r"C:\Windows\Fonts\simhei.ttf",
]

font = None
for fp in font_paths:
    if os.path.exists(fp):
        try:
            font = ImageFont.truetype(fp, size=60)
            print(f"Using font: {fp}", file=sys.stderr)
            break
        except Exception as e:
            print(f"Failed {fp}: {e}", file=sys.stderr)

if font is None:
    print("ERROR: No font found", file=sys.stderr)
    sys.exit(1)

# 二分查找最佳字号
lo, hi = 20, 100
best_size = 20
while lo <= hi:
    mid = (lo + hi) // 2
    test_font = ImageFont.truetype(fp, size=mid)
    bbox = test_font.getbbox(text)
    w = bbox[2] - bbox[0]
    if w <= TARGET_W:
        best_size = mid
        lo = mid + 1
    else:
        hi = mid - 1

print(f"Best font size: {best_size}", file=sys.stderr)
font = ImageFont.truetype(fp, size=best_size)
bbox = font.getbbox(text)
tw = bbox[2] - bbox[0]
th = bbox[3] - bbox[1]
print(f"Text size: {tw}x{th}", file=sys.stderr)

# 创建单色位图 (白底黑字 → 实际上色逻辑在代码里处理)
img = Image.new('1', (tw, th), 0)  # 黑底
draw = ImageDraw.Draw(img)
draw.text((-bbox[0], -bbox[1]), text, font=font, fill=1)  # 白字

print(f"Bitmap: {tw}x{th}", file=sys.stderr)

# 预览
print("// Preview:", file=sys.stderr)
for row in range(th):
    line = ""
    for col in range(tw):
        line += "##" if img.getpixel((col, row)) else ".."
    print(line, file=sys.stderr)

# 生成 C 数组 (1 bit per pixel, 每行 (tw+7)/8 字节)
width_bytes = (tw + 7) // 8
bytes_list = []
for row in range(th):
    row_data = 0
    bit_count = 0
    for col in range(tw):
        if img.getpixel((col, row)):
            row_data |= (0x80 >> (col % 8))
        if col % 8 == 7:
            bytes_list.append(row_data)
            row_data = 0
    if tw % 8 != 0:
        bytes_list.append(row_data)

total_bytes = len(bytes_list)
print(f"\n// Total bytes: {total_bytes}", file=sys.stderr)
print(f"// Width: {tw}, Height: {th}")
print(f"// Width bytes per row: {width_bytes}")
print(f"const unsigned char LicenseImage[{total_bytes}] = {{")
for i in range(0, len(bytes_list), 16):
    line_data = bytes_list[i:i+16]
    hex_str = ",".join(f"0x{b:02X}" for b in line_data)
    if i + 16 < len(bytes_list):
        print(f"    {hex_str},")
    else:
        print(f"    {hex_str}")
print("};")
