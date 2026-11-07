use std::env;
use std::fmt::{Display, Formatter, Result as FmtResult};
use std::fs;
use std::hint::black_box;
use std::process::{Command, Stdio};
use std::time::{Duration, Instant};

mod day01;
mod day02;
mod day03;
mod day04;
mod day05;

// ============================================================================
// 1. Core Solution Abstraction
// ============================================================================

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Solution {
    None,
    I8(i8),
    I16(i16),
    I32(i32),
    I64(i64),
    I128(i128),
    Isize(isize),
    U8(u8),
    U16(u16),
    U32(u32),
    U64(u64),
    U128(u128),
    Usize(usize),
    Str(String),
}

impl Display for Solution {
    fn fmt(&self, f: &mut Formatter<'_>) -> FmtResult {
        match self {
            Solution::None => write!(f, "—"),
            Solution::I8(x) => x.fmt(f),
            Solution::I16(x) => x.fmt(f),
            Solution::I32(x) => x.fmt(f),
            Solution::I64(x) => x.fmt(f),
            Solution::I128(x) => x.fmt(f),
            Solution::Isize(x) => x.fmt(f),
            Solution::U8(x) => x.fmt(f),
            Solution::U16(x) => x.fmt(f),
            Solution::U32(x) => x.fmt(f),
            Solution::U64(x) => x.fmt(f),
            Solution::U128(x) => x.fmt(f),
            Solution::Usize(x) => x.fmt(f),
            Solution::Str(x) => x.fmt(f),
        }
    }
}

macro_rules! impl_from_num {
    ($($type_:ident => $kind_:ident),*) => {
        $(
            impl From<$type_> for Solution {
                #[inline]
                fn from(val: $type_) -> Self {
                    Solution::$kind_(val)
                }
            }
        )*
    };
}

impl_from_num!(
    i8 => I8, i16 => I16, i32 => I32, i64 => I64, i128 => I128, isize => Isize,
    u8 => U8, u16 => U16, u32 => U32, u64 => U64, u128 => U128, usize => Usize
);

impl From<String> for Solution {
    #[inline]
    fn from(s: String) -> Self {
        Solution::Str(s)
    }
}

impl From<&str> for Solution {
    #[inline]
    fn from(s: &str) -> Self {
        Solution::Str(s.to_owned())
    }
}

impl<T: Into<Solution>> From<Option<T>> for Solution {
    #[inline]
    fn from(opt: Option<T>) -> Self {
        match opt {
            Some(val) => val.into(),
            None => Solution::None,
        }
    }
}

// ============================================================================
// 2. Benchmarking Engine & Trait Abstraction
// ============================================================================

#[derive(Debug, Clone, Copy)]
pub struct TimingStats {
    pub min: Duration,
    pub median: Duration,
    pub mean: Duration,
}

pub struct BenchmarkResult {
    pub day: u8,
    pub p1: Solution,
    pub p2: Solution,
    pub parse_stats: TimingStats,
    pub p1_stats: TimingStats,
    pub p2_stats: Option<TimingStats>,
    pub total_median: Duration,
}

pub trait DayRunner {
    fn run(&self, input: &str, warmup: usize, samples: usize) -> BenchmarkResult;
}

struct DaySolver<T, C, P, F1, F2>
where
    P: Fn(&str) -> T,
    F1: Fn(&T) -> (Solution, C),
    F2: Fn(&T, &C) -> Solution,
{
    day: u8,
    parse: P,
    part1: F1,
    part2: F2,
}

