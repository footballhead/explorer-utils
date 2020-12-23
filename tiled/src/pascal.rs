/// Convert a byte length-prefixed string into a Rust string.
/// Only single byte lengths are supported (max 255 chars)
///
/// https://en.wikipedia.org/wiki/String_(computer_science)#Length-prefixed
pub fn from_pascal_string(pstring: &[u8]) -> String {
    let length = pstring[0] as usize;
    String::from_utf8(pstring[1..length + 1].to_vec()).unwrap()
}

/// Convert a Rust string of at most 255 chars to a Pascal String.
/// Will truncate if `data.len() > max_length`.
///
/// https://en.wikipedia.org/wiki/String_(computer_science)#Length-prefixed
pub fn to_pascal_string(data: &str, max_length: u8) -> Vec<u8> {
    // TODO: Warn if str.len() > 255?
    let length = if data.len() > max_length as usize {
        max_length
    } else {
        data.len() as u8
    };
    let mut builder = vec![length];
    builder.extend_from_slice(&data.as_bytes()[0..length as usize]);
    return builder;
}
