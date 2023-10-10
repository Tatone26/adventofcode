from alive_progress import alive_it, alive_bar

NORTH = (0, -1)
SOUTH = (0, 1)
WEST = (-1, 0)
EAST = (1, 0)


def must_move(e, elves):
    return (e[0] + 1, e[1] + 1) in elves or (e[0], e[1] + 1) in elves or (e[0] - 1, e[1] + 1) in elves or (e[0] - 1, e[1]) in elves or (e[0] - 1, e[1] - 1) in elves or (e[0], e[1] - 1) in elves or (e[0] + 1, e[1] - 1) in elves or (e[0] + 1, e[1]) in elves

def can_move(e, elves, d):
    if d == NORTH:
        if (e[0] - 1, e[1] - 1) not in elves and (e[0], e[1] - 1) not in elves and (e[0] + 1, e[1] - 1) not in elves:
            return True
        return False
    elif d  == SOUTH:
        if (e[0] - 1, e[1] + 1) not in elves and (e[0], e[1] + 1) not in elves and (e[0] + 1, e[1] + 1) not in elves:
            return True
        return False
    elif d == WEST:
        if (e[0] - 1, e[1] - 1) not in elves and (e[0] - 1, e[1]) not in elves and (e[0] - 1, e[1] + 1) not in elves:
            return True
        return False
    elif d == EAST:
        if (e[0] + 1, e[1] - 1) not in elves and (e[0] + 1, e[1]) not in elves and (e[0] + 1, e[1] + 1) not in elves:
            return True
        return False
    else:
        print("pb avec directions")
        raise Exception

def part1():

    print("="*50+"\nPART 1")
    with open("day-23/day-23.txt", "r") as f:
        lines = f.read().splitlines()
        elves = set()
        for y in range(len(lines)):
            for x in range(len(lines[y])):
                if lines[y][x] == "#":
                    elves.add((x, y))
        directions = [NORTH, SOUTH, WEST, EAST]
        for _ in alive_it(range(10), title = "Simulating"):
            next_positions = dict()
            tests = dict()
            for e in elves:
                if must_move(e, elves):
                    for d in directions:
                        if can_move(e, elves, d):
                            temp = (e[0] + d[0], e[1] + d[1])
                            next_positions[e] = temp
                            if temp not in tests:
                                tests[temp] = 1
                            else:
                                tests[temp] += 1
                            break
            temp = set()
            for e in elves:
                if e in next_positions and tests[next_positions[e]] == 1:
                    temp.add(next_positions[e])
                    continue
                temp.add(e)
            elves = temp
            directions.append(directions.pop(0))
        minx = min(set(map(lambda x : x[0], elves)))
        maxx = max(set(map(lambda x : x[0], elves)))
        miny = min(set(map(lambda x : x[1], elves)))
        maxy = max(set(map(lambda x : x[1], elves)))
        width = maxx - minx + 1
        height = maxy - miny + 1
        print("->", (width*height)-len(elves))
        # for y in range(miny, maxy + 1):
        #     line = ""
        #     for x in range(minx, maxx + 1):
        #         if (x, y) in elves:
        #             line = line + "#"
        #         else:
        #             line = line + "."
        #     print(line)

def part2():

    print("="*50+"\nPART 2")
    with open("day-23/day-23.txt", "r") as f:
        lines = f.read().splitlines()
        elves = set()
        for y in range(len(lines)):
            for x in range(len(lines[y])):
                if lines[y][x] == "#":
                    elves.add((x, y))
        directions = [NORTH, SOUTH, WEST, EAST]
        i = 1
        with alive_bar(title = "Simulating") as bar:
            while True:
                next_positions = dict()
                tests = dict()
                for e in elves:
                    if must_move(e, elves):
                        for d in directions:
                            if can_move(e, elves, d):
                                temp = (e[0] + d[0], e[1] + d[1])
                                next_positions[e] = temp
                                if temp not in tests:
                                    tests[temp] = 1
                                else:
                                    tests[temp] += 1
                                break
                temp = set()
                moved = False
                for e in elves:
                    if e in next_positions and tests[next_positions[e]] == 1:
                        temp.add(next_positions[e])
                        moved = True
                        continue
                    temp.add(e)
                if not moved:
                    break
                elves = temp
                directions.append(directions.pop(0))
                i += 1
                bar()
        print("->", i)

part1()
part2()