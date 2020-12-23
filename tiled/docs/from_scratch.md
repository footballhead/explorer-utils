# Creating maps from scratch

If you're interested in how I made `sample.tmx` and associated files.

## Create New Map

*File > New > New Map...*. Use these parameters:

![New map dialog](new_map.png)

## Change Tile Layer Format to XML

*Map > Map Properties...*

In the *Properties* panel, find *Tile Layer Format* and change it to *XML (Deprecated)*.

![Map properties panel](xml_format.png)

## Create Tileset

`egapics.png` is a conversion of `EGAPICS.PIC` using the `mksheet` tool, provided for the convenience of making tilesets.

![EGA tileset graphics](../egapics.png)

In the *Tilesets* panel (defaults to bottom-right of Tiled), click the *New Tileset...* button:

![New tileset dialog](new_tileset.png)

## Rename tile layer

The tile layer must be called *Tiles*. There must only be one layer called *Tiles*.
