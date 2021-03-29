MAIN: main.o Robot/logger/logger.o
	g++ -O3 -o eCHAIN Robot/logger/logger.o -Ofast -march=armv8-a+crc+simd -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard

logger.o: Robot/logger/logger.cpp
	g++ -O3 -o Robot/logger/logger.o -c Robot/logger/logger.cpp -Wall -Ofast -march=armv8-a+crc+simd -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard

main.o: main.cpp Robot/logger/logger.hpp
	g++ -O3 -o main.o -c main.cpp -Wall -lwiringPi -Ofast -march=armv8-a+crc+simd -mcpu=cortex-a72 -mfpu=vfp -mfloat-abi=hard

clean:
	@echo "cleaning"
	rm *.o
	rm -r Robot/logger/*.o
	make
