use std::hash::Hash;

use itertools::Itertools;
use rayon::prelude::*;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////

#[derive(Debug, PartialEq, Eq, Clone, Copy)]
struct Brick {
    name: u64,
    x: (u64, u64),
    y: (u64, u64),
    z: (u64, u64),
}

type BrickList = Vec<Brick>;

impl Brick {
    fn from_str(buffer: &str) -> Self {
        let (x, y, z, a, b, c) = buffer
            .split([',', '~'])
            .map(|c| c.parse().unwrap())
            .collect_tuple()
            .unwrap();
        Self {
            name: 0,
            x: (x, a),
            y: (y, b),
            z: (z, c),
        }
    }

    fn is_directly_under(&self, other: &Self) -> bool {
        self.z.1 < other.z.0 && intersect(self.x, other.x) && intersect(self.y, other.y)
    }

    /// list is supposed sorted, smallest first, and all under self.
    fn find_blockade(&self, list: &BrickList) -> u64 {
        if let Some(r) = list.iter().rev().find(|b| b.is_directly_under(self)) {
            r.z.1
        } else {
            0
        }
    }
}

impl Ord for Brick {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.z.1.cmp(&other.z.1)
    }
}

impl PartialOrd for Brick {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}

impl Hash for Brick {
    fn hash<H: std::hash::Hasher>(&self, state: &mut H) {
        self.name.hash(state);
    }
}

fn intersect(a: (u64, u64), b: (u64, u64)) -> bool {
    !(b.0 > a.1 || a.0 > b.1)
}

fn read_input(buffer: &str) -> BrickList {
    buffer
        .lines()
        .enumerate()
        .map(|(i, line)| {
            let mut b = Brick::from_str(line);
            b.name = i as u64;
            b
        })
        .sorted()
        .collect()
}

/// Need a sorted input.
fn make_input_fall(input: &BrickList, ignoring: Option<u64>) -> (BrickList, u64) {
    let mut new = vec![];
    let mut has_fallen = 0;
    for b in input.iter() {
        if ignoring.is_some() && b.name == ignoring.unwrap() {
            continue;
        }
        let down = b.find_blockade(&new);
        if down + 1 != b.z.0 {
            has_fallen += 1;
        }
        let new_brick = Brick {
            name: b.name,
            x: b.x,
            y: b.y,
            z: (down + 1, down + b.z.1 - b.z.0 + 1),
        };
        // that way, new stays sorted, which is the condition for my algo.
        let pos = new.binary_search(&new_brick).unwrap_or_else(|e| e);
        new.insert(pos, new_brick);
    }
    (new, has_fallen)
}

fn test_everything(input: &mut BrickList) -> (u64, u64) {
    (*input, _) = make_input_fall(input, None);
    input
        .par_iter()
        .map(|b| {
            let r = make_input_fall(input, Some(b.name)).1;
            if r == 0 {
                (1_u64, 0)
            } else {
                (0, r)
            }
        })
        .reduce(|| (0, 0), |acc, elem| (acc.0 + elem.0, acc.1 + elem.1))
}

pub fn solve(buffer: &str) -> SolutionPair {
    // Your solution here...
    let mut input = read_input(buffer);
    /*  let sol1: u64 = part_one(&mut input);
    let sol2: u64 = part_two(&input); */
    let (sol1, sol2) = test_everything(&mut input);

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day22.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 5);
    assert_eq!(s2, 7);
}

// 428 - 35654
