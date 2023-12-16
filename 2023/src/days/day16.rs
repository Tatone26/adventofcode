use std::cmp::max;

use itertools::Itertools;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
/// Optimisation : dynamic programming with **recursion**

fn get_input(buffer: &str) -> Vec<Vec<Mirror>> {
    buffer
        .lines()
        .map(|c| {
            c.chars()
                .map(|c| match c {
                    '-' => Mirror::Horizontal,
                    '|' => Mirror::Vertical,
                    '/' => Mirror::Slash,
                    '\\' => Mirror::AntiShlash,
                    _ => Mirror::Ground,
                })
                .collect_vec()
        })
        .collect_vec()
}

#[derive(Debug)]
enum Mirror {
    Ground,
    Vertical,
    Horizontal,
    Slash,
    AntiShlash,
}

#[derive(Debug, Clone, Copy, Hash, PartialEq, Eq)]
enum Dir {
    N,
    S,
    E,
    W,
}

struct VisitedMap {
    contenu: Vec<Vec<[bool; 4]>>,
}

impl VisitedMap {
    fn from_input(input: &[Vec<Mirror>]) -> Self {
        let height = input.len();
        let width = input[0].len();
        let contenu = vec![vec![[false; 4]; width]; height];
        VisitedMap { contenu }
    }

    fn contains(&self, p: &(usize, usize, Dir)) -> bool {
        match p.2 {
            Dir::N => self.contenu[p.1][p.0][0],
            Dir::S => self.contenu[p.1][p.0][1],
            Dir::E => self.contenu[p.1][p.0][2],
            Dir::W => self.contenu[p.1][p.0][3],
        }
    }

    fn insert(&mut self, p: &(usize, usize, Dir)) {
        match p.2 {
            Dir::N => self.contenu[p.1][p.0][0] = true,
            Dir::S => self.contenu[p.1][p.0][1] = true,
            Dir::E => self.contenu[p.1][p.0][2] = true,
            Dir::W => self.contenu[p.1][p.0][3] = true,
        }
    }
}

fn propagate_laser(input: &[Vec<Mirror>], start: (i32, i32, Dir)) -> u64 {
    let mut already_visited = VisitedMap::from_input(input);
    let mut next_to_do: Vec<(i32, i32, Dir)> = vec![start];
    loop {
        let mut next = vec![];
        for (x, y, dir) in next_to_do {
            let (next_x, next_y): (i32, i32) = match dir {
                Dir::N => (x, y - 1),
                Dir::S => (x, y + 1),
                Dir::E => (x + 1, y),
                Dir::W => (x - 1, y),
            };
            if next_x >= 0
                && next_y >= 0
                && next_y < input.len() as i32
                && next_x < input[next_y as usize].len() as i32
            {
                let next_points = match (&input[next_y as usize][next_x as usize], dir) {
                    (Mirror::Horizontal, Dir::N | Dir::S) => [
                        Some((next_x, next_y, Dir::E)),
                        Some((next_x, next_y, Dir::W)),
                    ],
                    (Mirror::Vertical, Dir::E | Dir::W) => [
                        Some((next_x, next_y, Dir::N)),
                        Some((next_x, next_y, Dir::S)),
                    ],
                    (Mirror::Slash, Dir::E) | (Mirror::AntiShlash, Dir::W) => {
                        [Some((next_x, next_y, Dir::N)), None]
                    }
                    (Mirror::Slash, Dir::W) | (Mirror::AntiShlash, Dir::E) => {
                        [Some((next_x, next_y, Dir::S)), None]
                    }
                    (Mirror::Slash, Dir::N) | (Mirror::AntiShlash, Dir::S) => {
                        [Some((next_x, next_y, Dir::E)), None]
                    }
                    (Mirror::Slash, Dir::S) | (Mirror::AntiShlash, Dir::N) => {
                        [Some((next_x, next_y, Dir::W)), None]
                    }
                    _ => [Some((next_x, next_y, dir)), None], // vide
                };
                for n in next_points.into_iter().flatten() {
                    let p = (n.0 as usize, n.1 as usize, n.2);
                    if !already_visited.contains(&p) {
                        already_visited.insert(&p);
                        next.push(n);
                    }
                }
            }
        }
        if next.is_empty() {
            break;
        }
        next_to_do = next;
    }
    already_visited
        .contenu
        .iter()
        .flat_map(|v| v.iter())
        .filter(|b| b.iter().any(|c| *c))
        .count() as u64
}

fn part_two(input: &[Vec<Mirror>]) -> u64 {
    let mut max_result = 0;
    for y in 0..input.len() {
        let nb1 = propagate_laser(input, (-1, y as i32, Dir::E));
        let nb2 = propagate_laser(input, (input[y].len() as i32 + 1, y as i32, Dir::W));
        max_result = max(max_result, nb1);
        max_result = max(max_result, nb2);
    }
    for x in 0..input[0].len() {
        let nb1 = propagate_laser(input, (x as i32, -1, Dir::S));
        let nb2 = propagate_laser(input, (x as i32, input.len() as i32 + 1, Dir::N));
        max_result = max(max_result, nb1);
        max_result = max(max_result, nb2);
    }
    max_result
}

pub fn solve(buffer: &str) -> SolutionPair {
    // Your solution here...
    let input = get_input(buffer);
    let sol1: u64 = propagate_laser(&input, (-1, 0, Dir::E));
    let sol2: u64 = part_two(&input);

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day16.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 46);
    assert_eq!(s2, 51);
}
