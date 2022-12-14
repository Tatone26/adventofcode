def part1():
    with open("day-9.txt", "r") as file:
        lines = file.readlines()
        actions = [line.strip().split(" ") for line in lines]
        actions = [(a[0], int(a[1])) for a in actions]
        head_pos = [0, 0]
        tail_pos = [0, 0]
        visited_by_tail = {(0, 0)}
        for act in actions:
            d, v = act
            direct = [0, 0]
            if d == "R":
                direct = [1, 0]
            elif d == "L":
                direct = [-1, 0]
            elif d == "U":
                direct = [0, -1]
            elif d == "D":
                direct = [0, 1]
            for i in range(v):
                previous = head_pos.copy()
                head_pos[0] += direct[0]
                head_pos[1] += direct[1]
                if abs(head_pos[0] - tail_pos[0]) >= 2 or abs(head_pos[1] - tail_pos[1]) >= 2:
                    tail_pos = previous
                    visited_by_tail.add((tail_pos[0], tail_pos[1]))
        print("Nombre de cases visitées par la queue :", len(visited_by_tail))


def part2():
    with open("day-9.txt", "r") as file:
        lines = file.readlines()
        actions = [line.strip().split(" ") for line in lines]
        actions = [(a[0], int(a[1])) for a in actions]
        snake = [[0, 0] for x in range(10)]
        visited = {(0, 0)}
        for act in actions:
            d, v = act
            direct = [0, 0]
            if d == "R":
                direct = [1, 0]
            elif d == "L":
                direct = [-1, 0]
            elif d == "U":
                direct = [0, -1]
            elif d == "D":
                direct = [0, 1]
            for _x in range(v):  # on fait x fois dnas la direction donnée
                snake[0][0] += direct[0]
                snake[0][1] += direct[1]
                test = snake[-1].copy()
                for i in range(1, len(snake)):
                    if abs(snake[i-1][0] - snake[i][0]) >= 2 or abs(snake[i-1][1] - snake[i][1]) >= 2:
                        if snake[i-1][0] == snake[i][0] + 2 :
                            if snake[i-1][1] == snake[i][1]:
                                snake[i][0] += 1
                            else:
                                snake[i][0] += 1
                                if snake[i][1] < snake[i-1][1]:
                                    snake[i][1] += 1
                                else:
                                    snake[i][1] -= 1
                        elif snake[i-1][0] == snake[i][0] - 2 :
                            if snake[i-1][1] == snake[i][1]:
                                snake[i][0] -= 1
                            else:
                                snake[i][0] -= 1
                                if snake[i][1] < snake[i-1][1]:
                                    snake[i][1] += 1
                                else:
                                    snake[i][1] -= 1
                        elif snake[i-1][1] == snake[i][1] + 2 :
                            if snake[i-1][0] == snake[i][0]:
                                snake[i][1] += 1
                            else:
                                snake[i][1] += 1
                                if snake[i][0] < snake[i-1][0]:
                                    snake[i][0] += 1
                                else:
                                    snake[i][0] -= 1
                        elif snake[i-1][1] == snake[i][1] - 2 :
                            if snake[i-1][0] == snake[i][0]:
                                snake[i][1] -= 1
                            else:
                                snake[i][1] -= 1
                                if snake[i][0] < snake[i-1][0]:
                                    snake[i][0] += 1
                                else:
                                    snake[i][0] -= 1
                if test != snake[-1]:
                    visited.add((snake[-1][0], snake[-1][1]))
        print(len(visited))


part2()
