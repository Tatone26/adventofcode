from alive_progress import alive_it, alive_bar
import numpy as np
import matplotlib.pyplot as plt
def part1():

    print("="*50+"\nPART 1")
    with open("day-21/day-21.txt", "r") as f:
        lines = f.read().splitlines()
        split_lines = [line.split(": ") for line in lines]
        dico = dict()
        for l in alive_it(split_lines, title = "Reading file"):
            if l[1].isdigit():
                dico[l[0]] = int(l[1])
            else:
                temp = l[1].split(" ")
                dico[l[0]] = temp
        
        def count(name):
            if isinstance(dico[name], int):
                return dico[name]
            else:
                op = dico[name][1]
                if op == "+":
                    return count(dico[name][0]) + count(dico[name][2])
                elif op == "-":
                    return count(dico[name][0]) - count(dico[name][2])
                elif op == "*":
                    return count(dico[name][0]) * count(dico[name][2])
                elif op == "/":
                    return count(dico[name][0]) // count(dico[name][2])
        print("Calculating...")
        print("->", count("root"))

def part2():

    print("="*50+"\nPART 2")
    with open("day-21/day-21.txt", "r") as f:
        lines = f.read().splitlines()
        split_lines = [line.split(": ") for line in lines]
        dico = dict()
        for l in alive_it(split_lines, title = "Reading file"):
            if l[1].isdigit():
                dico[l[0]] = int(l[1])
            else:
                temp = l[1].split(" ")
                dico[l[0]] = temp

        def count(name):
            if isinstance(dico[name], int):
                return dico[name]
            else:
                op = dico[name][1]
                if op == "+":
                    return count(dico[name][0]) + count(dico[name][2])
                elif op == "-":
                    return count(dico[name][0]) - count(dico[name][2])
                elif op == "*":
                    return count(dico[name][0]) * count(dico[name][2])
                elif op == "/":
                    return count(dico[name][0]) // count(dico[name][2])
        saving = dico["humn"]

        first = count(dico["root"][0])
        second = count(dico["root"][2])
        dico["humn"] = 1
        first_bis = count(dico["root"][0])
        second_bis = count(dico["root"][2])
        searching = 0
        base = 0
        if first == first_bis and second != second_bis:
            searching = 2
            base = first
        elif first != first_bis and second == second_bis:
            searching = 0
            base = second
        else:
            print("Error !")

        liste_valeurs = []
        liste_y = []
        for i in alive_it(range(0, 100000, 100), title = "Parameters approx"):
            dico["humn"] = i
            liste_valeurs.append(count(dico["root"][searching]))
            liste_y.append(i)
        slope, intercept = np.polyfit(liste_y, liste_valeurs, 1)
        ant = (second - intercept)/slope

        dico["humn"] = int(ant)
        range_to_search = range(max(int(ant)-100000, 0), int(ant)+100001)
        i = max(int(ant)-10000, 0)
        with alive_bar(title = "Searching") as bar:
            while True:
                dico["humn"] = i
                temp1 = count(dico["root"][searching])
                if temp1 == base:
                    break
                if abs(temp1 - base) > 1000:
                    i += 200
                    bar(i - bar.current())
                else:
                    i += 1
                    bar()
        print("->", i)

part1()
part2()