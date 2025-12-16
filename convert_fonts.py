import os
import struct

def convert_png_to_8bit(input_path, output_path):
    """Convert a PNG to 8-bit grayscale depth"""
    with open(input_path, 'rb') as f:
        data = f.read()
    
    # PNG signature
    if data[:8] != b'\x89PNG\r\n\x1a\n':
        raise ValueError("Not a valid PNG file")
    
    output = bytearray()
    output.extend(data[:8])  # PNG signature
    
    pos = 8
    while pos < len(data):
        # Read chunk
        length = struct.unpack('>I', data[pos:pos+4])[0]
        chunk_type = data[pos+4:pos+8]
        chunk_data = data[pos+8:pos+8+length]
        crc = data[pos+8+length:pos+12+length]
        
        if chunk_type == b'IHDR':
            # Modify IHDR to set bit depth to 8 and color type to grayscale (0)
            width, height, bit_depth, color_type = struct.unpack('>IIBB', chunk_data[:10])
            new_chunk_data = struct.pack('>IIBB', width, height, 8, 0) + chunk_data[10:]
            
            # Recalculate CRC
            import zlib
            crc_data = b'IHDR' + new_chunk_data
            new_crc = struct.pack('>I', zlib.crc32(crc_data) & 0xffffffff)
            
            output.extend(struct.pack('>I', len(new_chunk_data)))
            output.extend(b'IHDR')
            output.extend(new_chunk_data)
            output.extend(new_crc)
        elif chunk_type == b'PLTE':
            # Skip palette chunk for grayscale
            pass
        elif chunk_type == b'tRNS':
            # Skip transparency chunk
            pass
        else:
            # Copy other chunks as-is
            output.extend(data[pos:pos+12+length])
        
        pos += 12 + length
    
    with open(output_path, 'wb') as f:
        f.write(output)

# Convert all fonts
fonts = [
    ('emerald_latin_small.png', 'font0_lat.png'),
    ('emerald_latin_normal.png', 'font1_lat.png'),
    ('emerald_latin_short.png', 'font3_lat.png'),
    ('emerald_latin_narrow.png', 'font4_lat.png'),
]

temp_dir = os.environ['TEMP']
base_dir = r'c:\Users\Jimmy\Documents\decomps\pokeruby\graphics\fonts'

for src, dst in fonts:
    src_path = os.path.join(temp_dir, src)
    dst_path = os.path.join(base_dir, dst)
    if os.path.exists(src_path):
        try:
            convert_png_to_8bit(src_path, dst_path)
            print(f"Converted {src} -> {dst}")
        except Exception as e:
            print(f"Error converting {src}: {e}")
