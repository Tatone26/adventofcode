from alive_progress import alive_bar, alive_it

def part1():

    print("="*50+"\nPART 1")
    with open("day-19/day-19.txt", "r") as f:
        lines = f.read().splitlines()
        initial_tuple = (24, 0, 1, 0, 0, 0, 0, 0, 0)
        # steps_left, ore, ore_robots, clay, clay_rob, obi, obi_rob, opened, geode_rob
        list_scores = []
        for line in alive_it(lines):
            l = line.split(": ")
            l = l[1].split(". ")
            ore_robot_cost = int(l[0][-5])
            clay_robot_cost = int(l[1][-5])
            ol = l[2].split(" ore and ")
            obsidian_robot_cost = (int(ol[0][-1]), int(ol[1][:2].strip(" ")))
            gl = l[3].split(" ore and ")
            geode_robot_cost = (int(gl[0][-1]), int(gl[1][:2].strip(" ")))
            to_try = set([initial_tuple])
            max_result = 0
            max_ore_cost = max(clay_robot_cost, obsidian_robot_cost[0], geode_robot_cost[0])
            for i in range(24):
                next_to_try = set()
                for t in to_try:
                    if t[7]+t[8]*t[0] < max_result:
                        continue
                    max_result = max(max_result, t[7]+t[8]*t[0])
                    tl = list(t)
                    tl[0] -= 1
                    tl[1] += tl[2]
                    tl[3] += tl[4]
                    tl[5] += tl[6]
                    tl[7] += tl[8]
                    if tl[1] - tl[2] >= geode_robot_cost[0] and tl[5] - tl[6] >= geode_robot_cost[1]:
                        temp = tl.copy()
                        temp[1] -= geode_robot_cost[0]
                        temp[5] -= geode_robot_cost[1]
                        temp[8] += 1
                        next_to_try.add(tuple(temp))
                    if tl[6] < geode_robot_cost[1] and tl[1] - tl[2] >= obsidian_robot_cost[0] and tl[3] - tl[4] >= obsidian_robot_cost[1]:
                        temp = tl.copy()
                        temp[1] -= obsidian_robot_cost[0]
                        temp[3] -= obsidian_robot_cost[1]
                        temp[6] += 1
                        next_to_try.add(tuple(temp))
                    if tl[4] < obsidian_robot_cost[1] and tl[1] - tl[2] >= clay_robot_cost:
                        temp = tl.copy()
                        temp[1] -= clay_robot_cost
                        temp[4] += 1
                        next_to_try.add(tuple(temp))
                    if tl[2] < max_ore_cost and tl[1] - tl[2] >= ore_robot_cost:
                        temp = tl.copy()
                        temp[1] -= ore_robot_cost
                        temp[2] += 1
                        next_to_try.add(tuple(temp))
                    next_to_try.add(tuple(tl))  # do_nothing
                to_try = next_to_try.copy()
            maxi_score = max(set(map(lambda x: x[7], to_try)))
            list_scores.append(maxi_score)
        score = 0
        for i in range(len(list_scores)):
            score += list_scores[i]*(i+1)
        print("->", score)
        print(list_scores)

def part2():

    print("="*50+"\nPART 2")
    with open("day-19/day-19.txt", "r") as f:
        lines = f.read().splitlines()[:3]
        initial_tuple =  (0, 1, 0, 0, 0, 0, 0, 0)
        # steps_left, ore, ore_robots, clay, clay_rob, obi, obi_rob, opened, geode_rob
        list_scores = []
        for line in lines:
            l = line.split(": ")
            l = l[1].split(". ")
            ore_robot_cost = int(l[0][-5])
            clay_robot_cost = int(l[1][-5])
            ol = l[2].split(" ore and ")
            obsidian_robot_cost = (int(ol[0][-1]), int(ol[1][:2].strip(" ")))
            gl = l[3].split(" ore and ")
            geode_robot_cost = (int(gl[0][-1]), int(gl[1][:2].strip(" ")))
            to_try = set([initial_tuple])
            max_result = 0
            max_ore_cost = max(clay_robot_cost, obsidian_robot_cost[0], geode_robot_cost[0])
            visited = set()
            for i in alive_it(range(32)):
                next_to_try = set()
                for t in to_try:
                    visited.add(t)
                    if t[6]+(t[7]+(32-i))*(32-i) < max_result:
                        continue
                    tl = list(t)
                    tl[0] += tl[1]
                    tl[2] += tl[3]
                    tl[4] += tl[5]
                    tl[6] += tl[7]
                    can_create_all_robots = True
                    if tl[0] - tl[1] >= geode_robot_cost[0] and tl[4] - tl[5] >= geode_robot_cost[1]:
                        temp = tl.copy()
                        temp[0] -= geode_robot_cost[0]
                        temp[4] -= geode_robot_cost[1]
                        temp[7] += 1
                        max_result = max(max_result, t[6]+t[7]*(32-i))
                        if tuple(temp) not in visited:
                            next_to_try.add(tuple(temp))
                    else:
                        can_create_all_robots = False
                    if tl[5] < geode_robot_cost[1] and tl[0] - tl[1] >= obsidian_robot_cost[0] and tl[2] - tl[3] >= obsidian_robot_cost[1]:
                        temp = tl.copy()
                        temp[0] -= obsidian_robot_cost[0]
                        temp[2] -= obsidian_robot_cost[1]
                        temp[5] += 1
                        if tuple(temp) not in visited:
                            next_to_try.add(tuple(temp))
                    else:
                        can_create_all_robots = False
                    if tl[3] < obsidian_robot_cost[1] and tl[0] - tl[1] >= clay_robot_cost:
                        temp = tl.copy()
                        temp[0] -= clay_robot_cost
                        temp[3] += 1
                        if tuple(temp) not in visited:
                            next_to_try.add(tuple(temp))
                    else:
                        can_create_all_robots = False
                    if tl[1] < max_ore_cost and tl[0] - tl[1] >= ore_robot_cost:
                        temp = tl.copy()
                        temp[0] -= ore_robot_cost
                        temp[1] += 1
                        if tuple(temp) not in visited:
                            next_to_try.add(tuple(temp))
                    else:
                        can_create_all_robots = False
                    if not can_create_all_robots:
                        next_to_try.add(tuple(tl))  # do_nothing
                to_try = next_to_try.copy()
            maxi_score = max(set(map(lambda x: x[6], to_try)))
            list_scores.append(maxi_score)
        print("->", list_scores[0]*list_scores[1]*list_scores[2])
        print(list_scores)
#part1()
part2()
