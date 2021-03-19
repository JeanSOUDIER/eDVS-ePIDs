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
	private:
		bool IsExist(std::string name);

		std::ofstream m_file;
		char m_delimiter = ';';
};

#endif //LOGGER_H