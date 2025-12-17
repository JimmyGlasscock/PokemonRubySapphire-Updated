#!/usr/bin/env python3
"""
Remap Emerald fonts from 2-byte encoding (even positions) to Ruby's 1-byte encoding.
Emerald stores glyphs at positions 0, 2, 4, 6, ... for characters 0, 1, 2, 3, ...
Ruby stores glyphs at positions 0, 1, 2, 3, ... for characters 0, 1, 2, 3, ...
"""

from PIL import Image

def remap_emerald_to_ruby(emerald_path, ruby_original_path, ruby_output_path, glyph_height=16):
    # Load Emerald font (256x512, 32 glyphs per row)
    emerald = Image.open(emerald_path)
    
    # Load Ruby's original font as base
    ruby = Image.open(ruby_original_path)
    
    # Ensure palette mode
    if emerald.mode != 'P':
        emerald = emerald.convert('P', palette=Image.ADAPTIVE, colors=4)
    if ruby.mode != 'P':
        ruby = ruby.convert('P', palette=Image.ADAPTIVE, colors=3)
    
    # Set Ruby palette (256 colors for 8-bit depth)
    # Ruby font rendering expects colors at indices 0, 14 (shadow), and 15 (foreground)
    ruby_palette = [255, 255, 255] * 256  # Initialize all to white
    ruby_palette[0:3] = [255, 255, 255]   # Index 0: white (transparent/background)
    ruby_palette[14*3:14*3+3] = [216, 216, 216]  # Index 14 (0xE): light gray (shadow)
    ruby_palette[15*3:15*3+3] = [56, 56, 56]     # Index 15 (0xF): dark (foreground/text)
    ruby.putpalette(ruby_palette)
    
    # Get pixel access
    ruby_pixels = ruby.load()
    em_pixels = emerald.load()
    
    # Calculate max glyphs Ruby can hold
    ruby_glyphs = (ruby.height // glyph_height) * (ruby.width // 8)
    
    glyphs_copied = 0
    glyphs_kept = 0
    
    # For each Ruby glyph position
    for ruby_pos in range(ruby_glyphs):
        # Emerald uses 2-byte encoding: character N is at glyph position 2*N
        emerald_pos = ruby_pos * 2
        
        # Calculate Ruby position (16-wide layout)
        rb_col = ruby_pos % 16
        rb_row = ruby_pos // 16
        rb_x = rb_col * 8
        rb_y = rb_row * glyph_height
        
        # Calculate Emerald position (32-wide layout, always 16 tall)
        em_col = emerald_pos % 32
        em_row = emerald_pos // 32
        em_x = em_col * 8
        em_y = em_row * 16
        
        # Check if Emerald position is within bounds
        if em_y + 16 > emerald.height:
            glyphs_kept += 1
            continue
        
        # Check if Emerald glyph has content
        has_content = False
        for dy in range(16):
            for dx in range(8):
                if em_pixels[em_x + dx, em_y + dy] != 0:
                    has_content = True
                    break
            if has_content:
                break
        
        if has_content:
            # Copy Emerald glyph with color remapping
            glyphs_copied += 1
            # For 8x8 glyphs, only copy the top half
            copy_height = min(glyph_height, 16)
            for dy in range(copy_height):
                for dx in range(8):
                    em_val = em_pixels[em_x + dx, em_y + dy]
                    # Emerald: 0=bg, 1=dark, 2=gray, 3=white 
                    # -> Ruby: 0=white(transparent), 14=gray(shadow), 15=dark(foreground)
                    if em_val == 0 or em_val == 3:
                        ruby_pixels[rb_x + dx, rb_y + dy] = 0   # Background
                    elif em_val == 1:
                        ruby_pixels[rb_x + dx, rb_y + dy] = 15  # Foreground (dark text)
                    elif em_val == 2:
                        ruby_pixels[rb_x + dx, rb_y + dy] = 14  # Shadow (gray)
        else:
            # Keep Ruby's original
            glyphs_kept += 1
    
    ruby.save(ruby_output_path)
    print(f"Created {ruby_output_path}")
    print(f"  Remapped {glyphs_copied} glyphs from Emerald 2-byte encoding")
    print(f"  Kept {glyphs_kept} glyphs from Ruby original")

if __name__ == '__main__':
    # Font3: 8x16 glyphs (layout mode 1) - use Emerald
    remap_emerald_to_ruby('../pokeemerald/graphics/fonts/latin_normal.png',
                          'graphics/fonts/font3_lat_original.png',
                          'graphics/fonts/font3_lat_new.png',
                          glyph_height=16)
    
    # Font4: 8x8 glyphs - keep Ruby original since Emerald only has 16-tall glyphs
    # Cutting them in half causes rendering issues
    from shutil import copy
    copy('graphics/fonts/font4_lat_original.png', 'graphics/fonts/font4_lat_new.png')
    print("Created graphics/fonts/font4_lat_new.png")
    print("  Kept Ruby original font4 (Emerald fonts are 16px tall, incompatible with 8x8)")
    
    print("\nDone! Fonts remapped from Emerald 2-byte to Ruby 1-byte encoding.")
