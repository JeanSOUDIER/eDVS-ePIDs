//const int lim = 2;

int cmd = 0;
int sens = 0;
int r_old = 0;
int lim = 0;

//Serial
int vel = 0;
int RcvData = 0;
int mode = 0;
int cc = 0;

void Sending(int msg) {
  Serial.write(255);
  int temp = (unsigned char)(msg/256);
  Serial.write(temp);
  int temp2 = (unsigned char)(msg%256);
  Serial.write(temp2);
  Serial.write((unsigned char)((temp+temp2)%256));
}

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, 0);
  Serial.begin(115200);
  analogWrite(5, 0);
  analogWrite(6, 0);
  r_old = 265;
  digitalWrite(13, 1);
}

void loop() {
  if(sens) {
    analogWrite(5, 0);
    analogWrite(6, cmd);
  } else {
    analogWrite(6, 0);
    analogWrite(5, cmd);
  }
  int r = analogRead(A0);
  if(lim == 0) {
    Sending(r);
  } else {
    if(r-r_old >= lim) {
      r_old += lim;
      Sending(r_old);
    } else if(r-r_old <= -lim) {
      r_old -= lim;
      Sending(r_old);
    }
  }
  delay(1);
}

void serialEvent() {
  while(Serial.available()) {
    char c = Serial.read();
      //Serial.println((int)(c));
      int velo;
      switch(mode) {
        case 0: // Start char = 255
          if(c == 255) {
            mode = 1;
          }
          break;
        case 1: // 8H velocity left
          velo = c;
          vel = velo<<8;
          cc = c;
          mode = 2;
          break;
        case 2: // 8L velocity left
          velo = c;
          vel += velo;
          cc += c;
          mode = 3;
          break;
        case 3: // Checksum char = sum(bits[])%256
          if(cc%256 == c) {
            if(vel&0x4000) {
              lim = vel & 0x01FF;
              int r = analogRead(A0);
              Sending(r);
            } else if(vel&0x8000) {
              r_old = vel & 0x03FF;
            } else {
              cmd = vel & 0x01FF;
              sens = vel & 0x1000;
            }
          }
          mode = 0;
          break;
        default:
          mode = 0;
          break;
    }
  }
}
