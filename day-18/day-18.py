from alive_progress import alive_it, alive_bar

def part1():
    print("="*50+"\nPART 1")
    tests = ((1, 0, 0), (-1, 0, 0), (0, 1, 0), (0, -1, 0), (0, 0, 1), (0, 0, -1))
    with open("day-18\day-18.txt", "r") as f:
        lines = f.read().splitlines()
        lines = [l.split(",") for l in lines]
        cubes = {(int(l[0]), int(l[1]), int(l[2])) for l in lines}
        count = len(cubes)*6
        for c in alive_it(cubes):
            for t in tests:
                if (c[0]+t[0], c[1]+t[1], c[2]+t[2]) in cubes:
                    count -= 1
        print("Nombre de surfaces non recouvertes ->", count)

def part2():
    print("="*50+"\nPART 2")
    tests = ((1, 0, 0), (-1, 0, 0), (0, 1, 0), (0, -1, 0), (0, 0, 1), (0, 0, -1))
    with open("day-18\day-18.txt", "r") as f:
        lines = f.read().splitlines()
        lines = [l.split(",") for l in lines]
        cubes = {(int(l[0]), int(l[1]), int(l[2])) for l in lines}
        max_x = max(map(lambda x : x[0], cubes))
        max_y = max(map(lambda x : x[1], cubes))
        max_z = max(map(lambda x : x[2], cubes))
        min_x = min(map(lambda x : x[0], cubes))
        min_y = min(map(lambda x : x[1], cubes))
        min_z = min(map(lambda x : x[2], cubes))
        water = set([(min_x - 1, min_y - 1, min_z - 1)])
        visited_water = set([(min_x - 1, min_y - 1, min_z - 1)])
        interesting_water = set()
        end = False
        with alive_bar(title = "Flooding") as bar:
            while not end:
                end = True
                temp = set()
                for w in water:
                    for t in tests:
                        act =  (w[0]+t[0], w[1]+t[1], w[2]+t[2])
                        if -1 <= act[0] <= max_x + 1 and -1 <= act[1] <= max_y + 1 and -1 <= act[2] <= max_z + 1:
                            if act not in visited_water and act not in cubes:
                                temp.add(act) # what we will check next
                                visited_water.add(act)
                                end = False
                            elif act in cubes:
                                interesting_water.add(w)
                water = temp
                bar()
        count = 0
        for c in alive_it(interesting_water, title="Checking"):
            for t in tests:
                if (c[0]+t[0], c[1]+t[1], c[2]+t[2]) in cubes:
                    count += 1

        print("Surface extérieure ->", count)


part1()
part2()