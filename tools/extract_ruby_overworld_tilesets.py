#!/usr/bin/env python3
"""
Copy overworld tile LZ blobs and palettes from an official Pokemon Ruby ROM (English
build that matches this decomp's `pokeruby.map`) into Ruby-only paths:

  data/tilesets/primary/general/tiles_ruby.4bpp.lz
  data/tilesets/primary/general/palettes_ruby/{00-15}.pal
  data/tilesets/secondary/cave/tiles_ruby.4bpp.lz
  data/tilesets/secondary/cave/palettes_ruby/{00-15}.pal

Usage:
  make ruby   # produces pokeruby.map
  make extract_ruby_tilesets RUBY_ROM=/path/to/pokeruby.gba

Requires a ROM that matches your pokeruby.map (typically US rev0 for compare builds).
"""
from __future__ import annotations

import argparse
import os
import re
import struct
import sys


def parse_map_symbols(map_path: str) -> dict[str, int]:
    out: dict[str, int] = {}
    with open(map_path, encoding="utf-8", errors="replace") as f:
        for line in f:
            m = re.search(r"\b0x([0-9a-fA-F]+)\s+(gTileset(?:Tiles|Palettes)_(?:General|Cave))\b", line)
            if m:
                out[m.group(2)] = int(m.group(1), 16)
    return out


def rom_off(cpu_addr: int) -> int:
    return cpu_addr - 0x08000000


def rgb16_to_jasc_triplet(w: int) -> tuple[int, int, int]:
    r = (w >> 0) & 0x1F
    g = (w >> 5) & 0x1F
    b = (w >> 10) & 0x1F
    # Match gbagfx-style 5 -> 8-bit expansion
    return (
        min(255, (r * 255) // 31),
        min(255, (g * 255) // 31),
        min(255, (b * 255) // 31),
    )


def write_jasc_pal(path: str, colors: list[tuple[int, int, int]]) -> None:
    os.makedirs(os.path.dirname(path) or ".", exist_ok=True)
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write("JASC-PAL\n0100\n16\n")
        for r, g, b in colors:
            f.write(f"{r} {g} {b}\n")


def lz77_decompress(data: memoryview) -> tuple[bytearray, int]:
    """GBA LZ77 (type 0x10). Returns (uncompressed, compressed_byte_len)."""
    if len(data) < 4 or data[0] != 0x10:
        raise ValueError("not a GBA LZ77 stream")
    dest_size = data[1] | (data[2] << 8) | (data[3] << 16)
    dest = bytearray(dest_size)
    src_pos = 4
    dest_pos = 0
    while dest_pos < dest_size:
        if src_pos >= len(data):
            raise ValueError("LZ: truncated input")
        flags = data[src_pos]
        src_pos += 1
        for _ in range(8):
            if dest_pos >= dest_size:
                break
            if flags & 0x80:
                if src_pos + 2 > len(data):
                    raise ValueError("LZ: bad block")
                b0 = data[src_pos]
                b1 = data[src_pos + 1]
                src_pos += 2
                block_size = (b0 >> 4) + 3
                block_dist = (((b0 & 0xF) << 8) | b1) + 1
                block_pos = dest_pos - block_dist
                if block_pos < 0:
                    raise ValueError("LZ: bad distance")
                if dest_pos + block_size > dest_size:
                    block_size = dest_size - dest_pos
                for j in range(block_size):
                    dest[dest_pos] = dest[block_pos + j]
                    dest_pos += 1
            else:
                if src_pos >= len(data):
                    raise ValueError("LZ: missing literal")
                dest[dest_pos] = data[src_pos]
                dest_pos += 1
                src_pos += 1
            flags <<= 1
    return dest, src_pos


def find_lz_blob(rom: bytes, cpu_addr: int) -> bytes:
    off = rom_off(cpu_addr)
    mv = memoryview(rom)[off : off + 0x100000]
    _, comp_len = lz77_decompress(mv)
    return rom[off : off + comp_len]


def extract_palette_block(rom: bytes, cpu_addr: int) -> list[list[tuple[int, int, int]]]:
    off = rom_off(cpu_addr)
    # 16 palettes x 16 colors x 2 bytes
    raw = rom[off : off + 16 * 16 * 2]
    if len(raw) < 16 * 16 * 2:
        raise ValueError("palette read past ROM end")
    pals: list[list[tuple[int, int, int]]] = []
    for p in range(16):
        colors = []
        for c in range(16):
            w = struct.unpack_from("<H", raw, (p * 16 + c) * 2)[0]
            colors.append(rgb16_to_jasc_triplet(w))
        pals.append(colors)
    return pals


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--rom", required=True, help="Official Pokemon Ruby .gba (matching pokeruby.map)")
    ap.add_argument("--map", default="pokeruby.map", help="Map from a matching `make ruby`")
    ap.add_argument(
        "--root",
        default=os.path.join(os.path.dirname(__file__), ".."),
        help="Repo root",
    )
    args = ap.parse_args()
    root = os.path.abspath(args.root)
    map_path = os.path.join(root, args.map)
    if not os.path.isfile(args.rom):
        print(
            f"error: RUBY_ROM not found: {args.rom}\n"
            "Provide an absolute path to an actual `pokeruby.gba` file (not a placeholder).\n",
            file=sys.stderr,
        )
        return 1
    if not os.path.isfile(map_path):
        print(f"error: map not found: {map_path}\nRun `make ruby` first.", file=sys.stderr)
        return 1
    sym = parse_map_symbols(map_path)
    need = (
        "gTilesetTiles_General",
        "gTilesetPalettes_General",
        "gTilesetTiles_Cave",
        "gTilesetPalettes_Cave",
    )
    for n in need:
        if n not in sym:
            print(f"error: symbol {n} missing from {map_path}", file=sys.stderr)
            return 1

    with open(args.rom, "rb") as f:
        rom = f.read()

    gen_lz = find_lz_blob(rom, sym["gTilesetTiles_General"])
    out_gen_lz = os.path.join(root, "data/tilesets/primary/general/tiles_ruby.4bpp.lz")
    os.makedirs(os.path.dirname(out_gen_lz), exist_ok=True)
    with open(out_gen_lz, "wb") as f:
        f.write(gen_lz)
    print(f"wrote {out_gen_lz} ({len(gen_lz)} bytes compressed)")

    gen_pals = extract_palette_block(rom, sym["gTilesetPalettes_General"])
    for i, pal in enumerate(gen_pals):
        name = os.path.join(root, f"data/tilesets/primary/general/palettes_ruby/{i:02d}.pal")
        write_jasc_pal(name, pal)
    print(f"wrote data/tilesets/primary/general/palettes_ruby/*.pal")

    cave_lz = find_lz_blob(rom, sym["gTilesetTiles_Cave"])
    out_cave_lz = os.path.join(root, "data/tilesets/secondary/cave/tiles_ruby.4bpp.lz")
    os.makedirs(os.path.dirname(out_cave_lz), exist_ok=True)
    with open(out_cave_lz, "wb") as f:
        f.write(cave_lz)
    print(f"wrote {out_cave_lz} ({len(cave_lz)} bytes compressed)")

    cave_pals = extract_palette_block(rom, sym["gTilesetPalettes_Cave"])
    for i, pal in enumerate(cave_pals):
        name = os.path.join(root, f"data/tilesets/secondary/cave/palettes_ruby/{i:02d}.pal")
        write_jasc_pal(name, pal)
    print(f"wrote data/tilesets/secondary/cave/palettes_ruby/*.pal")
    print("Next: make ruby")
    return 0


if __name__ == "__main__":
    sys.exit(main())
