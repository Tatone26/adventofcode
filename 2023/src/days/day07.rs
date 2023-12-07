use std::{fs::File, io::Read};

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////

#[derive(Debug, PartialEq, PartialOrd, Eq, Ord)]
struct Hand {
    card_type: HandType,
    cards_numbers: Vec<u8>,
    cards: String,
    bid: u64,
}

/* #[derive(Debug, PartialEq, Eq, PartialOrd, Ord)]
enum HandType {
    Five,
    Four,
    Full,
    Brelan,
    TwoPairs,
    OnePair,
    High,
} */

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord)]
enum HandType {
    High,
    OnePair,
    TwoPairs,
    Brelan,
    Full,
    Four,
    Five,
}

fn card_to_power(card: char) -> u8 {
    match card {
        'A' => 13,
        'K' => 12,
        'Q' => 11,
        'J' => 10,
        'T' => 9,
        '9' => 8,
        '8' => 7,
        '7' => 6,
        '6' => 5,
        '5' => 4,
        '4' => 3,
        '3' => 2,
        '2' => 1,
        _ => 0,
    }
}

fn card_to_power2(card: char) -> u8 {
    match card {
        'A' => 13,
        'K' => 12,
        'Q' => 11,
        'T' => 10,
        '9' => 9,
        '8' => 8,
        '7' => 7,
        '6' => 6,
        '5' => 5,
        '4' => 4,
        '3' => 3,
        '2' => 2,
        'J' => 1,
        _ => 0,
    }
}

fn get_cards_type(cards: &[u8]) -> HandType {
    let mut count: Vec<(char, u8)> = Vec::new();
    for c in cards.iter() {
        if let Some((_, y)) = count.iter_mut().find(|(d, _)| *d == (*c as char)) {
            *y += 1;
        } else {
            count.push((*c as char, 1));
        }
    }
    match count.len() {
        1 => HandType::Five,
        2 => {
            if count.iter().any(|(_, i)| *i == 4) {
                HandType::Four
            } else {
                HandType::Full
            }
        }
        3 => {
            if count.iter().any(|(_, i)| *i == 3) {
                HandType::Brelan
            } else {
                HandType::TwoPairs
            }
        }
        4 => HandType::OnePair,
        5 => HandType::High,
        _ => panic!("nooo"),
    }
}

fn get_better_card_type(cards: &[u8]) -> HandType {
    if !cards.contains(&b'J') {
        return get_cards_type(cards);
    }
    let mut count: Vec<(char, u8)> = Vec::new();
    let mut count_joker: u8 = 0;
    for c in cards.iter() {
        if *c == b'J' {
            count_joker += 1;
            continue;
        }
        if let Some((_, y)) = count.iter_mut().find(|(d, _)| *d == (*c as char)) {
            *y += 1;
        } else {
            count.push((*c as char, 1));
        }
    }
    match count.iter().max_by(|(_, i), (_, j)| i.cmp(j)) {
        Some((c, x)) => match *x + count_joker {
            5 => HandType::Five,
            4 => HandType::Four,
            3 => {
                if count
                    .iter()
                    .filter(|(a, _)| *a != *c)
                    .max_by(|(_, i), (_, j)| i.cmp(j))
                    .map(|(_, i)| *i)
                    == Some(2)
                {
                    HandType::Full
                } else {
                    HandType::Brelan
                }
            }
            2 => HandType::OnePair,
            _ => HandType::High,
        },
        None => HandType::Five, // 5 jokers !
    }
}

fn get_input(filename: &str) -> Vec<Hand> {
    let mut buffer: String = String::new();
    File::open(filename)
        .unwrap_or_else(|_| panic!("No {filename} file."))
        .read_to_string(&mut buffer)
        .unwrap_or_else(|_| panic!("Error reading {filename} as file."));

    let mut r = Vec::new();
    buffer.lines().for_each(|line| {
        let mut it = line.split(' ');
        let cards_array = it.next().unwrap();
        let bid_number = it.next().unwrap().parse().unwrap();
        r.push(Hand {
            cards: cards_array.to_owned(),
            cards_numbers: {
                (0..5)
                    .map(|i| card_to_power(cards_array.chars().nth(i).unwrap()))
                    .collect()
            },
            bid: bid_number,
            card_type: get_cards_type(cards_array.as_bytes()),
        })
    });
    r
}

fn get_input_2(filename: &str) -> Vec<Hand> {
    let mut buffer: String = String::new();
    File::open(filename)
        .unwrap_or_else(|_| panic!("No {filename} file."))
        .read_to_string(&mut buffer)
        .unwrap_or_else(|_| panic!("Error reading {filename} as file."));

    let mut r = Vec::new();
    buffer.lines().for_each(|line| {
        let mut it = line.split(' ');
        let cards_array = it.next().unwrap();
        let bid_number = it.next().unwrap().parse().unwrap();
        r.push(Hand {
            cards: cards_array.to_owned(),
            cards_numbers: {
                (0..5)
                    .map(|i| card_to_power2(cards_array.chars().nth(i).unwrap()))
                    .collect()
            },
            bid: bid_number,
            card_type: get_better_card_type(cards_array.as_bytes()),
        })
    });
    r
}

pub fn solve(filename: &'static str) -> SolutionPair {
    let mut input = get_input(filename);
    input.sort();
    println!("{input:?}");
    let sol1: u64 = input
        .iter()
        .enumerate()
        .map(|(i, hand)| (i + 1) as u64 * hand.bid)
        .sum();
    let mut input_2 = get_input_2(filename);
    input_2.sort();
    println!("{input_2:?}");
    let sol2: u64 = input_2
        .iter()
        .enumerate()
        .map(|(i, hand)| (i + 1) as u64 * hand.bid)
        .sum();

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) = solve("input/test_day7.txt") else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 6440);
    assert_eq!(s2, 5905);
}
