use itertools::Itertools;

use crate::{Solution, SolutionPair};
use rayon::prelude::*;
use rustc_hash::FxHashMap;

///////////////////////////////////////////////////////////////////////////////

enum Tile {
    Wall,
    Ground,
    RightSlope,
    LeftSlope,
    DownSlope,
    UpSlope,
}

impl core::fmt::Debug for Tile {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::Wall => write!(f, "#"),
            Self::Ground => write!(f, "."),
            Self::RightSlope => write!(f, ">"),
            Self::LeftSlope => write!(f, "<"),
            Self::DownSlope => write!(f, "v"),
            Self::UpSlope => write!(f, "^"),
        }
    }
}

type Map = Vec<Vec<Tile>>;
type Node = usize;

#[derive(Clone, Debug)]
struct Edge {
    pos: (usize, usize),
    pos_fast: Node,
    size: u64,
    from_here: bool,
}
// type Graph = FxHashMap<Node, Vec<Edge>>;
type Graph = Vec<Vec<Edge>>;
const DIRECTIONS: [(i32, i32); 4] = [(0, 1), (0, -1), (1, 0), (-1, 0)];

fn read_input(buffer: &str) -> Map {
    buffer
        .lines()
        .map(|line| {
            line.chars()
                .map(|c| match c {
                    '>' => Tile::RightSlope,
                    '<' => Tile::LeftSlope,
                    '^' => Tile::UpSlope,
                    'v' => Tile::DownSlope,
                    '.' => Tile::Ground,
                    _ => Tile::Wall,
                })
                .collect_vec()
        })
        .collect_vec()
}

/// From a given position, return the next Edges of the graph (the Edges starting from there)
fn intersections(map: &Map, current_pos: (usize, usize)) -> Vec<Edge> {
    let mut result = vec![];
    for (x, y) in DIRECTIONS {
        let next_pos = (current_pos.0 as i32 + x, current_pos.1 as i32 + y);
        if next_pos.0 < 0
            || next_pos.1 < 0
            || next_pos.0 >= map[0].len() as i32
            || next_pos.1 >= map.len() as i32
        {
            continue;
        }
        let true_next_pos = (next_pos.0 as usize, next_pos.1 as usize);
        if !matches!(map[true_next_pos.1][true_next_pos.0], Tile::Wall) {
            let r = next_intersect(map, true_next_pos, current_pos);
            let edge = Edge {
                pos: r.pos,
                pos_fast: 0,
                size: r.size + 1,
                from_here: r.from_here,
            };
            result.push(edge);
        }
    }
    result
}

fn is_intersection(map: &Map, pos: (usize, usize)) -> bool {
    assert!(!matches!(map[pos.1][pos.0], Tile::Wall));
    DIRECTIONS
        .iter()
        .filter(|p| p.0 + pos.0 as i32 >= 0 && p.1 + pos.1 as i32 >= 0)
        .filter_map(|p| {
            map.get((p.1 + pos.1 as i32) as usize).and_then(|l| {
                l.get((p.0 + pos.0 as i32) as usize)
                    .filter(|&x| !matches!(x, Tile::Wall))
            })
        })
        .count()
        != 2
}

fn next_intersect(map: &Map, current_pos: (usize, usize), last_pos: (usize, usize)) -> Edge {
    if is_intersection(map, current_pos) {
        Edge {
            pos: current_pos,
            pos_fast: 0,
            size: 0,
            from_here: true,
        }
    } else {
        let next_dir = DIRECTIONS
            .iter()
            .filter(|p| p.0 + current_pos.0 as i32 >= 0 && p.1 + current_pos.1 as i32 >= 0)
            .find(|p| {
                let n = (
                    (p.0 + current_pos.0 as i32) as usize,
                    (p.1 + current_pos.1 as i32) as usize,
                );
                !matches!(map[n.1][n.0], Tile::Wall) && n != last_pos
            })
            .unwrap();
        let next_pos = (
            (next_dir.0 + current_pos.0 as i32) as usize,
            (next_dir.1 + current_pos.1 as i32) as usize,
        );
        let can_in_this_dir = match (&map[next_pos.1][next_pos.0], next_dir) {
            (Tile::Wall, _) => false,
            (Tile::Ground, _) => true,
            (Tile::RightSlope, (1, 0) | (0, 1) | (0, -1)) => true,
            (Tile::LeftSlope, (-1, 0) | (0, 1) | (0, -1)) => true,
            (Tile::UpSlope, (0, -1) | (1, 0) | (-1, 0)) => true,
            (Tile::DownSlope, (0, 1) | (1, 0) | (-1, 0)) => true,
            _ => false,
        };
        let t = next_intersect(
            map,
            (
                (next_dir.0 + current_pos.0 as i32) as usize,
                (next_dir.1 + current_pos.1 as i32) as usize,
            ),
            current_pos,
        );
        Edge {
            pos: t.pos,
            pos_fast: 0,
            size: t.size + 1,
            from_here: t.from_here && can_in_this_dir,
        }
    }
}

