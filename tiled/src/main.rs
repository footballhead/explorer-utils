use std::collections::HashMap;
use std::path::PathBuf;

mod pascal;
mod rms;

const MAX_TILE: i32 = 84;
const MIN_TILE: i32 = 1;
const MAX_ROOMS: u8 = 128;

struct RmsTmxIntermediate {
    id: u8,
    tiles: Vec<u8>,
    name: Option<String>,
    // File name of room to north (sans extension)
    north: Option<String>,
    east: Option<String>,
    south: Option<String>,
    west: Option<String>,
    up: Option<String>,
    down: Option<String>,
}

fn validate_tmx(map: &tmx::Map) -> Result<RmsTmxIntermediate, &'static str> {
    if map.width() != rms::ROOM_WIDTH || map.height() != rms::ROOM_HEIGHT {
        return Err("Bad map dimensions");
    }

    let mut intermediate = RmsTmxIntermediate {
        id: 0,
        tiles: Vec::new(),
        name: None,
        north: None,
        east: None,
        south: None,
        west: None,
        up: None,
        down: None,
    };

    for property in map.properties() {
        match property.name() {
            "name" => intermediate.name = Some(property.value().to_string()),
            "north" => intermediate.north = Some(property.value().to_string()),
            "east" => intermediate.east = Some(property.value().to_string()),
            "south" => intermediate.south = Some(property.value().to_string()),
            "west" => intermediate.west = Some(property.value().to_string()),
            "up" => intermediate.up = Some(property.value().to_string()),
            "down" => intermediate.down = Some(property.value().to_string()),
            _ => {}
        }
    }

    let mut first_gid_tiles: Option<u32> = None;
    let mut first_gid_objects: Option<u32> = None;
    let mut first_gid_monsters: Option<u32> = None;
    for tileset in map.tilesets() {
        match tileset.name() {
            "Tiles" => first_gid_tiles = Some(tileset.first_gid()),
            "Objects" => first_gid_objects = Some(tileset.first_gid()),
            "Monsters" => first_gid_monsters = Some(tileset.first_gid()),
            _ => println!("Unrecognized tileset: {}", tileset.name())
        }
    }

    if first_gid_tiles.is_none() {
        return Err("Didn't find Tiles tileset");
    }
    if first_gid_objects.is_none() {
        return Err("Didn't find Objects tileset");
    }
    if first_gid_monsters.is_none() {
        return Err("Didn't find Monsters tileset");
    }

    let first_gid_tiles = first_gid_tiles.unwrap();
    let first_gid_objects = first_gid_objects.unwrap();
    let first_gid_monsters = first_gid_monsters.unwrap();

    let mut found_tiles_layer = false;
    for layer in map.layers() {
        match layer.name() {
            "Tiles" => {
                found_tiles_layer = true;
                if layer.data().is_none() {
                    return Err("Tiles layer missing data!");
                }
    
                let data = layer.data().unwrap();
                let tiles = data.tiles();
    
                for tile in tiles {
                    let tile = (tile.gid() - first_gid_tiles as i32) + 1;
                    if tile < MIN_TILE || tile > MAX_TILE {
                        return Err("Tile data outside expected bounds");
                    }
    
                    let tile = tile as u8;
                    intermediate.tiles.push(tile);
                }
    
                if intermediate.tiles.len() != rms::ROOM_AREA {
                    return Err("Mismatch between expected number of tiles and actual!");
                }
            },
            _ => {}
        }
    }

    if !found_tiles_layer {
        return Err("Didn't find Tiles layer!");
    }

    Ok(intermediate)
}

