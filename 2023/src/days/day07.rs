use crate::{Solution, SolutionPair};

use itertools::Itertools;

///////////////////////////////////////////////////////////////////////////////

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord)]
struct NewHand {
    hand_type: NewHandType,
    bid: u64,
}

#[derive(Debug, PartialEq, PartialOrd, Eq, Ord)]
enum NewHandType {
    High(u32),
    OnePair(u32),
    TwoPairs(u32),
    Brelan(u32),
    Full(u32),
    Four(u32),
    Five(u32),
}

fn power_of_card(card: char, joker: char) -> u8 {
    if card == joker {
        return 0;
    }
    (match card {
        'A' => 14,
        'K' => 13,
        'Q' => 12,
        'J' => 11,
        'T' => 10,
        '9' => 9,
        '8' => 8,
        '7' => 7,
        '6' => 6,
        '5' => 5,
        '4' => 4,
        '3' => 3,
        '2' => 2,
        _ => 1,
    }) - 1
}

/// Algorithm courtesy of u/Afkadrian, would not have think of it otherwise. https://github.com/adriandelgado/advent-of-code/blob/main/src/y2023/d07.rs
/// Could not bear to do it any other way, this really is the best way for me, with my architecture.
/// My real solution would be to match all cases, not just (4 + max_count - unique_numbers) :
/// match (max_count, unique_numbers) { (1, 5) => One, (5, 1) => Five, etc. }
/// Seeing this solution really is impressive !
fn get_hand(hand: &str, joker: bool) -> NewHandType {
    let mut counter = [0; 14];
    let mut score = 0;
    for c in hand.chars() {
        let power = power_of_card(c, if joker { 'J' } else { '_' });
        counter[power as usize] += 1;
        score = score * 13 + power as u32;
    }
    let [joker_count, count @ ..] = counter;
    let unique_numbers = count.iter().filter(|x| **x != 0).count();
    let max_count = count.iter().max().unwrap() + if joker { joker_count } else { 0 };
    match 4 + max_count - unique_numbers {
        0 => NewHandType::High(score),
        2 => NewHandType::OnePair(score),
        3 => NewHandType::TwoPairs(score),
        4 => NewHandType::Brelan(score),
        5 => NewHandType::Full(score),
        6 => NewHandType::Four(score),
        _ => NewHandType::Five(score),
    }
}

fn get_input(buffer: &str, joker: bool) -> Vec<NewHand> {
    buffer
        .lines()
        .filter(|line| !line.is_empty())
        .map(|line| {
            let mut it = line.split(' ');
            let cards_array = it.next().unwrap();
            let bid_number = it.next().unwrap().parse().unwrap();
            NewHand {
                hand_type: get_hand(cards_array, joker),
                bid: bid_number,
            }
        })
        .collect_vec()
}

pub fn solve(buffer: &str) -> SolutionPair {
    let mut input = get_input(buffer, false);
    input.sort();
    let sol1: u64 = input
        .iter()
        .enumerate()
        .map(|(i, hand)| (i + 1) as u64 * hand.bid)
        .sum();

    let mut input_2 = get_input(buffer, true);
    input_2.sort();
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
