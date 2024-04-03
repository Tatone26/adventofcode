use itertools::Itertools;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
/// Not finished, stays in my head until I've done day 20.
/// First part was easy, just needed to realise he can do back-and-forth infinitely. (and that the same for part2)
/// -> Just need to find the positions where he can get with an even/odd number of moves, and that's the answer.
/// Part2 is the same, but it's infinite, so the calculation is clearly different. (maybe / instead of % or something like that.)
/// Apparently we need to use a special property of the input :(((((

#[derive(Debug)]
enum Tile {
    Plot,
    Rock,
    Start,
}

type Map = Vec<Vec<Tile>>;

fn read_input(buffer: &str) -> Map {
    buffer
        .lines()
        .map(|line| {
            line.chars()
                .map(|c| match c {
                    '.' => Tile::Plot,
                    '#' => Tile::Rock,
                    _ => Tile::Start,
                })
                .collect_vec()
        })
        .collect_vec()
}

fn find_start(input: &Map) -> (usize, usize) {
    input
        .iter()
        .enumerate()
        .find_map(|(i, line)| {
            let x = line.iter().position(|c| matches!(c, Tile::Start));
            if x.is_some() {
                return Some((i, x.unwrap()));
            } else {
                None
            }
        })
        .unwrap()
}

/// Does BFS to get the distances to everything. -> just need to get the even or odd ones then.
fn possible_outcomes(input: &Map, number_of_steps: u64) -> Vec<Vec<Option<u64>>> {
    let start = find_start(input);
    let mut result = vec![vec![None; input[0].len()]; input.len()];
    result[start.1][start.0] = Some(0);
    let mut current = vec![start];
    for i in 1..=number_of_steps {
        let mut next_current = vec![];
        for pos in current {
            const DIRECTIONS: [(i32, i32); 4] = [(0, 1), (1, 0), (-1, 0), (0, -1)];
            for d in DIRECTIONS {
                let next_pos = (pos.0 as i32 + d.0, pos.1 as i32 + d.1);
                if next_pos.0 >= 0
                    && next_pos.1 >= 0
                    && next_pos.0 < input[0].len() as i32
                    && next_pos.1 < input.len() as i32
                {
                    match input[next_pos.1 as usize][next_pos.0 as usize] {
                        Tile::Plot => match result[next_pos.1 as usize][next_pos.0 as usize] {
                            Some(_) => (),
                            None => {
                                result[next_pos.1 as usize][next_pos.0 as usize] = Some(i);
                                next_current.push((next_pos.0 as usize, next_pos.1 as usize))
                            }
                        },
                        Tile::Rock => (),
                        Tile::Start => (),
                    }
                }
            }
        }
        current = next_current;
    }
    result
}

/// Does BFS to get the distances to everything. -> just need to get the even or odd ones then.
/// The result will be different :
/// There is wrapping, and for each point instead of checking if it has already a position, need to check if already a even/odd position ?
/// I have no idea if that is good, but it may be.
/// It seems to me that is tis pretty logical... BUTNO! because if you can get there from two different directions, it won't be the same.
/// Somehow need to count the number of possibilities to get to this point...
/// Maybe I should start with 3*3 and print all the stpes, see if there is a logic there.
/// I think just need to find the logic, like with the first part.
/// Maybe I can extrapolate the 3*3 result to what I want ? Like with divisions ? or something like that ?
/// Or maybe I just do it with wrapping, get for each case if it's possible to get there on even or odd ?Don't know if its good enough.
/* fn possible_outcomes_infinite(input: &Map, number_of_steps: u64) -> Vec<Vec<Option<u64>>> {
    let start = find_start(input);
    let mut result = vec![vec![None; input[0].len()]; input.len()];
    result[start.1][start.0] = Some(0);
    let mut current = vec![start];
    for i in 1..=number_of_steps {
        let mut next_current = vec![];
        for pos in current {
            const DIRECTIONS: [(i32, i32); 4] = [(0, 1), (1, 0), (-1, 0), (0, -1)];
            for d in DIRECTIONS {
                let next_pos = (pos.0 as i32 + d.0, pos.1 as i32 + d.1);
                if next_pos.0 >= 0
                    && next_pos.1 >= 0
                    && next_pos.0 < input[0].len() as i32
                    && next_pos.1 < input.len() as i32
                {
                    match input[next_pos.1 as usize][next_pos.0 as usize] {
                        Tile::Plot => match result[next_pos.1 as usize][next_pos.0 as usize] {
                            Some(_) => (),
                            None => {
                                result[next_pos.1 as usize][next_pos.0 as usize] = Some(i);
                                next_current.push((next_pos.0 as usize, next_pos.1 as usize))
                            }
                        },
                        Tile::Rock => (),
                        Tile::Start => (),
                    }
                }
            }
        }
        current = next_current;
    }
    result
} */

fn part_one(input: &Vec<Vec<Option<u64>>>, number_of_steps: u64) -> u64 {
    input
        .iter()
        .flat_map(|line| {
            line.iter()
                .flatten()
                .filter(|c| *c % 2 == number_of_steps % 2)
        })
        .count() as u64
}

pub fn solve(buffer: &str) -> SolutionPair {
    let input = read_input(buffer);
    let outcomes = possible_outcomes(&input, 10);
    /* for v in outcomes.iter() {
        println!("{v:?}");
    } */
    let sol1: u64 = part_one(&outcomes, 10); /*
                                             const PART_TWO: u64 = 26501365;
                                             const PART_TWO_TEST: u64 = 5000; */
    let sol2: u64 = 0;

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day21.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 42);
    assert_eq!(s2, 16733044);
}
