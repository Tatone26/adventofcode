use itertools::Itertools;
// use petgraph::graphmap::UnGraphMap;
use rustc_hash::FxHashSet;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////

// UnGraphMap ?

#[derive(Debug, PartialEq, Eq, Hash)]
struct Edge {
    from: String,
    to: String,
}

fn read_input(buffer: &str) -> FxHashSet<Edge> {
    let nodes = buffer.lines().flat_map(|line| {
        let mut it = line.split(": ");
        let name = it.next().unwrap();
        let linked = it.next().unwrap().split_ascii_whitespace().collect_vec();
        linked.iter().map(|&p| (name, p)).collect_vec()
    });
    nodes
        .flat_map(|o| {
            [
                Edge {
                    from: o.0.to_string(),
                    to: o.1.to_string(),
                },
                Edge {
                    from: o.1.to_string(),
                    to: o.0.to_string(),
                },
            ]
        })
        .collect()
}

/* fn read_input(buffer: &str) {
    /* let nodes = buffer.lines().flat_map(|line| {
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
    } */

    let test = graph_from_input(buffer);
    println!("{test:?}");
} */

pub fn solve(buffer: &str) -> SolutionPair {
    let input = read_input(buffer);
    // println!("{input:?}");
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
