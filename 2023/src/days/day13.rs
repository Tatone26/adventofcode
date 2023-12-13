use std::cmp::min;

use itertools::Itertools;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
/// Pretty easy day, but stupid problems made me spend hours on it... (couldn't transpose easily, ended up just bruteforcing every point in every group,
/// didn't understand the requirements for the new reflexion (different from the last one) etc.
/// Finally made a nice version, using ints and binary to represent the data, and being smart (looking for a precise number of differences.)
/// At least it's a day where the time is spent only on parsing the input !

type Line = u32;

/// Need to speed THAT up if I want more speed
fn read_input(buffer: &str) -> Vec<(Vec<Line>, usize)> {
    buffer
        .split("\n\n")
        .map(|group| {
            (
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
                        r
                    })
                    .collect(),
                group.lines().next().unwrap().len(),
            )
        })
        .collect_vec()
}

/// For each line, counts the number of differences. If it's 0 (first part) or 1 (second part) (or anything else really), returns this axis.
/// Could make it better specially for 0 and 1 diff, but do I care ? nah that's alright like that.
/// PS : for part 2, better way would be { let mut used_diff : bool = false; if (input[i - j] ^ input[i + 1 + j]).is_power_of_two() { if used_diff { continue 'out} else { used_diff = true}}}
fn horizontal_reflection(input: &[Line], target_diff: u32) -> u64 {
    'out: for i in 0..(input.len() - 1) {
        let mut diffs = 0;
        for j in 0..(min(i + 1, input.len() - i - 1)) {
            diffs += (input[i - j] ^ input[i + 1 + j]).count_ones();
            if diffs > target_diff {
                continue 'out;
            }
        }
        if diffs == target_diff {
            return (i + 1) as u64;
        }
    }
    0
}

fn transpose(input: &[Line], size: usize) -> Vec<Line> {
    let mut r = vec![0; size];
    input.iter().for_each(|line| {
        for (i, x) in r.iter_mut().enumerate() {
            *x = (*x << 1) | ((line & (1 << i)) >> i);
        }
    });
    r.reverse();
    r
}

#[inline]
fn vertical_reflection(input: &[Line], target_differences: u32, size: usize) -> u64 {
    horizontal_reflection(&transpose(input, size), target_differences)
}

fn new_reflection(group: &[Line], size: usize) -> (u64, u64) {
    let old_x = horizontal_reflection(group, 0);
    let old_y = vertical_reflection(group, 0, size);

    let new_x = horizontal_reflection(group, 1);
    if new_x != 0 && new_x != old_x {
        return (new_x, 0);
    }
    let new_y = vertical_reflection(group, 1, size);
    if new_y != 0 && new_y != old_y {
        return (0, new_y);
    }
    unreachable!()
}

fn part_two(input: &[(Vec<Line>, usize)]) -> u64 {
    let test: Vec<(u64, u64)> = input
        .iter()
        .map(|group| new_reflection(&group.0, group.1))
        .collect_vec();
    test.iter().map(|(a, b)| b + 100 * a).sum()
}

fn part_one(input: &[(Vec<Line>, usize)]) -> u64 {
    let test: Vec<(u64, u64)> = input
        .iter()
        .map(|group| {
            let x = horizontal_reflection(&group.0, 0);
            let y = vertical_reflection(&group.0, 0, group.1);
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
    assert_eq!(s2, 400);
}
