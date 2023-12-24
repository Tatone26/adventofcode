use itertools::Itertools;
use num::Zero;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////

#[derive(Debug)]
struct Point {
    x: f64,
    y: f64,
    z: f64,
}

#[derive(Debug)]
struct Hail {
    position: Point,
    velocity: Point,
}

impl Hail {
    fn will_collide_with_2d(&self, other: &Self) -> Option<Point> {
        let det = self.velocity.x * other.velocity.y - self.velocity.y * other.velocity.x;
        if det.is_zero() {
            return None;
        }
        let t = ((other.position.x - self.position.x) * other.velocity.y
            - (other.position.y - self.position.y) * other.velocity.x) as f64
            / det as f64;
        /* let s = ((other.position.x - self.position.x) * self.velocity.y
        - (other.position.y - self.position.y) * self.velocity.x) as f64
        / det as f64; */
        Some(Point {
            x: self.position.x + t * self.velocity.x,
            y: self.position.y + t * self.velocity.y,
            z: 0.0,
        })
    }
}

fn read_input(buffer: &str) -> Vec<Hail> {
    buffer
        .lines()
        .filter(|line| !line.is_empty())
        .map(|line| {
            let (a, b, c, d, e, f) = line
                .split([',', ' ', '@'])
                .filter(|p| !p.is_empty())
                .map(|p| p.parse::<f64>().unwrap())
                .collect_tuple()
                .unwrap();
            Hail {
                position: Point { x: a, y: b, z: c },
                velocity: Point { x: d, y: e, z: f },
            }
        })
        .collect_vec()
}

fn is_in(p: &Point, interval: (f64, f64)) -> bool {
    interval.0 <= p.x && p.x <= interval.1 && interval.0 <= p.y && p.y <= interval.1
}

fn is_in_future(p: &Point, hail: &Hail) -> bool {
    if hail.velocity.x > 0.0 {
        if p.x > hail.position.x {
            return true;
        }
    } else if hail.velocity.y > 0.0 {
        if p.y > hail.position.y {
            return true;
        }
    } else if hail.velocity.x < 0.0 {
        if p.x < hail.position.x {
            return true;
        }
    } else if hail.velocity.y < 0.0 {
        if p.y < hail.position.y {
            return true;
        }
    }
    false
}

fn part_one(input: &[Hail], range: (f64, f64)) -> u64 {
    input
        .iter()
        .tuple_combinations()
        .filter(|(a, b)| {
            let inter_pos = a.will_collide_with_2d(b);
            inter_pos.is_some()
                && is_in(inter_pos.as_ref().unwrap(), range)
                && is_in_future(inter_pos.as_ref().unwrap(), *a)
                && is_in_future(inter_pos.as_ref().unwrap(), *b)
        })
        /* .map(|p| {
            println!(
                "{:?} will collide with {:?} inside range.",
                p.0.position, p.1.position
            )
        }) */
        .count() as u64
}

fn part_two(_input: &[Hail]) -> u64 {
    0
}

pub fn solve(buffer: &str) -> SolutionPair {
    let input = read_input(buffer);
    println!("{input:?}");
    let sol1: u64 = part_one(&input, (200000000000000.0, 400000000000000.0));
    let sol2: u64 = part_two(&input);

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let buffer = std::fs::read_to_string("input/test_day24.txt").unwrap();
    let input = read_input(&buffer);
    /* let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day24.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    }; */
    println!("{input:?}");
    let s1 = part_one(&input, (7.0, 27.0));
    let s2 = part_two(&input);
    assert_eq!(s1, 2);
    assert_eq!(s2, 47);
}

// pour test : position 24, 13, 10 avec velocité -3, 1, 2
