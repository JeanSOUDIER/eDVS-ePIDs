#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

class logger {
	public:
		logger(const std::string fileName, char delimiter = ';');
		~logger();

		void Write(std::vector<int> values);
		void WriteD(std::vector<double> values);
		void WriteUL(std::vector<unsigned long> values);
	private:
		bool IsExist(std::string name);

		std::ofstream m_file;
		char m_delimiter = ';';
		bool m_empty = true;
		std::string m_file_name;
};

#endif //LOGGER_H