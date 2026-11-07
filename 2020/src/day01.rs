pub struct ParsedData {
    pub numbers: Vec<i64>,
    pub target: i64,
}

pub fn parse(input: &str) -> ParsedData {
    let numbers: Vec<i64> = input
        .lines()
        .filter_map(|line| line.trim().parse::<i64>().ok())
        .collect();

    ParsedData {
        numbers,
        target: 2020,
    }
}

// an idea would be to sort the numbers to "find" directly what is necessary, but it may not be faster than this.

pub fn part1(data: &ParsedData) -> (i64, ()) {
    let mut p1_val = 0;

    'myloop: for (idx, &i) in data.numbers.iter().enumerate() {
        for &j in data.numbers.iter().skip(idx + 1) {
            if (i + j) == data.target {
                p1_val = i * j;
                break 'myloop;
            }
        }
    }

    (p1_val, ())
}

pub fn part2(data: &ParsedData, _ctx: &()) -> i64 {
    let mut p2_val = 0;

    'myloop: for (idx, &i) in data.numbers.iter().enumerate() {
        for (idx2, &j) in data.numbers.iter().skip(idx + 1).enumerate() {
            if (i + j) > data.target {
                continue;
            }
            for &k in data.numbers.iter().skip(idx + idx2 + 1) {
                if (i + j + k) == data.target {
                    p2_val = i * j * k;
                    break 'myloop;
                }
            }
        }
    }

    p2_val
}
