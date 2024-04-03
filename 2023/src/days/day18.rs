use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
/// SHOELACE

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum Dir {
    Up,
    Down,
    Right,
    Left,
}

type Rules = Vec<(Dir, u32)>;

fn read_input(buffer: &str) -> (Rules, Rules) {
    buffer
        .lines()
        .map(|line| {
            let mut it = line.split(' ');
            let d: &str = it.next().unwrap();
            let u: u32 = it.next().unwrap().parse().unwrap();
            let s: String = it.next().unwrap().to_string();
            let dir = match d {
                "R" => Dir::Right,
                "D" => Dir::Down,
                "U" => Dir::Up,
                _ => Dir::Left,
            };
            let color = s.strip_prefix("(#").unwrap().strip_suffix(')').unwrap();
            let distance = u32::from_str_radix(&color[0..5], 16).unwrap();
            let direction = match color.chars().nth(5).unwrap().to_digit(16) {
                Some(0) => Dir::Right,
                Some(1) => Dir::Down,
                Some(2) => Dir::Left,
                _ => Dir::Up,
            };
            ((dir, u), (direction, distance))
        })
        .unzip()
}

// SHOELACE
fn area(input: &Rules) -> u64 {
    let mut coordos: (i64, i64) = (0, 0);
    let mut somme: i64 = 0;
    let mut perimeter: i64 = 0;
    for y in input.iter() {
        perimeter += y.1 as i64;
        let new_coordos = match y.0 {
            Dir::Up => (coordos.0, coordos.1 - y.1 as i64),
            Dir::Down => (coordos.0, coordos.1 + y.1 as i64),
            Dir::Right => (coordos.0 + y.1 as i64, coordos.1),
            Dir::Left => (coordos.0 - y.1 as i64, coordos.1),
        };

        somme += coordos.0 * new_coordos.1 - coordos.1 * new_coordos.0;
        coordos = new_coordos;
    }
    (somme / 2 + perimeter / 2 + 1) as u64
}

pub fn solve(buffer: &str) -> SolutionPair {
    // Your solution here...
    let (input_part_one, input_part_two) = read_input(buffer);
    let sol1: u64 = area(&input_part_one);
    let sol2: u64 = area(&input_part_two);

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day18.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 62);
    assert_eq!(s2, 952408144115);
}
