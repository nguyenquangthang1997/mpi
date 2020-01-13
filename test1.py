import matplotlib.pyplot as pl

data = open("result1.txt", "r")

# data_line = data.readline()

point1 = []
point2 = []
for line in data:
    data_string = line.split(" ")
    point1.append(int(data_string[0]))
    point2.append(int(data_string[1][:len(data_string[1]) - 1]))
    # point.append(temp)

data = open("result2.txt", "r")

# data_line = data.readline()

point3 = []
point4 = []
temp = []
for line in data:
    data_string = line.split(" ")
    point3.append(int(data_string[0]))
    point4.append(int(data_string[1][:len(data_string[1]) - 1]))
    # point1.append(temp)

data = open("result3.txt", "r")

# data_line = data.readline()

point5 = []
point6 = []
temp = []
for line in data:
    data_string = line.split(" ")
    point5.append(int(data_string[0]))
    point6.append(int(data_string[1][:len(data_string[1]) - 1]))

data = open("result4.txt", "r")

# data_line = data.readline()

point7 = []
point8 = []
temp = []
for line in data:
    data_string = line.split(" ")
    point7.append(int(data_string[0]))
    point8.append(int(data_string[1][:len(data_string[1]) - 1]))


pl.plot(point1, point2, "go")
pl.plot(point5, point6, "go")
pl.plot(point3, point4)
pl.plot(point7, point8)
pl.show()
