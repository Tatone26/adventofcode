use itertools::Itertools;
use num::abs;
use rustc_hash::FxHashSet;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////

#[derive(Debug, PartialEq, Eq, Clone, Copy)]
enum Dir {
    N,
    S,
    E,
    W,
    Ground,
}

type Map = Vec<Vec<char>>;
type Pos = (usize, usize);

#[allow(dead_code)]
fn print_loop(input: &Map, looping: &FxHashSet<Pos>) {
    for (y, line) in input.iter().enumerate() {
        for (x, _) in line.iter().enumerate() {
            if looping.contains(&(y, x)) {
                print!("{}", &input[y][x]);
            } else {
                print!(" ");
            }
        }
        println!();
    }
    println!();
}

/// For once, it's easy.
fn get_map(buffer: &str) -> Map {
    buffer
        .lines()
        .map(|line| line.chars().collect_vec())
        .collect_vec()
}

/// To know where we should start.
fn find_initial_direction(map: &Map, start_pos: &Pos) -> Dir {
    match map.get(start_pos.0 + 1).map(|y| y[start_pos.1]) {
        Some('|') | Some('L') | Some('J') => return Dir::S,
        _ => {}
    }
    match map.get(start_pos.0 - 1).map(|y| y[start_pos.1]) {
        Some('|') | Some('7') | Some('F') => return Dir::N,
        _ => {}
    }
    match map[start_pos.0].get(start_pos.1 + 1) {
        Some('-') | Some('L') | Some('J') => return Dir::E,
        _ => {}
    }
    match map[start_pos.0].get(start_pos.1 - 1) {
        Some('-') | Some('F') | Some('7') => return Dir::W,
        _ => {}
    }
    unreachable!();
}

/// Run along the loop.
fn next_step(map: &Map, pos: &Pos, last_direction: &Dir) -> Dir {
    match &map[pos.0][pos.1] {
        '|' => *last_direction,
        '-' => *last_direction,
        'L' => match last_direction {
            Dir::S => Dir::E,
            Dir::W => Dir::N,
            _ => unreachable!(),
        },
        'J' => match last_direction {
            Dir::S => Dir::W,
            Dir::E => Dir::N,
            _ => unreachable!(),
        },
        '7' => match last_direction {
            Dir::N => Dir::W,
            Dir::E => Dir::S,
            _ => unreachable!(),
        },
        'F' => match last_direction {
            Dir::N => Dir::E,
            Dir::W => Dir::S,
            _ => unreachable!(),
        },
        'S' => find_initial_direction(map, pos),
        _ => unreachable!(),
    }
}

/// Does both part at the same time, since the first is just counting the len of the loop...
/// See [the shoelace formula](https://en.wikipedia.org/wiki/Shoelace_formula) and [Pick's theorem](https://en.wikipedia.org/wiki/Pick%27s_theorem) to understand this function.
fn get_loop(map: &Map) -> (u64, u64) {
    let start_pos = map
        .iter()
        .enumerate()
        .find_map(|(y, line)| line.iter().position(|c| *c == 'S').map(|x| (y, x)))
        .unwrap_or((0, 0));

    let mut pos = start_pos;
    let mut last_dir = Dir::Ground;

    // The number of VERTICES of the loop.
    let mut size = 0;
    // The area calculated by the shoelace formula.
    let mut shoelace_area = 0;

    loop {
        last_dir = next_step(map, &pos, &last_dir);
        let new_pos = match last_dir {
            Dir::S => (pos.0 + 1, pos.1),
            Dir::N => (pos.0 - 1, pos.1),
            Dir::W => (pos.0, pos.1 - 1),
            Dir::E => (pos.0, pos.1 + 1),
            _ => (pos.0, pos.1),
        };
        shoelace_area += (pos.0 + new_pos.0) as i16 * (pos.1 as i16 - new_pos.1 as i16);
        size += 1;
        pos = new_pos;
        if pos == start_pos {
            break;
        }
    }
    (
        size / 2, // The furthest possible is the number of vertices / 2 : if not integer, then there are two points at equal distant so take the floor.
        (abs(shoelace_area) as f32 / 2.0 - size as f32 / 2.0 + 1.0) as u64,
    )
}

pub fn solve(buffer: &str) -> SolutionPair {
    let input = get_map(buffer);
    let (sol1, sol2) = get_loop(&input);
    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day10.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 80);
    assert_eq!(s2, 10);
}
