with open("day-6\day-6.txt", "r") as file:
    line = file.readlines()[0]
    for i in range(3, len(line)):
        last_four_chars = line[i-3:i+1]
        if len(set(last_four_chars)) >= 4:
            print("Le premier marqueur est au caractère :", i + 1)
            break
    for j in range(13, len(line)):
        last_four_chars = line[j-13:j+1]
        if len(set(last_four_chars)) >= 14:
            print("Le premier marqueur de début de message est au caractère :", j + 1)
            break

