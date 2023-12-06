use std::{fs::File, io::Read};

use itertools::Itertools;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
/// Easy day again. Wtf. Not my proudest of algos, but hey it works.

fn read_input(buf: &str) -> Vec<(u64, u64)> {
    let mut it = buf.lines();
    it.next()
        .unwrap()
        .split_ascii_whitespace()
        .zip(it.next().unwrap().split_ascii_whitespace())
        .skip(1)
        .map(|(x, y)| (x.parse().unwrap(), y.parse().unwrap()))
        .collect_vec()
}

fn do_win(i: &u64, time: &u64, distance: &u64) -> bool {
    i * (time - i) > *distance
}

// Considering that everything before start (1..start) is not a victory.
fn ways_to_win(start: u64, (time, distance): &(u64, u64)) -> Option<u64> {
    // There must be a result in this range (1..time/2) or the game is impossible to beat.
    (start..(*time / 2))
        .find(|i| do_win(i, time, distance))
        .map(|x| time - 1 - ((x - 1) * 2))
}

/// Not 100% sure of this algo but fast and working for my input, so I guess it's good.
fn smart_ways_to_win((time, distance): &(u64, u64)) -> Option<u64> {
    let mut i = time / 4;
    // some kind of "binary search"
    while do_win(&i, time, distance) {
        i -= i / 2;
    }
    // fast advance
    let up = 1000;
    while !do_win(&(i + up), time, distance) {
        i += up;
    }
    // finishing slowly
    ways_to_win(i, &(*time, *distance))
    // = fast way to find answer !
}

fn input_part_two(input: &[(u64, u64)]) -> (u64, u64) {
    input
        .iter()
        .map(|(x, y)| (x.to_string(), y.to_string()))
        .reduce(|(acc_time, acc_dist), (elem_time, elem_dist)| {
            // concat via string (bad)
            (acc_time + &elem_time, acc_dist + &elem_dist)
        })
        .map(|(x, y)| (x.parse().unwrap(), y.parse().unwrap()))
        .unwrap()
}

fn part_two(input: &(u64, u64)) -> u64 {
    smart_ways_to_win(input).unwrap()
}

fn part_one(input: &[(u64, u64)]) -> u64 {
    input.iter().filter_map(|x| ways_to_win(1, x)).product()
}

pub fn solve(filename: &'static str) -> SolutionPair {
    let mut buffer: String = String::new();
    File::open(filename)
        .unwrap_or_else(|_| panic!("No {filename} file."))
        .read_to_string(&mut buffer)
        .unwrap_or_else(|_| panic!("Error reading {filename} as file."));

    let input = read_input(&buffer);
    let sol1: u64 = part_one(&input);
    //let sol1 = 0;
    let input_2 = input_part_two(&input);
    let sol2: u64 = part_two(&input_2);
    //let sol2 = 0;
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
