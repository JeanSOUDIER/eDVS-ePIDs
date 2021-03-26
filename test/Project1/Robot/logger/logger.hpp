#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#include <ctime>

//#define NO_LOG

class logger {
	public:
		logger(const std::string fileName, char delimiter = ';');
		~logger();

		void Write(const std::vector<int>& values);
		void WriteN(const std::vector<int> &values);
		void WriteD(const std::vector<double>& values);
		void WriteDN(const std::vector<double>& values);
		void WriteF(const std::vector<float> &values);
		void WriteFN(const std::vector<float>& values);
		void WriteUL(const std::vector<unsigned long> &values);
		void WriteULN(const std::vector<unsigned long>& values);
		void WriteLI(const std::vector<long int>& values);
		void WriteLIN(const std::vector<long int> &values);
		void Tic();
		void Tac();
		void TacF();
	private:
		bool IsExist(std::string name);

		std::ofstream m_file;
		char m_delimiter = ';';
		std::string m_file_name;
		//std::chrono::time_point<std::chrono::high_resolution_clock> m_zero_timer;
};

#endif //LOGGER_H