use std::{cmp::max, fs::File, io::Read};

use text_io::read;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
/// Day2, Pretty simple and had fun with implementing my own arithmetic operator before going for some simple variants.
///

#[derive(Debug, Clone, Copy)]
struct Balls {
    green: u8,
    blue: u8,
    red: u8,
}

impl Balls {
    fn create_min(&self, &other: &Balls) -> Balls {
        Balls {
            green: max(self.green, other.green),
            red: max(self.red, other.red),
            blue: max(self.blue, other.blue),
        }
    }

    fn is_greater(&self, &other: &Balls) -> bool {
        self.green > other.green || self.red > other.red || self.blue > other.blue
    }
}

/// Most of the difficulties come from here, but it's not too bad for now.
fn read_input(filename: &'static str) -> Vec<Vec<Balls>> {
    let mut buffer: String = String::new();
    File::open(filename)
        .unwrap()
        .read_to_string(&mut buffer)
        .unwrap();
    let mut res = vec![Vec::<Balls>::new(); buffer.lines().count()];
    for (i, s) in buffer.lines().enumerate() {
        s.split(": ")
            .last() // list of games not separated (without game X: )
            .expect("A line did not have any ':' in it. ")
            .split("; ") // list of draws (separated by ;)
            .for_each(|c| {
                // for each draw
                let mut b = Balls {
                    green: 0,
                    red: 0,
                    blue: 0,
                };
                c.split(", ").for_each(|word| match word.as_bytes() {
                    // For each balls (separated by , )
                    [.., b'g', b'r', b'e', b'e', b'n'] => {
                        b.green = read!("{} green", word.bytes());
                    }
                    [.., b'b', b'l', b'u', b'e'] => {
                        b.blue = read!("{} blue", word.bytes());
                    }
                    [.., b'r', b'e', b'd'] => {
                        b.red = read!("{} red", word.bytes());
                    }
                    _ => {
                        panic!("The input does not have the right format !")
                    }
                });
                res[i].push(b);
            })
    }
    res
}

pub fn solve(filename: &'static str) -> SolutionPair {
    let input = read_input(filename);

    let max_cubes = Balls {
        // Part 1 limit
        red: 12,
        green: 13,
        blue: 14,
    };
    let sol1: u64 = input
        .iter()
        .enumerate()
        .map(|(i, game)| {
            if { game.iter().find(|b| b.is_greater(&max_cubes)) }.is_some() {
                0
            } else {
                (i + 1) as u64
            }
        })
        .sum();

    let sol2: u64 = input
        .iter()
        .map(|game| {
            let res = game.iter().fold(
                Balls {
                    green: 0,
                    red: 0,
                    blue: 0,
                },
                |cur, next| cur.create_min(next),
            );
            (res.green as u64) * (res.blue as u64) * (res.red as u64)
        })
        .sum();

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) = solve("input/test_day2.txt") else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 8);
    assert_eq!(s2, 2286);
}
