#include "logger.hpp"

logger::logger(const std::string fileName, char delimiter) {
	m_file_name = "files/" + fileName;
	int cpt = 0;
	while(IsExist(m_file_name+"_"+std::to_string(cpt)+".csv")) {cpt++;}
	m_file_name = m_file_name+"_"+std::to_string(cpt--)+".csv";
	m_file = std::ofstream(m_file_name);
	m_delimiter = delimiter;
	std::cout << "logger Start on " << m_file_name << std::endl;
}

logger::~logger() {
	m_file.close();
	if(m_empty) {
		std::remove(m_file_name.c_str());
	}
}

bool logger::IsExist(std::string name) {
	std::ifstream f(name.c_str());
	bool a = f.good();
	f.close();
	std::cout << name.c_str() << " " << a << std::endl;
	return a;
}

void logger::Write(std::vector<int> values) {
	m_empty = false;
	for(int i=0;i<values.size();i++) {
		m_file << values.at(i) << m_delimiter;
	}
	m_file << "\n";
}

void logger::WriteD(std::vector<double> values) {
	m_empty = false;
	for (int i = 0; i < values.size(); i++) {
		m_file << values.at(i) << m_delimiter;
	}
	m_file << "\n";
}

void logger::WriteUL(std::vector<unsigned long> values) {
	m_empty = false;
	for (int i = 0; i < values.size(); i++) {
		m_file << values.at(i) << m_delimiter;
	}
	m_file << "\n";
}
