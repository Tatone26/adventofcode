def get_indices(line):
    l = [c for c in line]
    indices = dict()
    for i in range(len(l)):
        c = l[i]
        if c != " " and c != "\n":
            indices[c] = i
    return indices

with open("day-5\day-5.txt") as file:
    lines = file.readlines()
    crates_indices = dict()
    for line in lines:
        if "1" in line:
            crates_indices = get_indices(line)
            break
    crates = dict()
    crates_9001 = dict()
    i = 0
    while "1" not in lines[i]:
        line = lines[i]
        for k, v in crates_indices.items():
            if len(line) > v and line[v] != " " and line[v] != "" and line[v] != "\n":
                if k not in crates:
                    crates[k] = [line[v]]
                    crates_9001[k] = [line[v]]
                else:
                    crates[k].insert(0, line[v])
                    crates_9001[k].insert(0, line[v])
        i += 1
    i += 2
    for line in lines[i:]:
        actions = [x for x in line[:-1].split(" ") if x != "move" and x != "from" and x != "to"]
        for j in range(int(actions[0])):
            temp = crates[actions[1]][-1]
            crates[actions[1]].pop()
            crates[actions[2]].append(temp)
        temp_2 = crates_9001[actions[1]][-int(actions[0]):]
        crates_9001[actions[1]] = crates_9001[actions[1]][:-int(actions[0])]
        crates_9001[actions[2]] = crates_9001[actions[2]] + temp_2
    res = ""
    res_2 = ""
    for j in range(1, 10):
        res = res + crates[str(j)][-1]
        res_2 = res_2 + crates_9001[str(j)][-1]
    print("Les crates sur le dessus seront :", res)
    print("Avec un CrateMover 9001, les crates sur le dessus seront :", res_2)

    


