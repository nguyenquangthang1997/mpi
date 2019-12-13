import matplotlib.pyplot as pl

data = open("data.txt","r")

data_line = data.readline()

point = []
for line in data_line:
    data_string = line.split(" ")
    point.append(data_string)

pl.plot(point,"go")
pl.show()
