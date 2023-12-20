use core::fmt;

use itertools::Itertools;
use rustc_hash::FxHashMap;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
/// Tried a better 2D struct, but was nos faster.

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
        let mut j = 1;
        while j < input.len() {
            if let Rock::Round = input[j][i] {
                let mut go_up = 0;
                while go_up < j && matches!(input[j - go_up - 1][i], Rock::Empty) {
                    go_up += 1;
                }
                if go_up != 0 {
                    input[j][i] = Rock::Empty;
                    input[j - go_up][i] = Rock::Round;
                } else {
                    j += 1;
                }
            }
            j += 1;
        }
    }
}

fn move_south(input: &mut [Vec<Rock>]) {
    for i in 0..input[0].len() {
        let mut j = (input.len() - 2) as i32;
        while j >= 0 {
            if let Rock::Round = input[j as usize][i] {
                let mut go_down = 0;
                while (go_down + j as usize) < input.len() - 1
                    && matches!(input[j as usize + go_down + 1][i], Rock::Empty)
                {
                    go_down += 1;
                }
                if go_down != 0 {
                    input[j as usize][i] = Rock::Empty;
                    input[j as usize + go_down][i] = Rock::Round;
                } else {
                    j -= 1;
                }
            }
            j -= 1;
        }
    }
}

fn move_east(input: &mut [Vec<Rock>]) {
    for line in input.iter_mut() {
        let mut i = (line.len() - 2) as i32;
        while i >= 0 {
            if let Rock::Round = line[i as usize] {
                let mut go_right = 0;
                while go_right + (i as usize) < line.len() - 1
                    && matches!(line[i as usize + go_right + 1], Rock::Empty)
                {
                    go_right += 1;
                }
                if go_right != 0 {
                    line[i as usize] = Rock::Empty;
                    line[i as usize + go_right] = Rock::Round;
                } else {
                    i -= 1;
                }
            }
            i -= 1;
        }
    }
}

fn move_west(input: &mut [Vec<Rock>]) {
    for line in input.iter_mut() {
        let mut i = 1;
        while i < line.len() {
            if let Rock::Round = line[i] {
                let mut go_left = 0;
                while go_left < i && matches!(line[i - go_left - 1], Rock::Empty) {
                    go_left += 1;
                }
                if go_left != 0 {
                    line[i] = Rock::Empty;
                    line[i - go_left] = Rock::Round;
                } else {
                    i += 1;
                }
            }
            i += 1;
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
        .rev()
        .enumerate()
        .map(|(i, line)| {
            line.iter().filter(|c| matches!(c, Rock::Round)).count() as u64 * (i + 1) as u64
        })
        .sum()
}

fn to_string(input: &[Vec<Rock>]) -> u128 {
    input
        .iter()
        .map(|line| {
            let mut r: u128 = 0;
            for rock in line {
                r <<= 1;
                if let Rock::Round = rock {
                    r |= 1;
                }
            }
            r
        })
        .sum::<u128>()
}

fn part_two(input: &mut [Vec<Rock>]) -> u64 {
    const MAX_CYCLE: u64 = 1000000000;
    let mut set: FxHashMap<u128, u64> = FxHashMap::default();
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
        set.insert(hash, i);
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
