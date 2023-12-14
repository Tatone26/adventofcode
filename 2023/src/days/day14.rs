use core::fmt;

use itertools::Itertools;
use rustc_hash::FxHashMap;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
/// Easy but really ugly, need to use a better 2d struct for grids.
/// Need to make my own simple struct, since I do not need anything fancy (just creating it and accessing precise points.)
/// Would be a lot faster (it would be 1D mapped to 2D of course.)

enum Rock {
    Round,
    Cube,
    Empty,
}

impl fmt::Debug for Rock {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Rock::Round => write!(f, "O"),
            Rock::Cube => write!(f, "#"),
            Rock::Empty => write!(f, "."),
        }
    }
}

fn read_input(buffer: &str) -> Vec<Vec<Rock>> {
    buffer
        .lines()
        .map(|line| {
            line.chars()
                .map(|c| match c {
                    'O' => Rock::Round,
                    '#' => Rock::Cube,
                    _ => Rock::Empty,
                })
                .collect_vec()
        })
        .collect_vec()
}

fn move_north(input: &mut [Vec<Rock>]) {
    for i in 0..input[0].len() {
        for j in 1..input.len()
        // 0 will never move
        {
            if let Rock::Round = input[j][i] {
                let mut go_up = 0;
                while go_up < j && matches!(input[j - go_up - 1][i], Rock::Empty) {
                    go_up += 1;
                }
                if go_up != 0 {
                    input[j][i] = Rock::Empty;
                    input[j - go_up][i] = Rock::Round;
                }
            }
        }
    }
}

fn move_south(input: &mut [Vec<Rock>]) {
    for i in 0..input[0].len() {
        for j in (0..(input.len() - 1)).rev() {
            if let Rock::Round = input[j][i] {
                let mut go_down = 0;
                while go_down + j < input.len() - 1
                    && matches!(input[j + go_down + 1][i], Rock::Empty)
                {
                    go_down += 1;
                }
                if go_down != 0 {
                    input[j][i] = Rock::Empty;
                    input[j + go_down][i] = Rock::Round;
                }
            }
        }
    }
}

fn move_east(input: &mut [Vec<Rock>]) {
    for line in input.iter_mut() {
        for i in (0..(line.len() - 1)).rev() {
            {
                if let Rock::Round = line[i] {
                    let mut go_right = 0;
                    while go_right + i < line.len() - 1
                        && matches!(line[i + go_right + 1], Rock::Empty)
                    {
                        go_right += 1;
                    }
                    if go_right != 0 {
                        line[i] = Rock::Empty;
                        line[i + go_right] = Rock::Round;
                    }
                }
            }
        }
    }
}

fn move_west(input: &mut [Vec<Rock>]) {
    for line in input.iter_mut() {
        for i in 1..line.len() {
            {
                if let Rock::Round = line[i] {
                    let mut go_left = 0;
                    while go_left < i && matches!(line[i - go_left - 1], Rock::Empty) {
                        go_left += 1;
                    }
                    if go_left != 0 {
                        line[i] = Rock::Empty;
                        line[i - go_left] = Rock::Round;
                    }
                }
            }
        }
    }
}

fn cycle(input: &mut [Vec<Rock>], skip_first: bool) {
    if !skip_first {
        move_north(input);
    }
    move_west(input);
    move_south(input);
    move_east(input);
}

fn count_load(input: &[Vec<Rock>]) -> u64 {
    input
        .iter()
        .zip((1..(input.len() + 1)).rev())
        .map(|(line, i)| line.iter().filter(|c| matches!(c, Rock::Round)).count() as u64 * i as u64)
        .sum()
}

fn to_string(input: &[Vec<Rock>]) -> Vec<u64> {
    input
        .iter()
        .map(|line| {
            let mut r = 0;
            line.iter().for_each(|rock| {
                r <<= 1;
                if let Rock::Round = rock {
                    r |= 1
                }
            });
            r
        })
        .collect::<Vec<u64>>()
}

fn part_two(input: &mut [Vec<Rock>]) -> u64 {
    const MAX_CYCLE: u64 = 1000000000;
    let mut set: FxHashMap<Vec<u64>, u64> = FxHashMap::default();
    cycle(input, true);
    set.insert(to_string(input), 0);
    for i in 1..MAX_CYCLE {
        cycle(input, false);
        let hash = to_string(input);
        if let Some(x) = set.get(&hash) {
            for _ in 0..((MAX_CYCLE - i) % (i - x) - 1) {
                // don't know why -1
                cycle(input, false);
            }
            return count_load(input);
        }
        set.insert(to_string(input), i);
    }
    unreachable!()
}

pub fn solve(buffer: &str) -> SolutionPair {
    let mut input = read_input(buffer);

    move_north(&mut input);

    let sol1: u64 = count_load(&input);
    let sol2: u64 = part_two(&mut input);

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day14.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 136);
    assert_eq!(s2, 64);
}
