#include "logger.hpp"

logger::logger(const std::string fileName, char delimiter) {
	/*std::string m_file_name = "/files/"+file_name;
	int cpt = 0;
	while(!IsExist(m_file_name+"_"+std::to_string(cpt)+".csv")) {cpt++;}
	m_file_name = m_file_name+"_"+std::to_string(cpt--)+".csv";
	m_file = std::ofstream(m_file_name);
	//m_delimiter = delimiter;
	std::cout << "logger Start on " << m_file_name << std::endl;*/
}

logger::~logger() {
	//m_file.close();
}

bool logger::IsExist(std::string name) {
	/*if(FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    } */
	return false;
}

void logger::Write(std::vector<int> values) {
	for(int i=0;i<values.size();i++) {
		//m_file << values.at(i) << m_delimiter;
	}
	//m_file << "\n";
}
