use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////

/// Pulse from Name
/* #[derive(Debug, Clone)]
enum Pulse {
    Low(String),
    High(String),
}

#[derive(Debug, Clone)]
enum ModuleType {
    FlipFlop(bool),
    Conjuction(Vec<Pulse>),
    Broadcast,
}

#[derive(Debug)]
struct Module {
    name: String,
    module_type: ModuleType,
    next_modules: Vec<String>,
}

impl Module {
    fn receive_pulse(&mut self, p: Pulse) -> Option<Pulse> {
        match (p, self.module_type.clone()) {
            (Pulse::Low(_), ModuleType::FlipFlop(true)) => {
                self.module_type = ModuleType::FlipFlop(false);
                Some(Pulse::Low(self.name.clone()))
            }
            (Pulse::Low(_), ModuleType::FlipFlop(false)) => {
                self.module_type = ModuleType::FlipFlop(true);
                Some(Pulse::High(self.name.clone()))
            }
            (Pulse::High(n), ModuleType::Conjuction(v)) => {
                let new_values;
                if v.iter()
                    .find(|b| match b {
                        Pulse::High(s) | Pulse::Low(s) => s.eq(&n),
                    })
                    .is_some()
                {
                    new_values = v
                        .iter()
                        .map(|p| {
                            if match p {
                                Pulse::High(u) | Pulse::Low(u) => u.eq(&n),
                            } {
                                Pulse::High(n.to_string())
                            } else {
                                p.clone()
                            }
                        })
                        .collect_vec();
                } else {
                    let mut temp = v.clone();
                    temp.push(Pulse::High(n.to_string()));
                    new_values = temp;
                }
                self.module_type = ModuleType::Conjuction(new_values.clone());
                if new_values.iter().all(|s| matches!(s, Pulse::High(_))) {
                    Some(Pulse::Low(self.name.clone()))
                } else {
                    Some(Pulse::High(self.name.clone()))
                }
            }
            (Pulse::Low(n), ModuleType::Conjuction(v)) => {
                let new_values;
                if v.iter()
                    .find(|b| match b {
                        Pulse::High(s) | Pulse::Low(s) => s.eq(&n),
                    })
                    .is_some()
                {
                    new_values = v
                        .iter()
                        .map(|p| {
                            if match p {
                                Pulse::High(u) | Pulse::Low(u) => u.eq(&n),
                            } {
                                Pulse::Low(n.to_string())
                            } else {
                                p.clone()
                            }
                        })
                        .collect_vec();
                } else {
                    let mut temp = v.clone();
                    temp.push(Pulse::Low(n.to_string()));
                    new_values = temp;
                }
                self.module_type = ModuleType::Conjuction(new_values.clone());

                if new_values.iter().all(|s| matches!(s, Pulse::High(_))) {
                    Some(Pulse::Low(self.name.clone()))
                } else {
                    Some(Pulse::High(self.name.clone()))
                }
            }
            (_, ModuleType::Broadcast) => Some(Pulse::Low(self.name.clone())),
            (_, _) => None,
        }
    }
}

fn read_input(buffer: &str) -> Vec<Module> {
    buffer
        .lines()
        .map(|line| {
            let (name, outputs) = line.split(" -> ").collect_tuple().unwrap();

            let next_modules = outputs.split(", ").map(|t| t.to_string()).collect_vec();
            if name == "broadcaster" {
                Module {
                    name: "broadcaster".to_string(),
                    module_type: ModuleType::Broadcast,
                    next_modules,
                }
            } else {
                let module_type;
                let module_name;
                if name.starts_with('%') {
                    module_type = ModuleType::FlipFlop(false);
                    module_name = name.strip_prefix('%').unwrap().to_string();
                } else {
                    module_name = name.strip_prefix('&').unwrap().to_string();
                    module_type = ModuleType::Conjuction(vec![]);
                }
                Module {
                    name: module_name,
                    module_type,
                    next_modules,
                }
            }
        })
        .collect_vec()
}

fn initialise_input(input: &mut Vec<Module>) {
    for j in 0..input.len() {
        let name = input[j].name.clone();
        for i in input[j].next_modules.clone().iter() {
            input.iter_mut().find(|p| p.name.eq(i)).and_then(|p| {
                println!("{p:?} receives pulse from {name}");
                p.receive_pulse(Pulse::Low(name.clone()))
            });
        }
    }
} */

/* fn push_button(input: &mut Vec<Module>) -> (u64, u64) {
    let mut count_low: u64 = 0;
    let mut count_high: u64 = 0;

    let pulses_to_do = vec![input
        .iter()
        .find(|p| matches!(p.module_type, ModuleType::Broadcast))
        .unwrap()
        .next_modules
        .iter()
        .map(|s| Pulse::Low(()))];

    (count_low, count_high)
} */

pub fn solve(_buffer: &str) -> SolutionPair {
    // Your solution here...
    /* let mut input = read_input(buffer);
    initialise_input(&mut input);
    println!("{input:#?}"); */
    let sol1: u64 = 0;
    let sol2: u64 = 0;

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
