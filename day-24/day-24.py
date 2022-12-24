from alive_progress import alive_bar, alive_it

def part1():

    print("="*50+"\nPART 1")
    with open("day-24/day-24.txt", "r") as f:
        lines = f.read().splitlines()
        bliz = set()
        for y in range(len(lines)):
            for x in range(len(lines[y])):
                temp = lines[y][x]
                if temp == ">":
                    bliz.add((x, y, 1, 0))
                elif temp == "<":
                    bliz.add((x, y, -1, 0))
                elif temp == "v":
                    bliz.add((x, y, 0, 1))
                elif temp == "^":
                    bliz.add((x, y, 0, -1))
        maxx = len(lines[0]) - 2
        maxy = len(lines) - 2
        print(maxx, maxy)
        start = (1, 0)
        goal = (maxx, maxy + 1)
        directions = [(1, 0), (-1, 0), (0, 1), (0, -1), (0, 0)]
        goals = [goal, start, goal]
        indice = 0
        with alive_bar(title = "Searching") as bar:
            i = 0
            positions = set([start])
            while True:
                next_bliz = set()
                for b in bliz:
                    new_pos = (b[0] + b[2], b[1] + b[3])
                    if 1 <= new_pos[0] <= maxx and 1 <= new_pos[1] <= maxy:
                        next_bliz.add((new_pos[0], new_pos[1], b[2], b[3]))
                    else:
                        if b[2] == 1:
                            next_bliz.add((1, b[1], b[2], b[3]))
                        elif b[2] == -1:
                            next_bliz.add((maxx, b[1], b[2], b[3]))
                        elif b[3] == 1:
                            next_bliz.add((b[0], 1, b[2], b[3]))
                        elif b[3] == -1:
                            next_bliz.add((b[0], maxy, b[2], b[3]))
                bliz = next_bliz
                temp = set(map(lambda x : (x[0], x[1]), bliz))
                next_positions = set()
                for p in positions:
                    for d in directions:
                        t = (p[0] + d[0], p[1] + d[1])
                        if 1 <= t[0] <= maxx and 1 <= t[1] <= maxy and t not in temp:
                            next_positions.add(t)
                        elif t[0] == maxx and t[1] == maxy + 1:
                            next_positions.add(t)
                        elif t == start:
                            next_positions.add(t)
                if len(next_positions) == 0:
                    print("ERROR")
                    break
                positions = next_positions
                if goals[indice] in positions:
                    indice += 1
                    positions = set([goals[indice - 1]])
                    if indice == len(goals):
                        i += 1
                        bar()
                        break
                i += 1
                bar()
            print("->", i)


part1()