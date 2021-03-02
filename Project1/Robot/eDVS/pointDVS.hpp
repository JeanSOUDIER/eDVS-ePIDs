#ifndef POINT_DVS_H
#define POINT_DVS_H

#include <iostream>
#include <vector>

template<class T>
class pointDVS {
protected:
       T m_x;
       T m_y;
       T m_t;
       bool m_vt;
       bool m_pol;
    public:
        pointDVS() {
            m_x = 0;
            m_y = 0;
            m_t = 0;
            m_pol = true;
            m_vt = false;
        }
    	pointDVS(std::vector<T> data) {
    		m_x = data.at(0);
    		m_y = data.at(1);
    		m_t = data.at(2);
    		m_pol = data.at(3);
    		if(data.size > 4) {
    			m_vt = data.at(5);
    		} else {
    			m_vt = true;
    		}
    	}
        pointDVS(const T x, const T y, const T t, const bool polarity, const bool validTimestamp = true) {
        	m_x = x;
        	m_y = y;
        	m_t = t;
        	m_pol = polarity;
        	m_vt = validTimestamp;
        }

        T getTimestamp() {return m_t;}
        T getX() {return m_x;}
        T getY() {return m_y;}
        bool getPolarity() {return m_pol;}
        bool getValidTimestamp() {return m_vt;}
        std::vector<T> getPoint() {
        	std::vector<T> p = {m_x, m_y, m_t, m_pol, m_vt};
        	return p;
        }

        void setTimestamp(T t) {m_t = t;}
        void setX(T x) {m_x = x;}
        void setY(T y) {m_y = y;}
        void setPolarity(bool polarity) {m_pol = polarity;}
        void setValidTimestamp(bool val) {m_vt = val;}
        void setPoint(std::vector<T> p) {
        	m_x = p.at(0);
        	m_y = p.at(1);
        	m_t = p.at(2);
        	m_pol = p.at(3);
    		if(p.size > 4) {
    			m_vt = p.at(5);
    		} else {
    			m_vt = true;
    		}
        }

        virtual void operator=(const std::vector<T> p) {
        	m_x = p.at(0);
        	m_y = p.at(1);
        	m_t = p.at(2);
        	m_pol = p.at(3);
    		if(p.size() > 4) {
    			m_vt = p.at(4);
    		} else {
    			m_vt = true;
    		}
        }
        friend std::ostream& operator<<(std::ostream& os, const pointDVS<T>& p) {
            pointDVS<T> pp = p;
        	char c = '\0';
        	if(!pp.getPolarity()) {c = '-';}
        	if(pp.getValidTimestamp()) {
                os << "(" << pp.getX() << ";" << pp.getY() << ") is " << c << pp.getTimestamp();
        	} else {
        		os << "Point not valid timestamp";
        	}
    		return os;
		}
        
    private:

};



#endif //POINT_DVS_H
