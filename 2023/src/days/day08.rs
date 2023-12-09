use itertools::Itertools;
use num::Integer;
use rustc_hash::FxHashMap;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
/// Got around the need to call hashmap.get() by creating an array with all the nodes
/// pointing to another position. So, instant .get() after spending some time creating this array.
/// Still the slowest day so far, but I feel like I'm getting better in rust.

type Label = [u8; 3];
type MyMap = Vec<Node>;

#[derive(Debug, Clone, Copy)]
struct Node {
    label: Label,
    left: usize,
    right: usize,
}

fn get_input(buf: &str) -> (String, MyMap) {
    let mut it = buf.lines();

    let input = it.next().unwrap().to_string();

    // Stores the map, with Nodes (each slot has a Node, with its label and the position in this same array of the next ones)
    let mut map: Vec<Option<Node>> = vec![None; it.clone().skip(1).count()];
    // Stores the (future) indexes corresponding to the keys.
    let mut indexes_hash = FxHashMap::<Label, usize>::default();

    // aaaaaaaaa it was not easy ! But after two days finally found the struct I wanted. But I'm certain there is a lot of room for improvement.
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
fn find_end(input: &str, nodes: &MyMap, start: &Node, end: fn(&Label) -> bool) -> u64 {
    let mut last_node: &Node = start;
    input
        .chars()
        .cycle()
        .enumerate()
        .find(|(_, c)| {
            if end(&last_node.label) {
                true
            } else {
                if *c == 'L' {
                    last_node = nodes.get(last_node.left).unwrap();
                } else {
                    last_node = nodes.get(last_node.right).unwrap();
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
