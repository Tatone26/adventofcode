from alive_progress import alive_it, alive_bar
import multiprocessing.dummy as dummy

def part1(y):

    with open("day-15\day-15.txt", "r") as f:

        lines = f.readlines()
        split = [line[12:].strip("\n").split(", y=") for line in lines]
        for act in split:
            act[1] = act[1].split(": closest beacon is at x=")

        sensors = [((int(x[0]), int(x[1][0])), (int(x[1][1]), int(x[2]))) for x in split]

        cannot_contain_beacon = set()
        beacons_and_sensors = set()

        covered_ranges = set()

        for s in sensors:

            cannot_contain_beacon.add(s[0])
            cannot_contain_beacon.add(s[1])
            beacons_and_sensors.add(s[0])
            beacons_and_sensors.add(s[1])


        for s in alive_it(sensors, title = "Part 1"):
            distance_max = abs(s[0][0] - s[1][0]) + abs(s[0][1] - s[1][1])
            shortest_distance_to_y =  abs(s[0][1] - y)
            if shortest_distance_to_y <= distance_max: # could be mathematically optimised a LOT
                delta_x = distance_max - shortest_distance_to_y
                covered_ranges.add((s[0][0] - delta_x, s[0][0] + delta_x))
                
        left = set(map(lambda x: x[0], covered_ranges))
        right = set(map(lambda x : x[1], covered_ranges))
        left_most = min(left)
        right_most = max(right)
        count = abs(right_most) + abs(left_most)
        print("->", count)


part1(2000000)


def part2(maxy):

    with open("day-15\day-15.txt", "r") as f:

        lines = f.readlines()
        split = [line[12:].strip("\n").split(", y=") for line in lines]
        for act in split:
            act[1] = act[1].split(": closest beacon is at x=")

        sensors = [((int(x[0]), int(x[1][0])), (int(x[1][1]), int(x[2]))) for x in split]
        cannot_contain_beacon = set()
        beacons_and_sensors = set()

        for s in sensors:

            cannot_contain_beacon.add(s[0])
            cannot_contain_beacon.add(s[1])
            beacons_and_sensors.add(s[0])
            beacons_and_sensors.add(s[1])
        
        with alive_bar(len(sensors)) as bar:

            def search(s):
                bar()
                distance_max = abs(s[0][0] - s[1][0]) + abs(s[0][1] - s[1][1]) + 1
                for x in range(-distance_max, distance_max + 1):
                    tests = set()
                    tests.add((s[0][0] + x, s[0][1] + (distance_max - x)))
                    tests.add((s[0][0] + x, s[0][1] - (distance_max + x)))
                    for t in tests:
                        if 0 <= t[0] <= maxy and 0 <= t[1] <= maxy:
                            found = True
                            for s2 in sensors:
                                distance_max_2 = abs(s2[0][0] - s2[1][0]) + abs(s2[0][1] - s2[1][1])
                                if abs(s2[0][0] - t[0]) + abs(s2[0][1] - t[1]) <= distance_max_2:
                                    found = False
                                    break
                            if found:
                                print("Une case a été trouvée :", t, "; soit un résultat de :", t[0] * 4000000 + t[1])
                                return # change to break to test every single possible position

            p = dummy.Pool(4)
            p.map(search, sensors)
            p.close()

part2(4000000)


# LETS GOOO JE L'AI TROUVE JUSTE APRES LE 6e SENSOR LETS GOOO