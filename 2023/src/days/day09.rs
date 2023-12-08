use std::fs;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////

pub fn solve(filename: &'static str) -> SolutionPair {
    let _buffer = fs::read_to_string(filename);

    let sol1: u64 = 0;
    let sol2: u64 = 0;
    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) = solve("input/test_day9.txt") else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 0);
    assert_eq!(s2, 0);
}
