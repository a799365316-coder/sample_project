from PIL import Image, ImageDraw, ImageFont
import os, sys

W, H = 32, 41

font_paths = [
    r"C:\Windows\Fonts\msyh.ttc",
    r"C:\Windows\Fonts\simhei.ttf",
    r"C:\Windows\Fonts\simsun.ttc",
    r"C:\Windows\Fonts\STSONG.TTF",
]

font = None
for fp in font_paths:
    if os.path.exists(fp):
        try:
            font = ImageFont.truetype(fp, 30)
            print(f"Using font: {fp}", file=sys.stderr)
            break
        except Exception as e:
            print(f"Failed {fp}: {e}", file=sys.stderr)

if font is None:
    # Try any available font
    fonts_dir = r"C:\Windows\Fonts"
    for f in os.listdir(fonts_dir):
        if f.endswith('.ttf') or f.endswith('.ttc'):
            try:
                fp = os.path.join(fonts_dir, f)
                font = ImageFont.truetype(fp, 30)
                print(f"Using font: {fp}", file=sys.stderr)
                break
            except:
                continue

if font is None:
    print("ERROR: No font found", file=sys.stderr)
    sys.exit(1)

img = Image.new('1', (W, H), 0)
draw = ImageDraw.Draw(img)

bbox = draw.textbbox((0, 0), "粤", font=font)
tw = bbox[2] - bbox[0]
th = bbox[3] - bbox[1]
x = (W - tw) // 2 - bbox[0]
y = (H - th) // 2 - bbox[1]
draw.text((x, y), "粤", font=font, fill=1)

# Preview
print("// Bitmap preview for 粤:", file=sys.stderr)
for row in range(H):
    line = ""
    for col in range(W):
        line += "##" if img.getpixel((col, row)) else ".."
    print(line, file=sys.stderr)

# Generate C array
bytes_list = []
for row in range(H):
    row_data = 0
    bit_idx = 0
    for col in range(W):
        if img.getpixel((col, row)):
            row_data |= (0x80 >> (col % 8))
        if col % 8 == 7:
            bytes_list.append(row_data)
            row_data = 0
    if W % 8 != 0:
        bytes_list.append(row_data)

print(f"// Total bytes: {len(bytes_list)}", file=sys.stderr)

# Output C array
for i in range(0, len(bytes_list), 16):
    line_data = bytes_list[i:i+16]
    hex_str = ",".join(f"0x{b:02X}" for b in line_data)
    print(f"    {hex_str},")
