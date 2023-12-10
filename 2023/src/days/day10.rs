use itertools::Itertools;
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

impl Dir {
    fn inverse(&self) -> Self {
        match self {
            Dir::N => Dir::S,
            Dir::S => Dir::N,
            Dir::E => Dir::W,
            Dir::W => Dir::E,
            _ => panic!(),
        }
    }

    fn coordinates(&self) -> (i16, i16) {
        match self {
            Dir::S => (1, 0),
            Dir::N => (-1, 0),
            Dir::W => (0, -1),
            Dir::E => (0, 1),
            _ => (0, 0),
        }
    }
}

type Map = Vec<Vec<(char, bool)>>;
type Pos = (usize, usize);

#[allow(dead_code)]
fn print_loop(input: &Map, looping: &FxHashSet<Pos>) {
    for (y, line) in input.iter().enumerate() {
        for (x, _) in line.iter().enumerate() {
            if looping.contains(&(y, x)) {
                print!("{}", &input[y][x].0);
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
        .map(|line| line.chars().map(|c| (c, false)).collect_vec())
        .collect_vec()
}

/// To know where we should start.
fn get_first_step(map: &Map, start_pos: &Pos) -> Dir {
    if let Some(y) = map.get(start_pos.0 + 1) {
        if matches!(y[start_pos.1].0, '|' | 'L' | 'J') {
            return Dir::S;
        }
    }
    if let Some(y) = map.get(start_pos.0 - 1) {
        if matches!(y[start_pos.1].0, '|' | '7' | 'F') {
            return Dir::N;
        }
    }
    if let Some(x) = map[start_pos.0].get(start_pos.1 + 1) {
        if matches!(x.0, '-' | 'J' | '7') {
            return Dir::E;
        }
    }
    if let Some(x) = map[start_pos.0].get(start_pos.1 - 1) {
        if matches!(x.0, '-' | 'F' | 'L') {
            return Dir::W;
        }
    }
    unreachable!();
}

/// Run along the loop.
fn next_step(map: &Map, pos: &Pos, last_direction: &Dir) -> Dir {
    match &map[pos.0][pos.1].0 {
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
        'S' => get_first_step(map, pos),
        _ => unreachable!(),
    }
}

/// Finds the loop, runs alongs it while modifying the map (setting to "true" every point that is in the loop.)
fn measure_loop(map: &mut Map) {
    let pos_start = map
        .iter()
        .enumerate()
        .find_map(|(y, line)| line.iter().position(|(c, _)| *c == 'S').map(|x| (y, x)))
        .unwrap_or((0, 0));

    map[pos_start.0][pos_start.1].1 = true;

    let mut last_dir = next_step(map, &pos_start, &Dir::Ground);
    let mut pos = (
        (pos_start.0 as i16 + last_dir.coordinates().0) as usize,
        (pos_start.1 as i16 + last_dir.coordinates().1) as usize,
    );
    let start_dir = last_dir;

    loop {
        map[pos.0][pos.1].1 = true;
        last_dir = next_step(map, &pos, &last_dir);
        pos = (
            (pos.0 as i16 + last_dir.coordinates().0) as usize,
            (pos.1 as i16 + last_dir.coordinates().1) as usize,
        );
        if pos == pos_start {
            break;
        }
    }
    // Replaces the "S" by its true meaning, otherwise big boom
    map[pos_start.0][pos_start.1] = match (start_dir, last_dir.inverse()) {
        (Dir::N, Dir::S) | (Dir::S, Dir::N) => ('|', true),
        (Dir::E, Dir::W) | (Dir::W, Dir::E) => ('-', true),
        (Dir::N, Dir::W) | (Dir::W, Dir::N) => ('J', true),
        (Dir::N, Dir::E) | (Dir::E, Dir::N) => ('L', true),
        (Dir::S, Dir::W) | (Dir::W, Dir::S) => ('7', true),
        (Dir::S, Dir::E) | (Dir::E, Dir::S) => ('F', true),
        _ => ('.', true),
    };
}

fn part_2(map: &Map) -> u64 {
    map.iter()
        .flat_map(|line| {
            let mut counter = 0;
            let mut last_dir = ' ';
            line.iter().filter(move |(c, in_loop)| {
                if *in_loop {
                    match c {
                        '|' => counter += 1,
                        'J' => {
                            if last_dir == 'F' {
                                counter += 1
                            }
                        }
                        '7' => {
                            if last_dir == 'L' {
                                counter += 1
                            }
                        }
                        _ => (),
                    }
                    if *c != '-' {
                        last_dir = *c;
                    }
                    false
                } else {
                    counter % 2 == 1
                }
            })
        })
        .count() as u64
}

pub fn solve(buffer: &str) -> SolutionPair {
    let mut input = get_map(buffer);
    measure_loop(&mut input);
    let sol1: u64 = input
        .iter()
        .flat_map(|line| line.iter().filter(|x| x.1))
        .count() as u64
        / 2;
    let sol2: u64 = part_2(&input);
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
// Answer lower than 453.
