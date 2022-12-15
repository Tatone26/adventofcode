def compte_calories(lines):
    maxi = 0
    maxi_indice = 0
    maxi_indice_fin = 0
    x = 0
    x_indice = 0
    for i in range(len(lines)-1):
        if len(lines[i].strip("\n")) > 0: # si y a qqchose dans la liste
            x = x + int(lines[i].strip("\n"))
        else:
            if x > maxi:
                maxi = x
                maxi_indice = x_indice
                maxi_indice_fin = i
            x = 0
            x_indice = i + 2
    return maxi, maxi_indice, maxi_indice_fin

def compte_triple_calories():
    with open('day-1\day-1.txt', 'r') as file:
        lines = file.readlines()
    first_score, start, end = compte_calories(lines)
    lines = lines[:start] + lines[end:]
    second_score, start, end = compte_calories(lines)
    lines = lines[:start] + lines[end:]
    third_score, start, end = compte_calories(lines)
    print("Calories de chaques lutins :", first_score, second_score, third_score)
    print("Calories totales :", first_score + second_score + third_score)
    

with open('day-1\day-1.txt', 'r') as file:
    lines = file.readlines()
    maxi, maxi_indice, maxi_indice_fin = compte_calories(lines)
    print(maxi, "calories entre la ligne", maxi_indice, "et la ligne", maxi_indice_fin)

print("-"*50)
compte_triple_calories()