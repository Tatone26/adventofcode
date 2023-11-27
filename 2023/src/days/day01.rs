use crate::{Solution, SolutionPair};
use std::fs::File;
use std::io::Read;
use text_io::read;
///////////////////////////////////////////////////////////////////////////////

pub fn solve(filename: &'static str) -> SolutionPair {
    // Your solution here...
    let mut file = File::open(filename).unwrap().bytes().map(|ch| ch.unwrap());
    let output: i32 = read!("this is test {} !", file);
    println!("number : {:?}", output);
    let sol1 = rand::random::<u64>();
    let sol2 = rand::random::<u64>();
    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) = solve("input/test_day1.txt") else {
        panic!("\x1b[1;31mMauvais type de solutions pour le test, mettez le à jour !\x1b[m")
    };
    assert_eq!(s1, 0);
    assert_eq!(s2, 0);
}
