from alive_progress import alive_bar, alive_it

list_rocks = [((2, 0), (3, 0), (4, 0), (5, 0)), ((3, 0), (2, 1), (3, 1), (4, 1), (3, 2)), ((
    2, 0), (3, 0), (4, 0), (4, 1), (4, 2)), ((2, 0), (2, 1), (2, 2), (2, 3)), ((2, 0), (3, 0), (2, 1), (3, 1))]


def part1():
    print("="*50+"\nPART 1")
    with open("day-17\day-17.txt", "r") as f:
        line = f.read().splitlines()[0]
        
        placed = set()
        max_height = -1
        actual_jet_ind = 0
        for y in alive_it(range(2022)):
            rock = [list(list_rocks[y%5][x]) for x in range(len(list_rocks[y%5]))]
            for c in range(len(rock)):
                rock[c] = [rock[c][0], rock[c][1] + max_height + 4]
            
            while True:
                if line[actual_jet_ind] == "<":
                    can_move = True
                    for b in rock:
                        if b[0] - 1 < 0:
                            can_move = False
                            break
                        elif (b[0] - 1, b[1]) in placed:
                            can_move = False
                            break
                    if can_move:
                        for i in range(len(rock)):
                            rock[i] = [rock[i][0] - 1, rock[i][1]]
                elif line[actual_jet_ind] == ">":
                    can_move = True
                    for b in rock:
                        if b[0] + 1 > 6:
                            can_move = False
                            break
                        elif (b[0] + 1, b[1]) in placed:
                            can_move = False
                            break
                    if can_move:
                        for i in range(len(rock)):
                            rock[i] = [rock[i][0] + 1, rock[i][1]]
                actual_jet_ind += 1
                if actual_jet_ind >= len(line):
                    actual_jet_ind = 0
                can_fall = True
                for b in rock:
                    if b[1] - 1 < 0:
                        can_fall = False
                        break
                    elif (b[0], b[1] - 1) in placed:
                        can_fall = False
                        break
                if can_fall:
                    for i in range(len(rock)):
                        rock[i] = [rock[i][0], rock[i][1] - 1]
                else:
                    for b in rock:
                        placed.add(tuple(b))
                    max_height = max(max(set(map(lambda x : x[1], rock))), max_height)
                    break
        # to_show = [".......\n"]*(max_height+1)
        # for i in placed:
        #     to_show[i[1]] = to_show[i[1]][:i[0]] + "#" + to_show[i[1]][i[0]+1:]
        # for x in to_show[::-1]:
        #     print(x)
        print("Altitude obtenue ->", max_height + 1)

def part2(turns):
    print("="*50+"\nPART 2")
    with open("day-17\day-17.txt", "r") as f:
        line = f.read().splitlines()[0]
        
        placed = dict()
        max_height = -1
        gain_max_height = 0
        actual_jet_ind = 0
        # NEED TO FIND PERIODICITY
        tests = []
        period_found = False
        last_ten_height_deltas = []
        with alive_bar(turns) as bar:
            y = 0
            while y < turns:
                if max_height > 40:
                    while len(placed) > 40:
                        placed.pop(min(placed.keys()))
                rock = [list(list_rocks[y%5][x]) for x in range(len(list_rocks[y%5]))]
                for c in range(len(rock)):
                    rock[c] = [rock[c][0], rock[c][1] + max_height + 4]
                
                while True:
                    if line[actual_jet_ind] == "<":
                        can_move = True
                        for b in rock:
                            if b[0] - 1 < 0:
                                can_move = False
                                break
                            elif b[1] in placed and (b[0] - 1) in placed[b[1]]:
                                can_move = False
                                break
                        if can_move:
                            for i in range(len(rock)):
                                rock[i] = [rock[i][0] - 1, rock[i][1]]
                    elif line[actual_jet_ind] == ">":
                        can_move = True
                        for b in rock:
                            if b[0] + 1 > 6:
                                can_move = False
                                break
                            elif b[1] in placed and (b[0] + 1) in placed[b[1]]:
                                can_move = False
                                break
                        if can_move:
                            for i in range(len(rock)):
                                rock[i] = [rock[i][0] + 1, rock[i][1]]
                    actual_jet_ind += 1
                    if actual_jet_ind >= len(line):
                        actual_jet_ind = 0
                    can_fall = True
                    for b in rock:
                        if b[1] - 1 < 0:
                            can_fall = False
                            break
                        elif b[1] - 1 in placed and (b[0]) in placed[b[1] - 1]:
                            can_fall = False
                            break
                    if can_fall:
                        for i in range(len(rock)):
                            rock[i] = [rock[i][0], rock[i][1] - 1]
                    else:
                        for b in rock:
                            if b[1] in placed:
                                placed[b[1]].add(b[0])
                            else:
                                placed[b[1]] = set([b[0]])
                        new_height = max(set(map(lambda x : x[1], rock)))
                        max_height = max(new_height, max_height)
                        if not period_found and max_height > 20:
                            t = ({(x, y - min(placed.keys())) for y in placed for x in placed[y]}, actual_jet_ind, (y+1)%5, y, max_height)
                            for test in tests:
                                if test[1] == t[1] and test[2] == t[2]:
                                    if test[0] == t[0]:
                                        period_found = True
                                        period = y - test[3]
                                        print("Period size :", period)
                                        y = test[3] + period*(turns//period - 1)
                                        period_height = max_height - test[4]
                                        print("Height gained :", period_height)
                                        gain_max_height = ((turns-test[3])//period - 1) * period_height
                                        print("Starting at :", test[3])
                            tests.append(t)
                        break
                y += 1
                bar(y - bar.current())
        print("Altitude obtenue ->", gain_max_height + max_height + 1)


part1()
part2(1000000000000)
