import json

def compare(left, right):
    for i in range(min(len(left), len(right))):
        if isinstance(left[i], int) and isinstance(right[i], int):
            if left[i] > right[i]:
                return False, True
            elif left[i] < right[i]:
                return True, True
        elif isinstance(left[i], list) and isinstance(right[i], list):
            test, breaking = compare(left[i], right[i])
            if breaking:
                return test, True
        elif isinstance(left[i], list) and isinstance(right[i], int):
            test, breaking = compare(left[i], [right[i]])
            if breaking:
                return test, True
        elif isinstance(left[i], int) and isinstance(right[i], list):
            test, breaking = compare([left[i]], right[i])
            if breaking:
                return test, True
    return len(left) < len(right), len(left) != len(right)

def part1():
    with open("day-13.txt", "r") as f:
        lines = f.readlines()
        good_lines = [line.strip("\n") for line in lines]
        actions = []
        for l in good_lines:
            if len(l) > 0:
                actions.append(json.loads(l))

        good_indices = []

        for i in range(0, len(actions), 2):
            #print(i//2)
            value1 = actions[i]
            value2 = actions[i+1]
            test, _ = compare(value1, value2)
            #print("Fin du test :", test)
            if test:
                good_indices.append(i//2 + 1)

        print("La somme des indices des paquets bien ordonnés est : ", sum(good_indices))


def part2():
    with open("day-13.txt", "r") as f:
        lines = f.readlines()
        good_lines = [line.strip("\n") for line in lines]
        actions = []
        for l in good_lines:
            if len(l) > 0:
                actions.append(json.loads(l))
        actions.append([[2]])
        actions.append([[6]])

        sorted = False
        while not sorted:
            sorted = True
            for i in range(len(actions)-1):
                temp = actions[i].copy()
                right_order, _ = compare(actions[i], actions[i+1])
                if not right_order:
                    actions[i] = actions[i + 1]
                    actions[i + 1] = temp
                    sorted = False

        dividers_indices = []
        for i in range(len(actions)):
            if actions[i] == [[2]] or actions[i] == [[6]]:
                dividers_indices.append(i + 1)
        print(dividers_indices)
        print("La clé de décodeur est :", dividers_indices[0] * dividers_indices[1])

part2()
