#![feature(is_sorted)]
mod days;
mod etc;

use days::{
    day01, day02, day03, day04, day05, day06, day07, day08, day09, day10, day11, day12, day13,
    day14, day15, day16, day17, day18, day19, day20, day21, day22, day23, day24, day25,
};
use etc::solution::Solution;
use std::io::Write;
use std::process::{Command, Stdio};
use std::str::FromStr;
use std::time::Instant;
use std::{env, fs};

pub type SolutionPair = (Solution, Solution);

/// TODO AT THE END : This simple timing works, but it would be better to get true results with Criterion or other benchmarking tools for real analysis.

/// To run : cargo run \[--release] \[1 .. 25 separated by spaces] or \[all]  + optional : \[mean (measure 100 executions of the given days)]
/// The time is the rough measurement of the day execution, without IO, but with parsing since it's part of the difficulty.
/// -> I call the day function with a String that contains the file contents, instead of a Filename.
fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        panic!("\x1b[1;36mPlease provide the day(s) to run as a command-line argument.\x1b[0m");
    }

    let mut mean: bool = false;

    let days: Vec<u8> = {
        if args.contains(&"-mean".to_string()) {
            mean = true;
        }
        if args.contains(&"all".to_string()) {
            (1..26).map(|x| x.to_string()).collect() // All 25 days !
        } else {
            args[1..].to_vec() // Given arguments
        }
    }
    .iter()
    .filter(|s| !s.contains("-mean"))
    .map(|x| {
        x.parse()
            .unwrap_or_else(|_| panic!("Not a valid day : {x}"))
    })
    .collect();

    let mut runtime = 0.0;
    let mut last_word: String = String::from_str("no result").unwrap();

    println!("\x1b[5S"); // == \n\n\n\n\n
    for day in &days {
        let (func, filename) = get_day_solver(*day);

        let (elapsed_ms, (p1, p2)) = execute_and_time(func, filename, if !mean { 1 } else { 100 });

        println!("\n\x1b[1m=== Day {:02} ===\x1b[0m", day);
        println!("  · Part 1: \x1b[3;32m{}\x1b[0m", p1);
        println!("  · Part 2: \x1b[3;32m{}\x1b[0m", p2);
        println!("\x1b[2;3m  · Elapsed: {:.4} ms\x1b[0m", elapsed_ms);

        runtime += elapsed_ms;
        last_word = p2.to_string();
    }
    println!("Total runtime: \x1b[36m{:.4} ms\x1b[m", runtime);

    copy_to_clipboard(last_word);
}

/// Maps the number to the corresponding solve function and input file name.
///
/// Panics if given a number not between 1 and 25.
fn get_day_solver(day: u8) -> (fn(&str) -> SolutionPair, &'static str) {
    match day {
        1 => (day01::solve, "input/day1.txt"),
        2 => (day02::solve, "input/day2.txt"),
        3 => (day03::solve, "input/day3.txt"),
        4 => (day04::solve, "input/day4.txt"),
        5 => (day05::solve, "input/day5.txt"),
        6 => (day06::solve, "input/day6.txt"),
        7 => (day07::solve, "input/day7.txt"),
        8 => (day08::solve, "input/day8.txt"),
        9 => (day09::solve, "input/day9.txt"),
        10 => (day10::solve, "input/day10.txt"),
        11 => (day11::solve, "input/day11.txt"),
        12 => (day12::solve, "input/day12.txt"),
        13 => (day13::solve, "input/day13.txt"),
        14 => (day14::solve, "input/day14.txt"),
        15 => (day15::solve, "input/day15.txt"),
        16 => (day16::solve, "input/day16.txt"),
        17 => (day17::solve, "input/day17.txt"),
        18 => (day18::solve, "input/day18.txt"),
        19 => (day19::solve, "input/day19.txt"),
        20 => (day20::solve, "input/day20.txt"),
        21 => (day21::solve, "input/day21.txt"),
        22 => (day22::solve, "input/day22.txt"),
        23 => (day23::solve, "input/day23.txt"),
        24 => (day24::solve, "input/day24.txt"),
        25 => (day25::solve, "input/day25.txt"),
        _ => unimplemented!(),
    }
}

/// Copies the given word to the global clipboard.
/// * word - The word to copy.
/// # Note
/// I have no idea how this really works, found it on [reddit](https://www.reddit.com/r/rust/comments/ssz0gr/comment/hx0r6zd/?utm_source=share&utm_medium=web2x&context=3), which seem to be copied more or less from the [Stdio] doc.
/// I tried to do it without std::process::Command, but to no avail.
fn copy_to_clipboard(word: String) {
    let mut child = Command::new("xclip")
        .arg("-selection")
        .arg("clipboard")
        .stdin(Stdio::piped())
        .spawn()
        .expect("\x1b[31mFailed to spawn 'xclip', ensure it is on your system. Did not copy to clipboard.\x1b[0m");
    let mut stdin = child
        .stdin
        .take()
        .expect("\x1b[31mFailed to copy to clipboard.\x1b[0m");
    std::thread::spawn(move || {
        stdin
            .write_all(word.as_bytes())
            .expect("\x1b[31mFailed to copy to clipboard.\x1b[0m")
    });
    child
        .wait_with_output()
        .expect("\x1b[31mFailed to copy to clipboard.\x1b[0m");
    println!("\x1b[2;3mResult copied to clipboard !\x1b[m");
}

fn execute_and_time(
    func: fn(&str) -> SolutionPair,
    filename: &'static str,
    i: u32,
) -> (f64, SolutionPair) {
    let mut mean_time: f64 = 0_f64;
    let mut solutions: Option<SolutionPair> = None;
    let buffer: String = fs::read_to_string(filename).unwrap_or_default();
    for _ in 0..i {
        let time = Instant::now();
        let (p1, p2) = func(&buffer);
        let elapsed_ms = time.elapsed().as_nanos() as f64 / 1_000_000.0;
        if solutions.is_none() {
            solutions = Some((p1, p2));
        }
        mean_time = (mean_time * (i as f64) + elapsed_ms) / (i as f64 + 1_f64);
    }
    (mean_time, solutions.unwrap())
}
