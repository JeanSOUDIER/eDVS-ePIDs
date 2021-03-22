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
		std::cout << "remove " << m_file_name << std::endl;
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

void logger::Write(std::vector<int> values, bool test) {
	m_empty = false;
	for(int i=0;i<values.size();i++) {
		m_file << values.at(i) << m_delimiter;
	}
	if (test) { m_file << '\n'; }
}

void logger::WriteD(std::vector<double> values, bool test) {
	m_empty = false;
	for (int i = 0; i < values.size(); i++) {
		m_file << values.at(i) << m_delimiter;
	}
	if (test) { m_file << '\n'; }
}

void logger::WriteUL(std::vector<unsigned long> values, bool test) {
	m_empty = false;
	for (int i = 0; i < values.size(); i++) {
		m_file << values.at(i) << m_delimiter;
	}
	if (test) { m_file << '\n'; }
}

void logger::WriteLI(std::vector<long int> values, bool test) {
	m_empty = false;
	for (int i = 0; i < values.size(); i++) {
		m_file << values.at(i) << m_delimiter;
	}
	if(test) {m_file << '\n';}
}

void logger::Tic() {
	auto t0 = std::chrono::high_resolution_clock::now();
	auto nanosec = t0.time_since_epoch();
	m_file << nanosec.count() / 1000 << m_delimiter;
	//m_file << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_zero_timer).count() << m_delimiter;
}

void logger::Tac() {
	auto t0 = std::chrono::high_resolution_clock::now();
	auto nanosec = t0.time_since_epoch();
	m_file << nanosec.count() / 1000 << '\n';
	//m_file << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_zero_timer).count() << '\n';
}
