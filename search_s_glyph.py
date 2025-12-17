from PIL import Image
import numpy as np

# Load Emerald font
img = Image.open('graphics/fonts/latin_normal.png')
pixels = img.load()

# Search for a glyph that looks like 'S' - should have good amount of non-zero pixels
# organized in an S shape
print("Searching for glyphs with content in latin_normal.png...")

glyphs_with_content = []
for i in range(256):
    col = i % 32
    row = i // 32
    x = col * 8
    y = row * 16
    
    glyph = img.crop((x, y, x + 8, y + 16))
    arr = np.array(glyph)
    non_zero = (arr != 0).sum()
    
    if non_zero > 10:  # Has significant content
        glyphs_with_content.append((i, non_zero, x, y))

print(f"\nFound {len(glyphs_with_content)} glyphs with content in first 256 positions:")
for idx, count, x, y in glyphs_with_content[:20]:  # Show first 20
    print(f"  Position {idx} (0x{idx:02X}): {count} pixels at ({x}, {y})")

# Check ASCII position for 'S' which is 0x53 = 83
print(f"\n--- Checking ASCII position 0x53 (83) ---")
glyph = img.crop((152, 32, 160, 48))  # 83 = row 2, col 19
arr = np.array(glyph)
print(f"Pixels: {np.unique(arr, return_counts=True)}")
if (arr != 0).sum() > 0:
    glyph_large = glyph.resize((80, 160), Image.NEAREST)
    glyph_large.save('emerald_pos_53.png')
    print("Saved to emerald_pos_53.png - THIS MIGHT BE 'S'!")
