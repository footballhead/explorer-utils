mod pascal;
mod rms;

const EXPECTED_TILE_LAYER_NAME: &str = "Tiles";
const MAX_TILE: u32 = 84;
const MIN_TILE: u32 = 1;

struct RmsTmxIntermediate {
    tiles: Vec<u8>,
}

fn validate_tmx(map: &tmx::Map) -> Option<RmsTmxIntermediate> {
    if map.width != rms::ROOM_WIDTH as i32 || map.height != rms::ROOM_HEIGHT as i32 {
        println!("Bad map dimensions");
        return None;
    }

    let mut intermediate = RmsTmxIntermediate {
        tiles: Vec::new(),
    };

    let mut found_tiles_layer = false;
    for layer in &map.layers {
        if layer.name == EXPECTED_TILE_LAYER_NAME {
            found_tiles_layer = true;
            match &layer.data {
                tmx::layer::LayerData::Tiles(tiles) => {
                    if tiles.len() != rms::ROOM_AREA {
                        println!("Mismatch between expected number of tiles and actual!");
                        return None;
                    }

                    for tile in tiles {
                        let tile = tile.gid();
                        if tile < MIN_TILE || tile > MAX_TILE {
                            println!("Tile data outside expected bounds");
                            return None;
                        }

                        let tile = tile as u8;
                        intermediate.tiles.push(tile);
                    }
                }
                _ => {
                    println!("Unexpected tile data type");
                    return None;
                }
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

    let mut rms = rms::Room::new(1, tmx_file);

    let tmx = std::fs::read(tmx_file).unwrap();
    let tmx = tmx::Map::from_xml_data(&tmx).unwrap();
    let tmx = validate_tmx(&tmx);
    if tmx.is_none() {
        return;
    }
    let tmx = tmx.unwrap();

    for (i, tile) in tmx.tiles.iter().enumerate() {
        let i = i as u32;
        rms.set_tile(i % rms::ROOM_WIDTH, i / rms::ROOM_WIDTH, *tile);
    }

    rms::save_rooms(&vec![rms], out_file).unwrap();
}
