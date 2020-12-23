mod pascal;
mod rms;

const EXPECTED_TILE_LAYER_NAME: &str = "Tiles";
const MAX_TILE: i32 = 84;
const MIN_TILE: i32 = 1;

struct RmsTmxIntermediate {
    tiles: Vec<u8>,
    name: String,
}

fn validate_tmx(map: &tmx::Map) -> Option<RmsTmxIntermediate> {
    if map.width() != rms::ROOM_WIDTH || map.height() != rms::ROOM_HEIGHT {
        println!("Bad map dimensions");
        return None;
    }

    let mut intermediate = RmsTmxIntermediate {
        tiles: Vec::new(),
        name: String::new(),
    };

    for property in map.properties() {
        if property.name() == "name" {
            intermediate.name = property.value().to_string();
        }
    }

    let mut found_tiles_layer = false;
    for layer in map.layers() {
        if layer.name() == EXPECTED_TILE_LAYER_NAME {
            found_tiles_layer = true;
            if layer.data().is_none() {
                println!("Tiles layer missing data!");
                return None;
            }

            let data = layer.data().unwrap();
            let tiles = data.tiles();

            for tile in tiles {
                let tile = tile.gid();
                if tile < MIN_TILE || tile > MAX_TILE {
                    println!("Tile data outside expected bounds");
                    return None;
                }

                let tile = tile as u8;
                intermediate.tiles.push(tile);
            }

            if intermediate.tiles.len() != rms::ROOM_AREA {
                println!("Mismatch between expected number of tiles and actual!");
                return None;
            }
        }
    }

    if !found_tiles_layer {
        println!("Didn't find Tiles layer!");
        return None;
    }

    Some(intermediate)
}

fn main() {
    let args: Vec<String> = std::env::args().collect();

    if args.len() < 3 {
        println!("Usage: {} in.tmx out.rms", args[0]);
        return;
    }

    let tmx_file = &args[1];
    let out_file = &args[2];

    let tmx = tmx::Map::open(tmx_file).unwrap();
    let tmx = validate_tmx(&tmx);
    if tmx.is_none() {
        return;
    }
    let tmx = tmx.unwrap();

    let room_id = 1;
    let room_name = if tmx.name.is_empty() {
        format!("Room #{}", room_id)
    } else {
        tmx.name
    };
    let mut rms = rms::Room::new(1, &room_name);

    for (i, tile) in tmx.tiles.iter().enumerate() {
        let i = i as u32;
        rms.set_tile(i % rms::ROOM_WIDTH, i / rms::ROOM_WIDTH, *tile);
    }

    rms::save_rooms(&vec![rms], out_file).unwrap();
}