impl<T, C, P, F1, F2> DayRunner for DaySolver<T, C, P, F1, F2>
where
    P: Fn(&str) -> T,
    F1: Fn(&T) -> (Solution, C),
    F2: Fn(&T, &C) -> Solution,
{
    fn run(&self, input: &str, warmup: usize, samples: usize) -> BenchmarkResult {
        // 1. Benchmark Parsing
        for _ in 0..warmup {
            black_box((self.parse)(black_box(input)));
        }
        let mut parse_samples = Vec::with_capacity(samples);
        for _ in 0..samples {
            let start = Instant::now();
            let parsed = (self.parse)(black_box(input));
            let elapsed = start.elapsed();
            black_box(parsed);
            parse_samples.push(elapsed);
        }
        let parse_stats = compute_stats(&mut parse_samples);

        // Pre-parse once for algorithm execution timing
        let parsed_data = (self.parse)(input);

        // 2. Benchmark Part 1
        for _ in 0..warmup {
            black_box((self.part1)(black_box(&parsed_data)));
        }
        let mut p1_samples = Vec::with_capacity(samples);
        let mut p1_sol = Solution::None;

        for i in 0..samples {
            let start = Instant::now();
            let (sol, _ctx) = (self.part1)(black_box(&parsed_data));
            let elapsed = start.elapsed();
            if i == 0 {
                p1_sol = sol;
            }
            p1_samples.push(elapsed);
        }
        let p1_stats = compute_stats(&mut p1_samples);

        // Extract context once from Part 1 to supply to Part 2
        let (_p1_check, context) = (self.part1)(&parsed_data);

        // Initial check for Part 2
        let initial_p2 = (self.part2)(&parsed_data, &context);

        let (p2_sol, p2_stats) = if initial_p2 == Solution::None {
            (Solution::None, None)
        } else {
            for _ in 0..warmup {
                black_box((self.part2)(black_box(&parsed_data), black_box(&context)));
            }
            let mut p2_samples = Vec::with_capacity(samples);
            for _ in 0..samples {
                let start = Instant::now();
                let res = (self.part2)(black_box(&parsed_data), black_box(&context));
                let elapsed = start.elapsed();
                black_box(res);
                p2_samples.push(elapsed);
            }
            (initial_p2, Some(compute_stats(&mut p2_samples)))
        };

        let total_median = parse_stats.median
            + p1_stats.median
            + p2_stats.map(|s| s.median).unwrap_or(Duration::ZERO);

        BenchmarkResult {
            day: self.day,
            p1: p1_sol,
            p2: p2_sol,
            parse_stats,
            p1_stats,
            p2_stats,
            total_median,
        }
    }
}

fn compute_stats(samples: &mut [Duration]) -> TimingStats {
    samples.sort_unstable();
    let min = samples[0];
    let median = samples[samples.len() / 2];
    let total_nanos: u128 = samples.iter().map(|d| d.as_nanos()).sum();
    let mean = Duration::from_nanos((total_nanos / samples.len() as u128) as u64);

    TimingStats { min, median, mean }
}

fn make_runner<T, C, P, F1, F2, S1, S2>(
    day: u8,
    parse: P,
    part1: F1,
    part2: F2,
) -> Box<dyn DayRunner>
where
    T: 'static,
    C: 'static,
    P: Fn(&str) -> T + 'static,
    F1: Fn(&T) -> (S1, C) + 'static,
    F2: Fn(&T, &C) -> S2 + 'static,
    S1: Into<Solution> + 'static,
    S2: Into<Solution> + 'static,
{
    Box::new(DaySolver {
        day,
        parse,
        part1: move |data| {
            let (s1, ctx) = part1(data);
            (s1.into(), ctx)
        },
        part2: move |data, ctx| part2(data, ctx).into(),
    })
}

