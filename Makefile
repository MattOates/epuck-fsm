rwalk: robot.o spicomm.o support.o diffuse.o
	gcc -o rwalk -lm -lpthread diffuse.o robot.o gps_client.o spicomm.o support.o
robot_wrap: robot.o spicomm.o support.o
	swig -module Robot -perl5 robot.c
	gcc -c robot_wrap.c -Dbool=char -I/usr/lib/perl/5.10/CORE -lm -pthread robot.o spicomm.o support.o
clean:
	rm -f *.o rwalk
robot.o: robot.c robot.h gps_client.o
gps_client.o: gps_client.c gps_client.h
spicomm.o: spicomm.c spicomm.h
support.o: support.c support.h
diffuse.o: diffuse.c
