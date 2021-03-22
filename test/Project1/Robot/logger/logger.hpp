#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#include <ctime>

class logger {
	public:
		logger(const std::string fileName, char delimiter = ';');
		~logger();

		void Write(std::vector<int> values, bool test = true);
		void WriteD(std::vector<double> values, bool test = true);
		void WriteUL(std::vector<unsigned long> values, bool test = true);
		void WriteLI(std::vector<long int> values, bool test = true);
		void Tic();
		void Tac();
	private:
		bool IsExist(std::string name);

		std::ofstream m_file;
		char m_delimiter = ';';
		bool m_empty = true;
		std::string m_file_name;
		//std::chrono::time_point<std::chrono::high_resolution_clock> m_zero_timer;
};

#endif //LOGGER_H