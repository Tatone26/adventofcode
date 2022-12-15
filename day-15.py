def part1(y):

    with open("day-15.txt", "r") as f:

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

        i = 0
        for s in sensors:
            distance_max = abs(s[0][0] - s[1][0]) + abs(s[0][1] - s[1][1])
            if s[0][1] - distance_max - 1 <= y <= s[0][1] + distance_max + 1:
                for x in range(s[0][0] - distance_max - 2, s[0][0] + distance_max + 2):
                    if abs(s[0][0] - x) + abs(s[0][1] - y) <= distance_max  :
                        cannot_contain_beacon.add((x, y))
            i += 1
            print(i, len(sensors))

        count = len(set(filter(lambda x : x not in beacons_and_sensors, cannot_contain_beacon)))

        print(count)


#part1(2000000)


def part2(maxy):

    with open("day-15.txt", "r") as f:

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

        it_could_be_here = set()
        
        j = 0
        for s in sensors:
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
                            print(t)
                            it_could_be_here.add(t)
                            if len(it_could_be_here) > 1:
                                print("F*CK")
                                return
                            break
            j += 1
            print(j, len(sensors))

    res = list(it_could_be_here)[0]
    print(res[0]*4000000 + res[1])


part2(4000000)


# LETS GOOO JE L'AI TROUVE JUSTE APRES LE 6e SENSOR LETS GOOO