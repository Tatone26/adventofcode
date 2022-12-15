def add_if_needed(cycle, X) -> int:
    if cycle < 221 and cycle % 40 == 20:
        return cycle*X
    else:
        return 0


def line_check(cycle, x, line) -> str:
    if cycle % 40 == 0:
        print(line)
        return ""
    else:
        pos = cycle % 40
        if x - 1 == pos or x == pos or x + 1 == pos:
            return line + "#"
        return line + " "


with open("day-10\day-10.txt", 'r') as file:
    lines = file.readlines()
    actions = [line.strip().split(" ") for line in lines]
    X = 1
    cycle = 0
    somme = 0
    line = ""
    for act in actions:
        if len(act) == 1:
            cycle += 1
            line = line_check(cycle, X, line)
            somme += add_if_needed(cycle, X)
        else:
            value = int(act[1])
            cycle += 1
            line = line_check(cycle, X, line)
            somme += add_if_needed(cycle, X)
            cycle += 1
            X += value
            line = line_check(cycle, X, line)
            somme += add_if_needed(cycle, X)
    print("La somme des signaux tous les 40 tours est :", somme)

# mdr y a un bug il manque la première colonne mais on s'en foooout