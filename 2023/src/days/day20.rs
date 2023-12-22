use std::collections::VecDeque;

use itertools::Itertools;

use rustc_hash::FxHashMap;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////
/// Part2 is pretty hard. Need to check for cycles and everything...
/// this does NOT answer the question :
/// Need to find when the last module send a SINGLE low.
/// In my case, it's a conjunction one : I need to find a cycle where every single parent send HIGH too. (otherwise, it will send HIGH to the child.)
/// Simple : just LLM the parents, they send HIGH in cycles. (thanks AOC for the well-crafted inputs...)
/// Found the answer manually.
/// My Answer : LLM of [4079, 3919, 3797, 3761]
/// Project of the holidays : make this process automatic. (beacuse i hate things that are not automatic.)

#[derive(Debug, Clone)]
struct Machine {
    content: Vec<Module>,
}

#[derive(Debug, Clone, Copy)]
enum ModuleType {
    FlipFlop,
    Conjunction,
    Broadcast,
    Unknown,
}

#[derive(Debug, Clone, Copy)]
enum Pulse {
    High,
    Low,
}

#[derive(Debug, Clone)]
struct Module {
    parents: Vec<usize>,
    parents_receiving: Vec<Pulse>,
    switch: bool,
    module_type: ModuleType,
    childs: Vec<usize>,
}

impl Module {
    fn receive_pulse(&mut self, pulse: Pulse, from: usize) -> Option<Pulse> {
        let index = self.parents.iter().position(|&x| x == from).unwrap();
        if matches!(self.module_type, ModuleType::FlipFlop) && matches!(pulse, Pulse::Low) {
            self.switch = !self.switch;
            if self.switch {
                Some(Pulse::High)
            } else {
                Some(Pulse::Low)
            }
        } else if matches!(self.module_type, ModuleType::Conjunction) {
            // Conjunction type.
            self.parents_receiving[index] = pulse;
            if self
                .parents_receiving
                .iter()
                .any(|p| matches!(p, Pulse::Low))
            {
                Some(Pulse::High)
            } else {
                Some(Pulse::Low)
            }
        } else if matches!(self.module_type, ModuleType::Broadcast) {
            return Some(pulse);
        } else {
            None
        }
    }
}

// todo : factoriser ce truc.
fn read_input(buffer: &str, to_check: &str) -> (Machine, usize) {
    let mut machine_content = vec![None; buffer.lines().count() + 1];
    let mut machine_positions: FxHashMap<String, usize> = FxHashMap::default();
    let mut next_position: usize = 0;

    for line in buffer.lines() {
        let (name, childs) = line.split(" -> ").collect_tuple().unwrap();
        let (module_type, real_name) = if name == "broadcaster" {
            (ModuleType::Broadcast, "broadcaster")
        } else if name.starts_with('%') {
            (ModuleType::FlipFlop, name.strip_prefix('%').unwrap())
        } else {
            (ModuleType::Conjunction, name.strip_prefix('&').unwrap())
        };
        let position = machine_positions.get(real_name);
        let real_position = if let Some(&pos) = position {
            pos
        } else {
            machine_positions.insert(real_name.to_string(), next_position);
            next_position += 1;
            next_position - 1
        };

        let mut childs_positions = vec![];
        for c in childs.split(", ") {
            let position_child = machine_positions.get(c);
            let real_position_child = if let Some(&pos) = position_child {
                pos
            } else {
                machine_positions.insert(c.to_string(), next_position);
                next_position += 1;
                next_position - 1
            };
            childs_positions.push(real_position_child);
        }

        machine_content[real_position] = Some(Module {
            parents: vec![],
            parents_receiving: vec![],
            switch: false,
            module_type,
            childs: childs_positions,
        });
    }

    let mut true_content = machine_content
        .iter()
        .map(|p| {
            if p.is_some() {
                p.clone().unwrap()
            } else {
                Module {
                    parents: vec![],
                    parents_receiving: vec![],
                    switch: false,
                    module_type: ModuleType::Unknown,
                    childs: vec![],
                }
            }
        })
        .collect_vec();

    for (i, m) in true_content.clone().iter().enumerate() {
        for c in m.childs.iter() {
            if true_content.get(*c).is_some() {
                true_content[*c].parents.push(i);
                true_content[*c].parents_receiving.push(Pulse::Low);
            }
        }
    } /*

      println!("{true_content:?}");
      println!("{machine_positions:?}"); */
    (
        Machine {
            content: true_content,
        },
        *machine_positions.get(to_check).unwrap_or(&usize::MAX),
    )
}

fn press_button(input: &mut Machine, pulse: Pulse, check: Option<usize>) -> (u64, u64, bool) {
    let broadcast_position = input
        .content
        .iter()
        .position(|p| matches!(p.module_type, ModuleType::Broadcast))
        .unwrap();

    let mut todo = VecDeque::default();
    todo.push_front((pulse, broadcast_position));
    let mut counter = match pulse {
        Pulse::High => (0, 1),
        Pulse::Low => (1, 0),
    };
    let mut precise_counter = false;
    while let Some(x) = todo.pop_front() {
        if check.is_some() && x.1 == check.unwrap() && matches!(x.0, Pulse::High) {
            precise_counter = true;
        }
        for c in input.content[x.1].childs.clone().iter() {
            match x.0 {
                Pulse::High => counter.1 += 1,
                Pulse::Low => counter.0 += 1,
            }
            /* if check.is_some() && check.unwrap() == *c {
                // println!("receiving pulse {:?} from {}", x.0, x.1);
                match x.0 {
                    Pulse::High => precise_counter.1 += 1,
                    Pulse::Low => precise_counter.0 += 1,
                }
            } */
            if *c >= input.content.len() {
                continue;
            }
            let next_pulse = input.content[*c].receive_pulse(x.0, x.1);
            if let Some(p) = next_pulse {
                todo.push_back((p, *c));
            }
        }
    }
    (counter.0, counter.1, precise_counter)
}

fn part_one(input: &mut Machine) -> u64 {
    let mut counter = (0, 0);
    for _ in 0..1000 {
        let r = press_button(input, Pulse::Low, None);
        // println!("{:?}", r.2);
        counter.0 += r.0;
        counter.1 += r.1;
    }
    counter.0 * counter.1
}

fn part_two(input: &mut Machine, check: usize) -> u64 {
    if check == usize::MAX {
        return 0;
    }
    let mut counter = 0;
    loop {
        let result = press_button(input, Pulse::Low, Some(check));
        counter += 1;

        // println!("{:?}", result.2);
        if result.2 {
            println!("for {check} : {counter} presses to HIGH.");
            break;
        }
        /* if result.2 .0 >= 1 {
            break;
        } */
    }
    counter
}

pub fn solve(buffer: &str) -> SolutionPair {
    let (mut input, to_check) = read_input(buffer, "fk");
    let mut input_copy = input.clone();
    let sol1: u64 = part_one(&mut input);
    let sol2: u64 = part_two(&mut input_copy, to_check);
    //let sol2: u64 = 0;
    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day20.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 11687500);
    assert_eq!(s2, 0);
}
