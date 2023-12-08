use std::fs::{self};

use itertools::Itertools;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////

/// Takes the range on which the mapping take effect, and the offset it needs to apply (negative or positive)
#[derive(Debug)]
struct MapRange {
    range: (u64, u64),
    offset: i64,
}

type Map = Vec<MapRange>;

impl MapRange {
    fn apply_offset(&self, nb: &u64) -> u64 {
        (self.offset + *nb as i64) as u64
    }

    fn contains(&self, nb: &u64) -> bool {
        self.range.0 <= *nb && self.range.1 >= *nb
    }
}

/// hmmmmm
fn read_input(buf: &str) -> (Vec<u64>, Vec<Map>) {
    let mut it = buf.split("\n\n");

    // First line
    let seeds = it
        .next()
        .unwrap()
        .split(' ')
        .skip(1)
        .map(|c| c.parse().unwrap())
        .collect();

    // Everything else, may look bad but really it is pretty good.
    let maps = it
        .map(|m| {
            m.lines()
                .skip(1)
                .filter_map(|l| {
                    l.split(' ')
                        .map(|t| t.parse().unwrap())
                        .collect_tuple()
                        .map(|(dest, start, ra)| MapRange {
                            range: (start, start + ra - 1),
                            offset: (dest as i64 - start as i64),
                        })
                })
                .sorted_by(|f, g| f.range.0.cmp(&g.range.0))
                .collect_vec()
        })
        .collect_vec();

    (seeds, maps)
}

/// Do the mapping given the step and the number, stops as soon as found the right mapping.
fn use_maps_on_nb(maps: &Map, nb: u64) -> u64 {
    maps.iter()
        .find_map(|m| {
            if m.contains(&nb) {
                Some(m.apply_offset(&nb))
            } else {
                None
            }
        })
        .unwrap_or(nb)
}

/// Nice.
fn part_one(seeds: &[u64], maps: &[Map]) -> u64 {
    seeds
        .iter()
        .map(|s| maps.iter().fold(*s, |acc, elem| use_maps_on_nb(elem, acc)))
        .min()
        .unwrap()
}

fn read_ranges(seeds: &[u64]) -> Vec<(u64, u64)> {
    seeds
        .iter()
        .batching(|it| match it.next() {
            // Pattern matching to make sure we don't overlook the list of seeds.
            None => None,
            Some(x) => it.next().map(|y| (*x, x + y)),
        })
        .collect()
}

/// Do one step with one range (maps being the list of all the mapping for a given step)
/// All of that works because ranges are continuous.
fn use_map_on_range(mapping: &Map, ran: (u64, u64)) -> Vec<(u64, u64)> {
    let mut r = Vec::new();
    let mut todo = ran;
    let mut not_finished_right = true;
    for m in mapping.iter() {
        match (m.contains(&todo.0), m.contains(&todo.1)) {
            (true, true) => {
                // Inside mapping -> just apply it and that's it.
                r.push((m.apply_offset(&todo.0), m.apply_offset(&todo.1)));
                not_finished_right = false; // nothing left on the right, exit early.
            }
            (true, false) => {
                // The left is inside -> Apply it to that and continue reading mapping with the right part.
                r.push((m.apply_offset(&todo.0), m.apply_offset(&m.range.1)));
                todo = (m.range.1 + 1, ran.1);
            }
            (false, true) => {
                // The right is inside -> apply to that and don't forget to keep the unmodified left part.
                r.push((todo.0, &m.range.0 - 1));
                r.push((m.apply_offset(&m.range.0), m.apply_offset(&todo.1)));
                not_finished_right = false; // Nothing left on the right, we can break since the other mappings will be more to the right.
            }
            (false, false) => {
                // Other cases :
                // The mapping is totally inside the range -> apply to that part, and don't forget the untouched left part
                // And continue with what is left on the right.
                if todo.0 < m.range.0 && m.range.1 < todo.1 {
                    r.push((m.apply_offset(&m.range.0), m.apply_offset(&m.range.1)));
                    r.push((ran.0, m.range.0 - 1));
                    todo = (m.range.1 + 1, ran.1);
                }
                // or the mapping has nothing to do with that range, so we do nothing.
            }
        }
    }
    // If unfinished business to the right (didn't get to exit early) then just add what there is left.
    if not_finished_right {
        r.push(todo);
    }
    r
}

fn part_two(seeds: &[(u64, u64)], maps: &[Map]) -> u64 {
    seeds
        .iter()
        .flat_map(|s| {
            maps.iter().fold(vec![*s], |acc, elem| {
                acc.iter()
                    .flat_map(|a| use_map_on_range(elem, *a))
                    .collect_vec()
            })
        })
        .map(|(x, _)| x)
        .min()
        .unwrap()
}

pub fn solve(filename: &'static str) -> SolutionPair {
    let buffer: String = fs::read_to_string(filename).unwrap_or_default();

    let (seeds, maps) = read_input(&buffer);
    let seeds_ranges = read_ranges(&seeds);
    let sol1: u64 = part_one(&seeds, &maps);
    let sol2: u64 = part_two(&seeds_ranges, &maps);

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) = solve("input/test_day5.txt") else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 35);
    assert_eq!(s2, 46);
}
