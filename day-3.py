def find_shared_items():
    res = []
    with open("day-3.txt", "r") as file:
        lines = file.readlines()
        for line in lines:
            partone = line[:len(line)//2]
            parttwo = line[len(line)//2:-1]
            for c in partone:
                if c in parttwo:
                    res.append(c)
                    break
    return res

def priority(letter):
    if ord("a") <= ord(letter) <= ord("z"):
        return ord(letter) - ord("a") + 1
    elif ord("A") <= ord(letter) <= ord("Z"):
        return ord(letter) - ord("A") + 27
    else:
        return ord(letter)

def sum_of_priorities(letters):
    return sum([priority(x) for x in letters])

def find_group_items():
    res = []
    with open("day-3.txt", 'r') as file:
        lines = file.readlines()
        for i in range(0, len(lines), 3):
            lineone = lines[i]
            linetwo = lines[i+1]
            linethree = lines[i+2]
            for c in lineone:
                if c in linetwo and c in linethree:
                    res.append(c)
                    break
    return res

shared = find_shared_items()
print("Q1 :", sum_of_priorities(shared))

group_items = find_group_items()
print("Q2 :", sum_of_priorities(group_items))