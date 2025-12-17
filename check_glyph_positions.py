from PIL import Image

# Open Emerald font
img = Image.open('graphics/fonts/latin_short.png')

# Position of glyph 0xCD (205 decimal)
# Layout: 32 glyphs per row, 8 pixels wide, 16 pixels tall
glyph_idx = 0xCD
col = glyph_idx % 32
row = glyph_idx // 32
x = col * 8
y = row * 16

print(f"Glyph 0xCD (should be 'S'):")
print(f"  Position in 32-wide layout: row {row}, col {col}")
print(f"  Pixel coordinates: ({x}, {y})")

# Extract and save this glyph
glyph = img.crop((x, y, x + 8, y + 16))
glyph = glyph.resize((80, 160), Image.NEAREST)  # Scale up for visibility
glyph.save('glyph_CD.png')
print("  Saved to glyph_CD.png")

# Also check position 0x68 (104) which should be 'â'
glyph_idx = 0x68
col = glyph_idx % 32
row = glyph_idx // 32
x = col * 8
y = row * 16

print(f"\nGlyph 0x68 (should be 'â'):")
print(f"  Position in 32-wide layout: row {row}, col {col}")
print(f"  Pixel coordinates: ({x}, {y})")

glyph = img.crop((x, y, x + 8, y + 16))
glyph = glyph.resize((80, 160), Image.NEAREST)
glyph.save('glyph_68.png')
print("  Saved to glyph_68.png")

# Check what's at position 83 (0x53) which is where it might be looking
glyph_idx = 0x53
col = glyph_idx % 32
row = glyph_idx // 32
x = col * 8
y = row * 16

print(f"\nGlyph 0x53 (83 decimal, ASCII 'S' position):")
print(f"  Position in 32-wide layout: row {row}, col {col}")
print(f"  Pixel coordinates: ({x}, {y})")

glyph = img.crop((x, y, x + 8, y + 16))
glyph = glyph.resize((80, 160), Image.NEAREST)
glyph.save('glyph_53.png')
print("  Saved to glyph_53.png")
