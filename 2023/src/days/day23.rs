use std::{cmp::max, collections::BinaryHeap};

use itertools::Itertools;
use rustc_hash::FxHashSet;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
/// For part 2, Need to reduce the input to a graph ("easily" donc with next_intersection)
/// And find the longest path in this graph.
/// Right now it computes it every single time. Which is stupid as hell. Maybe some simple memoization can work here at first...
/// but i don't think so.
/// Now holidays !! will make it better another day.

enum Tile {
    Wall,
    Ground,
    RightSlope,
    LeftSlope,
    DownSlope,
    UpSlope,
}

#[derive(Debug, PartialEq, Eq, Clone, Copy)]
enum Direction {
    Up,
    Down,
    Left,
    Right,
}

impl Direction {
    fn reverse(&self) -> Self {
        match self {
            Direction::Up => Direction::Down,
            Direction::Down => Direction::Up,
            Direction::Left => Direction::Right,
            Direction::Right => Direction::Left,
        }
    }
}

impl core::fmt::Debug for Tile {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::Wall => write!(f, "#"),
            Self::Ground => write!(f, "."),
            Self::RightSlope => write!(f, ">"),
            Self::LeftSlope => write!(f, "<"),
            Self::DownSlope => write!(f, "v"),
            Self::UpSlope => write!(f, "^"),
        }
    }
}

type Map = Vec<Vec<Tile>>;

fn read_input(buffer: &str) -> Map {
    buffer
        .lines()
        .map(|line| {
            line.chars()
                .map(|c| match c {
                    '>' => Tile::RightSlope,
                    '<' => Tile::LeftSlope,
                    '^' => Tile::UpSlope,
                    'v' => Tile::DownSlope,
                    '.' => Tile::Ground,
                    _ => Tile::Wall,
                })
                .collect_vec()
        })
        .collect_vec()
}

#[derive(Debug, PartialEq, Eq)]
struct Hike {
    current_pos: (usize, usize),
    already_visited_intersection: FxHashSet<(usize, usize)>,
    number_of_steps: u64,
    last_direction: Direction,
}

impl Hike {
    fn new(pos: (usize, usize), direction: Direction) -> Self {
        Self {
            current_pos: pos,
            already_visited_intersection: FxHashSet::default(),
            number_of_steps: 0,
            last_direction: direction,
        }
    }
}

impl Ord for Hike {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.number_of_steps.cmp(&other.number_of_steps)
    }
}

impl PartialOrd for Hike {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}

fn find_next_intersection(
    map: &Map,
    hike: &Hike,
    stop_at_y: usize,
) -> ((usize, usize), u64, Direction) {
    let mut pos = hike.current_pos;
    let mut number_of_steps = 0;
    let mut last_direction = hike.last_direction;
    const DIRECTIONS: [Direction; 4] = [
        Direction::Down,
        Direction::Up,
        Direction::Left,
        Direction::Right,
    ];
    loop {
        let mut already_found_one = false;
        let mut next_pos = pos;
        let mut next_direction = None;
        for d in DIRECTIONS
            .iter()
            .filter(|&p| *p != last_direction.reverse())
            .filter(|&p| {
                if pos.1 == 0 {
                    !matches!(p, Direction::Up)
                } else {
                    true
                }
            })
        {
            let temp = match d {
                Direction::Up => (pos.0, pos.1 - 1),
                Direction::Down => (pos.0, pos.1 + 1),
                Direction::Left => (pos.0 - 1, pos.1),
                Direction::Right => (pos.0 + 1, pos.1),
            };
            if matches!(map[temp.1][temp.0], Tile::Wall) {
                continue;
            }

            if already_found_one {
                return (pos, number_of_steps, last_direction);
            }
            already_found_one = true;
            next_direction = Some(*d);
            next_pos = temp;
        }
        assert!(already_found_one);
        number_of_steps += 1;
        pos = next_pos;
        last_direction = next_direction.unwrap();
        if pos.1 == stop_at_y {
            return (pos, number_of_steps, last_direction);
        }
    }
}

fn can_go_there(map: &Map, pos: (usize, usize), last_pos: (usize, usize), strong: bool) -> bool {
    if strong {
        !matches!(map[pos.1][pos.0], Tile::Wall)
    } else {
        match map[pos.1][pos.0] {
            Tile::Wall => false,
            Tile::Ground => true,
            Tile::DownSlope => pos.1 > last_pos.1,
            Tile::LeftSlope => pos.0 < last_pos.0,
            Tile::RightSlope => pos.0 > last_pos.0,
            Tile::UpSlope => pos.1 < last_pos.1,
        }
    }
}

/// lol
fn find_longest_path(map: &Map, strong: bool) -> u64 {
    let start: (usize, usize) = (
        map.first()
            .unwrap()
            .iter()
            .position(|p| !matches!(p, Tile::Wall))
            .unwrap(),
        0,
    );
    let the_end = map.len() - 1;
    let mut to_try: BinaryHeap<Hike> = BinaryHeap::default();
    to_try.push(Hike::new(start, Direction::Down));
    const DIRECTIONS: [Direction; 4] = [
        Direction::Down,
        Direction::Up,
        Direction::Left,
        Direction::Right,
    ];
    let mut result = 0;
    while let Some(hike) = to_try.pop() {
        // println!("hike : {hike:?}");
        let (intersect_pos, steps, dir) = find_next_intersection(map, &hike, the_end);
        if intersect_pos.1 == the_end {
            result = max(result, hike.number_of_steps + steps);
            println!("{result}");
            continue;
        }
        if hike.already_visited_intersection.contains(&intersect_pos) {
            continue;
        }
        for d in DIRECTIONS
            .iter()
            .filter(|&p| *p != dir.reverse())
            .filter(|&p| {
                if hike.current_pos.1 == 0 {
                    !matches!(p, Direction::Up)
                } else {
                    true
                }
            })
        {
            let next_pos = match d {
                Direction::Up => (intersect_pos.0, intersect_pos.1 - 1),
                Direction::Down => (intersect_pos.0, intersect_pos.1 + 1),
                Direction::Left => (intersect_pos.0 - 1, intersect_pos.1),
                Direction::Right => (intersect_pos.0 + 1, intersect_pos.1),
            };
            if can_go_there(map, next_pos, intersect_pos, strong) {
                let mut new_hike = Hike::new(next_pos, *d);
                new_hike.number_of_steps = hike.number_of_steps + steps + 1;
                new_hike.already_visited_intersection = hike.already_visited_intersection.clone();
                new_hike.already_visited_intersection.insert(intersect_pos);
                to_try.push(new_hike);
            }
        }
    }
    result
}

pub fn solve(buffer: &str) -> SolutionPair {
    let input = read_input(buffer);
    println!("{input:?}");
    let sol1: u64 = find_longest_path(&input, false);
    let sol2: u64 = find_longest_path(&input, true);

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day23.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 94);
    assert_eq!(s2, 154);
}

// 6654 !!
