from alive_progress import alive_it, alive_bar

def get_shortest_distance(v1, v2, valves):

    actual = set()
    visited = set()
    actual.add(v1)
    visited.add(v1)
    i = 0
    while True:
        temp = set()
        for s in actual:
            for v in valves[s]:
                if v not in visited:
                    temp.add(v)
                    visited.add(v)
        if v2 in temp:
            return i + 1
        actual = temp
        i += 1


def cost(path, distances):
    cost = 0
    for i in range(len(path)-1):
        cost += distances[path[i]][path[i+1]] + 1
    return cost


def score(path, distance, flow_rates, max_iter):
    score = 0
    moving = -1
    current_valve = 0
    current_flow = 0
    i = 1
    while i <= max_iter:
        score += current_flow
        if moving == 0:
            current_flow += flow_rates[path[current_valve]]
            moving = -1
        elif moving == -1:
            if current_valve < len(path) - 1:
                moving = distance[path[current_valve]
                                  ][path[current_valve + 1]] - 1
                current_valve += 1
            else:
                moving -= 1
        else:
            moving -= 1
        i += 1
    return score


def part1():
    print("="*50+"\nPART 1")
    with open("day-16\day-16.txt", "r") as f:

        visitedList = set((()))

        def depthFirst(graph, currentVertex, visited, distances, bar):
            visited.append(currentVertex)
            if cost(visited, distances) <= 30:
                for vertex in graph[currentVertex]:
                    if vertex not in visited:
                        depthFirst(graph, vertex, visited.copy(),
                                   distances, bar)
            if cost(visited, distances) <= 30:
                visitedList.add(tuple(visited))
                bar()

        lines = [l[6:].split("; tunnels lead to valves ")
                 for l in f.read().splitlines()]
        for i in range(len(lines)):
            l = lines[i]
            if len(l) == 1:
                lines[i] = l[0].split("; tunnel leads to valve ")
        valves = dict()
        flow_rates = dict()
        for line in lines:
            valves[line[0][:2]] = line[1].split(", ")
            flow_rates[line[0][:2]] = int(line[0].split(" has flow rate=")[1])

        distances = dict()
        for v in alive_it(valves, title="Distances"):
            distances[v] = dict()
            for v2 in valves:
                if v2 != v:
                    distances[v][v2] = get_shortest_distance(v, v2, valves)

        non_zero_distances = dict()
        for k, v in distances.items():
            if flow_rates[k] != 0 or k == "AA":
                temp = dict()
                for k2, v2 in v.items():
                    if flow_rates[k2] != 0:
                        temp[k2] = v2
                non_zero_distances[k] = temp

        non_zero_graph = dict()
        for k, v in non_zero_distances.items():
            temp = []
            for k2 in v:
                temp.append(k2)
            non_zero_graph[k] = temp

        best_score = 0
        with alive_bar(title="Possible paths") as bar:
            depthFirst(non_zero_graph, "AA", [], non_zero_distances, bar)

        best_score = 0
        for p in alive_it(visitedList, title="Best"):
            best_score = max(score(p, distances, flow_rates, 30), best_score)
        print("Maximum trouvé ->", best_score)




def part2():

    print("="*50+"\nPART 2")
    with open("day-16\day-16.txt", "r") as f:

        visitedList = set((()))

        def depthFirst(graph, currentVertex, visited, distances, bar):
            visited.append(currentVertex)
            if cost(visited, distances) <= 26:
                for vertex in graph[currentVertex]:
                    if vertex not in visited:
                        depthFirst(graph, vertex, visited.copy(),
                                   distances, bar)
            if cost(visited, distances) <= 26:
                visitedList.add(tuple(visited))
                bar()

        lines = [l[6:].split("; tunnels lead to valves ")
                 for l in f.read().splitlines()]
        for i in range(len(lines)):
            l = lines[i]
            if len(l) == 1:
                lines[i] = l[0].split("; tunnel leads to valve ")
        valves = dict()
        flow_rates = dict()
        for line in lines:
            valves[line[0][:2]] = line[1].split(", ")
            flow_rates[line[0][:2]] = int(line[0].split(" has flow rate=")[1])

        distances = dict()
        for v in alive_it(valves, title="Distances"):
            distances[v] = dict()
            for v2 in valves:
                if v2 != v:
                    distances[v][v2] = get_shortest_distance(v, v2, valves)

        non_zero_distances = dict()
        for k, v in distances.items():
            if flow_rates[k] != 0 or k == "AA":
                temp = dict()
                for k2, v2 in v.items():
                    if flow_rates[k2] != 0:
                        temp[k2] = v2
                non_zero_distances[k] = temp

        non_zero_graph = dict()
        for k, v in non_zero_distances.items():
            temp = []
            for k2 in v:
                temp.append(k2)
            non_zero_graph[k] = temp

        best_score = 0
        with alive_bar(title="Possible paths") as bar:
            depthFirst(non_zero_graph, "AA", [], non_zero_distances, bar)

        best_score = 0
        best_path = ()
        visitedListBis = visitedList.copy()
        visitedListBisWithScores = {k : score(k, distances, flow_rates, 26) for k in visitedListBis}
        for p in alive_it(visitedList, title="Best (be patient !)"):
            t = set(p[1:])
            for b in visitedListBis:
                if t.isdisjoint(b):
                    best_score = max(visitedListBisWithScores[b]+visitedListBisWithScores[p], best_score)
            visitedListBis.remove(p)

        print("Maximum trouvé avec un éléphant ->", best_score)


part1()
part2()
