from PIL import Image
import numpy as np

# Check all Emerald Latin fonts to see where content actually is
fonts = ['latin_normal', 'latin_narrow', 'latin_short', 'latin_small']

for font_name in fonts:
    try:
        img = Image.open(f'../pokeemerald/graphics/fonts/{font_name}.png')
        pixels = img.load()
        
        print(f"\n{font_name}.png ({img.size[0]}x{img.size[1]}):")
        
        # Check first 256 glyph positions
        non_empty_glyphs = []
        for i in range(min(256, (img.size[0]//8) * (img.size[1]//16))):
            col = i % (img.size[0] // 8)
            row = i // (img.size[0] // 8)
            x = col * 8
            y = row * 16
            
            # Check if glyph has any non-zero pixels
            has_content = False
            for dy in range(16):
                for dx in range(8):
                    if pixels[x + dx, y + dy] != 0:
                        has_content = True
                        break
                if has_content:
                    break
            
            if has_content:
                non_empty_glyphs.append(i)
        
        print(f"  Non-empty glyphs in first 256: {len(non_empty_glyphs)}")
        if len(non_empty_glyphs) > 0:
            print(f"  First 20 positions: {non_empty_glyphs[:20]}")
            print(f"  Last 20 positions: {non_empty_glyphs[-20:]}")
            
            # Check if they're in a pattern
            if len(non_empty_glyphs) > 1:
                gaps = [non_empty_glyphs[i+1] - non_empty_glyphs[i] for i in range(min(10, len(non_empty_glyphs)-1))]
                print(f"  First gaps: {gaps}")
    except Exception as e:
        print(f"  Error: {e}")
