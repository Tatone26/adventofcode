pub struct ParsedData {
    pub values: Vec<String>,
}

pub fn parse(input: &str) -> ParsedData {
    let lines = input
        .split("\n\n")
        .map(|s| s.trim().to_string())
        .filter(|s| !s.is_empty())
        .collect();

    ParsedData { values: lines }
}

pub fn part1(data: &ParsedData) -> (u64, Vec<usize>) {
    let mut p1_val = 0;

    const CHECKS: [&str; 7] = ["byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"];

    let mut all_valid_passports: Vec<usize> = Vec::new();

    for (i, s) in data.values.iter().enumerate() {
        let words: Vec<&str> = s.split_whitespace().filter(|s| !s.is_empty()).collect();
        let count = words.len();

        if count > CHECKS.len() {
            p1_val += 1;
            all_valid_passports.push(i);
        } else if count == CHECKS.len() {
            if CHECKS
                .iter()
                .all(|c| words.iter().any(|w| w.starts_with(c)))
            {
                p1_val += 1;
                all_valid_passports.push(i);
            }
        } // else -> obviously wrong
    }

    (p1_val, all_valid_passports)
}

pub fn part2(data: &ParsedData, ctx: &Vec<usize>) -> u64 {
    let mut p2_val = 0;

    // we are iterating only on passports that have been validated by part 1.
    // it is quite fast then since most of the input has already been removed.

    const CHECKS: [&str; 7] = ["byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"];

    const ECL_CHECKS: [&str; 7] = ["amb", "blu", "brn", "gry", "grn", "hzl", "oth"];

    for i in ctx.iter() {
        let passport: Vec<&str> = data.values[*i]
            .split_whitespace()
            .filter(|s| !s.is_empty())
            .collect();

        let mut ok = true;

        for (i, c) in CHECKS.iter().enumerate() {
            let test = passport
                .iter()
                .find(|w| w.starts_with(c))
                .unwrap()
                .strip_prefix(c)
                .unwrap()
                .strip_prefix(":")
                .unwrap();

            let valid = match i {
                0 => test.len() == 4 && test.parse::<i64>().is_ok_and(|i| i >= 1920 && i <= 2002),
                1 => test.len() == 4 && test.parse::<i64>().is_ok_and(|i| i >= 2010 && i <= 2020),
                2 => test.len() == 4 && test.parse::<i64>().is_ok_and(|i| i >= 2020 && i <= 2030),
                3 => {
                    (test.strip_suffix("cm").is_some_and(|s| {
                        s.chars().all(|d| d.is_ascii_digit())
                            && s.parse::<i64>().is_ok_and(|n| n >= 150 && n <= 193)
                    })) || (test.strip_suffix("in").is_some_and(|s| {
                        s.chars().all(|d| d.is_ascii_digit())
                            && s.parse::<i64>().is_ok_and(|n| n >= 59 && n <= 76)
                    }))
                }
                4 => {
                    test.len() == 7
                        && test.chars().next().unwrap() == '#'
                        && test
                            .chars()
                            .skip(1)
                            .all(|c| c.is_ascii_digit() || c.is_ascii_lowercase())
                }
                5 => ECL_CHECKS.contains(&test),
                6 => test.len() == 9 && test.chars().all(|c| c.is_ascii_digit()),
                _ => false,
            };

            if !valid {
                ok = false;
                break;
            }
        }

        if ok {
            p2_val += 1;
        }
    }

    p2_val
}
