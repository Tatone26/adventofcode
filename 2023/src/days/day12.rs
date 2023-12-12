use core::fmt;

use itertools::Itertools;

use rayon::prelude::*;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////

#[derive(Clone, Copy)]
enum Spring {
    Operational,
    Damaged,
    Unknown,
}

impl Spring {
    #[inline]
    fn from_char(c: char) -> Self {
        match c {
            '.' => Self::Operational,
            '#' => Self::Damaged,
            _ => Self::Unknown,
        }
    }
}

impl fmt::Debug for Spring {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            Spring::Operational => write!(f, "."),
            Spring::Damaged => write!(f, "#"),
            Spring::Unknown => write!(f, "?"),
        }
    }
}

type SpringGroup<'a> = (&'a [Spring], &'a [u16]);

fn read_input(buffer: &str) -> Vec<(Vec<Spring>, Vec<u16>)> {
    buffer
        .lines()
        .map(|line| {
            let mut it = line.split(' ');
            (
                it.next()
                    .unwrap()
                    .chars()
                    .map(Spring::from_char)
                    .collect_vec(),
                it.next()
                    .unwrap()
                    .split(',')
                    .map(|c| c.parse().unwrap())
                    .collect_vec(),
            )
        })
        .collect_vec()
}

// Thank god caching was NOT a problem. And thank you reddit. And I was *this* close to using OCaml and the very goods list pattern matching it provides.
fn find_number_of_goods(group: SpringGroup, set: &mut [Option<u64>], index: usize) -> u64 {
    let h = hash(index, group.1.len());
    if set[h].is_some() {
        return set[h].unwrap();
    }
    match group {
        ([], [_, ..]) => 0,
        (t, []) => {
            if t.iter().any(|s| matches!(s, Spring::Damaged)) {
                0
            } else {
                1
            }
        }
        ([Spring::Damaged, t @ ..], [a, tail @ ..]) => {
            if t.len() < *a as usize - 1
                || t[0..(*a as usize - 1)]
                    .iter()
                    .any(|s| matches!(s, Spring::Operational))
            {
                0
            } else if t.len() == *a as usize - 1 {
                if tail.is_empty() {
                    1
                } else {
                    0
                }
            } else if matches!(t[*a as usize - 1], Spring::Damaged) {
                0
            } else {
                find_number_of_goods((&t[(*a as usize)..], tail), set, index + *a as usize)
            }
        }
        ([Spring::Operational, t @ ..], tail) => {
            let i = t
                .iter()
                .take_while(|a| matches!(a, Spring::Operational))
                .count();
            find_number_of_goods((&t[i..], tail), set, index + 1 + i)
        }
        ([Spring::Unknown, t @ ..], tail) => {
            let mut new_slice = t.to_vec();
            new_slice.insert(0, Spring::Damaged);
            let nb = find_number_of_goods((&new_slice, tail), set, index);
            new_slice[0] = Spring::Operational;
            let nb2 = find_number_of_goods((&new_slice, tail), set, index);
            set[h] = Some(nb + nb2);
            nb + nb2
        }
    }
}

fn count_permutations(input: &[SpringGroup]) -> u64 {
    input
        .par_iter()
        .map(|&(springs, nums)| {
            let mut set: Vec<Option<u64>> = vec![None; hash(springs.len(), nums.len()) + 1];
            find_number_of_goods((springs, nums), &mut set, 0)
        })
        .sum()
}

/// Faster to use an array to cache the result instead of a hashmap here, because there is not a lot of possibilities.
/// Not a lot faster though... I don't know why snif
#[inline]
fn hash(springs: usize, nums: usize) -> usize {
    (springs << 6) | nums
}

fn input_part_two(input: &[SpringGroup]) -> Vec<(Vec<Spring>, Vec<u16>)> {
    input
        .iter()
        .map(|(springs, nums)| {
            let mut new_vec_1 = springs.to_vec();
            new_vec_1.push(Spring::Unknown);
            new_vec_1 = new_vec_1.repeat(5);
            new_vec_1.pop();
            let mut new_vec_2 = nums.to_vec();
            new_vec_2 = new_vec_2.repeat(5);
            (new_vec_1, new_vec_2)
        })
        .collect_vec()
}

pub fn solve(buffer: &str) -> SolutionPair {
    let input = read_input(buffer);
    let input_modif = input
        .iter()
        .map(|(s, u)| (s.as_slice(), u.as_slice()))
        .collect_vec();

    let sol1: u64 = count_permutations(&input_modif);
    let input_2 = input_part_two(&input_modif);
    let input_2_modif = input_2
        .iter()
        .map(|(s, u)| (s.as_slice(), u.as_slice()))
        .collect_vec();

    let sol2: u64 = count_permutations(&input_2_modif);
    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day12.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 21);
    assert_eq!(s2, 525152);
}
