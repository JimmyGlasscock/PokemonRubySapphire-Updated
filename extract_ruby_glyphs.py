#!/usr/bin/env python3
"""
Extract first 256 glyphs from Emerald's 256-wide font and rearrange to Ruby's 128-wide layout.
Emerald fonts: 256x512 (32 glyphs wide x 32 rows = 1024 glyphs of 8x16)
Ruby fonts: 128x256 (16 glyphs wide x 16 rows = 256 glyphs of 8x16)
"""

from PIL import Image
import sys

def extract_ruby_glyphs(emerald_path, ruby_original_path, ruby_output_path):
    # Load Emerald font (256x512, 32 glyphs per row, 8x16 per glyph)
    emerald = Image.open(emerald_path)
    
    # Load Ruby's original font to preserve encoding
    ruby = Image.open(ruby_original_path)
    
    # Ensure both are in palette mode
    if emerald.mode != 'P':
        emerald = emerald.convert('P', palette=Image.ADAPTIVE, colors=4)
    if ruby.mode != 'P':
        ruby = ruby.convert('P', palette=Image.ADAPTIVE, colors=3)
    
    # Ensure Ruby has full 256-color palette for 8-bit depth
    ruby_palette = [
        255, 255, 255,  # Index 0: white (transparent background)
        56, 56, 56,     # Index 1: dark
        216, 216, 216,  # Index 2: light gray
    ] + [0] * (256 * 3 - 9)  # Pad to 256 colors
    ruby.putpalette(ruby_palette)
    
    # Get pixel access
    ruby_pixels = ruby.load()
    em_pixels = emerald.load()
    
    glyphs_copied = 0
    glyphs_kept = 0
    
    # Calculate how many glyphs Ruby font can hold (based on its dimensions)
    ruby_glyphs = (ruby.height // 16) * (ruby.width // 8)
    
    # For each glyph position
    for i in range(min(256, ruby_glyphs)):
        # Calculate source position in Emerald font (32-wide layout)
        em_col = i % 32
        em_row = i // 32
        em_x = em_col * 8
        em_y = em_row * 16
        
        # Calculate position in Ruby font (16-wide layout)
        rb_col = i % 16
        rb_row = i // 16
        rb_x = rb_col * 8
        rb_y = rb_row * 16
        
        # Check if Emerald glyph has any non-background content
        has_content = False
        for dy in range(16):
            for dx in range(8):
                em_val = em_pixels[em_x + dx, em_y + dy]
                if em_val != 0:  # Not background
                    has_content = True
                    break
            if has_content:
                break
        
        # If Emerald has content, copy it; otherwise keep Ruby's original
        if has_content:
            glyphs_copied += 1
            # Copy and remap: Emerald 0=bg, 1=dark, 2=gray, 3=white -> Ruby 0=white(transparent), 1=dark, 2=gray
            for dy in range(16):
                for dx in range(8):
                    em_val = em_pixels[em_x + dx, em_y + dy]
                    if em_val == 0 or em_val == 3:  # bg or white -> transparent
                        ruby_pixels[rb_x + dx, rb_y + dy] = 0
                    elif em_val == 1:  # dark -> dark
                        ruby_pixels[rb_x + dx, rb_y + dy] = 1
                    elif em_val == 2:  # gray -> gray
                        ruby_pixels[rb_x + dx, rb_y + dy] = 2
        else:
            glyphs_kept += 1
            # Keep Ruby's original glyph (already there)

    
    ruby.save(ruby_output_path)
    print(f"Created {ruby_output_path} from {emerald_path} overlaid on {ruby_original_path}")
    print(f"  Copied {glyphs_copied} glyphs from Emerald, kept {glyphs_kept} from Ruby")

if __name__ == '__main__':
    extract_ruby_glyphs('../pokeemerald/graphics/fonts/latin_normal.png', 
                        'graphics/fonts/font3_lat_original.png',
                        'graphics/fonts/font3_lat_new.png')
    extract_ruby_glyphs('../pokeemerald/graphics/fonts/latin_narrow.png', 
                        'graphics/fonts/font4_lat_original.png',
                        'graphics/fonts/font4_lat_new.png')
    print("Done! Fonts converted to 3-color indexed mode.")
