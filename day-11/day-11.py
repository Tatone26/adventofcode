import numpy as np
import math
from alive_progress import alive_it


class Monkey:

    def __init__(self, items, operation, test, monkey1, monkey2):
        self.items = items
        self.operation = operation
        self.test = test
        self.monkey1 = monkey1
        self.monkey2 = monkey2

    def __repr__(self):
        return str(self.items) + ", " + str(self.operation) + ", " + str(self.test) + ", " + str(self.monkey1) + ", " + str(self.monkey2)

    def use_operation(self, x):
        actions = self.operation.copy()
        for i in range(3):
            if actions[i] == "x":
                actions[i] = x
        if actions[1] == "*":
            return np.multiply(np.uint64(actions[0]), np.uint64(actions[2]), dtype=np.uint64)
        else:
            return np.add(np.uint64(actions[0]), np.uint64(actions[2]), dtype=np.uint64)

    def add_item(self, x):
        self.items = np.append(self.items, x)

def get_monkeys():
    with open("day-11\day-11.txt", "r") as f:
        lines = f.readlines()
        monkeys = []
        test_items = np.empty(0, dtype = np.int64)
        test_operation = ""
        test_test = 0
        test_monkey1 = 0
        test_monkey2 = 0
        for line in lines:
            if len(line) <= 5:
                monkeys.append(Monkey(test_items.copy(), test_operation,
                            test_test, test_monkey1, test_monkey2))
                test_items = np.empty(0, dtype=np.uint64)
                test_operation = []
                test_test = 0
                test_monkey1 = 0
                test_monkey2 = 0
            else:
                if "Starting items:" in line:
                    its = line[18:].strip().split(", ")
                    for i in its:
                        test_items = np.append(test_items, int(i))
                elif "Operation:" in line:
                    x = line[19:-1]
                    test_operation = x.split(" ")
                    for i in range(0, 3, 2):
                        if test_operation[i] == "old":
                            test_operation[i] = "x"
                        else:
                            test_operation[i] = np.uint64(test_operation[i])
                elif "Test:" in line:
                    test_test = int(line[21:-1])
                elif "If true:" in line:
                    test_monkey1 = int(line[29:-1])
                elif "If false:" in line:
                    test_monkey2 = int(line[30:-1])
        return monkeys

monkeys = get_monkeys()
np_monkeys = np.array(monkeys)

def part1(monkeys):
    count = [0 for i in range(len(monkeys))]
    for i in alive_it(range(20), title = "Part 1"):
        for j in range(len(monkeys)):
            m = monkeys[j]
            for item in m.items:
                count[j] += 1
                worry_test = m.use_operation(item)//3
                if worry_test % m.test == 0:
                    monkeys[m.monkey1].add_item(worry_test)
                else:
                    monkeys[m.monkey2].add_item(worry_test)
            m.items = np.array([])
    count_test = np.array(count)
    max_ind = np.argmax(count_test)
    print("The monkey business after 20 rounds is: ", count_test[max_ind]*max(count[:max_ind]+count[max_ind+1:]))


def part2(monkeys):
    count = [0 for i in range(len(monkeys))]
    modulo = math.lcm(*map(lambda m: m.test, monkeys))
    for i in alive_it(range(10000), title = "Part 2"):
        #print(i)
        for j in range(len(monkeys)):
            m = monkeys[j]
            count[j] += len(m.items)
            for item in m.items:
                worry_test = m.use_operation(item)
                if worry_test % m.test == 0:
                    monkeys[m.monkey1].add_item(np.uint64(worry_test % modulo))
                else:
                    monkeys[m.monkey2].add_item(np.uint64(worry_test % modulo))
            m.items = np.empty(0, dtype = np.uint64)
    count_test = np.array(count)
    max1 = np.max(count_test)
    count_test = np.delete(count_test, np.argmax(count_test))
    max2 = np.max(count_test)
    print("The monkey business after 10000 rounds is: ", int(max1)*int(max2))

part1(monkeys)
part2(np_monkeys)