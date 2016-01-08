import time
from subprocess import call

localTime = int(time.time()-time.altzone)
if (time.daylight):
    localTime -= 3600
localTime += 25 # somewhat arbitrary adjustment for time it takes to cimpile and upload

with open("main.cpp", "r") as f:
    lines = f.readlines()

with open("main.cpp", "w") as f:
	for line in lines:
		if line.find("Clock") >= 0:
			line = "\tClock.setTime(" + str(localTime) + ");\n"
		f.write(line)

call("make")