#!/usr/bin/env python3
"""Generate LVGL chess tile sprites from individual PNG files."""

from __future__ import annotations

import argparse
import re
from pathlib import Path
from typing import Dict, List

from PIL import Image

# Explicit ordering map (26 total) to avoid accidental reorder.
SPRITE_NAMES = [
    "wk_light", "wk_dark",
    "bk_light", "bk_dark",
    "wq_light", "wq_dark",
    "bq_light", "bq_dark",
    "wb_light", "wb_dark",
    "bb_light", "bb_dark",
    "wn_light", "wn_dark",
    "bn_light", "bn_dark",
    "wr_light", "wr_dark",
    "br_light", "br_dark",
    "wp_light", "wp_dark",
    "bp_light", "bp_dark",
    "empty_light", "empty_dark",
]


FILENAME_RE = re.compile(
    r"^Piece=(?P<piece>[^,]+),\s*Piece Color=(?P<color>[^,]+),\s*Square=(?P<square>[^.]+)\.png$",
    re.IGNORECASE,
)


def canonical_name_from_figma_filename(filename: str) -> str:
    match = FILENAME_RE.match(filename)
    if not match:
        raise ValueError(f"Unexpected sprite filename format: {filename}")

    piece = match.group("piece").strip().lower()
    color = match.group("color").strip().lower()
    square = match.group("square").strip().lower()
    if square not in ("light", "dark"):
        raise ValueError(f"Invalid square value in filename: {filename}")

    if piece == "none":
        return f"empty_{square}"

    piece_map = {
        "king": "k",
        "queen": "q",
        "bishop": "b",
        "knight": "n",
        "rook": "r",
        "pawn": "p",
    }
    color_map = {"white": "w", "black": "b"}
    if piece not in piece_map or color not in color_map:
        raise ValueError(f"Invalid piece/color in filename: {filename}")

    return f"{color_map[color]}{piece_map[piece]}_{square}"


def load_sprites_from_directory(source_dir: Path, tile_size: int | None) -> Dict[str, Image.Image]:
    sprites: Dict[str, Image.Image] = {}
    detected_size: tuple[int, int] | None = None
    for path in sorted(source_dir.glob("*.png")):
        canonical = canonical_name_from_figma_filename(path.name)
        tile = Image.open(path).convert("L")
        if detected_size is None:
            detected_size = tile.size
        elif tile.size != detected_size:
            raise RuntimeError(f"Mismatched source dimensions. {path.name} has {tile.size}, expected {detected_size}")

        if tile_size is not None and (tile.width != tile_size or tile.height != tile_size):
            tile = tile.resize((tile_size, tile_size), Image.Resampling.LANCZOS)
        sprites[canonical] = tile

    missing = [name for name in SPRITE_NAMES if name not in sprites]
    extras = [name for name in sprites.keys() if name not in SPRITE_NAMES]
    if missing or extras:
        raise RuntimeError(
            "Sprite set mismatch.\n"
            f"Missing: {missing}\n"
            f"Unexpected: {extras}"
        )
    return sprites


def write_renamed_pngs(sprites: Dict[str, Image.Image], output_dir: Path) -> None:
    output_dir.mkdir(parents=True, exist_ok=True)
    for name in SPRITE_NAMES:
        sprites[name].save(output_dir / f"{name}.png")


def to_bw_rgb565(tile: Image.Image, threshold: int) -> bytes:
    out = bytearray()
    for y in range(tile.height):
        for x in range(tile.width):
            luminance = tile.getpixel((x, y))
            is_black = luminance < threshold
            color = 0x0000 if is_black else 0xFFFF
            out.append(color & 0xFF)
            out.append((color >> 8) & 0xFF)
    return bytes(out)


def write_sprite_sources(
    sprites: Dict[str, Image.Image], out_dir: Path, threshold: int, tile_size: int
) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)
    data_map: Dict[str, bytes] = {}
    for name, tile in sprites.items():
        data_map[name] = to_bw_rgb565(tile, threshold)

    header = out_dir / "chess_sprites.h"
    source = out_dir / "chess_sprites.c"

    header.write_text(
        "\n".join(
            [
                "#pragma once",
                "",
                "#include <stdint.h>",
                "#include \"lvgl.h\"",
                "",
                "#ifdef __cplusplus",
                "extern \"C\" {",
                "#endif",
                "",
                f"#define CHESS_SPRITE_TILE_SIZE {tile_size}",
                f"#define CHESS_SPRITE_TILE_BYTES ({tile_size} * {tile_size} * 2)",
                "",
                "typedef enum {",
                *[f"    CHESS_SPRITE_{name.upper()}," for name in SPRITE_NAMES],
                "    CHESS_SPRITE_COUNT",
                "} chess_sprite_id_t;",
                "",
                "const lv_image_dsc_t *chess_sprite_get(chess_sprite_id_t id);",
                "",
                "#ifdef __cplusplus",
                "}",
                "#endif",
                "",
            ]
        ),
        encoding="utf-8",
    )

    lines: List[str] = [
        "#include \"chess_sprites.h\"",
        "",
        "#include <stddef.h>",
        "",
    ]
    for name in SPRITE_NAMES:
        blob = data_map[name]
        lines.append(f"static const uint8_t g_{name}_map[{len(blob)}] = {{")
        row = []
        for i, b in enumerate(blob):
            row.append(f"0x{b:02X}")
            if len(row) == 16 or i == len(blob) - 1:
                lines.append("    " + ", ".join(row) + ",")
                row = []
        lines.append("};")
        lines.append("")
        lines.extend(
            [
                f"static const lv_image_dsc_t g_{name}_img = {{",
                "    .header = {",
                "        .magic = LV_IMAGE_HEADER_MAGIC,",
                "        .cf = LV_COLOR_FORMAT_RGB565,",
                "        .flags = 0,",
                f"        .w = {tile_size},",
                f"        .h = {tile_size},",
                f"        .stride = {tile_size * 2},",
                "    },",
                f"    .data_size = {len(blob)},",
                f"    .data = g_{name}_map,",
                "};",
                "",
            ]
        )

    lines.extend(
        [
            "static const lv_image_dsc_t *const g_sprites[CHESS_SPRITE_COUNT] = {",
            *[f"    &g_{name}_img," for name in SPRITE_NAMES],
            "};",
            "",
            "const lv_image_dsc_t *chess_sprite_get(chess_sprite_id_t id)",
            "{",
            "    if ((int)id < 0 || id >= CHESS_SPRITE_COUNT) {",
            "        return NULL;",
            "    }",
            "    return g_sprites[id];",
            "}",
            "",
        ]
    )

    source.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-dir", required=True, help="Directory containing 26 exported PNGs")
    parser.add_argument("--out-dir", required=True, help="Output directory for generated C files")
    parser.add_argument("--tile-size", type=int, help="Output tile size in pixels (omit for native 1:1)")
    parser.add_argument("--threshold", type=int, default=128, help="0-255 luminance threshold")
    parser.add_argument("--renamed-png-dir", help="Optional output dir for normalized PNG names")
    args = parser.parse_args()

    src_dir = Path(args.source_dir)
    out_dir = Path(args.out_dir)
    sprites = load_sprites_from_directory(src_dir, args.tile_size)
    first = sprites[SPRITE_NAMES[0]]
    tile_size = first.width
    if args.renamed_png_dir:
        write_renamed_pngs(sprites, Path(args.renamed_png_dir))
    write_sprite_sources(sprites, out_dir, args.threshold, tile_size)
    print(f"Generated {len(sprites)} sprites in {out_dir} ({tile_size}x{tile_size})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
