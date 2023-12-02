use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////

pub fn solve(_filename: &'static str) -> SolutionPair {
    // Your solution here...
    let sol1: u64 = 0;
    let sol2: u64 = 0;

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) = solve("input/test_day4.txt") else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 0);
    assert_eq!(s2, 0);
}
