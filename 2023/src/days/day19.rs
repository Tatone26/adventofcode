use itertools::Itertools;
use rustc_hash::FxHashMap;

use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////

#[derive(Debug)]
struct Ratings {
    content: [u64; 4],
}

impl Ratings {
    fn get(&self, value: char) -> u64 {
        match value {
            'x' => self.content[0],
            'm' => self.content[1],
            'a' => self.content[2],
            _ => self.content[3],
        }
    }

    fn from_str(line: &str) -> Option<Self> {
        if let Some((extrem, musical, aero, shiny)) = line
            .split(['=', ',', '{', '}'])
            .filter(|s| !s.is_empty() && s.chars().next().unwrap().is_ascii_digit())
            .map(|s| s.parse::<u64>().unwrap())
            .collect_tuple()
        {
            Some(Ratings {
                content: [extrem, musical, aero, shiny],
            })
        } else {
            None
        }
    }

    fn result(&self) -> u64 {
        self.content.iter().sum()
    }
}

#[derive(Debug)]
enum Conclusion {
    Accepted,
    Rejected,
    Next(String),
}

impl Conclusion {
    fn from_str(buffer: &str) -> Self {
        match buffer {
            "A" => Conclusion::Accepted,
            "R" => Conclusion::Rejected,
            _ => Conclusion::Next(buffer.to_string()),
        }
    }
}

/// With: true => greater than.
#[derive(Debug)]
struct Operation {
    on: Option<char>,
    with: Option<bool>,
    to: Option<u64>,
    conclusion: Conclusion,
}

impl Operation {
    fn from_str(buffer: &str) -> Self {
        if !buffer.contains(':') {
            Operation {
                on: None,
                with: None,
                to: None,
                conclusion: Conclusion::from_str(buffer),
            }
        } else {
            let on = buffer.split(['>', '<']).next().unwrap();
            assert!(on.len() == 1);
            let with = buffer.contains('>');
            let to = buffer
                .split(['>', '<', ':'])
                .nth(1)
                .unwrap()
                .parse::<u64>()
                .unwrap_or_else(|_| {
                    println!("{buffer} trying to parse.");
                    panic!()
                });
            let conc = Conclusion::from_str(buffer.split([':']).nth(1).unwrap());
            Operation {
                on: Some(on.chars().next().unwrap()),
                with: Some(with),
                to: Some(to),
                conclusion: conc,
            }
        }
    }
}

type Workflows = FxHashMap<String, Vec<Operation>>;

fn read_input(buffer: &str) -> (Workflows, Vec<Ratings>) {
    let mut it = buffer.split("\n\n");

    // Reading workflows.
    let mut workflows = FxHashMap::default();
    it.next().unwrap().lines().for_each(|line| {
        let start = line.find('{').unwrap();
        let name: String = line[..start].to_string();
        let mut rules: Vec<Operation> = vec![];
        for x in line.strip_suffix('}').unwrap()[(start + 1)..].split(',') {
            rules.push(Operation::from_str(x));
        }
        workflows.insert(name, rules);
    });

    // Reading ratings.
    let ratings = it
        .next()
        .map(|g| {
            g.lines()
                .map(|line| Ratings::from_str(line).unwrap())
                .collect_vec()
        })
        .unwrap();
    (workflows, ratings)
}

fn aux(workflows: &Workflows, input: &Ratings, current: &str) -> u64 {
    if let Some(v) = workflows.get(current) {
        for w in v.iter() {
            if match w.with {
                Some(false) => input.get(w.on.unwrap()) < w.to.unwrap(),
                Some(true) => input.get(w.on.unwrap()) > w.to.unwrap(),
                None => true,
            } {
                return match &w.conclusion {
                    Conclusion::Accepted => input.result(),
                    Conclusion::Rejected => 0,
                    Conclusion::Next(s) => aux(workflows, input, s),
                };
            }
        }
    }
    // else
    0
}

fn part_one(workflows: &Workflows, input: &[Ratings]) -> u64 {
    input.iter().map(|r| aux(workflows, r, "in")).sum()
}

#[derive(Debug)]
struct RatingRange {
    content: [(u64, u64); 4],
}

impl RatingRange {
    fn get(&self, value: char) -> (u64, u64) {
        self.content[RatingRange::get_index(value)]
    }

    fn get_index(value: char) -> usize {
        match value {
            'x' => 0,
            'm' => 1,
            'a' => 2,
            _ => 3,
        }
    }

    fn result(&self) -> u64 {
        self.content.iter().map(|(x, y)| y - x + 1).product()
    }

    /// If no split, returns None. If split, just returns the two separated ones.
    /// The other logic is not here.
    fn split(&self, operator: &Operation) -> Option<[Self; 2]> {
        let index = RatingRange::get_index(operator.on?);
        let (lower, upper) = self.get(operator.on?);
        // No split.
        match operator.with {
            Some(false) => {
                if lower >= operator.to? || upper < operator.to? {
                    // no split, but operator = true.
                    return None;
                }
            }
            Some(true) => {
                if upper <= operator.to? || lower > operator.to? {
                    return None;
                }
            }
            _ => unreachable!(),
        }
        let mut new_content_1 = self.content;
        new_content_1[index] = (
            new_content_1[index].0,
            operator.to.unwrap() - if operator.with.unwrap() { 0 } else { 1 }, // oooh the off by one error I thought would not happen. Sad.
        );
        let mut new_content_2 = self.content;
        new_content_2[index] = (
            operator.to.unwrap() + if operator.with.unwrap() { 1 } else { 0 },
            new_content_2[index].1,
        );
        Some([
            RatingRange {
                content: new_content_1,
            },
            RatingRange {
                content: new_content_2,
            },
        ])
    }
}

fn aux2(workflows: &Workflows, input: &RatingRange, w: &str) -> u64 {
    let current_workflow = workflows.get(w);
    if current_workflow.is_none() {
        return 0;
    }
    for op in current_workflow.unwrap().iter() {
        if let Some(next) = input.split(op) {
            return next.iter().map(|t| aux2(workflows, t, w)).sum();
        } else {
            let conc = match op.with {
                Some(false) => {
                    if input.get(op.on.unwrap()).1 < op.to.unwrap() {
                        Some(&op.conclusion)
                    } else {
                        None
                    }
                }
                Some(true) => {
                    if input.get(op.on.unwrap()).0 > op.to.unwrap() {
                        Some(&op.conclusion)
                    } else {
                        None
                    }
                }
                None => Some(&op.conclusion),
            };
            match conc {
                Some(Conclusion::Accepted) => return input.result(),
                Some(Conclusion::Rejected) => return 0,
                Some(Conclusion::Next(x)) => return aux2(workflows, input, x),
                None => (),
            }
        }
    }
    0
}

fn part_two(workflows: &Workflows) -> u64 {
    let range = RatingRange {
        content: [(1, 4000), (1, 4000), (1, 4000), (1, 4000)],
    };
    aux2(workflows, &range, "in")
}

pub fn solve(buffer: &str) -> SolutionPair {
    let input = read_input(buffer);
    let sol1: u64 = part_one(&input.0, &input.1);
    let sol2: u64 = part_two(&input.0);

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day19.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 19114);
    assert_eq!(s2, 167409079868000);
}
