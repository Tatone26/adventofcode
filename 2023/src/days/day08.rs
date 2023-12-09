use num::Integer;
use rayon::iter::{IntoParallelRefIterator, ParallelIterator};
use rustc_hash::FxHashMap;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
/// Already had to use rayon to get a really good time. Yay.

type Label = [u8; 3];
type MyMap = FxHashMap<Label, (Label, Label)>;

fn get_input(buf: &str) -> (String, MyMap) {
    let mut it = buf.lines();
    let input = it.next().unwrap().to_string();
    let nodes: MyMap = it
        .skip(1)
        .map(|line| {
            let mut it = line
                .split(|c: char| !c.is_digit(36))
                .filter(|s| !s.is_empty());
            let key = it.next().unwrap();
            let v1 = it.next().unwrap();
            let v2 = it.next().unwrap();
            (
                key.as_bytes().try_into().unwrap(),
                (
                    v1.as_bytes().try_into().unwrap(),
                    v2.as_bytes().try_into().unwrap(),
                ),
            )
        })
        .collect();
    (input, nodes)
}

/// All the time lost is here (more or less), with the get(). I guess doing 200 000 gets ends up kinda slow, even with HashMaps.
fn find_end(input: &str, nodes: &MyMap, start: &Label, end: fn(&[u8]) -> bool) -> u64 {
    let mut last_node: &Label = start;
    input
        .chars()
        .cycle()
        .enumerate()
        .find(|(_, c)| {
            end(last_node) || {
                let next_node = &nodes
                    .get(last_node)
                    .unwrap_or_else(|| panic!("Node not in map !"));
                if *c == 'L' {
                    last_node = &next_node.0;
                } else {
                    last_node = &next_node.1;
                };
                false
            }
        })
        .map(|(i, _)| i as u64)
        .unwrap_or(0)
}

/// yeah. only works because of the good cycles in the inputs. Would explode otherwise.
fn part_two(input: &str, nodes: &MyMap) -> u64 {
    nodes
        .par_iter()
        .filter(|(k, _)| matches!(**k, [_, _, b'A']))
        .map(|(s, _)| find_end(input, nodes, s, |node: &[u8]| node[2..3] == [b'Z']))
        .reduce(|| 1, |acc, elem| acc.lcm(&elem))
}

pub fn solve(buffer: &str) -> SolutionPair {
    let (input, nodes) = get_input(&buffer);
    let sol1: u64 = find_end(&input, &nodes, &[b'A', b'A', b'A'], |node: &[u8]| {
        node[0..3] == [b'Z', b'Z', b'Z']
    });
    let sol2: u64 = part_two(&input, &nodes);

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) = solve("input/test_day8.txt") else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 1); // differents tests for the two parts = not nice but my first part does work, so there is that.
    assert_eq!(s2, 6);
}
