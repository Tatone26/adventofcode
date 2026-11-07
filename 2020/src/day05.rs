pub struct ParsedData {
    pub numbers: Vec<char>,
    pub seat_size: usize,
    pub number_of_seats: usize,
}

pub fn parse(input: &str) -> ParsedData {
    let seat_size = input.lines().next().unwrap().trim().len();
    assert!(seat_size == 10);
    let mut tot = 0;
    let mut numbers = Vec::new();
    for s in input.lines().map(|s| s.trim()).filter(|s| !s.is_empty()) {
        s.chars().for_each(|c| numbers.push(c));
        tot += 1;
    }

    ParsedData {
        numbers,
        seat_size,
        number_of_seats: tot,
    }
}

pub fn part1(data: &ParsedData) -> (u16, Vec<u16>) {
    let mut ids = Vec::new();

    for i in 0..data.number_of_seats {
        let mut id: u16 = 0;
        for j in 0..10 {
            id |= (match data.numbers[i * data.seat_size + j] {
                'F' | 'L' => 0,
                'B' | 'R' => 1,
                _ => 0,
            }) << (9 - j);
        }

        ids.push(id);
    }

    (*ids.iter().max().unwrap(), ids)
}

pub fn part2(_data: &ParsedData, ctx: &Vec<u16>) -> u16 {
    let mut numbers: Vec<u16> = ctx.clone();
    numbers.sort();
    numbers
        .windows(2)
        .find(|block| block[0] + 2 == block[1])
        .map(|x| x[0] + 1)
        .unwrap_or(0)
}