// ============================================================================
// 3. Main CLI & Runner
// ============================================================================

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!(
            "\x1b[1;31mUsage:\x1b[0m cargo run [--release] [-- [DAYS...] [all] [--bench] [--test]]"
        );
        return;
    }

    let is_bench = args
        .iter()
        .any(|a| a == "--bench" || a == "-bench" || a == "-mean");
    let is_test = args.iter().any(|a| a == "--test" || a == "-t");

    let runs_per_day = if is_bench { 100 } else { 1 };
    let warmup_runs = if is_bench { 10 } else { 0 };

    let days_to_run = parse_selected_days(&args);
    if days_to_run.is_empty() {
        eprintln!("\x1b[1;31mError:\x1b[0m No valid day numbers (1..=25) provided.");
        return;
    }

    print!("\x1b[2J\x1b[H");

    let mut total_duration = Duration::ZERO;
    let mut last_clipboard_str = String::new();

    for day in days_to_run {
        let (runner, input_file) = match get_day_runner(day, is_test) {
            Some(entry) => entry,
            None => {
                eprintln!(
                    "\x1b[33m[Day {:02}] Solver not implemented yet. Skipping.\x1b[0m",
                    day
                );
                continue;
            }
        };

        let input_content = match fs::read_to_string(&input_file) {
            Ok(content) => content,
            Err(_) => {
                eprintln!(
                    "\x1b[31m[Day {:02}] Missing input file: {}\x1b[0m",
                    day, input_file
                );
                continue;
            }
        };

        let res = runner.run(&input_content, warmup_runs, runs_per_day);

        let test_label = if is_test { " (TEST)" } else { "" };
        println!("\x1b[1m=== Day {:02}{} ===\x1b[0m", res.day, test_label);
        println!("  · Part 1: \x1b[1;32m{}\x1b[0m", res.p1);
        println!("  · Part 2: \x1b[1;32m{}\x1b[0m", res.p2);

        if is_bench {
            println!(
                "Parse:  \x1b[33m{:.3?}\x1b[0m \x1b[2m[min: {:.3?}, mean: {:.3?}]\x1b[0m",
                res.parse_stats.median, res.parse_stats.min, res.parse_stats.mean
            );
            println!(
                "Part 1: \x1b[33m{:.3?}\x1b[0m \x1b[2m[min: {:.3?}, mean: {:.3?}]\x1b[0m",
                res.p1_stats.median, res.p1_stats.min, res.p1_stats.mean
            );
            if let Some(p2_s) = res.p2_stats {
                println!(
                    "Part 2: \x1b[33m{:.3?}\x1b[0m \x1b[2m[min: {:.3?}, mean: {:.3?}]\x1b[0m",
                    p2_s.median, p2_s.min, p2_s.mean
                );
            }
            println!("Total:  \x1b[1;36m{:.3?}\x1b[0m", res.total_median);
        } else {
            println!("Parse:  \x1b[33m{:.3?}\x1b[0m", res.parse_stats.median);
            println!("Part 1: \x1b[33m{:.3?}\x1b[0m", res.p1_stats.median);
            if let Some(p2_s) = res.p2_stats {
                println!("Part 2: \x1b[33m{:.3?}\x1b[0m", p2_s.median);
            }
            println!("Total:  \x1b[1;36m{:.3?}\x1b[0m", res.total_median);
        }

        println!();

        total_duration += res.total_median;

        last_clipboard_str = match &res.p2 {
            Solution::None => res.p1.to_string(),
            p2 => p2.to_string(),
        };
    }

    println!(
        "Total Execution Time: \x1b[1;36m{:.3?}\x1b[0m",
        total_duration
    );

    if !last_clipboard_str.is_empty() && last_clipboard_str != "—" {
        copy_to_clipboard(&last_clipboard_str);
    }
}

fn parse_selected_days(args: &[String]) -> Vec<u8> {
    if args.iter().any(|a| a == "all") {
        return (1..=25).collect();
    }

    args.iter()
        .filter(|a| !a.starts_with('-'))
        .filter_map(|a| a.parse::<u8>().ok())
        .filter(|&day| (1..=25).contains(&day))
        .collect()
}

fn get_day_runner(day: u8, is_test: bool) -> Option<(Box<dyn DayRunner>, String)> {
    let filename = if is_test {
        format!("input/test_day{day}.txt")
    } else {
        format!("input/day{day}.txt")
    };

    let runner: Box<dyn DayRunner> = match day {
        1 => make_runner(1, day01::parse, day01::part1, day01::part2),
        2 => make_runner(2, day02::parse, day02::part1, day02::part2),
        3 => make_runner(3, day03::parse, day03::part1, day03::part2),
        4 => make_runner(4, day04::parse, day04::part1, day04::part2),
        5 => make_runner(5, day05::parse, day05::part1, day05::part2),

        _ => return None,
    };

    Some((runner, filename))
}

fn copy_to_clipboard(text: &str) {
    let (tool, args) = if env::var("WAYLAND_DISPLAY").is_ok() {
        ("wl-copy", vec![])
    } else {
        ("xclip", vec!["-selection", "clipboard"])
    };

    let mut child = match Command::new(tool)
        .args(&args)
        .stdin(Stdio::piped())
        .stdout(Stdio::null())
        .stderr(Stdio::null())
        .spawn()
    {
        Ok(child) => child,
        Err(_) => return,
    };

    if let Some(mut stdin) = child.stdin.take() {
        use std::io::Write;
        let _ = stdin.write_all(text.as_bytes());
    }
    let _ = child.wait();
}
