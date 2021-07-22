int cmd = 0;    //PWM command
int sens = 0;   //PWM direction
int r_old = 0;  //initial condition of the SOD (Send-On-Delta) function
float D_r = 0;  //Delta r value
float lim = 0;  //Threshold value

//Serial
int vel = 0;      //PWM read
int RcvData = 0;  //Data storage
int mode = 0;     //state machine of the communication
int cc = 0;       //checksum

void Sending(int msg) { //Sending function
  Serial.write(255);    //Start byte
  int temp = (unsigned char)(msg/256); //computing the checksum
  Serial.write(temp);   //MSB
  int temp2 = (unsigned char)(msg%256); //computing the checksum
  Serial.write(temp2);  //LSB
  Serial.write((unsigned char)((temp+temp2)%256)); //sending the checksum
}

void setup() {
  pinMode(13, OUTPUT);  //User LED to see if it start
  digitalWrite(13, 0);
  Serial.begin(115200); //115200 baud
  analogWrite(5, 0);    //all PWM to 0
  analogWrite(6, 0);
  r_old = 265;          //middle point
  digitalWrite(13, 1);  //Working
}

void loop() {
  if(sens) { //direction select the PWM pin 5 or 6
    analogWrite(5, 0);
    analogWrite(6, cmd); //apply PWM
  } else {
    analogWrite(6, 0);
    analogWrite(5, cmd); //apply PWM
  }
  int r = analogRead(A0); //read the analog value
  if(lim == 0) {          //time-based working
    Sending(r);           //send the value
  } else {
    D_r = r-r_old;        //compute the delta
    if(abs(D_r) >= lim) { //compute the event function
      r_old += lim*(int)(D_r/lim); //add to the actual point the number of thresholds just under thee actual value
      Sending(r_old);     //send it
    }
  }
  delayMicroseconds(100); //wait 100 us (no USB saturation)
}

void serialEvent() {          //interrupt function
  while(Serial.available()) { //if a byte is received
    char c = Serial.read();   //read it
      int velo;
      switch(mode) {          //state machine
        case 0:               // STATE : Start char = 255
          if(c == 255) {      //if the start byte is valid pass to the next state
            mode = 1;
          }
          break;
        case 1:               // STATE : 8H velocity left
          velo = c;           //save the MSB
          vel = velo<<8;      //shift it
          cc = c;             //compute the checksum
          mode = 2;           //pass to the next state
          break;
        case 2:               // STATE : 8L velocity left
          velo = c;           //save the LSB
          vel += velo;        //add it to the MSB
          cc += c;            //compute the checksum
          mode = 3;           //pass to the next state
          break;
        case 3:               // STATE : Checksum char = sum(bits[])%256
          if(cc%256 == c) {   //if the checksum is valid
            if(vel&0x4000) {  //if it is a threshold message
              lim = vel & 0x01FF;     //mask the value and save it
              int r = analogRead(A0); //send a new data
              Sending(r);
            } else if(vel&0x8000) {   //if it is a middle point message
              r_old = vel & 0x03FF;   //mask the value and save it
            } else {                  //else it is a PWM command
              cmd = vel & 0x01FF;     //save the speed
              sens = vel & 0x1000;    //save the direction
            }
          }
          mode = 0; //return to the start mode
          break;
        default:
          mode = 0; //if enterring in a non declare state, return to start mode
          break;
    }
  }
}
