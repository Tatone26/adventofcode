def day1():
    with open("2015\day-1-2015.txt", "r") as file:
        line = file.readlines()[0]
        pos = 0
        for i in range(len(line)):
            if line[i] == "(":
                pos += 1
            elif line[i] == ")":
                pos -= 1
            if pos == -1:
                print("Arrivée au sous-sol après l'instruction :", i+1)
        print("Arrivée à l'étage :", pos)

def day2():
    with open("2015\day-2-2015.txt", "r") as file:
        lines = file.readlines()
        total = 0
        total_ribbon = 0
        for line in lines:
            numbers = line[:-1].split("x")
            l = int(numbers[0])
            w = int(numbers[1])
            h = int(numbers[2])
            total = total + 2*l*w + 2*w*h + 2*h*l + min([l*w, w*h, h*l])
            total_ribbon = total_ribbon +  min([2*l+2*w, 2*w+2*h, 2*h+2*l]) + l*w*h
        print("Papier cadeau nécessaire total :", total)
        print("Ruban total nécessaire :", total_ribbon)

def day3():
    with open("2015\day-3-2015.txt", "r") as file:
        line = file.readlines()[0]
        visited = {(0, 0)}
        actual_pos = [0, 0] # North is (0, 1), East is (1, 0)
        for c in line:
            if c == ">":
                actual_pos[0] -= 1
            elif c == "<":
                actual_pos[0] += 1
            elif c == "^":
                actual_pos[1] -= 1
            elif c == "v":
                actual_pos[1] += 1
            else:
                print("Nope")
            visited.add((actual_pos[0], actual_pos[1]))
        print("Le père noël a visité :", len(visited), "maison différentes.")


        with open("2015\day-3-2015.txt", "r") as file:
            line = file.readlines()[0]
            visited = {(0, 0)}
            actual_pos = [0, 0] # North is (0, 1), East is (1, 0)
            actual_pos_robot = [0, 0]
            for i in range(len(line)):
                c = line[i]
                mvt = [0, 0]
                if c == ">":
                    mvt[0] -= 1
                elif c == "<":
                    mvt[0] += 1
                elif c == "^":
                    mvt[1] -= 1
                elif c == "v":
                    mvt[1] += 1
                else:
                    print("Nope")
                if i%2 == 0:
                    actual_pos[0] += mvt[0]
                    actual_pos[1] += mvt[1]
                    visited.add((actual_pos[0], actual_pos[1]))
                else:
                    actual_pos_robot[0] += mvt[0]
                    actual_pos_robot[1] += mvt[1]
                    visited.add((actual_pos_robot[0], actual_pos_robot[1]))
            print("Le père noël avec le robot ont visité :", len(visited), "maison différentes.")

def day4():
    import hashlib
    puzzle_input = "yzbqklnj"
    i = 1
    while True:
        test = puzzle_input + str(i)
        test = bytes(test, "utf-8")
        result = hashlib.md5(test)
        if result.digest().hex()[:6] == "000000":
            print("La clé trouvée est :", i)
            break
        i = i+1


def day5():
    def compte_voyelles(line):
        v = "aeiou"
        score = 0
        for c in line:
            if c in v:
                score += 1
        return score

    def has_double_letter(line):
        for i in range(len(line)-1):
            if line[i+1] == line[i]:
                return True
        return False

    def has_naughty_string(line):
        if "ab" in line or "cd" in line or "pq" in line or "xy" in line:
            return True
        return False

    with open("2015\day-5-2015.txt", "r") as file:
        lines = file.readlines()
        count = 0
        for line in lines:
            voyelles = compte_voyelles(line)
            dbl = has_double_letter(line)
            if voyelles >= 3 and dbl and not has_naughty_string(line):
                count += 1
        print("Il y a :", count, "nice strings.")

    def has_repeted_double_letter(line):
        for i in range(len(line)-1):
            for j in range(i+2, len(line)):
                if line[j] == line[i] and line[i+1] == line[j+1]:
                    return True
        return False
    
    def has_separated_double_letter(line):
        for i in range(len(line)-2):
            if line[i+2] == line[i]:
                return True
        return False

    with open("2015\day-5-2015.txt", "r") as file:
        lines = file.readlines()
        count = 0
        for line in lines:
            if has_repeted_double_letter(line) and has_separated_double_letter(line):
                count += 1
        print("Il y a :", count, "nice strings avec les nouvelles conditions.")

day5()