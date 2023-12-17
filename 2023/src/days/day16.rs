use crate::{Solution, SolutionPair};
use itertools::Itertools;
use rayon::prelude::*;

///////////////////////////////////////////////////////////////////////////////
// Optimisation : dynamic programming -> need to return an entire map of the point visited from a given point and direction... I really may not be useful.
// (All of that because multiple lasers go onto the same one.)

/// reads input
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
    N = 0,
    S = 1,
    E = 2,
    W = 3,
}

struct VisitedMap<T> {
    contenu: Vec<Vec<[T; 4]>>,
}

impl<T: Clone + Copy> VisitedMap<T> {
    fn from_input(input: &[Vec<Mirror>], default: T) -> Self {
        let height = input.len();
        let width = input[0].len();
        let contenu = vec![vec![[default; 4]; width]; height];
        VisitedMap { contenu }
    }

    #[inline]
    fn contains(&self, p: &(usize, usize, Dir)) -> T {
        self.contenu[p.1][p.0][p.2 as usize]
    }

    #[inline]
    fn insert(&mut self, p: &(usize, usize, Dir), value: T) {
        self.contenu[p.1][p.0][p.2 as usize] = value;
    }
}

fn propagate_laser(input: &[Vec<Mirror>], start: (i32, i32, Dir)) -> u64 {
    let mut already_visited = VisitedMap::<bool>::from_input(input, false);
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
                    _ => [Some((next_x, next_y, dir)), None], // ground or parrallel with rayon
                };
                for n in next_points.into_iter().flatten() {
                    let p = (n.0 as usize, n.1 as usize, n.2);
                    if !already_visited.contains(&p) {
                        already_visited.insert(&p, true);
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

/// one optimisation to do : get the position of where the lasers go out, can't do better from there.
fn part_two(input: &[Vec<Mirror>]) -> u64 {
    (0..input.len())
        .flat_map(|y| {
            [
                (-1, y as i32, Dir::E),
                (input[y].len() as i32 + 1, y as i32, Dir::W),
            ]
        })
        .chain((0..input[0].len()).flat_map(|x| {
            [
                (x as i32, -1, Dir::S),
                (x as i32, input.len() as i32 + 1, Dir::N),
            ]
        }))
        .par_bridge() // holy cow that's some speed improvement here
        .map(|start| propagate_laser(input, start))
        .max()
        .unwrap()
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
