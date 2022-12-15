with open("day-4.txt", "r") as file:
    lines = file.readlines()
    count = 0
    count_overlap = 0
    for line in lines:
        sides = line[:-1].split(",")
        range1 = list(map(int, sides[0].split("-")))
        range2 = list(map(int, sides[1].split("-")))
        if (range1[0] >= range2[0] and range1[1] <= range2[1]) or (range2[0] >= range1[0] and range2[1] <= range1[1]):
            count += 1
            count_overlap += 1
        elif (range1[1] >= range2[0] and range1[0] <= range2[1]) or (range2[1] >= range1[0] and range2[0] <= range1[1]):
            count_overlap += 1
    print("Il y a :", count, "intervalles totalement contenus dans celui de leur paire.")
    print("Il y a :", count_overlap, "intervalles superposés.")