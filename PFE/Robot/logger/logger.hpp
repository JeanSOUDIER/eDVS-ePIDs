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
		logger(const std::string fileName, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file = -1, char delimiter = ';');
		~logger();

		void Write(const std::vector<int>& values);
		void WriteN(const std::vector<int> &values);
		void WriteIN(const std::vector<int> &values);
		//void WriteD(const std::vector<double>& values);
		//void WriteDN(const std::vector<double>& values);
		//void WriteF(const std::vector<float> &values);
		void WriteUI16N(const uint16_t &values);
		void WriteFN(const std::vector<float>& values);

		//void WriteUL(const std::vector<unsigned long> &values);
		//void WriteULN(const std::vector<unsigned long>& values);
		//void WriteLI(const std::vector<long int>& values);
		//void WriteLIN(const std::vector<long int> &values);
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
