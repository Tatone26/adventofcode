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

type SpringGroup<'a> = (&'a [Spring], &'a [usize]);

fn read_input(buffer: &str) -> Vec<(Vec<Spring>, Vec<usize>)> {
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

fn aux(
    springs: &[Spring],
    nums: &[usize],
    springs_index: usize,
    nums_index: usize,
    set: &mut Vec<Option<u64>>,
    first_element: Option<&Spring>,
) -> u64 {
    let h = hash(springs_index, nums_index);
    if set[h].is_some() {
        return set[h].unwrap();
    }
    match (springs.get(springs_index), nums.get(nums_index)) {
        (None, Some(_)) => 0,
        (Some(_), None) => {
            if springs[(springs_index)..]
                .iter()
                .any(|s| matches!(s, Spring::Damaged))
            {
                0
            } else {
                1
            }
        }
        (None, None) => 1,
        (_, _) => match first_element {
            Some(Spring::Operational) => aux(
                springs,
                nums,
                springs_index + 1,
                nums_index,
                set,
                springs.get(springs_index + 1),
            ),
            Some(Spring::Damaged) => {
                if springs.len() - springs_index < nums[nums_index]
                    || springs[(springs_index + 1)..(springs_index + nums[nums_index])]
                        .iter()
                        .any(|s| matches!(s, Spring::Operational))
                {
                    0
                } else if springs.len() - springs_index == nums[nums_index] {
                    if nums.len() - nums_index == 1 {
                        1
                    } else {
                        0
                    }
                } else if matches!(springs[springs_index + nums[nums_index]], Spring::Damaged) {
                    0
                } else {
                    aux(
                        springs,
                        nums,
                        springs_index + nums[nums_index] + 1,
                        nums_index + 1,
                        set,
                        springs.get(springs_index + nums[nums_index] + 1),
                    )
                }
            }
            Some(Spring::Unknown) => {
                let nb1 = aux(
                    springs,
                    nums,
                    springs_index,
                    nums_index,
                    set,
                    Some(&Spring::Damaged),
                );
                let nb2 = aux(
                    springs,
                    nums,
                    springs_index,
                    nums_index,
                    set,
                    Some(&Spring::Operational),
                );
                set[h] = Some(nb1 + nb2);
                nb1 + nb2
            }
            None => unreachable!(),
        },
    }
}

fn count_permutations(input: &[SpringGroup]) -> u64 {
    input
        .par_iter()
        .map(|&(springs, nums)| {
            let mut set: Vec<Option<u64>> = vec![None; hash(springs.len(), nums.len()) + 1];
            aux(springs, nums, 0, 0, &mut set, Some(&springs[0]))
        })
        .sum()
}

/// Faster to use an array to cache the result instead of a hashmap here, because there is not a lot of possibilities.
/// Not a lot faster though... I don't know why snif
#[inline]
fn hash(springs: usize, nums: usize) -> usize {
    (springs << 6) | nums
}

fn input_part_two(input: &[SpringGroup]) -> Vec<(Vec<Spring>, Vec<usize>)> {
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
    //let sol2: u64 = 0;
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
