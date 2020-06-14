run:	proyecto.o Learner.o
	g++ proyecto.o Learner.o -o run -lreadline

proyecto.o:	Learner.h proyecto.cpp
	g++ -c proyecto.cpp -lreadline

Learner.o:	Learner.h Learner.cpp
	g++ -c Learner.cpp

clean:
	rm *.o