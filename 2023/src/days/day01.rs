use crate::{Solution, SolutionPair};
use std::fs::{self};
///////////////////////////////////////////////////////////////////////////////
/// there is a lot of things I could do to speed this up, like checking on bytes instead of strings,
/// maybe a little recursion... But my timing are already +- 50% when I run, so it may just be some micro-optimisation.
/// Pretty bad day1, but I have never really done any hard programming in Rust..
/// I haven't had any problem with the overlapping numbers like some people, but blocked on the repeted text number (like sevenghj1fd5oneseven)
/// It's the slowest day of the first week !! Whyyyy

/// returns first number found in string
fn find_first_number(line: &str, reverse: bool) -> u32 {
    line.chars()
        .enumerate()
        .find_map(|(i, c)| {
            if c.is_ascii_digit() {
                c.to_digit(10)
            } else {
                if !reverse {
                    // don't like that
                    [
                        "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
                    ]
                } else {
                    [
                        "eno", "owt", "eerht", "ruof", "evif", "xis", "neves", "thgie", "enin",
                    ]
                }
                .iter()
                .enumerate()
                .find_map(|(v, w)| {
                    if line.get(i..).unwrap().starts_with(w) {
                        // don't like that, this get(i) is really baaaad
                        Some((v + 1) as u32)
                    } else {
                        None
                    }
                })
            }
        })
        .unwrap() // there should always be one, or the input is wrong.
}

fn part_one(s: &str) -> u32 {
    s.lines()
        .map(|line| {
            let mut values = line.chars().filter_map(|c| c.to_digit(10));
            match values.next() {
                // first
                Some(x) => {
                    x * 10
                        + match values.last() {
                            // and last digit
                            Some(x2) => x2,
                            None => x,
                        }
                }
                None => 0,
            }
        })
        .sum()
}

fn part_two(s: &str) -> u32 {
    s.lines()
        .map(|line| {
            find_first_number(line, false) * 10
                + find_first_number(line.chars().rev().collect::<String>().as_str(), true)
        })
        .sum()
}

pub fn solve(filename: &'static str) -> SolutionPair {
    // Your solution here...
    let buffer: String = fs::read_to_string(filename).unwrap_or_default();

    let sol1 = part_one(&buffer);
    let sol2 = part_two(&buffer);
    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U32(s1), Solution::U32(s2)) = solve("input/test_day1.txt") else {
        panic!("\x1b[1;31mMauvais type de solutions pour le test, mettez le à jour !\x1b[m")
    };
    println!("{}", s1);
    println!("{}", s2);
    assert_eq!(s1, 220);
    assert_eq!(s2, 303);
}
