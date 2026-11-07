pub struct ParsedData {
    pub lowest: Vec<u8>,
    pub highest: Vec<u8>,
    pub letter: Vec<char>,
    pub passwords: Vec<String>,
}

pub fn parse(input: &str) -> ParsedData {
    let mut parsed_data = ParsedData {
        lowest: Vec::new(),
        highest: Vec::new(),
        letter: Vec::new(),
        passwords: Vec::new(),
    };

    input
        .lines()
        .map(|line| line.trim().split_whitespace().collect())
        .filter(|p: &Vec<&str>| !p.is_empty() && p.len() == 3)
        .for_each(|p| {
            parsed_data
                .lowest
                .push(u8::from_str_radix(p[0].split('-').next().unwrap(), 10).unwrap());
            parsed_data
                .highest
                .push(u8::from_str_radix(p[0].split('-').skip(1).next().unwrap(), 10).unwrap());
            parsed_data.letter.push(p[1].chars().next().unwrap());
            parsed_data.passwords.push(p[2].to_string());
        });

    parsed_data
}

// an idea would be to sort the numbers to "find" directly what is necessary, but it may not be faster than this.

pub fn part1(data: &ParsedData) -> (usize, ()) {
    let valid = data
        .passwords
        .iter()
        .enumerate()
        .filter(|(i, s)| {
            let count = s.chars().filter(|&c| c == data.letter[*i]).count() as u8;
            count >= data.lowest[*i] && count <= data.highest[*i]
        })
        .count();

    (valid, ())
}

pub fn part2(data: &ParsedData, _ctx: &()) -> usize {
    let valid = data
        .passwords
        .iter()
        .enumerate()
        .filter(|(i, s)| {
            let c = data.letter[*i];
            let a = s
                .chars()
                .nth(data.lowest[*i] as usize - 1)
                .and_then(|x| Some(x == c))
                .unwrap_or(false);
            let b = s
                .chars()
                .nth(data.highest[*i] as usize - 1)
                .and_then(|x| Some(x == c))
                .unwrap_or(false);
            a ^ b && (a || b)
        })
        .count();

    valid
}
