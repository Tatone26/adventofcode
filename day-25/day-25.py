from alive_progress import alive_bar, alive_it

def snafu_to_decimal(string):
    result = 0
    n = 0
    for c in reversed(string):
        if c.isdigit():
            result += int(c)*(5**n)
        elif c == "=":
            result -= 2*(5**n)
        elif c == "-":
            result -= (5**n)
        n += 1
    return result

def snafu_digit(number):
    if 0 <= number <= 2:
        return str(number)
    else:
        if number == -1:
            return "-"
        elif number == -2:
            return "="

def numberToBase(n, b):
    if n == 0:
        return [0]
    digits = []
    while n:
        digits.append(int(n % b))
        n //= b
    return digits[::-1]

def decimal_to_snafu(number):
    result = ""
    n = number
    if n == 0:
        result = "0"
    with alive_bar(title = "To snafu") as bar:
        while n:
            t = int(n % 5)
            n //= 5
            if t == 3:
                result = "=" + result
                n += 1
            elif t == 4:
                result = "-" + result
                n += 1
            else:
                result = str(t) + result
            bar()
    return result
    


def part1():

    print("="*50+"\nPART 1")
    with open("day-25/day-25.txt", "r") as f:
        lines = f.read().splitlines()
        test = []
        for line in alive_it(lines, title = "Counting"):
            test.append(snafu_to_decimal(line))
        somme = sum(test)
        print("->", decimal_to_snafu(somme))
    print("It's the end.")

part1()