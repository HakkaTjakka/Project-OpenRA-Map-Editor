# Project OpenRA Map Editor
 c/c++ code (Linux, CodeBlocks) reading / writing / editing .oramap .bin .yaml of OpenRA Red Alert

Created under Ubuntu Linux.
```
Will continue later.. busy...
```

```
latest: 12 Dec 2022
Fighting with XCC and https://github.com/OpenRA/OpenRA/wiki/Utility
.mix files, shapes, images, etc...
```

***
How i get it:
![clipboard_small](https://github.com/HakkaTjakka/Project-OpenRA-Map-Editor/blob/main/my_production.png)
***
How it should look:
![clipboard_small](https://github.com/HakkaTjakka/Project-OpenRA-Map-Editor/blob/main/how_it_should_be.png)
***
Other initiatives: https://forum.openra.net/viewtopic.php?f=83&t=21083
***

```
latest: 10 Dec 2022
	's': 			smooth on/off
	pageup/pagedown: 	zoom in/out 5%
	ctrl + pageup/pagedown:	rotate counter-/clockwise 10 degrees.
	'v':			verticalsync enabled/disabled
	cursor left/right:	pan left/right
	cursor up/down:		pan up/down
	(need code to make it smooth.... keyboard speed / refresh_rate / timing) 
	SFML graphics / threads: stable!
	tiles still mess... help
	
latest:
	Fooled around with SFML. For now it reads and displays (part of) a .bin (SHOWBIN.sh)
	However something is wrong with the tiles.... Time to figure that out...
	Hia!
	Any questions / idea's? Ring the bell.

latest:
	Added a 251 x 251 (249 x 249) map MAZE
	program now converts output33.txt and adds this maze to the map.bin file.
	Then writes new bin file and outputs some actor data for the map.yaml file.
	
	./red_alert readbin MAZE/map.bin > output.txt 
	
	Creates output.txt with actor data for mines for the map.yaml file,
		put the right lines under actors in the map.yaml file for mines.

	Creates modified map.bin.new file with the changed map.bin data,
		rename the map.bin.new to map.bin

	Put dir (or .zip to .oramap inside the dir...) into right place of OpenRA,
		(something like: ~/snap/openra/1359/.config/openra/maps/ra/release-20210321)
		and test pathfinder with the new version with improved pathfinder code.

```
# New version: playtest-20221203
(https://www.openra.net/download/)





Needed:
CodeBlocks ide
Compilers, some libs (z, X11, sfml, search on how to get / install)

Reads / writes .bin and .yaml files for OpenRA Red Alert.
These are inside the .oramap (.zip) files.

Under construction....

Will contain graphical interface for editing tiles on the map.
Import / export.
Auto terrain functions. 
Map analysis. 
Etc.

Please leave idea's to implement.

Commander.

The XCC Utilities runs under Windows or with wine or mono under Ubuntu.

./red_alert readbin BattleStart3/map.bin

output for now:

Filesize of "BattleStart3/map.bin" is: 115537 bytes. malloc()=0x0x5643eeaf66e0, reading, 115537 bytes read, ok.
02 98 00 98 00 11 00 00 00 00 00 00 00 D1 0E 01 00
size_x= 152 (0098)
size_y= 152 (0098)
bin=( 152 x 152 ) x 5 + 17 = 115537 bytes.

***
![clipboard_small](https://github.com/HakkaTjakka/Project-OpenRA-Map-Editor/blob/main/RAEDITOR/Untitled.png)
***
![clipboard_small](https://github.com/HakkaTjakka/Project-OpenRA-Map-Editor/blob/main/input.png)
***
![clipboard_small](https://github.com/HakkaTjakka/Project-OpenRA-Map-Editor/blob/main/output.png)
***

Some info on the .bin file:
```
// File layout .bin file (inside the .oramap files, .zip file...)
// First 17 bytes: map info
//      Byte 1+2: 16 bit integer x-size map
//      Byte 3+4: 16 bit integer y-size map
//      Byte 0, 5-16: Unknown for now
//
// From byte 17: 3 bytes * size_x * size_y
//      Every 3 bytes: One 16 bit (2 bytes) integer for the TILENUMBER,
//          and one 8 bit (1 byte) integer for the VARIATIONS of this tile.
//
//      List of filenames of tiles and their corresponding (16 bin integer) code used in .bin file comes from:
//      File: OpenRA/mods/ra/tilesets/interior.yaml
//      GitHub source: https://github.com/OpenRA/OpenRA/mods/ra/tilesets/interior.yaml
//      And looks like:
/*
    ...
	Template@350:                   // one tile tile
		Id: 350                     // 16 bit integer from .bin
		Images: wall0022.int
		Size: 1,1                   // dimensions (# of tiles)
		Categories: Wall
		Tiles:                      // tiles / layout
			0: Wall                 // numbering (dimentions) left-right top-down (depending on shape... see the build in editor)
	Template@351:                   // 4 tile tile (2 x 2)
		Id: 351
		Images: wall0023.int
		Size: 2,2
		Categories: Wall
		Tiles:
			0: Wall                 // upper left  (0,0)
			1: Wall                 // upper right (0,1)
			2: Wall                 // lower left  (1,0) (3 would be 1,1)
    ...
*/
//
//      Starting with the interior tiles: (there are also winter desert etc.)
//      Tiles: the .png tiles comes from some .mix file of the OpenRA (interior.mix) found in the gamedir,
//           and where extracted with a tool called XCC to .png
// From there (byte 17 + 3 bytes * size_x * size_y)
//      Every 2 bytes: One 16 bit integer for the tile status, like emerald or shroud / damage stuff or so.
//      Values need to be determined what is what. On map with no spice they are all 0x0000 (hex)
```
