use core::panic;

use itertools::Itertools;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
/// Easy day again. Wtf. Did it by bruteforcing first, then realised later in the day that it's quadratic.
/// Just had to get a pen and paper to get the right equation.
/// Instant execution, slowed by IO.
//////////////////////////////////////////////////////////////////////////////

/// A race is a couple (time, distance) -> we are looking for the number of ways we can beat that distance in this amount of time.
type Race = (u64, u64);

fn input_part_one(buf: &str) -> Vec<Race> {
    let mut it = buf.lines();
    it.next()
        .expect("File probably empty !")
        .split_ascii_whitespace()
        .zip(
            it.next()
                .expect("File probably only one line long !")
                .split_ascii_whitespace(),
        )
        .skip(1)
        .filter_map(|(x, y)| match (x.parse(), y.parse()) {
            // Just some safety, which is funny when I use unwrap two lines higher.
            (Ok(x2), Ok(y2)) => Some((x2, y2)),
            (_, _) => None,
        })
        .collect_vec()
}

fn input_part_two(buf: &str) -> Race {
    if let Some((Ok(x), Ok(y))) = buf
        .lines()
        .map(|line| {
            line.chars()
                .filter(|c| c.is_ascii_digit())
                .collect::<String>()
                .parse()
        })
        .collect_tuple()
    {
        (x, y)
    } else {
        panic!("Error reading file for part two !!");
    }
}

fn dist_between_roots((time, distance): &Race) -> Option<u64> {
    // Not bothering to show the entire equation here, but really it's just a matter of f(x) = x * (t - x).
    let sqrt_delta: f64 = ((time * time) as f64 - 4.0 * (*distance + 1) as f64).sqrt();
    if sqrt_delta.is_nan() {
        println!("It seems like one the input ({time}, {distance}) can't be won...");
        None
    } else {
        let x1 = ((*time as f64 - sqrt_delta) / 2.0).ceil();
        let x2 = ((*time as f64 + sqrt_delta) / 2.0).floor();
        Some((x2 - x1) as u64 + 1)
    }
}

fn part_one(input: &[Race]) -> u64 {
    input.iter().filter_map(dist_between_roots).product()
}

fn part_two(input: &Race) -> u64 {
    dist_between_roots(input).unwrap()
}

pub fn solve(buffer: &str) -> SolutionPair {
    let input = input_part_one(buffer);
    let sol1: u64 = part_one(&input);
    let input_2 = input_part_two(buffer);
    let sol2: u64 = part_two(&input_2);
    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) = solve("input/test_day6.txt") else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 288);
    assert_eq!(s2, 71503);
}
