import numpy as np

def part1():
    with open("day-12\day-12.txt", "r") as f:
        lines = [x.strip() for x in f.readlines()]
        matrix_text = [[y for y in x] for x in lines]
        matrix = np.array([[ord(x) - ord("a") for x in y] for y in matrix_text])
        pos = np.where(matrix == -14)
        matrix[np.where(matrix == -14)] = 0
        final = list(np.where(matrix == -28))
        matrix[np.where(matrix == -28)] = 26
        maxi_iter = matrix.shape[0] * matrix.shape[1]
        i = 0
        emplacements = [pos]
        visited = [pos]
        while i < maxi_iter:
            i += 1
            new_emplacements = []
            for xy in emplacements:
                x, y = xy
                test = matrix[x, y] + 1
                if x < len(matrix) - 1 and matrix[x + 1, y] <= test:
                    if [np.array(x+1), np.array(y)] not in visited:
                        new_emplacements.append([np.array(x+1), np.array(y)])
                        visited.append([np.array(x+1), np.array(y)])
                if x > 0 and matrix[x - 1, y] <= test:
                    if [np.array(x-1), np.array(y)] not in visited:
                        new_emplacements.append([np.array(x-1), np.array(y)])
                        visited.append([np.array(x-1), np.array(y)])
                if y < len(matrix[0]) - 1 and matrix[x, y+1] <= test:
                    if [np.array(x), np.array(y+1)] not in visited:
                        new_emplacements.append([np.array(x), np.array(y+1)])
                        visited.append([np.array(x), np.array(y+1)])
                if y > 0 and matrix[x, y-1] <= test:
                    if [np.array(x), np.array(y-1)] not in visited:
                        new_emplacements.append([np.array(x), np.array(y-1)])
                        visited.append([np.array(x), np.array(y-1)])
            emplacements = new_emplacements
            if final in emplacements:
                break
            print(len(visited), maxi_iter)
        print(i)

def part2():
    with open("day-12\day-12.txt", "r") as f:
        lines = [x.strip() for x in f.readlines()]
        matrix_text = [[y for y in x] for x in lines]
        matrix = np.array([[ord(x) - ord("a") for x in y] for y in matrix_text])
        pos = np.where(matrix == -28)
        matrix[np.where(matrix == -14)] = 0
        matrix[np.where(matrix == -28)] = 26
        maxi_iter = matrix.shape[0] * matrix.shape[1]
        i = 0
        emplacements = [pos]
        visited = [pos]
        while i < maxi_iter and len(emplacements) > 0:
            i += 1
            new_emplacements = []
            for xy in emplacements:
                x, y = xy
                test = matrix[x, y] - 1
                if x < len(matrix) - 1 and matrix[x + 1, y] >= test:
                    if [np.array(x+1), np.array(y)] not in visited:
                        new_emplacements.append([np.array(x+1), np.array(y)])
                        visited.append([np.array(x+1), np.array(y)])
                if x > 0 and matrix[x - 1, y] >= test:
                    if [np.array(x-1), np.array(y)] not in visited:
                        new_emplacements.append([np.array(x-1), np.array(y)])
                        visited.append([np.array(x-1), np.array(y)])
                if y < len(matrix[0]) - 1 and matrix[x, y+1] >= test:
                    if [np.array(x), np.array(y+1)] not in visited:
                        new_emplacements.append([np.array(x), np.array(y+1)])
                        visited.append([np.array(x), np.array(y+1)])
                if y > 0 and matrix[x, y-1] >= test:
                    if [np.array(x), np.array(y-1)] not in visited:
                        new_emplacements.append([np.array(x), np.array(y-1)])
                        visited.append([np.array(x), np.array(y-1)])
            if len(new_emplacements) == 0:
                print("snif")
            emplacements = new_emplacements
            found_it = False
            for j in visited:
                if matrix[j[0], j[1]] == 0:
                    found_it = True
            if found_it:
                print("Found it !")
                break
            print(len(visited), maxi_iter)
        print(i)

part2()