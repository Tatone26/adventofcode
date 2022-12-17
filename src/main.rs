use std::fs;
use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;

pub mod day1;
fn main() {
    let res = day1::part1();
    println!("Résultat partie 1 : {res}")
}

pub fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>> where P: AsRef<Path>, {
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}