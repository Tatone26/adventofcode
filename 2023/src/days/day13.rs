use itertools::Itertools;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
/// For now, bruteforce it is i don't care it's fast enough ("smart" bruteforce I guess, using numbers and bits instead of array must be faster... :) )
/// Gonna need to do it with more intelligence later on, but I need to get fast day 12 first

type Line = (u32, usize);

fn read_input(buffer: &str) -> Vec<Vec<Line>> {
    buffer
        .split("\n\n")
        .map(|group| {
            group
                .lines()
                .map(|l| {
                    let mut r = 0;
                    l.chars().for_each(|c| {
                        r <<= 1;
                        r |= match c {
                            '.' => 0,
                            _ => 1,
                        }
                    });
                    (r, l.len())
                })
                .collect()
        })
        .collect_vec()
}

/// Truth : it's the same for vertical but transposed, so I found it easier to transpose it before
/// Should transpose at creation for real
fn horizontal_reflection(input: &[Line], not_this_one: u64) -> u64 {
    for i in 0..(input.len() - 1) {
        if (i + 1) as u64 == not_this_one {
            continue;
        }
        let mut j = 0;
        while (i as i16 - j as i16) >= 0 && i + 1 + j < input.len() {
            if (input[i - j].0 ^ input[i + 1 + j].0) != 0 {
                j = 0;
                break;
            }
            j += 1;
        }
        if j != 0 {
            return (i + 1) as u64;
        }
    }
    0
}

// Should not use that lol, or maybe transpose everything once at the start.
fn transpose(input: &[Line]) -> Vec<Line> {
    let size = input.first().unwrap().1;
    (0..size)
        .rev()
        .map(|i| {
            let mask = 1 << i;
            let mut r = 0;
            input.iter().for_each(|(u, _)| {
                r <<= 1;
                r |= (u & mask) >> i;
            });
            (r, size)
        })
        .collect_vec()
}

#[inline]
fn vertical_reflection(input: &[Line], not_this_one: u64) -> u64 {
    horizontal_reflection(&transpose(input), not_this_one)
}

fn new_reflection(group: &[Line]) -> (u64, u64) {
    let old_x = horizontal_reflection(group, 158);
    let old_y = vertical_reflection(group, 158);
    for i in 0..group.len() {
        for j in 0..group.get(i).unwrap().1 {
            let mut new_group = group.to_vec(); // bad lol
            new_group.get_mut(i).unwrap().0 ^= 1 << j;
            let x = horizontal_reflection(&new_group, old_x);
            if x != 0 {
                return (x, 0);
            }
            let y = vertical_reflection(&new_group, old_y);
            if y != 0 {
                return (0, y);
            }
        }
    }
    unreachable!()
}

fn part_two(input: &[Vec<Line>]) -> u64 {
    let test: Vec<(u64, u64)> = input.iter().map(|v| new_reflection(v)).collect_vec();
    test.iter().map(|(a, b)| b + 100 * a).sum()
}

fn part_one(input: &[Vec<Line>]) -> u64 {
    let test: Vec<(u64, u64)> = input
        .iter()
        .map(|group| {
            let x = horizontal_reflection(group, 258);
            let y = vertical_reflection(group, 258);
            (x, y)
        })
        .collect();
    // println!("{test:?}");
    test.iter().map(|(a, b)| b + 100 * a).sum()
}

pub fn solve(buffer: &str) -> SolutionPair {
    let input = read_input(buffer);

    let sol1: u64 = part_one(&input);
    let sol2: u64 = part_two(&input);
    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day13.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 405);
    assert_eq!(s2, 401);
}
