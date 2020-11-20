# maskconv

Distributions of Dungeon Explorer are missing `CGAMASK.PIC`, the file that stores transparency data for drawing monsters. As a result, the game crashes in CGA mode when drawing monsters.

This tool converts `PYMASK.PIC` (EGA) into `CGAMASK.PIC` (CGA) to replace the missing file and fix CGA mode.

`CGAMASK.PIC` in this folder is the result of running this tool so that you don't have to.

## Usage

```
maskconv PYMASK.PIC CGAMASK.PIC
```

* Input: `PYMASK.PIC`, a working mask spritesheet in EGA format
* Output: `CGAMASK.PIC`, the converted file
