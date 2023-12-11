use itertools::Itertools;

use crate::{Solution, SolutionPair};
///////////////////////////////////////////////////////////////////////////////

struct Point {
    x: u64,
    y: u64,
    x_offset: u64,
    y_offset: u64,
}

impl Point {
    fn calculate_coordinates(&self, offset: u64) -> (u64, u64) {
        (
            self.x + self.x_offset * (offset - 1),
            self.y + self.y_offset * (offset - 1),
        )
    }
}
type Points = Vec<Point>;

fn read_and_expand_input(buffer: &str) -> Points {
    let mut result: Points = vec![];
    let mut empty_columns: Vec<bool> =
        vec![true; buffer.lines().next().expect("Empty first line").len()];
    let mut empty_lines = 0;

    for (y, line) in buffer.lines().enumerate() {
        let mut empty_line = true;
        for (x, character) in line.chars().enumerate() {
            if character == '#' {
                result.push(Point {
                    x: x as u64,
                    y: y as u64,
                    x_offset: 0,
                    y_offset: empty_lines,
                });
                empty_line = false;
                empty_columns[x] = false;
            }
        }
        if empty_line {
            empty_lines += 1;
        }
    }

    for pt in result.iter_mut() {
        pt.x_offset += empty_columns
            .iter()
            .take(pt.x as usize)
            .filter(|&&b| b)
            .count() as u64
    }
    result
}

fn length_of_paths(input: &Points, offset: u64) -> u64 {
    let coordos: Vec<(u64, u64)> = input
        .iter()
        .map(|p| p.calculate_coordinates(offset))
        .collect();

    (0..input.len())
        .tuple_combinations()
        .map(|(i, j)| coordos[i].0.abs_diff(coordos[j].0) + coordos[i].1.abs_diff(coordos[j].1))
        .sum()
}

pub fn solve(buffer: &str) -> SolutionPair {
    const PART1_OFFSET: u64 = 2;
    const PART2_OFFSET: u64 = 1_000_000;
    let input = read_and_expand_input(buffer);
    let sol1: u64 = length_of_paths(&input, PART1_OFFSET);
    let sol2: u64 = length_of_paths(&input, PART2_OFFSET);

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day11.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 374);
    assert_eq!(s2, 82_000_210);
}
