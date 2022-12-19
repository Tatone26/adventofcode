from alive_progress import alive_bar, alive_it

def part1():

    print("="*50+"\nPART 1")
    with open("day-19/day-19.txt", "r") as f:
        lines = f.read().splitlines()
        initial_tuple = (24, 0, 1, 0, 0, 0, 0, 0, 0)
        # steps_left, ore, ore_robots, clay, clay_rob, obi, obi_rob, opened, geode_rob
        list_scores = []
        for line in lines:
            # get the parameters
            to_try = set([initial_tuple])
            for i in range(24):
                next_to_try = set()
                for t in to_try:
                    tl = list(t)
                    # collecting -> maybe after adding new robots ?
                    tl[0] -= 1
                    tl[1] += tl[2]
                    tl[3] += tl[4]
                    tl[5] += tl[6]
                    tl[7] += tl[8]
                    # here is the fun bitches
                    # possibilities : - do nothing and collect, if possible : create robot ore/clay/obsidian/geode
                    # max 5 possibilities/turn -> ez pour un ordi
                    next_to_try.add(tuple(tl)) # do_nothing
                    if tl[1] > 2: # TODO : pas 2 mais paramtre trouvé
                        temp = tl.copy()
                        temp[2] += 1
                        next_to_try.add(tuple(temp))
                    if tl[3] > 2:
                        temp = tl.copy()
                        temp[4] += 1
                        next_to_try.add(tuple(temp))
                    if tl[5] > 2:
                        temp = tl.copy()
                        temp[6] += 1
                        next_to_try.add(tuple(temp))
                to_try = next_to_try
            maxi_score = max(set(map(lambda x : x[7], to_try)))
            list_scores.append(to_try)
        score = 0
        for i in range(len(list_scores)):
            score += list_scores[i]*(i+1)
        print("->", score)
