use std::collections::VecDeque;

use itertools::Itertools;

use num::integer::lcm;
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
    positions: FxHashMap<String, usize>,
}

#[derive(Debug, Clone, Copy)]
enum ModuleType {
    FlipFlop,
    Conjunction,
    Broadcast,
    Unknown,
}

impl ModuleType {
    /// returns self and its name.
    fn from_str(buffer: &str) -> (Self, &str) {
        if buffer == "broadcaster" {
            (ModuleType::Broadcast, "broadcaster")
        } else if buffer.starts_with('%') {
            (ModuleType::FlipFlop, buffer.strip_prefix('%').unwrap())
        } else {
            (ModuleType::Conjunction, buffer.strip_prefix('&').unwrap())
        }
    }
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
        match self.module_type {
            ModuleType::FlipFlop => {
                if matches!(pulse, Pulse::Low) {
                    self.switch = !self.switch;
                    if self.switch {
                        Some(Pulse::High)
                    } else {
                        Some(Pulse::Low)
                    }
                } else {
                    None
                }
            }
            ModuleType::Conjunction => {
                let index = self.parents.iter().position(|&x| x == from).unwrap();
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
            }
            ModuleType::Broadcast => Some(pulse),
            ModuleType::Unknown => None,
        }
    }
}

fn get_or_insert(map: &mut FxHashMap<String, usize>, name: &str) -> usize {
    if let Some(&pos) = map.get(name) {
        pos
    } else {
        map.insert(name.to_string(), map.len());
        map.len() - 1
    }
}

// returns the machine, and the position of the module to check, if it exists (which it should for part 2)
fn read_input(buffer: &str) -> Machine {
    let mut machine_content = vec![None; buffer.lines().count() + 1];
    let mut machine_positions: FxHashMap<String, usize> = FxHashMap::default();

    for line in buffer.lines() {
        let (name, childs) = line.split(" -> ").collect_tuple().unwrap();
        let (module_type, real_name) = ModuleType::from_str(name);

        let real_position = get_or_insert(&mut machine_positions, real_name);
        let childs_positions = childs
            .split(", ")
            .map(|c| get_or_insert(&mut machine_positions, c))
            .collect_vec();

        machine_content[real_position] = Some(Module {
            parents: vec![],
            parents_receiving: vec![],
            switch: false,
            module_type,
            childs: childs_positions,
        });
    }

    machine_content
        .iter_mut()
        .filter(|p| p.is_none())
        .for_each(|v| {
            *v = Some(Module {
                parents: vec![],
                parents_receiving: vec![],
                switch: false,
                module_type: ModuleType::Unknown,
                childs: vec![],
            })
        });

    let mut true_content = machine_content
        .iter()
        .filter_map(|p| p.to_owned())
        .collect_vec();

    for (i, m) in true_content.clone().iter().enumerate() {
        for c in m.childs.iter() {
            if true_content.get(*c).is_some() {
                true_content[*c].parents.push(i);
                true_content[*c].parents_receiving.push(Pulse::Low);
            }
        }
    }
    Machine {
        content: true_content,
        positions: machine_positions,
    }
}

/// Modifies in-place the machine, and returns (number_of_low_pulse, number_of_high_pulse, check_has_send_high_pulse)
fn press_button(input: &mut Machine, pulse: Pulse, check: &[usize]) -> (u64, u64, Vec<bool>) {
    let broadcast_position = input
        .content
        .iter()
        .position(|p| matches!(p.module_type, ModuleType::Broadcast))
        .expect("No broadcast module in input.");

    // todo is composed of (pulse_type, who_sent_it)
    let mut todo = VecDeque::default();
    todo.push_front((pulse, broadcast_position));
    let mut counter = match pulse {
        Pulse::High => (0, 1),
        Pulse::Low => (1, 0),
    };
    let mut precise_counters = vec![false; check.len()];
    while let Some(x) = todo.pop_front() {
        if let Some(i) = check.iter().position(|&p| p == x.1) {
            if matches!(x.0, Pulse::High) {
                precise_counters[i] = true;
            }
        }

        for c in input.content[x.1].childs.clone().iter() {
            match x.0 {
                Pulse::High => counter.1 += 1,
                Pulse::Low => counter.0 += 1,
            }
            let next_pulse = input
                .content
                .get_mut(*c)
                .and_then(|p| p.receive_pulse(x.0, x.1));
            if let Some(p) = next_pulse {
                todo.push_back((p, *c));
            }
        }
    }
    (counter.0, counter.1, precise_counters)
}

fn part_one(input: &mut Machine) -> u64 {
    let mut counter = (0, 0);
    for _ in 0..1000 {
        let r = press_button(input, Pulse::Low, &[]);
        counter.0 += r.0;
        counter.1 += r.1;
    }
    counter.0 * counter.1
}

fn part_two(input: &mut Machine) -> u64 {
    const FINAL_MODULE_NAME: &str = "rx";

    let check = *input
        .positions
        .get(FINAL_MODULE_NAME)
        .expect("Final module not in input.");
    if check == usize::MAX {
        return 0;
    }

    assert!(
        input.content[check].parents.len() == 1,
        "multiple parents for the last module ? strange"
    );
    let parent = input.content[check].parents[0];

    let mut modules_to_monitor: Vec<usize> = vec![];
    match input.content[parent].module_type {
        ModuleType::FlipFlop => {
            unimplemented!("My input have a cunjunction parent, which make it easier")
        }
        ModuleType::Conjunction => modules_to_monitor.extend(
            // that's the important thing, you look for every parents that push in the cunjonction.
            input.content[input.content[check].parents[0]]
                .parents
                .clone(),
        ),
        ModuleType::Broadcast => {
            unreachable!("If the single parent is the broadcast, why even need this program...")
        }
        ModuleType::Unknown => unreachable!("Unknown parent type !"),
    }

    let mut counters = vec![0; modules_to_monitor.len()];
    let mut loop_counter = 0;
    loop {
        let result = press_button(input, Pulse::Low, &modules_to_monitor);
        loop_counter += 1;
        for (i, &v) in result.2.iter().enumerate() {
            if v {
                counters[i] = loop_counter;
            }
        }
        if counters.iter().all(|&p| p != 0) {
            return counters.iter().fold(1, |acc, &elem| lcm(acc, elem));
        }
    }
}

pub fn solve(buffer: &str) -> SolutionPair {
    let mut input = read_input(buffer);
    let mut input_copy = input.clone();
    let sol1: u64 = part_one(&mut input);
    let sol2: u64 = part_two(&mut input_copy);
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
