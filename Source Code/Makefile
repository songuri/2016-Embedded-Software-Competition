# made in Kobot

Compiler = g++
OBJECTS = main.o video.o RobotProtocol.o 

LIBS = `pkg-config opencv --cflags --libs`

kobot : $(OBJECTS) 
	$(Compiler) -o kobot $(OBJECTS) $(LIBS) 

video.o : video.h RobotProtocol.h 
	$(Compiler) -c video.cpp

RobotProtocol.o : RobotProtocol.h 
	$(Compiler) -c RobotProtocol.cpp 

main.o : kobot.h
	$(Compiler) -c main.cpp 

clean :
	rm $(OBJECTS)
	rm -r kobot
