#include "logger.hpp"

//#define NO_LOG //uncomment to not create log files

logger::logger(const std::string fileName, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, char delimiter)
	: m_begin_timestamp(begin_timestamp), m_delimiter(delimiter), m_num_file(num_file) {
	#ifndef NO_LOG
		m_file_name = "files/" + fileName; //add the repository path
		if(m_num_file < 0) { //if the number is negative then find the next number
			int cpt = 0;
			while(IsExist(m_file_name+"_"+std::to_string(cpt)+".csv")) {cpt++;} //test the existing files
			cpt;
			m_file_name = m_file_name+"_"+std::to_string(cpt)+".csv"; //create a file with the first unused number
			m_num_file = cpt;
			std::cout << "num file = " << m_num_file << std::endl;
		} else {
			m_file_name = m_file_name+"_"+std::to_string(m_num_file)+".csv"; //create a file with the number
		}
		m_file = std::ofstream(m_file_name);
		std::cout << "logger Start on " << m_file_name << std::endl;
	#endif
}

logger::~logger() {
	m_file.close();
	#ifdef NO_LOG
		std::cout << "remove " << m_file_name << std::endl;
		std::remove(m_file_name.c_str());
	#endif
}

bool logger::IsExist(std::string name) {
	std::ifstream f(name.c_str());  //open a file in reading
	bool a = f.good(); 				//test if the file exists
	f.close();
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
void logger::WriteIN(const std::vector<int> &values) { //used 2 eDVS(DVS point)
	#ifndef NO_LOG
		m_file << values.at(0) << m_delimiter << values.at(1) << m_delimiter;
	#endif
}

void logger::WriteFN(const std::vector<float> &values) { //used 3 eDVS, 3 ePID
	#ifndef NO_LOG
		m_file << values.at(0) << m_delimiter << values.at(1) << m_delimiter << values.at(2) << m_delimiter;
	#endif
}

void logger::WriteUI16N(const uint16_t &values) { //used 3 eDVS, 3 ePID
	#ifndef NO_LOG
		m_file << static_cast<float>(values) << m_delimiter;
	#endif
}

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
void logger::TacI() {
	#ifndef NO_LOG
		m_file << static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin_timestamp).count()) << '\n';
	#endif
}

const int logger::GetNumFile() {return m_num_file;}
