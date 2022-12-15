with open("day-8\day-8.txt", "r") as file:
    lines = file.readlines()
    visibles = 0
    best_scenic_score = 0
    best_tree = (0, 0)
    for j in range(1, len(lines)-1):
        for i in range(1, len(lines[0])-2):
            val = int(lines[j][i])
            visible_left = False
            visible_right = False
            visible_up = False
            visible_down = False
            up_count = 0
            right_count = 0
            left_count = 0
            down_count = 0
            if max([int(lines[j][k]) for k in range(0, i)]) < val:
                visible_left = True
            if max([int(lines[k][i]) for k in range(0, j)]) < val:
                visible_up = True
            if max([int(lines[j][k]) for k in range(i+1, len(lines[0])-1)]) < val:
                visible_right = True
            if max([int(lines[k][i]) for k in range(j+1, len(lines))]) < val:
                visible_down = True
            for k in range(i-1, -1, -1):
                left_count += 1
                if int(lines[j][k]) >= val:
                    break
            for k in range(j-1, -1, -1):
                up_count += 1
                if int(lines[k][i]) >= val:
                    break
            for k in range(i+1, len(lines[0])-1):
                right_count += 1
                if int(lines[j][k]) >= val:
                    break
            for k in range(j+1, len(lines)):
                down_count += 1
                if int(lines[k][i]) >= val:
                    break
            if visible_left or visible_up or visible_down or visible_right:
                visibles += 1
            if up_count * left_count * right_count * down_count > best_scenic_score:
                best_scenic_score = up_count * left_count * right_count * down_count
                best_tree = (i, j)
    print("Il y a :", visibles, "arbres de l'intérieur visibles.")
    print("Cela fait donc :", visibles + 2*len(lines) + 2*(len(lines[0])-3), "arbres visibles.")
    print("Le meilleur score scénique possible est :", best_scenic_score, "avec l'arbre situé en (", best_tree[0], ",", best_tree[1], ")")