//const int lim = 2;

int cmd = 0;
int sens = 0;
int fb = 0;

//Serial
int vel = 0;
int RcvData = 0;
int mode = 0;
int cc = 0;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, 0);
  Serial.begin(115200);
  analogWrite(5, 0);
  analogWrite(6, 0);
  fb = analogRead(A0);
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
  //if(abs(fb-r) > lim) {
    Serial.write(255);
    int temp = (unsigned char)(r/256);
    Serial.write(temp);
    int temp2 = (unsigned char)(r%256);
    Serial.write(temp2);
    Serial.write((unsigned char)((temp+temp2)%256));
  //}
  delayMicroseconds(100);
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
            cmd = vel & 0xEFFF;
            sens = vel & 0x1000;
          }
          mode = 0;
          break;
        default:
          mode = 0;
          break;
    }
  }
}
