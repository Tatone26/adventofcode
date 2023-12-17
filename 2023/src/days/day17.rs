use itertools::Itertools;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
// BRUTEFORCE 6S BOTH PARTS !! (first try for second part yepeee)
// Speed it up : better Disktra/A* priority queue.
// Don't care for now, got my finals.
// And try to parallise it if possible... But it may be difficult.

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
    N,
    S,
    E,
    W,
}

#[derive(Debug)]
struct PointWithMemory {
    pos: (i32, i32),
    last_dir: Option<Dir>,
    streak: u8,
    cost: u64,
}

fn insert_in_queue(queue: &mut Vec<PointWithMemory>, point: PointWithMemory) {
    for i in 0..queue.len() {
        if queue[i].cost < point.cost {
            queue.insert(i, point);
            return;
        }
    }
    queue.push(point);
}

type PossiblePoint = (u64, u8, Option<Dir>); // (cost, straight_line_streak, direction_to_get_here)
type RememberMap = Vec<Vec<Vec<PossiblePoint>>>;

fn min_path(input: &[Vec<u8>], start: (i32, i32)) -> u64 {
    let mut queue = vec![PointWithMemory {
        pos: start,
        last_dir: None,
        streak: 0,
        cost: 0,
    }];
    let mut remember: RememberMap = vec![vec![vec![]; input[0].len()]; input.len()];
    while let Some(point) = queue.pop() {
        if let Some(x) = remember[point.pos.1 as usize][point.pos.0 as usize]
            .iter()
            .find(|x| x.1 == point.streak && x.2 == point.last_dir)
        {
            if x.0 <= point.cost {
                continue;
            }
        }
        if point.pos.1 == input.len() as i32 - 1 && point.pos.0 == input[0].len() as i32 - 1 {
            println!("Finished !");
            return point.cost;
        }
        remember[point.pos.1 as usize][point.pos.0 as usize].push((
            point.cost,
            point.streak,
            point.last_dir,
        ));

        // println!("{:?}", point);
        let directions = match point.last_dir {
            Some(Dir::N) => [Some(Dir::W), Some(Dir::E), Some(Dir::N), None],
            Some(Dir::S) => [Some(Dir::S), Some(Dir::W), Some(Dir::E), None],
            Some(Dir::E) => [Some(Dir::N), Some(Dir::E), Some(Dir::S), None],
            Some(Dir::W) => [Some(Dir::N), Some(Dir::W), Some(Dir::S), None],
            None => [Some(Dir::N), Some(Dir::W), Some(Dir::E), Some(Dir::S)],
        };
        let nope = if point.streak == 3 {
            point.last_dir
        } else {
            None
        };
        for d in directions {
            if d != nope {
                let next_pos = match d {
                    Some(Dir::N) => (point.pos.0, point.pos.1 - 1),
                    Some(Dir::S) => (point.pos.0, point.pos.1 + 1),
                    Some(Dir::E) => (point.pos.0 + 1, point.pos.1),
                    Some(Dir::W) => (point.pos.0 - 1, point.pos.1),
                    None => (-1, -1),
                };
                if next_pos.0 < 0
                    || next_pos.0 >= input[0].len() as i32
                    || next_pos.1 < 0
                    || next_pos.1 >= input.len() as i32
                {
                    continue;
                }
                insert_in_queue(
                    &mut queue,
                    PointWithMemory {
                        pos: next_pos,
                        last_dir: d,
                        streak: if d == point.last_dir {
                            point.streak + 1
                        } else {
                            1
                        },
                        cost: point.cost + input[next_pos.1 as usize][next_pos.0 as usize] as u64,
                    },
                )
            }
        }
    }
    unreachable!()
}

fn min_path_part_2(input: &[Vec<u8>], start: (i32, i32)) -> u64 {
    let mut queue = vec![PointWithMemory {
        pos: start,
        last_dir: None,
        streak: 0,
        cost: 0,
    }];
    let mut remember: RememberMap = vec![vec![vec![]; input[0].len()]; input.len()];
    while let Some(point) = queue.pop() {
        if let Some(x) = remember[point.pos.1 as usize][point.pos.0 as usize]
            .iter()
            .find(|x| x.1 == point.streak && x.2 == point.last_dir)
        {
            if x.0 <= point.cost {
                continue;
            }
        }
        if point.pos.1 == input.len() as i32 - 1 && point.pos.0 == input[0].len() as i32 - 1 {
            if point.streak >= 4 {
                remember[point.pos.1 as usize][point.pos.0 as usize].push((
                    point.cost,
                    point.streak,
                    point.last_dir,
                ));
                println!("Finished !!");
                return point.cost;
            }
        } else {
            remember[point.pos.1 as usize][point.pos.0 as usize].push((
                point.cost,
                point.streak,
                point.last_dir,
            ));
        }

        // println!("{:?}", point);
        let directions = if point.streak < 4 && point.last_dir.is_some() {
            [point.last_dir, None, None, None]
        } else {
            match point.last_dir {
                Some(Dir::N) => [Some(Dir::W), Some(Dir::E), Some(Dir::N), None],
                Some(Dir::S) => [Some(Dir::S), Some(Dir::W), Some(Dir::E), None],
                Some(Dir::E) => [Some(Dir::N), Some(Dir::E), Some(Dir::S), None],
                Some(Dir::W) => [Some(Dir::N), Some(Dir::W), Some(Dir::S), None],
                None => [Some(Dir::N), Some(Dir::W), Some(Dir::E), Some(Dir::S)],
            }
        };
        let nope = if point.streak == 10 {
            point.last_dir
        } else {
            None
        };
        for d in directions {
            if d != nope {
                let next_pos = match d {
                    Some(Dir::N) => (point.pos.0, point.pos.1 - 1),
                    Some(Dir::S) => (point.pos.0, point.pos.1 + 1),
                    Some(Dir::E) => (point.pos.0 + 1, point.pos.1),
                    Some(Dir::W) => (point.pos.0 - 1, point.pos.1),
                    None => (-1, -1),
                };
                if next_pos.0 < 0
                    || next_pos.0 >= input[0].len() as i32
                    || next_pos.1 < 0
                    || next_pos.1 >= input.len() as i32
                {
                    continue;
                }
                insert_in_queue(
                    &mut queue,
                    PointWithMemory {
                        pos: next_pos,
                        last_dir: d,
                        streak: if d == point.last_dir {
                            point.streak + 1
                        } else {
                            1
                        },
                        cost: point.cost + input[next_pos.1 as usize][next_pos.0 as usize] as u64,
                    },
                )
            }
        }
    }
    unreachable!()
}

pub fn solve(buffer: &str) -> SolutionPair {
    // Your solution here...
    let input = read_input(buffer);
    let sol1: u64 = min_path(&input, (0, 0));
    let sol2: u64 = min_path_part_2(&input, (0, 0));

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
