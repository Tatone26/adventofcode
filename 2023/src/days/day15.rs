use crate::{Solution, SolutionPair};

///////////////////////////////////////////////////////////////////////////////

fn hash(text: &str) -> u64 {
    text.chars()
        .fold(0, |acc, elem| ((acc + elem as u64) * 17) % 256)
}

type Lens<'a> = (&'a str, u64);

fn part_two<'a>(input: impl Iterator<Item = &'a str>) -> u64 {
    let mut lens_boxes: Vec<Vec<Lens>> = vec![Vec::with_capacity(10); 256];
    'out: for line in input {
        let mut it = line.split(['-', '=']);
        if it.clone().count() > 2 {
            panic!("NOTGOODNOTGOODNOTGOOD");
        }
        let label = it.next().unwrap();
        let box_to_use = lens_boxes.get_mut(hash(label) as usize).unwrap();
        if line.contains('-') {
            for i in 0..box_to_use.len() {
                if box_to_use[i].0 == label {
                    box_to_use.remove(i);
                    continue 'out;
                }
            }
        } else if line.contains('=') {
            for b in box_to_use.iter_mut() {
                if b.0 == label {
                    *b = (label, it.next().unwrap().parse::<u64>().unwrap());
                    continue 'out;
                }
            }
            // If it was not present in the box
            box_to_use.push((label, it.next().unwrap().parse::<u64>().unwrap()));
        }
    }

    lens_boxes
        .iter()
        .enumerate()
        .flat_map(|(i, b)| {
            b.iter()
                .enumerate()
                .map(move |(j, e)| (i as u64 + 1) * (j as u64 + 1) * e.1)
        })
        .sum()
}

pub fn solve(buffer: &str) -> SolutionPair {
    let input = buffer.lines().next().unwrap().split(',');

    let sol1: u64 = input.clone().map(hash).sum();
    let sol2: u64 = part_two(input);

    (Solution::from(sol1), Solution::from(sol2))
}

#[test]
fn test() {
    let (Solution::U64(s1), Solution::U64(s2)) =
        solve(&std::fs::read_to_string("input/test_day15.txt").unwrap())
    else {
        panic!("Mauvais type de solutions pour le test.")
    };
    assert_eq!(s1, 1320);
    assert_eq!(s2, 145);
}
