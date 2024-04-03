use std::{cmp::Reverse, collections::BinaryHeap};

use itertools::Itertools;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
// first ty : BRUTEFORCE 6S BOTH PARTS !! (first try for second part yepeee)
// Using binary heap is litterally ten times faster.
// Dont' see how I can use rayon here.

fn read_input(buffer: &str) -> Vec<Vec<u8>> {
    buffer
        .lines()
        .map(|line| {
            line.chars()
                .map(|c| c.to_digit(10).unwrap() as u8)
                .collect_vec()
        })
        .collect_vec()
}

#[derive(Debug, PartialEq, Eq, Clone, Copy)]
enum Dir {
    Up,
    Down,
    Right,
    Left,
}

#[derive(Debug, Clone, PartialEq, Eq)]
struct Point {
    x: i32,
    y: i32,
}

#[derive(Debug, PartialEq, Eq, Clone)]
struct PointWithMemory {
    priority: u64,
    pos: Point,
    last_dir: Option<Dir>,
    streak: u8,
    cost: u64,
}

impl Ord for PointWithMemory {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.cost.cmp(&other.cost)
    }
}

impl PartialOrd for PointWithMemory {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}

type PossiblePoint = (u64, u8, Option<Dir>); // (cost, straight_line_streak, direction_to_get_here)
type RememberMap = Vec<Vec<Vec<PossiblePoint>>>;
type NextPointsFunction = fn(
    &[Vec<u8>],
    &mut RememberMap,
    &PointWithMemory,
    (usize, usize),
    u8,
    u8,
) -> Option<Vec<PointWithMemory>>;

fn dijkstra_strange(
    input: &[Vec<u8>],
    start: Point,
    get_next_points: NextPointsFunction,
    minimum_streak: u8,
    maximum_streak: u8,
) -> u64 {
    let mut queue = BinaryHeap::new();
    queue.push(Reverse(PointWithMemory {
        pos: start,
        last_dir: None,
        streak: 0,
        cost: 0,
        priority: 0,
    }));
    let size = (input[0].len(), input.len());
    let mut remember: RememberMap = vec![vec![vec![]; size.0]; size.1];
    while let Some(Reverse(point)) = queue.pop() {
        if point.streak >= minimum_streak
            && point.pos.y == size.1 as i32 - 1
            && point.pos.x == size.0 as i32 - 1
        {
            return point.cost;
        }

        let next_points = get_next_points(
            input,
            &mut remember,
            &point,
            size,
            minimum_streak,
            maximum_streak,
        );
        if let Some(v) = next_points {
            queue.extend(v.iter().map(|t| Reverse((*t).to_owned())));
        }
    }
    unreachable!()
}

// one optimisation, I think, is to compute only turns : an iteration is doing all possible "straight" moves, and call recursive on turns. I guess.
// I have no idea, that's the truth.
// And collecting in vec is bad, as always.
fn min_path(
    input: &[Vec<u8>],
    remember: &mut RememberMap,
    point: &PointWithMemory,
    size: (usize, usize),
    minimum_streak: u8,
    maximum_streak: u8,
) -> Option<Vec<PointWithMemory>> {
    if let Some(x) = remember[point.pos.y as usize][point.pos.x as usize]
        .iter()
        .find(|x| x.1 == point.streak && x.2 == point.last_dir)
    {
        if x.0 <= point.cost {
            return None;
        }
    }
    remember[point.pos.y as usize][point.pos.x as usize].push((
        point.cost,
        point.streak,
        point.last_dir,
    ));

    // println!("{:?}", point);
    let directions = if point.streak < minimum_streak && point.last_dir.is_some() {
        [point.last_dir, None, None, None]
    } else {
        match point.last_dir {
            Some(Dir::Up) => [Some(Dir::Left), Some(Dir::Right), Some(Dir::Up), None],
            Some(Dir::Down) => [Some(Dir::Down), Some(Dir::Left), Some(Dir::Right), None],
            Some(Dir::Right) => [Some(Dir::Up), Some(Dir::Right), Some(Dir::Down), None],
            Some(Dir::Left) => [Some(Dir::Up), Some(Dir::Left), Some(Dir::Down), None],
            None => [
                Some(Dir::Up),
                Some(Dir::Left),
                Some(Dir::Right),
                Some(Dir::Down),
            ],
        }
    };

    let nope = if point.streak == maximum_streak {
        point.last_dir
    } else {
        None
    };
    Some(
        directions
            .iter()
            .filter_map(|&d| {
                if d != nope {
                    let next_pos = match d {
                        Some(Dir::Up) => Point {
                            x: point.pos.x,
                            y: point.pos.y - 1,
                        },
                        Some(Dir::Down) => Point {
                            x: point.pos.x,
                            y: point.pos.y + 1,
                        },
                        Some(Dir::Right) => Point {
                            x: point.pos.x + 1,
                            y: point.pos.y,
                        },
                        Some(Dir::Left) => Point {
                            x: point.pos.x - 1,
                            y: point.pos.y,
                        },
                        None => Point { x: -1, y: -1 },
                    };
                    if next_pos.x < 0
                        || next_pos.x >= size.0 as i32
                        || next_pos.y < 0
                        || next_pos.y >= size.1 as i32
                    {
                        None
                    } else {
                        let cost =
                            point.cost + input[next_pos.y as usize][next_pos.x as usize] as u64;
                        Some(PointWithMemory {
                            pos: next_pos.clone(),
                            last_dir: d,
                            streak: if d == point.last_dir {
                                point.streak + 1
                            } else {
                                1
                            },
                            cost,
                            priority: cost
                                + (size.1 - next_pos.y as usize) as u64
                                + (size.0 - next_pos.x as usize) as u64,
                        })
                    }
                } else {
                    None
                }
            })
            .collect_vec(),
    )
}

pub fn solve(buffer: &str) -> SolutionPair {
    // Your solution here...
    let input = read_input(buffer);
    let start = Point { x: 0, y: 0 };
    let sol1: u64 = dijkstra_strange(&input, start.clone(), min_path, 0, 3);
    let sol2: u64 = dijkstra_strange(&input, start, min_path, 4, 10);

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day17.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 102);
    assert_eq!(s2, 94);
}

// 684, 822