fn main() {
    let args: Vec<String> = std::env::args().collect();

    if args.len() < 3 {
        println!("Usage: {} in.tmx out.rms", args[0]);
        return;
    }

    let tmx_file = &args[1];
    let out_file = &args[2];

    let mut next_room_id = 1;

    // All joined rooms must be in the same folder as the input
    let tmx_file = std::fs::canonicalize(tmx_file).unwrap();
    let load_path_parent = tmx_file.parent().unwrap().to_path_buf();

    let mut tmx_to_load: Vec<PathBuf> = vec![tmx_file];
    // Key is a file_stem
    let mut intermediates: HashMap<String, RmsTmxIntermediate> = HashMap::new();
    'process_to_load: loop {
        let to_load = tmx_to_load.pop();
        if to_load.is_none() {
            break 'process_to_load;
        }
        let to_load = to_load.unwrap();

        println!("Loading {:#?}", to_load);

        let tmx = tmx::Map::open(&to_load).unwrap();
        let mut tmx = validate_tmx(&tmx).unwrap();

        tmx.id = next_room_id;
        next_room_id += 1;
        if next_room_id > MAX_ROOMS {
            panic!("Too many rooms!");
        }

        if tmx.north.is_some() {
            let north = tmx.north.as_ref().unwrap();
            if !intermediates.contains_key(north) {
                let p = load_path_parent.join(PathBuf::from(north).with_extension("tmx"));
                tmx_to_load.push(p);
            }
        }
        if tmx.east.is_some() {
            let east = tmx.east.as_ref().unwrap();
            if !intermediates.contains_key(east) {
                let p = load_path_parent.join(PathBuf::from(east).with_extension("tmx"));
                tmx_to_load.push(p);
            }
        }
        if tmx.south.is_some() {
            let south = tmx.south.as_ref().unwrap();
            if !intermediates.contains_key(south) {
                let p = load_path_parent.join(PathBuf::from(south).with_extension("tmx"));
                tmx_to_load.push(p);
            }
        }
        if tmx.west.is_some() {
            let west = tmx.west.as_ref().unwrap();
            if !intermediates.contains_key(west) {
                let p = load_path_parent.join(PathBuf::from(west).with_extension("tmx"));
                tmx_to_load.push(p);
            }
        }
        if tmx.up.is_some() {
            let up = tmx.up.as_ref().unwrap();
            if !intermediates.contains_key(up) {
                let p = load_path_parent.join(PathBuf::from(up).with_extension("tmx"));
                tmx_to_load.push(p);
            }
        }
        if tmx.down.is_some() {
            let down = tmx.down.as_ref().unwrap();
            if !intermediates.contains_key(down) {
                let p = load_path_parent.join(PathBuf::from(down).with_extension("tmx"));
                tmx_to_load.push(p);
            }
        }

        intermediates.insert(to_load.file_stem().unwrap().to_str().unwrap().to_string(), tmx);
    }

    println!("Loaded {} rooms", intermediates.len());
    let mut loaded_rooms: Vec<rms::Room> = Vec::new();
    for (_, intermediate) in &intermediates {
        let room_name = if intermediate.name.is_none() {
            format!("Room #{}", intermediate.id)
        } else {
            intermediate.name.as_ref().unwrap().to_string()
        };

        let mut rms = rms::Room::new(intermediate.id, &room_name);
        for (i, tile) in intermediate.tiles.iter().enumerate() {
            let i = i as u32;
            rms.set_tile(i % rms::ROOM_WIDTH, i / rms::ROOM_WIDTH, *tile);
        }

        if intermediate.north.is_some() {
            let north = intermediates
                .get(intermediate.north.as_ref().unwrap())
                .unwrap();
            rms.nav_north = north.id;
        }
        if intermediate.east.is_some() {
            let east = intermediates
                .get(intermediate.east.as_ref().unwrap())
                .unwrap();
            rms.nav_east = east.id;
        }
        if intermediate.south.is_some() {
            let south = intermediates
                .get(intermediate.south.as_ref().unwrap())
                .unwrap();
            rms.nav_south = south.id;
        }
        if intermediate.west.is_some() {
            let west = intermediates
                .get(intermediate.west.as_ref().unwrap())
                .unwrap();
            rms.nav_west = west.id;
        }
        if intermediate.up.is_some() {
            let up = intermediates
                .get(intermediate.up.as_ref().unwrap())
                .unwrap();
            rms.nav_up = up.id;
        }
        if intermediate.down.is_some() {
            let down = intermediates
                .get(intermediate.down.as_ref().unwrap())
                .unwrap();
            rms.nav_down = down.id;
        }

        loaded_rooms.push(rms);
    }

    // EXPLORER.EXE expects the rooms on disk to be sorted.
    // Weird things happen if it's not (starting in wrong room, entrances not working, ...)
    loaded_rooms.sort_by_key(|k| k.id());

    rms::save_rooms(&loaded_rooms, out_file).unwrap();
}
