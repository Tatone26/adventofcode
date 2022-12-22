from alive_progress import alive_it

def get_next_direction(dir, letter):

    if letter == "R":
        if dir[0] == -1:
            return [0, -1]
        elif dir[0] == 1:
            return [0, 1]
        elif dir[1] == -1:
            return [1, 0]
        elif dir[1] == 1:
            return [-1, 0]
    elif letter == "L":
        if dir[0] == 1:
            return [0, -1]
        elif dir[0] == -1:
            return [0, 1]
        elif dir[1] == 1:
            return [1, 0]
        elif dir[1] == -1:
            return [-1, 0]

def part1():

    print("="*50+"\nPART 1")
    with open("day-22/day-22.txt", "r") as f:
        lines = f.read().splitlines()
        laby = dict()
        end_laby = 0
        for l in range(len(lines)):
            if len(lines[l]) > 0:
                laby[l] = dict()
                for c in range(len(lines[l])):
                    if lines[l][c] == ".":
                        laby[l][c] = 0
                    elif lines[l][c] == "#":
                        laby[l][c] = 1
            else:
                end_laby = l
                break
        actions = lines[end_laby + 1]
        act = []
        temp = ""
        for a in actions:
            if a.isdigit():
                temp = temp + a
            else:
                act.append([int(temp), a])
                temp = ""
        act.append([int(temp)])
        pos = [min(laby[0]), 0]
        direction = [1, 0]
        for d in alive_it(act, title = "moving"):
            for _ in range(d[0]):
                new_pos = [pos[0] + direction[0], pos[1] + direction[1]]
                if new_pos[1] in laby and new_pos[0] in laby[new_pos[1]]:
                    if laby[new_pos[1]][new_pos[0]] == 0:
                        pos = new_pos
                else:
                    if abs(direction[0]) > 0:
                        x = pos[0]
                        while x in laby[pos[1]]:
                            x = x - direction[0]
                        if laby[pos[1]][x + direction[0]] == 0:
                            pos = [x + direction[0], pos[1]]
                    elif abs(direction[1]) > 0:
                        y = pos[1]
                        while y in laby and pos[0] in laby[y]:
                            y = y - direction[1]
                        if laby[y + direction[1]][pos[0]] == 0:
                            pos = [pos[0], y + direction[1]]
            if len(d) > 1:
                direction = get_next_direction(direction, d[1])
        score = 0
        if direction[1] == 1:
            score = 1
        elif direction[0] == -1:
            score = 2
        elif direction[1] == -1:
            score = 3
        print("->", 1000*(pos[1] + 1) + 4*(pos[0] + 1) + score)

up_map = dict()
for x in range(49, 100):
    up_map[(x, 0)] = (0, x + 100, (1, 0))
for x in range(100, 150):
    up_map[(x, 0)] = (x - 100, 199, (0, -1))
for x in range(0, 50):
    up_map[(x, 100)] = (50, 50 + x, (1, 0))

down_map = dict()
for x in range(100, 150):
    down_map[(x, 49)] = (99, x - 50, (-1, 0))
for x in range(50, 100):
    down_map[(x, 149)] = (49, x + 100, (-1, 0))
for x in range(0, 50):
    down_map[(x, 199)] = (x + 100, 0, (0, 1))

right_map = dict()
for y in range(0, 50):
    right_map[(149, y)] = (99, 149 - y, (-1, 0))
for y in range(50, 100):
    right_map[(99, y)] = (y + 50, 49, (0, -1))
for y in range(100, 150):
    right_map[(99, y)] = (149, 149 - y, (-1, 0))
for y in range(150, 200):
    right_map[(49, y)] = (y - 100, 149, (0, -1))

left_map = dict()
for y in range(0, 50):
    left_map[(50, y)] = (0, 149 - y, (1, 0))
for y in range(50, 100):
    left_map[(50, y)] = (y - 50, 100, (0, 1))
for y in range(100, 150):
    left_map[(0, y)] = (50, 149 - y, (1, 0))
for y in range(150, 200):
    left_map[(0, y)] = (y - 100, 0, (0, 1))

def part2():

    print("="*50+"\nPART 2")
    print("WARNING : HARDCODED FOR REAL INPUT")
    with open("day-22/day-22.txt", "r") as f:
        lines = f.read().splitlines()
        laby = dict()
        end_laby = 0
        for l in range(len(lines)):
            if len(lines[l]) > 0:
                laby[l] = dict()
                for c in range(len(lines[l])):
                    if lines[l][c] == ".":
                        laby[l][c] = 0
                    elif lines[l][c] == "#":
                        laby[l][c] = 1
            else:
                end_laby = l
                break
        actions = lines[end_laby + 1]
        act = []
        temp = ""
        for a in actions:
            if a.isdigit():
                temp = temp + a
            else:
                act.append([int(temp), a])
                temp = ""
        act.append([int(temp)])
        pos = [50, 0]
        direction = [1, 0]
        for d in alive_it(act, title = "moving"):
            for _ in range(d[0]):
                new_pos = [pos[0] + direction[0], pos[1] + direction[1]]
                if new_pos[1] in laby and new_pos[0] in laby[new_pos[1]]:
                    if laby[new_pos[1]].get(new_pos[0]) == 0:
                        pos = new_pos
                else:
                    t = None
                    if direction == [-1, 0]:
                        t = left_map[tuple(pos)]
                    elif direction == [1, 0]:
                        t = right_map[tuple(pos)]
                    elif direction == [0, 1]:
                        t = down_map[tuple(pos)]
                    elif direction == [0, -1]:
                        t = up_map[tuple(pos)]
                    if laby[t[1]][t[0]] == 0:
                        temp = pos
                        pos = [t[0], t[1]]
                        direction = list(t[2])
                    

            if len(d) > 1:
                direction = get_next_direction(direction, d[1])
        score = 0
        if direction[1] == 1:
            score = 1
        elif direction[0] == -1:
            score = 2
        elif direction[1] == -1:
            score = 3
        print("->", 1000*(pos[1] + 1) + 4*(pos[0] + 1) + score)

part1()
part2()