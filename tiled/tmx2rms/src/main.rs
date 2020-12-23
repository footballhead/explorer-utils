mod pascal;
mod rms;

const EXPECTED_WIDTH: i32 = 20;
const EXPECTED_HEIGHT: i32 = 8;
const EXPECTED_TILE_LAYER_NAME: &str = "Tiles";
const MAX_TILE: u32 = 84;
const MIN_TILE: u32 = 1;

fn main() {
    let args: Vec<String> = std::env::args().collect();

    if args.len() < 3 {
        println!("Usage: {} in.tmx out.rms", args[0]);
        return;
    }

    let tmx_file = &args[1];
    let out_file = &args[2];

    let tmx = std::fs::read(tmx_file).unwrap();
    let tmx = tmx::Map::from_xml_data(&tmx).unwrap();

    if tmx.width != EXPECTED_WIDTH || tmx.height != EXPECTED_HEIGHT {
        println!("Bad map dimensions");
        return;
    }

    let mut tile_layer: Option<tmx::layer::Layer> = None;
    for layer in tmx.layers {
        if layer.name == EXPECTED_TILE_LAYER_NAME {
            tile_layer = Some(layer);
        }
    }

    if tile_layer.is_none() {
        println!("Couldn't find tile layer");
        return;
    }

    let mut rms = rms::Room::new(1, tmx_file);
    let tile_layer = tile_layer.unwrap();
    match tile_layer.data {
        tmx::layer::LayerData::Tiles(tiles) => {
            for (i, tile) in tiles.iter().enumerate() {
                let gid = tile.gid();
                if gid < MIN_TILE || gid > MAX_TILE {
                    println!("Tile data outside expected bounds");
                    return;
                }
                let gid = gid as u8;
                let i = i as u32;
                rms.set_tile(i % EXPECTED_WIDTH as u32, i / EXPECTED_WIDTH as u32, gid);
            }

        }
        _ => {
            println!("Unexpected tile data type");
            return;
        }
    }

    rms::save_rooms(&vec![rms], out_file).unwrap();
}
