#include "logger.hpp"

logger::logger(const std::string fileName, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, char delimiter)
	: m_begin_timestamp(begin_timestamp), m_delimiter(delimiter), m_num_file(num_file) {
	#ifndef NO_LOG
		m_file_name = "files/" + fileName;
		if(m_num_file < 0) {
			int cpt = 0;
			while(IsExist(m_file_name+"_"+std::to_string(cpt)+".csv")) {cpt++;}
			cpt;
			m_file_name = m_file_name+"_"+std::to_string(cpt)+".csv";
			m_num_file = cpt;
			std::cout << "num file = " << m_num_file << std::endl;
		} else {
			m_file_name = m_file_name+"_"+std::to_string(m_num_file)+".csv";
		}
		m_file = std::ofstream(m_file_name);
		std::cout << "logger Start on " << std::endl;
	#endif
}

logger::~logger() {
	#ifdef NO_LOG
		m_file.close();
		std::cout << "remove " << m_file_name << std::endl;
		std::remove(m_file_name.c_str());
	#endif
}

bool logger::IsExist(std::string name) {
	std::ifstream f(name.c_str());
	bool a = f.good();
	f.close();
	//std::cout << name.c_str() << " " << a << std::endl;
	return a;
}

void logger::Write(const std::vector<int>& values) { //used 2 eDVS(tTime)
	#ifndef NO_LOG
		m_file << values.at(0) << m_delimiter << values.at(1) << m_delimiter << '\n';
	#endif
}

void logger::WriteN(const std::vector<int> &values) { //used 2 eDVS(DVS point)
	#ifndef NO_LOG
		m_file << values.at(0) << m_delimiter << values.at(1) << m_delimiter << values.at(2) << m_delimiter;
	#endif
}

/*void logger::WriteD(const std::vector<double>& values) {
	for (unsigned int i = 0; i < values.size(); i++) {
		m_file << values.at(i) << m_delimiter;
	}
	m_file << '\n';
}

void logger::WriteDN(const std::vector<double> &values) {
	for (unsigned int i = 0; i < values.size(); i++) {
		m_file << values.at(i) << m_delimiter;
	}
}

void logger::WriteF(const std::vector<float>& values) {
	for (unsigned int i = 0; i < values.size(); i++) {
		m_file << values.at(i) << m_delimiter;
	}
	m_file << '\n';
}*/

void logger::WriteFN(const std::vector<float> &values) { //used 3 eDVS, 3 ePID
	#ifndef NO_LOG
		m_file << values.at(0) << m_delimiter << values.at(1) << m_delimiter << values.at(2) << m_delimiter;
	#endif
}

/*void logger::WriteUL(const std::vector<unsigned long>& values) {
	for (unsigned int i = 0; i < values.size(); i++) {
		m_file << values.at(i) << m_delimiter;
	}
	m_file << '\n';
}

void logger::WriteULN(const std::vector<unsigned long> &values) {
	for (unsigned int i = 0; i < values.size(); i++) {
		m_file << values.at(i) << m_delimiter;
	}
}

void logger::WriteLI(const std::vector<long int>& values) {
	for (unsigned int i = 0; i < values.size(); i++) {
		m_file << values.at(i) << m_delimiter;
	}
	m_file << '\n';
}

void logger::WriteLIN(const std::vector<long int> &values) {
	for (unsigned int i = 0; i < values.size(); i++) {
		m_file << values.at(i) << m_delimiter;
	}
}*/

void logger::Tic() {
	#ifndef NO_LOG
		m_file << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin_timestamp).count() << m_delimiter;
	#endif
}

void logger::Tac() {
	#ifndef NO_LOG
		m_file << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin_timestamp).count() << '\n';
	#endif
}

void logger::TacF() {
	#ifndef NO_LOG
		m_file << static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin_timestamp).count()) << '\n';
	#endif
}

const int logger::GetNumFile() {return m_num_file;}
