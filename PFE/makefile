MAIN: main.o Robot/RS232_CPP/rs232.o Robot/RS232_CPP/Usb.o Robot/logger/logger.o Robot/BaseThread/BaseThread.o Robot/Controller/HardCommand.o Robot/Controller/Hbridge.o Robot/MotorWheel/MotorWheel.o Robot/eDVS/DVS.o Robot/Controller/ePID.o
	g++ -O3 -o eCHAIN Robot/RS232_CPP/rs232.o Robot/RS232_CPP/Usb.o Robot/logger/logger.o Robot/BaseThread/BaseThread.o Robot/Controller/HardCommand.o Robot/Controller/Hbridge.o Robot/MotorWheel/MotorWheel.o Robot/eDVS/DVS.o Robot/Controller/ePID.o main.o -lwiringPi -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread `pkg-config --cflags --libs opencv4` -lwiringPi -lpigpio -I/usr/local/include/ -lraspicam -lraspicam_cv


rs232.o: Robot/RS232_CPP/rs232.c
	g -O3 -o Robot/RS232_CPP/rs232.o -c Robot/RS232_CPP/rs232.c -Wall -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard

Usb.o: Robot/RS232_CPP/Usb.cpp
	g++ -O3 -o Robot/RS232_CPP/Usb.o -c Robot/RS232_CPP/Usb.cpp -Wall -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard

logger.o: Robot/logger/logger.cpp
	g++ -O3 -o Robot/logger/logger.o -c Robot/logger/logger.cpp -Wall -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard

BaseThread.o: Robot/BaseThread/BaseThread.cpp
	g++ -O3 -o Robot/BaseThread/BaseThread.o -c Robot/BaseThread/BaseThread.cpp -Wall -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread

HardCommand.o: Robot/Controller/HardCommand.cpp
	g++ -O3 -o Robot/Controller/HardCommand.o -c Robot/Controller/HardCommand.cpp -Wall -lwiringPi -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread

Hbridge.o: Robot/Controller/Hbridge.cpp
	g++ -O3 -o Robot/Controller/Hbridge.o -c Robot/Controller/Hbridge.cpp -Wall -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard

MotorWheel.o: Robot/MotorWheel/MotorWheel.cpp
	g++ -O3 -o Robot/MotorWheel/MotorWheel.o -c Robot/MotorWheel/MotorWheel.cpp -Wall -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard

DVS.o: Robot/eDVS/DVS.cpp
	g++ -O3 -o Robot/eDVS/DVS.o -c Robot/eDVS/DVS.cpp -Wall -lwiringPi -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread

ePID.o: Robot/Controller/ePID.cpp
	g++ -O3 -o Robot/Controller/ePID.o -c Robot/Controller/ePID.cpp -Wall -lwiringPi -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread

main.o: main.cpp Robot/RS232_CPP/rs232.h Robot/RS232_CPP/Usb.hpp Robot/logger/logger.hpp Robot/BaseThread/BaseThread.hpp Robot/Controller/HardCommand.hpp Robot/Controller/Hbridge.hpp Robot/MotorWheel/MotorWheel.hpp Robot/eDVS/DVS.hpp Robot/Controller/ePID.hpp
	g++ -O3 -o main.o -c main.cpp -Wall -lwiringPi -Ofast -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread `pkg-config --cflags --libs opencv4` -lwiringPi -lpigpio -I/usr/local/include/ -lraspicam -lraspicam_cv

clean:
	@echo "cleaning"
	rm eCHAIN
	rm *.o
	rm Robot/logger/*.o
	rm Robot/Controller/*.o
	rm Robot/BaseThread/*.o
	rm Robot/eDVS/*.o
	rm Robot/MotorWheel/*.o
	rm Robot/RS232_CPP/*.o
	make