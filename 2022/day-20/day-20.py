from alive_progress import alive_it


def part1():

    print("="*50+"\nPART 1")
    with open("day-20/day-20.txt", "r") as f:
        lines = f.read().splitlines()
        to_do = list(map(lambda x : int(x), lines))
        with_order = [(i, to_do[i]) for i in range(len(to_do))]
        number_list = to_do.copy()
        maxi_size = len(lines) - 1
        for i in alive_it(range(len(lines)), title = "Mixing"):
            indice = 0
            for j in range(len(with_order)):
                if with_order[j][0] == i:
                    indice = j
                    break
            number = with_order.pop(indice)
            new_index = (indice + number[1])%maxi_size
            with_order.insert(new_index, number)
        final_number_list = list(map(lambda x : x[1], with_order))
        base_index = final_number_list.index(0)
        good_numbers = [final_number_list[(base_index+x)%len(lines)] for x in range(1000, 3001, 1000)]
        print(good_numbers)
        print("->", sum(good_numbers))
        # print(final_number_list)

def part2():

    print("="*50+"\nPART 2")
    with open("day-20/day-20.txt", "r") as f:
        lines = f.read().splitlines()
        to_do = list(map(lambda x : int(x)*811589153, lines))
        with_order = [(i, to_do[i]) for i in range(len(to_do))]
        number_list = to_do.copy()
        maxi_size = len(lines) - 1
        for _ in alive_it(range(10), title = "Mixing"):
            for i in range(len(lines)):
                indice = 0
                for j in range(0, len(lines)):
                    if with_order[j][0] == i:
                        indice = j
                        break
                number = with_order.pop(indice)
                new_index = (indice + number[1])%maxi_size
                with_order.insert(new_index, number)
        final_number_list = list(map(lambda x : x[1], with_order))
        base_index = final_number_list.index(0)
        good_numbers = [final_number_list[(base_index+x)%len(lines)] for x in range(1000, 3001, 1000)]
        print(good_numbers)
        print("->", sum(good_numbers))
        # print(final_number_list)

part1()
part2()