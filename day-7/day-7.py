def dir_size(sub_struct)->int:
    # lets goo
    somme = 0
    for k, v in sub_struct.items():
        if isinstance(v, int):
            somme += v
        elif isinstance(v, dict):
            somme += dir_size(sub_struct[k])
    return somme

def sum_of_small_dirs(sub_struct)->int:
    sum_dirs = 0
    for k, v in sub_struct.items():
        if isinstance(v, dict):
            size = dir_size(sub_struct[k])
            if size <= 100_000:
                sum_dirs += size
            sum_dirs += sum_of_small_dirs(sub_struct[k])
    return sum_dirs

def smallest_dir_to_delete(sub_struct, total)->int:
    smallest_size = 7_000_0000
    for k, v in sub_struct.items():
        if isinstance(v, dict):
            if 7_000_0000 - (total - dir_size(sub_struct[k])) >= 3_000_0000 and dir_size(sub_struct[k]) < smallest_size:
                smallest_size = dir_size(sub_struct[k])
            smaller = smallest_dir_to_delete(sub_struct[k], total)
            if smaller < smallest_size:
                smallest_size = smaller
    return smallest_size

with open("day-7.txt", "r") as file:
    # a dir is a Dict[str, (int or Dict[str, (int or ...)])]
    lines = file.readlines()
    struct = dict()  # complete struct
    actual_dir = "/"
    for line in lines:
        if line[:4] == "$ cd":
            if line[5:-1] != "/" and line[5:-1] != "..":
                actual_dir = actual_dir + line[5:-1] + "/"
            elif line[5:-1] == "..":
                actual_dir = actual_dir[:-1] # get rid of "/"
                for i in range(len(actual_dir)):
                    if actual_dir[-1] != "/":
                        actual_dir = actual_dir[:-1]
                    else:
                        break
        elif line[:4] != "$ ls":
            # going to where we need to be
            word = ""
            sub_struct = struct
            for c in actual_dir:
                if c == "/":
                    if len(word) > 0:
                        sub_struct = sub_struct[word]
                        word = ""
                else:
                    word = word + c
            # creating stuff in there
            if line[:4] == "dir ":
                sub_struct[line[4:-1]] = dict()
            else:
                number = ""
                for k in line:
                    if k != " ":
                        number = number + k
                    else:
                        break
                sub_struct[line[len(number)+1:-1]] = int(number)
    print("Complete structure :", struct) # struct is good !
    total_size = dir_size(struct)
    print("Total size :", total_size)

    total_sum_of_small_dirs = sum_of_small_dirs(struct)
    print("Sum of small dirs :", total_sum_of_small_dirs)

    smallest_size = smallest_dir_to_delete(struct, total_size)
    print("Smallest dir to delete :", smallest_size)

