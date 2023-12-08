use itertools::Itertools;

use crate::{Solution, SolutionPair};
use std::fs::{self};

///////////////////////////////////////////////////////////////////////////////

/// Represents the number of wins of the game, computed when read because don't care about anything else.
type Game = usize;

/// Again, all the difficulty is in the parsing, but very proud of this one.
fn read_games(buf: &str) -> Vec<Game> {
    buf.lines()
        .filter_map(|s| {
            s.split(':')
                .last()
                .expect("Input file not corresponding to day4.")
                .split('|')
                .map(|p| {
                    p.split_whitespace()
                        .map(|c| c.parse().unwrap())
                        .collect::<Vec<u8>>()
                })
                .collect_tuple()
                .map(|(win, car)| win.iter().filter(|i| car.contains(*i)).count())
        })
        // With all that, I ignore every line not well-formated (maybe)
        .collect()
}

fn score_game(game: &Game) -> u64 {
    match *game {
        0 => 0,
        p => 2_u64.pow((p - 1) as u32),
    }
}

fn part_two(games: &[Game]) -> u64 {
    let mut instances: Vec<u64> = vec![1; games.len()];
    for (u, g) in games.iter().enumerate() {
        for i in (u + 1)..(u + g + 1) {
            instances[i] += instances[u]; // Si j'ai ramassé 2 cartes X depuis le début, j'aurais X nouvelles cartes.
        }
    }
    instances.iter().sum()
}

pub fn solve(filename: &'static str) -> SolutionPair {
    let buffer: String = fs::read_to_string(filename).unwrap_or_default();

    let games = read_games(&buffer);

    let sol1: u64 = games.iter().map(score_game).sum();
    let sol2: u64 = part_two(&games);

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) = solve("input/test_day4.txt") else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 13);
    assert_eq!(s2, 30);
}
