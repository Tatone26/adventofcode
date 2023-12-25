use std::io::Write;

use itertools::Itertools;
use petgraph::{
    dot::{Config, Dot},
    graphmap::UnGraphMap,
    visit::{EdgeIndexable, IntoEdges},
};

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////

// UnGraphMap ?

fn read_input(buffer: &str) {
    let nodes = buffer.lines().flat_map(|line| {
        let mut it = line.split(": ");
        let name = it.next().unwrap();
        let linked = it.next().unwrap().split_ascii_whitespace().collect_vec();
        linked.iter().map(|&p| (name, p)).collect_vec()
    });

    let mut graph = UnGraphMap::<_, ()>::from_edges(nodes);
    graph.remove_edge("tqn", "tvf");
    graph.remove_edge("vzb", "tnr");
    graph.remove_edge("lmg", "krx");
    for i in petgraph::algo::tarjan_scc(&graph) {
        println!("{}", i.len());
    }
}

pub fn solve(buffer: &str) -> SolutionPair {
    read_input(buffer);
    let sol1: u64 = 0;
    let sol2: u64 = 0;

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day25.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 54);
    assert_eq!(s2, 0);
}

// for my input : tqn-tvf is a separator.
// "lmg" - "krx"
// "vzb" - "tnr"
// result : 746 and 781.
