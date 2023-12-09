use crate::{Solution, SolutionPair};
use itertools::Itertools;
use rustc_hash::FxHashMap;

///////////////////////////////////////////////////////////////////////////////
/// Let's squeeze more speed out of my probably-very-bad algorithm, with the help of the legendary hashmaps.
/// Input parsing this year is awful

type Point = (usize, usize);
type Number = (Point, Point, u32);

/// Day3, and already using HashMaps to get under 10 ms (in Debug mode of course). What is going on ?
fn read_symbols(buffer: &str) -> FxHashMap<Point, char> {
    buffer
        .lines()
        .enumerate()
        .flat_map(|(i, line)| {
            line.chars()
                .enumerate()
                .filter(|(_, c)| !c.is_ascii_digit() && *c != '.')
                .map(move |(j, c)| ((i, j), c))
        })
        .collect()
}

fn read_numbers(buffer: &str) -> Vec<Number> {
    let mut res: Vec<Number> = Vec::new();
    for (i, line) in buffer.lines().enumerate() {
        let mut number = 0; // Yes, I'm manually parsing the numbers. Because it is easier for the way my code is structured.
        let mut start_pos: Option<(usize, usize)> = None;
        for (j, c) in line.chars().enumerate() {
            if c.is_ascii_digit() {
                if start_pos.is_none() {
                    start_pos = Some((i, j));
                }
                number = number * 10 + c.to_digit(10).unwrap();
            }
            if number != 0 && (!c.is_ascii_digit() || j == line.len() - 1) {
                res.push((
                    start_pos.unwrap(),
                    (i, if j == line.len() - 1 { j } else { j - 1 }),
                    number,
                ));
                start_pos = None;
                number = 0;
            }
        }
    }
    res
}

fn around_the_number(number: &Number) -> impl Iterator<Item = (usize, usize)> {
    ((number.0 .0.saturating_sub(1))..(number.0 .0 + 2))
        .cartesian_product((number.0 .1.saturating_sub(1))..(number.1 .1 + 2))
}

/// Worst case : calls symbols.contains_key() (number_size * 3) + 6 -> 15 times for a 3-digits number, which is not that bad
fn is_close_to_symbol(number: &Number, symbols: &FxHashMap<Point, char>) -> Option<u64> {
    around_the_number(number)
        .any(|(x, y)| symbols.contains_key(&(x, y)))
        .then_some(number.2 as u64)
}

/// Returns all potentials gears (no matter how many numbers are around), with a single iteration over the numbers.
/// May not be really nice-looking, but efficient and working
fn get_gears(numbers: &[Number], symbols: &FxHashMap<Point, char>) -> FxHashMap<Point, (u32, u8)> {
    let mut res: FxHashMap<Point, (u32, u8)> = FxHashMap::<Point, (u32, u8)>::default();
    for nb in numbers.iter() {
        if let Some((x, y)) = {
            around_the_number(nb).find_map(|(x1, y1)| match symbols.get(&(x1, y1)) {
                // This should not work if a given number is around two gears at the same time, but it is not the case with this input.
                // Can use filter_map et for_each sinon.
                Some('*') => Some((x1, y1)),
                _ => None,
            })
        } {
            res.entry((x, y))
                .and_modify(|(ratio, quantity): &mut (u32, u8)| {
                    *ratio *= nb.2;
                    *quantity += 1
                })
                .or_insert((nb.2, 1));
        }
    }
    res
}

fn part_one(numbers: &[Number], symbols: &FxHashMap<Point, char>) -> u64 {
    numbers
        .iter()
        .filter_map(|x: &Number| is_close_to_symbol(x, symbols))
        .sum()
}

fn part_two(numbers: &[Number], symbols: &FxHashMap<Point, char>) -> u64 {
    get_gears(numbers, symbols)
        .iter()
        .filter_map(|(_, (v, nb))| if *nb == 2 { Some(*v as u64) } else { None })
        .sum()
}

pub fn solve(buffer: &str) -> SolutionPair {
    let symbols_positions = read_symbols(&buffer);
    let number_positions: Vec<Number> = read_numbers(&buffer);

    let sol1: u64 = part_one(&number_positions, &symbols_positions);
    let sol2: u64 = part_two(&number_positions, &symbols_positions);

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) = solve("input/test_day3.txt") else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 4361);
    assert_eq!(s2, 467835);
}
