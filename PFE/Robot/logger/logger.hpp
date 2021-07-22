#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#include <ctime>

/*

                     logger class

    SOUDIER Jean  (jean.soudier@insa-strasbourg.fr)


    Provides a class to create *.csv data logging

    Features :

    • Create csv files

    Functions :

    • logger                         | Constructor with arguments file name, reference time, the number of the file and the separator
    • WriteXX()                      | Function to add a line in the csv file
    • WriteXXN()                     | Function to add a line without new line
    • Tic()                          | Function to add a start time
    • Tac()                          | Function to add a stop time
    • TacX()                         | Function to add a stop time at a WriteXXN()
    • GetNumFile()                   | Function to return the number of the file
    • IsExist(name)                  | Function to check if a file exist

*/

class logger {
	public:
		logger(const std::string fileName, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file = -1, char delimiter = ';');
		~logger();

		void Write(const std::vector<int>& values);
		void WriteN(const std::vector<int> &values);
		void WriteIN(const std::vector<int> &values);
		void WriteUI16N(const uint16_t &values);
		void WriteFN(const std::vector<float>& values);
		
		void Tic();
		void Tac();
		void TacF();
		void TacI();
		const int GetNumFile();
	private:
		bool IsExist(std::string name);

		std::ofstream m_file;
		const char m_delimiter = ';';
		int m_num_file = -1;
		std::string m_file_name;
		const std::chrono::time_point<std::chrono::high_resolution_clock> m_begin_timestamp;
		
};

#endif //LOGGER_H
