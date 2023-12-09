use itertools::Itertools;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////

fn get_input(buf: &str) -> Vec<Vec<i64>> {
    buf.lines()
        .map(|line| {
            line.split_whitespace()
                .map(|c| c.parse().unwrap_or(0))
                .collect_vec()
        })
        .collect_vec()
}

// I don't like allocating a new one everytime, it's probably very slow.
fn next_step(input: &[i64]) -> Vec<i64> {
    input
        .iter()
        .tuple_windows()
        .map(|(a, b)| b - a)
        .collect_vec()
}

fn answer(input: &[i64]) -> i64 {
    let last = *input.last().unwrap_or(&0);
    if !input.iter().all_equal() {
        // We should stop when everything = 0 but one step before that, all numbers or equals (deriv of constant).
        last + answer(&next_step(input))
    } else {
        last
    }
}

pub fn solve(buffer: &str) -> SolutionPair {
    let input = get_input(buffer);
    let sol1: i64 = input.iter().map(|line| answer(line)).sum();
    let sol2: i64 = input
        .iter()
        .map(|line| answer(&line.iter().rev().copied().collect_vec()))
        .sum();
    if sol1.is_negative() || sol2.is_negative() {
        panic!("We got a negative answer !!");
    }
    (Solution::from(sol1 as u64), Solution::from(sol2 as u64))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day9.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 114);
    assert_eq!(s2, 2);
}
