use std::io::Write;

use super::pascal;

pub const ROOM_WIDTH: u32 = 20;
pub const ROOM_HEIGHT: u32 = 8;
pub const ROOM_AREA: usize = (ROOM_WIDTH * ROOM_HEIGHT) as usize;

const ROOM_RECORD_SIZE: usize = 0x168;
const ROOM_RECORD_UNKNOWN_A_OFFSET: usize = 0x0;
const ROOM_RECORD_TILE_OFFSET: usize = 0x1;
const ROOM_RECORD_OBJECT_OFFSET: usize = 0xA1;
const ROOM_RECORD_MONSTER_ID_OFFSET: usize = 0x141;
const ROOM_RECORD_MONSTER_COUNT_OFFSET: usize = 0x142;
const ROOM_RECORD_NORTH_OFFSET: usize = 0x143;
const ROOM_RECORD_ID_OFFSET: usize = 0x149;
const ROOM_RECORD_UNKNOWN_B_OFFSET: usize = 0x14A;
const ROOM_RECORD_UNKNOWN_C_OFFSET: usize = 0x14B;
const ROOM_RECORD_UNKNOWN_D_OFFSET: usize = 0x14C;
const ROOM_RECORD_NAME_OFFSET: usize = 0x14D;

const ROOM_RECORD_NAME_MAX_LENGTH: u8 = (ROOM_RECORD_SIZE - ROOM_RECORD_NAME_OFFSET) as u8;

pub enum ObjectType {
    None,
    Monster,
    Object,
}

/// TODO: Figure out and document unknowns
pub struct Room {
    unknown_a: u8,
    tiles: [u8; ROOM_AREA],
    objects: [u8; ROOM_AREA],
    pub monster_id: u8,
    monster_count: u8,
    // TODO: Make nav an array with enums?
    pub nav_north: u8,
    pub nav_east: u8,
    pub nav_south: u8,
    pub nav_west: u8,
    pub nav_up: u8,
    pub nav_down: u8,
    id: u8,
    unknown_b: u8,
    unknown_c: u8,
    unknown_d: u8,
    pub name: String,
}

impl Room {
    pub fn new(id: u8, name: &str) -> Room {
        Room {
            unknown_a: 0,
            tiles: [1; ROOM_AREA],
            objects: [0; ROOM_AREA],
            monster_id: 0,
            monster_count: 0,
            nav_north: 0,
            nav_east: 0,
            nav_south: 0,
            nav_west: 0,
            nav_up: 0,
            nav_down: 0,
            id: id,
            unknown_b: 0,
            unknown_c: 0,
            unknown_d: 0,
            name: name.to_string(),
        }
    }

    /// The null tile is 0 and should not be drawn, so don't forget to -1 the return value.
    pub fn get_tile(&self, x: u32, y: u32) -> u8 {
        // TODO: Panic if x or y out of bounds
        let tile = self.tiles[(y * ROOM_WIDTH + x) as usize];
        // Different traps are different ASCII characters, which is what > 84 catches
        return if tile > 84 { 21 } else { tile };
    }

    pub fn set_tile(&mut self, x: u32, y: u32, tile: u8) {
        // TODO: Panic if x or y out of bounds
        self.tiles[(y * ROOM_WIDTH + x) as usize] = tile;
    }

    pub fn get_object_type(&self, x: u32, y: u32) -> ObjectType {
        // TODO: Panic if x or y out of bounds
        let tile = self.objects[(y * ROOM_WIDTH + x) as usize];
        if tile == 0 {
            return ObjectType::None;
        }
        if tile <= 'c' as u8 {
            return ObjectType::Monster;
        }
        return ObjectType::Object;
    }

    pub fn get_object(&self, x: u32, y: u32) -> u8 {
        // TODO: Panic if x or y out of bounds
        let tile = self.objects[(y * ROOM_WIDTH + x) as usize];
        return get_object_tile(tile as char);
    }

    pub fn id(&self) -> u8 {
        self.id
    }
}

