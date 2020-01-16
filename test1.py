import matplotlib.pyplot as pl
import sys


data = open(sys.argv[1], "r")

# data_line = data.readline()

point1 = []
point2 = []
for line in data:
    data_string = line.split(" ")
    point1.append(int(data_string[0]))
    point2.append(int(data_string[1][:len(data_string[1]) - 1]))
    # point.append(temp)



data = open("/home/mpiuser/cloud/result.txt", "r")

# data_line = data.readline()
#
point7 = []
point8 = []
temp = []
for line in data:
    data_string = line.split(" ")
    point7.append(int(data_string[0]))
    point8.append(int(data_string[1][:len(data_string[1]) - 1]))
pl.plot(point7, point8)


pl.plot(point1, point2, "go")

pl.show()
