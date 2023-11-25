mod days;
mod etc;

use etc::solution::Solution;
use days::{day01, day02, day03, day04, day05,
           day06, day07, day08, day09, day10,
           day11, day12, day13, day14, day15,
           day16, day17, day18, day19, day20,
           day21, day22, day23, day24, day25};
use std::env;
use std::time::Instant;

pub type SolutionPair = (Solution, Solution);

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        panic!("Please provide the day(s) to run as a command-line argument.");
    }

    let days: Vec<u8> = args[1..].iter()
        .map(|x| x.parse().unwrap_or_else(|v| panic!("Not a valid day: {}", v)))
        .collect();

    let mut runtime = 0.0;

    for day in days {
        let (func, filename) = get_day_solver(day);

        let time = Instant::now();
        let (p1, p2) = func(filename);
        let elapsed_ms = time.elapsed().as_nanos() as f64 / 1_000_000.0;
        
        println!("\n=== Day {:02} ===", day);
        println!("  · Part 1: {}", p1);
        println!("  · Part 2: {}", p2);
        println!("  · Elapsed: {:.4} ms", elapsed_ms);

        runtime += elapsed_ms;
    }

    println!("Total runtime: {:.4} ms", runtime);
}

fn get_day_solver(day: u8) -> (fn(&'static str) -> SolutionPair, &'static str) {
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