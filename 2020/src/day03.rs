pub struct ParsedData {
    pub trees: Vec<bool>,
    pub height: u16,
    pub width: u16,
}

pub fn parse(input: &str) -> ParsedData {
    let numbers = input.lines().filter(|line| line.trim().len() > 2);

    let height = numbers.clone().count() as u16;

    let trees: Vec<bool> = numbers
        .flat_map(|s| s.chars())
        .map(|c| if c == '#' { true } else { false })
        .collect();

    let width = trees.len() as u16 / height;

    ParsedData {
        trees: trees,
        height: height,
        width: width,
    }
}

pub fn part1(data: &ParsedData) -> (u64, u64) {
    let mut p1_val = 0;

    let mut pos: (u16, u16) = (0, 0);
    let stride: (u16, u16) = (3, 1);

    while pos.1 < data.height {
        if data.trees[(pos.0 % data.width + pos.1 * data.width) as usize] {
            p1_val += 1;
        }

        pos.0 += stride.0;
        pos.1 += stride.1;
    }

    (p1_val, p1_val)
}

pub fn part2(data: &ParsedData, ctx: &u64) -> u64 {
    let mut p2_val = *ctx;

    let to_test = [(1, 1), (5, 1), (7, 1), (1, 2)];

    for stride in to_test.iter() {
        let mut pos: (u16, u16) = (0, 0);
        let mut temp = 0;

        while pos.1 < data.height {
            if data.trees[(pos.0 % data.width + pos.1 * data.width) as usize] {
                temp += 1;
            }

            pos.0 += stride.0;
            pos.1 += stride.1;
        }

        p2_val *= temp;
    }

    p2_val
}
