#include "MotorWheel.hpp"

MotorWheel::MotorWheel(const std::string nb_usb, const int bdrate, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const float e_lim, const int middle_point)
    : m_elim(e_lim), m_middle(middle_point) {
    m_usb = new Usb(nb_usb, bdrate); //open the USB connection
    std::cout << "MotorWheel start" << std::endl;

    m_mutexR.store(false); //mutex to prevent a simultaneous read/write operation
    m_mutexW.store(false);

    m_log = new logger("Pot_points", begin_timestamp, num_file); //logger on the sensor

    g_event[1].store(false); //set the feeback sensor to 0
    g_setpoint[1].store(0);
    g_feedback[1].store(0);
}

MotorWheel::MotorWheel(const int nb_usb, const int bdrate, std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp, const int num_file, const float e_lim, const int middle_point)
    : m_elim(e_lim), m_middle(middle_point) {
    m_usb = new Usb(nb_usb, bdrate); //same with USB(int,int) instead of USB(std::string,int)

    std::cout << "e_lim pot = " << m_elim << std::endl;

    std::cout << "MotorWheel start" << std::endl;

    m_mutexR.store(false);
    m_mutexW.store(false);

    m_log = new logger("Pot_points", begin_timestamp, num_file);

    g_event[1].store(false);
    g_setpoint[1].store(0);
    g_feedback[1].store(0);
}

MotorWheel::~MotorWheel() {
    delete m_usb;
}

void MotorWheel::ReadPose() { //function to read the position of the sensor
    std::vector<unsigned char> buffer;  //create a buffer
    while(m_mutexW.load()) {}           //wait until the write process is done
    m_mutexR.store(true);               //lock the reading process
    buffer = m_usb->ReadBytes(4000);    //Read 4000 bytes
    m_mutexR.store(false);              //unlock the reading process
    std::copy(buffer.begin(), buffer.end(), back_inserter(m_buf)); //add the readings to a global buffer (prevent from data splitting)
    while(m_buf.size() > 3) {           //while a message is complete (4bytes)
        if(m_buf.at(0) == 255) {        //if the start byte is equal to 255
            if((m_buf.at(1)+m_buf.at(2))%256 == m_buf.at(3)) { //check the checksum (LBS+MSB)%256 == CS
                int temp = m_buf.at(1); //save the MSB
                temp = temp << 8;       //shift it
                temp += m_buf.at(2);    //save the LSB
                m_y = temp;             //store the result
                m_y = (m_y-m_middle)*0.065f;    //adjust it to the sensor [rad]
                const float ysp = g_setpoint[1].load(); //read the setpoint
                const float e = ysp - m_y;      //compute the error
                m_log->WriteFN({m_y, ysp, 0});  //save all to the log
                m_log->TacF();                  //save the time in the log
                if(std::fabs(e) >= m_elim) {    //compute the event function
                    g_feedback[1].store(m_y);   //store the feedback
                    if(!g_event[1].load()) {    //if no event planned
                        g_event[1].store(true); //planned one
                        g_cv[1].notify_one();   //notify the thread
                    }
                }
                //std::cout << "mesure " << temp << " " << m_y << std::endl; //usefull to adjust the "middle point"
                m_buf.erase(m_buf.begin(), m_buf.begin() + 4); //erase 4 bytes to the buffer
            } else {
                std::cout << "error cc" << std::endl;   //notify an error of checksum
                m_buf.erase(m_buf.begin());             //erase one byte of the buffer (a false checksum is frequently a shifting in the messages)
            }
        } else {
            std::cout << "error sb" << std::endl;       //notify a start byte error
            m_buf.erase(m_buf.begin());                 //same action (try to find the beginning of the messages)
        }
    }
}

void MotorWheel::SetHbridge(int vel) { //function to drive the motor
    if(std::fabs(vel) > 0) {
        if(vel > 0) {
            if(vel > 256) {vel = 256;}//std::cout << "speed sat H" << std::endl;}
            SetSpeed(vel + 0x1000); //add a bit to notify the direction of the motor
        } else {
            vel *= -1;
            if(vel > 256) {vel = 256;}//std::cout << "speed sat L" << std::endl;}
            SetSpeed(vel);
        }
    } else {
        SetSpeed(0);
    }
}

void MotorWheel::SetLim(int lim) { //function to set the Send-On-Delta threshold of the sensor
    if(lim > 256) {lim = 256;std::cout << "lim sat H" << std::endl;}
    if(lim < 0) {lim = 0;std::cout << "lim sat L" << std::endl;}
    SetSpeed(lim + 0x4000);
}

void MotorWheel::SetMiddlePoint(int point) { //function to set the initial value of the Send-On-Delta of the sensor
    if(point > 1023) {point = 1023;std::cout << "middle point sat H" << std::endl;}
    if(point < 0) {point = 0;std::cout << "middle point sat L" << std::endl;}
    SetSpeed(point + 0x8000);
}

void MotorWheel::SetSpeed(int vel) { //function to send messages
    if(vel > 65535) {vel = 65535;std::cout << "msg sat H" << std::endl;}    //limit to 2 bytes
    if(vel < 0) {vel = 0;std::cout << "msg sat L" << std::endl;}
    const unsigned char Lc = static_cast<unsigned char>(vel>>8);            //cast them
    const unsigned char Ld = static_cast<unsigned char>(vel%256);
    const int cc = Lc+Ld;
    const unsigned char c = static_cast<unsigned char>(cc%256);             //compute the checksum
    const std::vector<char> sending{static_cast<char>(255), static_cast<char>(Lc), static_cast<char>(Ld), static_cast<char>(c)}; //create a vector to send
    while(m_mutexR.load()) {}   //wait the end of the reading process
    m_mutexW.store(true);       //lock the write process
    m_usb->SendBytes(sending);  //send the vector
    m_mutexW.store(false);      //unlock the write process
}
