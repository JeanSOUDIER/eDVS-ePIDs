MAIN: main.o Robot/RS232_CPP/rs232.o Robot/RS232_CPP/Usb.o Robot/logger/logger.o Robot/BaseThread/BaseThread.o Robot/MotorWheel/MotorWheel.o Robot/eDVS/DVS.o Robot/Controller/ePID.o Robot/Controller/PID.o Robot/CamPi/Camera.o
	g++ -Ofast -o eCHAIN Robot/RS232_CPP/rs232.o Robot/RS232_CPP/Usb.o Robot/logger/logger.o Robot/BaseThread/BaseThread.o Robot/MotorWheel/MotorWheel.o Robot/eDVS/DVS.o Robot/Controller/ePID.o Robot/Controller/PID.o Robot/CamPi/Camera.o main.o -lwiringPi -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread `pkg-config --cflags --libs opencv4` -lwiringPi -lpigpio -I/usr/local/include/ -lraspicam -lraspicam_cv

rs232.o: Robot/RS232_CPP/rs232.c
	g -Ofast -o Robot/RS232_CPP/rs232.o -c Robot/RS232_CPP/rs232.c -Wall -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard

Usb.o: Robot/RS232_CPP/Usb.cpp
	g++ -Ofast -o Robot/RS232_CPP/Usb.o -c Robot/RS232_CPP/Usb.cpp -Wall -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard

logger.o: Robot/logger/logger.cpp
	g++ -Ofast -o Robot/logger/logger.o -c Robot/logger/logger.cpp -Wall -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard

BaseThread.o: Robot/BaseThread/BaseThread.cpp
	g++ -Ofast -o Robot/BaseThread/BaseThread.o -c Robot/BaseThread/BaseThread.cpp -Wall -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread

MotorWheel.o: Robot/MotorWheel/MotorWheel.cpp
	g++ -Ofast -o Robot/MotorWheel/MotorWheel.o -c Robot/MotorWheel/MotorWheel.cpp -Wall -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard

Camera.o: Robot/CamPi/Camera.cpp
	g++ -Ofast -o Robot/CamPi/Camera.o -c Robot/CamPi/Camera.cpp -Wall `pkg-config --cflags --libs opencv4` -lwiringPi -lpigpio -I/usr/local/include/ -lraspicam -lraspicam_cv -lwiringPi -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread -std=c++11

DVS.o: Robot/eDVS/DVS.cpp
	g++ -Ofast -o Robot/eDVS/DVS.o -c Robot/eDVS/DVS.cpp -Wall -lwiringPi -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread

PID.o: Robot/Controller/PID.cpp
	g++ -Ofast -o Robot/Controller/PID.o -c Robot/Controller/PID.cpp -Wall -lwiringPi -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread

ePID.o: Robot/Controller/ePID.cpp
	g++ -Ofast -o Robot/Controller/ePID.o -c Robot/Controller/ePID.cpp -Wall -lwiringPi -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread

main.o: main.cpp Robot/RS232_CPP/rs232.h Robot/RS232_CPP/Usb.hpp Robot/logger/logger.hpp Robot/BaseThread/BaseThread.hpp Robot/MotorWheel/MotorWheel.hpp Robot/eDVS/DVS.hpp Robot/Controller/ePID.hpp Robot/Controller/PID.hpp Robot/CamPi/Camera.hpp
	g++ -Ofast -o main.o -c main.cpp -Wall `pkg-config --cflags --libs opencv4` -lwiringPi -lpigpio -I/usr/local/include/ -lraspicam -lraspicam_cv -lwiringPi -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard -pthread -std=c++11

clean:
	@echo "cleaning"
	find . -name "*.o" | xargs rm
	rm eCHAIN
	make Camera.o
	make
