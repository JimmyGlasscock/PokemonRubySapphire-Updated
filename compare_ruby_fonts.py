from PIL import Image

# Check both original Ruby font and our new one
ruby_orig = Image.open('graphics/fonts/font3_lat_original.png')
ruby_new = Image.open('graphics/fonts/font3_lat_new.png')

# Position 0xCD (205) - should be 'S'
glyph_idx = 0xCD
# Ruby layout: 16 glyphs per row
col = glyph_idx % 16
row = glyph_idx // 16
x = col * 8
y = row * 16

print(f"Position 0xCD (205 - should be 'S') in Ruby fonts:")
print(f"  16-wide layout: row {row}, col {col}, coords ({x}, {y})")

# Save both for comparison
glyph_orig = ruby_orig.crop((x, y, x + 8, y + 16))
glyph_orig = glyph_orig.resize((80, 160), Image.NEAREST)
glyph_orig.save('ruby_orig_CD.png')

glyph_new = ruby_new.crop((x, y, x + 8, y + 16))
glyph_new = glyph_new.resize((80, 160), Image.NEAREST)
glyph_new.save('ruby_new_CD.png')

print("  Saved ruby_orig_CD.png and ruby_new_CD.png")

# Position 0x68 (104) - should be 'â'  
glyph_idx = 0x68
col = glyph_idx % 16
row = glyph_idx // 16
x = col * 8
y = row * 16

print(f"\nPosition 0x68 (104 - should be 'â') in Ruby fonts:")
print(f"  16-wide layout: row {row}, col {col}, coords ({x}, {y})")

glyph_orig = ruby_orig.crop((x, y, x + 8, y + 16))
glyph_orig = glyph_orig.resize((80, 160), Image.NEAREST)
glyph_orig.save('ruby_orig_68.png')

glyph_new = ruby_new.crop((x, y, x + 8, y + 16))
glyph_new = glyph_new.resize((80, 160), Image.NEAREST)
glyph_new.save('ruby_new_68.png')

print("  Saved ruby_orig_68.png and ruby_new_68.png")