fn get_object_tile(object: char) -> u8 {
    match object {
        'd' => return 47, // Magical darkness
        'e' => return 21, // Treasure chest
        'f' => return 46, // Smoke
        'g' => return 29, // Movable block
        'h' => return 37, // Door (vertical)
        'i' => return 36, // Door (horizontal)
        'j' => return 0,  // TODO: Funny looking chest
        'k' => return 0,  // TODO: Soft section of wall
        'l' => return 42, // Soft piece of wall
        'm' => return 18, // Soft pile of rubble
        'n' => return 22, // Old body
        'o' => return 17, // Old bones
        'p' => return 49, // Old stone coffin
        'q' => return 54, // Old grave
        'r' => return 0,  // TODO: Movable glass block
        's' => return 0,  // TODO: Old skeleton
        't' => return 0,  // TODO: Old skeleton
        'u' => return 0,  // TODO: Hollow obilisk
        'v' => return 82, // "Just some blood"
        'w' => return 0,  // TODO: Stone marker
        _ => return 0,
    }
}

pub fn load_rooms(filename: &str) -> Vec<Room> {
    let rms_data = std::fs::read(filename).unwrap();
    return rms_data
        .chunks(ROOM_RECORD_SIZE)
        .map(|x| {
            let mut room = Room {
                unknown_a: x[ROOM_RECORD_UNKNOWN_A_OFFSET],
                tiles: [0; ROOM_AREA],
                objects: [0; ROOM_AREA],
                monster_id: x[ROOM_RECORD_MONSTER_ID_OFFSET],
                monster_count: x[ROOM_RECORD_MONSTER_COUNT_OFFSET],
                nav_north: x[ROOM_RECORD_NORTH_OFFSET],
                nav_east: x[ROOM_RECORD_NORTH_OFFSET + 1],
                nav_south: x[ROOM_RECORD_NORTH_OFFSET + 2],
                nav_west: x[ROOM_RECORD_NORTH_OFFSET + 3],
                nav_up: x[ROOM_RECORD_NORTH_OFFSET + 4],
                nav_down: x[ROOM_RECORD_NORTH_OFFSET + 5],
                id: x[ROOM_RECORD_ID_OFFSET],
                unknown_b: x[ROOM_RECORD_UNKNOWN_B_OFFSET],
                unknown_c: x[ROOM_RECORD_UNKNOWN_C_OFFSET],
                unknown_d: x[ROOM_RECORD_UNKNOWN_D_OFFSET],
                name: pascal::from_pascal_string(&x[ROOM_RECORD_NAME_OFFSET..ROOM_RECORD_SIZE]),
            };
            room.tiles
                .copy_from_slice(&x[ROOM_RECORD_TILE_OFFSET..ROOM_RECORD_TILE_OFFSET + ROOM_AREA]);
            room.objects.copy_from_slice(
                &x[ROOM_RECORD_OBJECT_OFFSET..ROOM_RECORD_OBJECT_OFFSET + ROOM_AREA],
            );
            room
        })
        .collect();
}

pub fn save_rooms(rooms: &Vec<Room>, filename: &str) -> std::io::Result<()> {
    // TODO: Is there a more Rust-idiomatic way of data serialization?
    let mut file = std::fs::File::create(filename)?;
    for room in rooms {
        file.write(&vec![room.unknown_a])?;
        file.write(&room.tiles)?;
        file.write(&room.objects)?;
        file.write(&vec![room.monster_id])?;
        file.write(&vec![room.monster_count])?;
        file.write(&vec![room.nav_north])?;
        file.write(&vec![room.nav_east])?;
        file.write(&vec![room.nav_south])?;
        file.write(&vec![room.nav_west])?;
        file.write(&vec![room.nav_up])?;
        file.write(&vec![room.nav_down])?;
        file.write(&vec![room.id])?;
        file.write(&vec![room.unknown_b])?;
        file.write(&vec![room.unknown_c])?;
        file.write(&vec![room.unknown_d])?;
        let mut name_pstr = pascal::to_pascal_string(&room.name, ROOM_RECORD_NAME_MAX_LENGTH);
        // Strings are up to ROOM_RECORD_NAME_MAX_LENGTH long, 0 padded otherwise
        // TODO: This is an unproven assumption. Look at how EXPLORER.EXE loads the record.
        // Make sure we're not trampling important data.
        name_pstr.resize(ROOM_RECORD_NAME_MAX_LENGTH as usize, 0);
        file.write(&name_pstr)?;
    }
    Ok(())
}
