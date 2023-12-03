use std::{fs::File, io::Read};

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
/// Edge case : number on the right border.
///

fn read_symbols(filename: &'static str) -> Vec<((u32, u32), char)> {
    let mut buffer: String = String::new();
    File::open(filename)
        .unwrap()
        .read_to_string(&mut buffer)
        .unwrap();
    let mut positions = Vec::new();
    buffer.lines().enumerate().for_each(|(i, line)| {
        line.chars().enumerate().for_each(|(j, c)| {
            if !c.is_ascii_digit() && c != '.' {
                positions.push(((i as u32, j as u32), c))
            }
        })
    });
    positions
}

fn read_numbers(filename: &'static str) -> Vec<((u32, u32), (u32, u32), u32)> {
    let mut buffer: String = String::new();
    File::open(filename)
        .unwrap()
        .read_to_string(&mut buffer)
        .unwrap();
    let mut positions = Vec::new();
    buffer.lines().enumerate().for_each(|(i, line)| {
        let mut number = 0;
        let mut start_pos: (u32, u32) = (0, 0);
        line.chars().enumerate().for_each(|(j, c)| {
            if c.is_ascii_digit() {
                if number == 0 {
                    start_pos = (i as u32, j as u32);
                }
                number = number * 10 + c.to_digit(10).unwrap();
            }
            if (!c.is_ascii_digit() || j == line.len() - 1) && number > 0 {
                positions.push((
                    start_pos,
                    (
                        i as u32,
                        if j == line.len() - 1 {
                            j as u32
                        } else {
                            (j - 1) as u32
                        },
                    ),
                    number,
                ));
                start_pos = (0, 0);
                number = 0;
            }
        })
    });
    positions
}

fn is_close_to_symbol(
    pos1: &(u32, u32),
    pos2: &(u32, u32),
    positions: &Vec<((u32, u32), char)>,
) -> bool {
    positions
        .iter()
        .find(|((i, j), _)| {
            (i.abs_diff(pos1.0) <= 1) && (pos1.1..(pos2.1 + 1)).any(|y| j.abs_diff(y) <= 1)
        })
        .is_some()
}

fn part_one(
    numbers: &Vec<((u32, u32), (u32, u32), u32)>,
    symbols: &Vec<((u32, u32), char)>,
) -> u64 {
    numbers
        .iter()
        .filter_map(|(pos1, pos2, value)| {
            if is_close_to_symbol(pos1, pos2, symbols) {
                Some(*value as u64)
            } else {
                None
            }
        })
        .sum()
}

fn is_gear(pos: (u32, u32), numbers: &Vec<((u32, u32), (u32, u32), u32)>) -> Option<u64> {
    let mut count = 0;
    let it = numbers.iter().filter_map(|(pos1, pos2, value)| {
        if count > 2 {
            return None;
        } else if pos.0.abs_diff(pos1.0) > 1 {
            None
        } else if (pos1.1..(pos2.1 + 1)).any(|y| pos.1.abs_diff(y) <= 1) {
            count += 1;
            Some(*value as u64)
        } else {
            None
        }
    });
    let r = it.reduce(|acc: u64, elem| acc * elem);
    if count != 2 {
        None
    } else {
        r
    }
}

fn part_two(
    numbers: &Vec<((u32, u32), (u32, u32), u32)>,
    symbols: &Vec<((u32, u32), char)>,
) -> u64 {
    symbols
        .iter()
        .filter_map(|(pos, sym)| {
            if *sym != '*' {
                None
            } else {
                is_gear(*pos, numbers)
            }
        })
        .sum()
}

pub fn solve(filename: &'static str) -> SolutionPair {
    let symbols_positions = read_symbols(filename);
    let number_positions = read_numbers(filename);

    let sol1: u64 = part_one(&number_positions, &symbols_positions);
    let sol2: u64 = part_two(&number_positions, &symbols_positions);
    /* let sol1 = 0;
    let sol2 = 0; */
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