fn find_start(map: &Map) -> (usize, usize) {
    (
        map[0]
            .iter()
            .position(|x| !matches!(x, Tile::Wall))
            .unwrap(),
        0_usize,
    )
}

fn find_end(map: &Map) -> (usize, usize) {
    (
        map.last()
            .unwrap()
            .iter()
            .position(|x| !matches!(x, Tile::Wall))
            .unwrap(),
        map.len() - 1,
    )
}

fn get_graph(map: &Map) -> (Graph, Node, Node) {
    let start = find_start(map);

    let mut graph_map: Graph = vec![];
    let mut positions = FxHashMap::<(usize, usize), Node>::default();
    positions.insert(start, 0);
    let mut todo = vec![];
    todo.push(start);
    while let Some(pos) = todo.pop() {
        let mut res = intersections(map, pos);
        for r in res.iter_mut() {
            let l = positions.len();
            let p = positions.entry(r.pos).or_insert_with(|| {
                graph_map.resize(l + 1, vec![]);
                l
            });
            if graph_map.get(*p).unwrap().is_empty() {
                todo.push(r.pos);
            }
            r.pos_fast = *p;
        }
        graph_map[*positions.get(&pos).unwrap()] = res;
    } /*
      println!("{positions:?}");
      println!("{graph_map:?}"); */
    (
        graph_map,
        *positions.get(&start).unwrap(),
        *positions.get(&find_end(map)).unwrap(),
    )
}

fn longest_path(
    graph: &Graph,
    from: Node,
    already_seen: Vec<Node>,
    end: Node,
    strong: bool,
    recursion_level: u8,
) -> Option<u64> {
    /* let h: (Node, BTreeSet<Node>) = (from, already_seen.);
    if let Some(x) = memo.get(&h) {
        // println!("cached !");
        return *x;
    } */
    if from == end {
        Some(0)
    } else {
        let filter = |i: &&Edge| (strong || i.from_here) && !already_seen.contains(&i.pos_fast);
        let inside_work = |i: &Edge| {
            let mut new_seen = already_seen.clone();
            new_seen.push(i.pos_fast);
            Some(
                longest_path(
                    graph,
                    i.pos_fast,
                    new_seen,
                    end,
                    strong,
                    recursion_level + 1,
                )? + i.size,
            )
        };
        if recursion_level < 12 {
            let r = graph
                .get(from)?
                .par_iter()
                .filter(filter)
                .map(inside_work)
                .max()?;
            // memo.insert(h, r);
            r
        } else {
            let r = graph
                .get(from)?
                .iter()
                .filter(filter)
                .map(inside_work)
                .max()?;
            // memo.insert(h, r);
            r
        }
    }
}

pub fn solve(buffer: &str) -> SolutionPair {
    let input = read_input(buffer);
    // println!("{input:?}");
    let (graph, start, end) = get_graph(&input);
    // println!("{graph:?}");
    // println!("{}", graph.len());
    // let mut memo = FxHashMap::default();
    let sol1 = longest_path(&graph, start, vec![], end, false, 0).unwrap(); // yes
                                                                            // memo.clear();
    let sol2 = longest_path(&graph, start, vec![], end, true, 0).unwrap(); // yes

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day23.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 94);
    assert_eq!(s2, 154);
}

// 6654 !!
