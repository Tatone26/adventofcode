import numpy as np

input_file = open('day-8.txt', 'r')
# input_file = open('input/input_test.txt', 'r')

tree_heights = np.array([[int(i) for i in line.strip()] for line in input_file], dtype=object)
scenic_scores = dict()

width = tree_heights.shape[0]
height = tree_heights.shape[1]

for i in range(width):
    for j in range(height):
        N_sum, S_sum, E_sum, W_sum = (0, 0, 0, 0)
        for N in reversed(range(0, j)):
            N_sum += 1
            if tree_heights[i, j] <= tree_heights[i, N]:
                break
        for S in range(j+1, height):
            S_sum += 1
            if tree_heights[i, j] <= tree_heights[i, S]:
                break
        for W in reversed(range(0, i)):
            W_sum += 1
            if tree_heights[i, j] <= tree_heights[W, j]:
                break
        for E in range(i + 1, width):
            E_sum += 1
            if tree_heights[i, j] <= tree_heights[E, j]:
                break
        # print((i, j), 'N:', N_sum, 'S:', S_sum, 'W:', W_sum, 'E:', E_sum)
        scenic_scores[(i, j)] = N_sum * S_sum * W_sum * E_sum

print(max(scenic_scores.values()))
