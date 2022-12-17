use std::{
    fs::File,
    io::{BufReader, Lines},
};

use crate::read_lines;

pub(crate) fn part1() -> u32 {
    if let Ok(lines) = read_lines("day-1/day-1.txt") {
        return compte_calories(lines).0;
    } else {
        panic!()
    }
}

pub(crate) fn part2() -> u32 {
    if let Ok(lines) = read_lines("day-1/day-1.txt") {
        let first = compte_calories(lines);
        return first.0;
    } else {
        panic!()
    }
}

fn compte_calories(lines: Lines<BufReader<File>>) -> (u32, usize) {
    let mut maxi = 0;
    let mut x = 0;
    let mut maxi_indice = 0;
    for (i, l) in lines.enumerate() {
        if l.is_ok() {
            let str_line = l.unwrap();
            if !str_line.is_empty() {
                let line: u32 = str_line.parse().unwrap();
                x += line;
            } else {
                if x > maxi {
                    maxi = x;
                    maxi_indice = i;
                }
                x = 0
            }
        }
    }
    return (maxi, maxi_indice)
}
