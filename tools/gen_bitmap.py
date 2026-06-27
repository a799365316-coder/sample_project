from PIL import Image, ImageDraw, ImageFont
import os, sys

TARGET_W = 240
text = "粤S65XP7"

fp = r"C:\Windows\Fonts\msyhbd.ttc"
if not os.path.exists(fp):
    fp = r"C:\Windows\Fonts\msyh.ttc"

lo, hi = 20, 100
best_size = 20
while lo <= hi:
    mid = (lo + hi) // 2
    test_font = ImageFont.truetype(fp, size=mid)
    bbox = test_font.getbbox(text)
    if (bbox[2] - bbox[0]) <= TARGET_W:
        best_size = mid
        lo = mid + 1
    else:
        hi = mid - 1

font = ImageFont.truetype(fp, size=best_size)
bbox = font.getbbox(text)
tw = bbox[2] - bbox[0]
th = bbox[3] - bbox[1]

img = Image.new('1', (tw, th), 0)
draw = ImageDraw.Draw(img)
draw.text((-bbox[0], -bbox[1]), text, font=font, fill=1)

w_byte = (tw + 7) // 8
bytes_list = []
for row in range(th):
    for col in range(0, tw, 8):
        byte_val = 0
        for bit in range(8):
            if col + bit < tw and img.getpixel((col + bit, row)):
                byte_val |= (0x80 >> bit)
        bytes_list.append(byte_val)

total = len(bytes_list)
print(f"// {tw}x{th}, {w_byte} bytes/row, {total} total bytes")
print(f"static const unsigned char LicenseImage[{total}] = {{")
for i in range(0, total, 16):
    line = bytes_list[i:i+16]
    hex_str = ",".join(f"0x{b:02X}" for b in line)
    if i + 16 < total:
        print(f"    {hex_str},")
    else:
        print(f"    {hex_str}")
print("};")
print(f"#define IMG_W  {tw}")
print(f"#define IMG_H  {th}")
print(f"#define IMG_WB {w_byte}")
