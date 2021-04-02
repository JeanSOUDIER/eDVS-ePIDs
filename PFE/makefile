MAIN: main.o Robot/logger/logger.o Robot/BaseThread/BaseThread.o Robot/Controller/HardCommand.o Robot/Controller/Hbridge.o
	g++ -O3 -o eCHAIN Robot/logger/logger.o Robot/BaseThread/BaseThread.o Robot/Controller/HardCommand.o Robot/Controller/Hbridge.o main.o -lwiringPi -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread `pkg-config --cflags --libs opencv4` -lwiringPi -lpigpio -I/usr/local/include/ -lraspicam -lraspicam_cv

logger.o: Robot/logger/logger.cpp
	g++ -O3 -o Robot/logger/logger.o -c Robot/logger/logger.cpp -Wall -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard

BaseThread.o: Robot/BaseThread/BaseThread.cpp
	g++ -O3 -o Robot/BaseThread/BaseThread.o -c Robot/BaseThread/BaseThread.cpp -Wall -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread

HardCommand.o: Robot/Controller/HardCommand.cpp
	g++ -O3 -o Robot/Controller/HardCommand.o -c Robot/Controller/HardCommand.cpp -Wall -lwiringPi -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread

Hbridge.o: Robot/Controller/Hbridge.cpp
	g++ -O3 -o Robot/Controller/Hbridge.o -c Robot/Controller/Hbridge.cpp -Wall -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard

main.o: main.cpp Robot/logger/logger.hpp Robot/BaseThread/BaseThread.hpp Robot/Controller/HardCommand.hpp Robot/Controller/Hbridge.hpp
	g++ -O3 -o main.o -c main.cpp -Wall -lwiringPi -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread `pkg-config --cflags --libs opencv4` -lwiringPi -lpigpio -I/usr/local/include/ -lraspicam -lraspicam_cv

clean:
	@echo "cleaning"
	rm eCHAIN
	rm *.o
	rm Robot/logger/*.o
	rm Robot/Controller/*.o
	make
