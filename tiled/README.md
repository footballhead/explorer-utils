# RMS Editing with Tiled

This folder consists of two parts:

1. How to make levels with [Tiled](https://www.mapeditor.org/): which values to use, tilesets, etc
2. A tool called `tmx2rms` (made in Rust) for turning those Tiled levels into RMS.

## Making New Rooms

[`sample/sample.tmx`](sample/sample.tmx) is provided for your convenience, copy and rename it. This has all the required parameters set, you just need to do the actual level editing part.

If you're curious about how I made `sample.tmx`, read [`docs/from_scratch.md`](docs/from_scratch.md).

## Running tmx2rms

`tmx2rms` is a tool written in Rust for performing the TMX to RMS conversion.

```
cargo run sample/sample.tmx test.rms
```

`test.rms` can be copied, along with an appropriate `test.ovr`, to the same directory as `EXPLORER.EXE`. Then, when you run `EXPLORER.EXE`, the `TEST` adventure should be available as an option.

## Required specs

In general,

* Levels must be 20x8 tiles, with 15x15px tile dimensions
* Level tile data must use XML format
* There must be only one tile layer, called _Tiles_
* There must not be any holes in the tile layer (places with no tile)
* Tiles must be between 1 and 84 (inclusive)
* Tilesets must match EGAPICS order (1 must be floor, 2 must be sand, etc)

Most of these are taken care of for you if you use `sample.tmx`.

The tilesets are mostly for your convenience; `tmx2rms` doesn't read them. You could use any tileset as long as it meets the requirements.

## Limitations

* You can't change the player start location
* You always start with:
   * no weapons or armor
   * 200 gold
   * 300 hits
   * level 1, no experience
   * no keys, potions, or wands
