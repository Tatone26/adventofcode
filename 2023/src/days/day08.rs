use std::fmt::Debug;

use itertools::Itertools;
use num::Integer;
use rustc_hash::FxHashMap;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
/// Got around the need to call hashmap.get() by creating an array with all the nodes
/// pointing to another position. So, instant .get() after spending some time creating this array.
/// The last big optimisation was switching from str.chars() with if c == 'L' to [Direction].iter() with match. Impressive change in speed !

type Label = [u8; 3];
type MyMap = Vec<Node>;
type Input = Vec<Direction>;

#[derive(Debug, Clone, Copy)]
struct Node {
    label: Label,
    left: usize,
    right: usize,
}

// Wow, using a small enum and matches is so much faster than if . == . {} else {}... I'm kinda shocked, but shoudn't be since it's probably optimised to boolean.
// You learn everyday with AoC !!
#[derive(Debug)]
enum Direction {
    Left,
    Right,
}

fn get_input(buf: &str) -> (Input, MyMap) {
    let mut it = buf.lines();

    let input = it
        .next()
        .map(|s| {
            s.chars().map(|c| {
                if c == 'L' {
                    Direction::Left
                } else {
                    Direction::Right
                }
            })
        })
        .expect("bad input")
        .collect_vec();

    // Stores the map, with Nodes (each slot has a Node, with its label and the position in this same array of the next ones)
    let mut map: Vec<Option<Node>> = vec![None; it.clone().skip(1).count()];
    // Stores the (future) indexes corresponding to the keys, not the best but it's okay.
    let mut indexes_hash = FxHashMap::<Label, usize>::default();

    it.skip(1).for_each(|line| {
        let ((key, k), (_, left), (_, right)): ((Label, usize), (Label, usize), (Label, usize)) =
            line.split([' ', ',', '(', ')', '='])
                .filter(|s| !s.is_empty())
                .map(|s| {
                    let code_s: [u8; 3] = s.as_bytes().try_into().unwrap();
                    let v = indexes_hash.get(&code_s).copied().unwrap_or_else(|| {
                        let n = indexes_hash.len();
                        indexes_hash.insert(code_s, n);
                        n
                    });
                    (code_s, v)
                })
                .collect_tuple()
                .unwrap();
        map[k] = Some(Node {
            label: key,
            left,
            right,
        });
    });
    (input, map.iter().flatten().copied().collect_vec())
}

/// All the time lost is here (more or less), with the get(). I guess doing 200 000 gets ends up kinda slow, even with HashMaps.
fn find_end(input: &[Direction], nodes: &MyMap, start: &Node, end: fn(&Label) -> bool) -> u64 {
    let mut last_node: &Node = start;
    for (i, c) in input.iter().cycle().enumerate() {
        if end(&last_node.label) {
            return i as u64;
        } else {
            last_node = match c {
                Direction::Left => &nodes[last_node.left],
                Direction::Right => &nodes[last_node.right],
            };
        }
    }
    0
}

/// yeah. only works because of the good cycles in the inputs. Would explode otherwise.
fn part_two(input: &[Direction], nodes: &MyMap) -> u64 {
    nodes
        .iter()
        .filter(|k| matches!(k.label, [_, _, b'A']))
        .map(|s| find_end(input, nodes, s, |node: &Label| matches!(node, [_, _, b'Z'])))
        .reduce(|acc, elem| acc.lcm(&elem))
        .unwrap()
}

pub fn solve(buffer: &str) -> SolutionPair {
    let (input, nodes) = get_input(buffer);
    let sol1: u64 = find_end(
        &input,
        &nodes,
        nodes
            .iter()
            .find(|k| k.label == [b'A', b'A', b'A'])
            .unwrap(),
        |node: &Label| *node == [b'Z', b'Z', b'Z'],
    );
    let sol2: u64 = part_two(&input, &nodes);

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day8.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 1); // differents tests for the two parts = not nice but my first part does work, so don't really care about the test.
    assert_eq!(s2, 6);
}
