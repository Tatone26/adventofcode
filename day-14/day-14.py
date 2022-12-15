def part1():

    with open("day-14.txt","r") as f:
        lines = [l.strip("\n").split(" -> ") for l in f.readlines()]

        # get a set with all walls represented by (x, y)
        walls = set()
        for wall in lines:
            for i in range(len(wall) - 1):
                start = list(map(int, wall[i].split(",")))
                end = list(map(int, wall[i+1].split(",")))
                if start[0] == end[0]:
                    for y in range(min(start[1], end[1]), max(start[1], end[1])+1):
                        walls.add((start[0], y))
                elif start[1] == end[1]:
                    for x in range(min(start[0], end[0]), max(start[0], end[0])+1):
                        walls.add((x, start[1]))

        y = {s[1] for s in walls}
        maxy = max(y)
        
        count = 0
        while True:
            sand_pos = [500, 0]
            while sand_pos[1] < maxy + 3:
                if (sand_pos[0], sand_pos[1] + 1) not in walls:
                    sand_pos[1] += 1
                elif (sand_pos[0] - 1, sand_pos[1] + 1) not in walls:
                    sand_pos[0] -= 1
                    sand_pos[1] += 1
                elif (sand_pos[0] + 1, sand_pos[1] + 1) not in walls:
                    sand_pos[0] += 1
                    sand_pos[1] += 1
                else:
                    walls.add((sand_pos[0], sand_pos[1]))
                    break
            if sand_pos[1] > maxy + 1:
                break
            count += 1
    print("Nombre de pieces of sand that comes to rest before going down to infinity : ", count)


def part2():

    with open("day-14.txt","r") as f:
        lines = [l.strip("\n").split(" -> ") for l in f.readlines()]

        # get a set with all walls represented by (x, y)
        walls = set()
        for wall in lines:
            for i in range(len(wall) - 1):
                start = list(map(int, wall[i].split(",")))
                end = list(map(int, wall[i+1].split(",")))
                if start[0] == end[0]:
                    for y in range(min(start[1], end[1]), max(start[1], end[1])+1):
                        walls.add((start[0], y))
                elif start[1] == end[1]:
                    for x in range(min(start[0], end[0]), max(start[0], end[0])+1):
                        walls.add((x, start[1]))

        y_set = {s[1] for s in walls}
        maxy = max(y_set)

        floor = maxy + 2
        
        count = 0
        while True:
            sand_pos = [500, 0]
            while sand_pos[1] < floor + 1:
                if (sand_pos[0], sand_pos[1] + 1) not in walls:
                    sand_pos[1] += 1
                elif (sand_pos[0] - 1, sand_pos[1] + 1) not in walls:
                    sand_pos[0] -= 1
                    sand_pos[1] += 1
                elif (sand_pos[0] + 1, sand_pos[1] + 1) not in walls:
                    sand_pos[0] += 1
                    sand_pos[1] += 1
                else:
                    walls.add((sand_pos[0], sand_pos[1]))
                    break
                if sand_pos[1] == floor - 1:
                    walls.add((sand_pos[0], sand_pos[1]))
                    break
            if sand_pos == [500, 0]:
                break
            count += 1

    print("Nombre de grains de sable avant d'arriver à (500, 0) :", count + 1)

part1()
part2()
