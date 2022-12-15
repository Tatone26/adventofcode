def compte_score():
    with open("day-1.txt", "r") as file:
        lines = file.readlines()
        score = 0
        for line in lines:
            a, b = line[:-1].split(" ")
            if b == "X":
                score += 1
            elif b == "Y":
                score += 2
            elif b == "Z":
                score += 3
            else:
                print("Nope")
            output = 3
            if a == "A" and b == "Y":
                output = 6
            elif a == "B" and b == "Z":
                output = 6
            elif a == "C" and b == "X":
                output = 6
            elif a == "A" and b == "Z":
                output = 0
            elif a == "B" and b == "X":
                output = 0
            elif a == "C" and b == "Y":
                output = 0
            # else : output = 3
            score += output
    print("Avec la première règle, le score sera :", score)

def compte_score_2():
    with open("day-2.txt", "r") as file:
        lines = file.readlines()
        score = 0
        for line in lines:
            a, b = line[:-1].split(" ")
            if b == "Y":
                score += 3
                if a == "A":
                    score += 1
                elif a == "B":
                    score += 2
                elif a == "C":
                    score += 3
            elif b == "Z":
                score += 6
                if a == "A":
                    score += 2
                elif a == "B":
                    score += 3
                elif a == "C":
                    score += 1
            elif b == "X":
                if a == "A":
                    score += 3
                elif a == "B":
                    score += 1
                elif a == "C":
                    score += 2
        print("Avec la deuxième règle, le score sera :", score)

compte_score()
compte_score_2()