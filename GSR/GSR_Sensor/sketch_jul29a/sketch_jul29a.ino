#include <SPI.h>

#include<Wire.h>
#include"RTClib.h"
#include <string.h>

#if defined(ARDUINO_ARCH_SAMD)
#endif

const int MPU = 0x69;  //MPU 6050 의 I2C 기본 주소 = 68// 5V제공 = 69
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

RTC_PCF8523 rtc;

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday" };
char fileName[] = "wer00000000.csv";
File myFile;

unsigned long curtime = 0;
unsigned long acctime = 0;
const long interval = 25;  //0.025초 //39개

void setup() {
  Wire.begin();      //Wire 라이브러리 초기화
  Wire.beginTransmission(MPU); //MPU로 데이터 전송 시작
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     //MPU-6050 시작 모드로
  Wire.endTransmission(true);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
#ifndef ESP8266

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

#endif

  Serial.print("Initializing SD card...");

  //SD카드 초기화 SD.begin(10) 는  CS핀번호
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  // test.txt 파일을 쓰기 위해서 Open한다.

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }

  if (!rtc.initialized()) {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //코드를 컴파일 할 때 컴퓨터의 날짜와 시간을 가져온다.
    //가져온 데이터를 RTC 프로그램에 사용한다.
    //한번 시간을 설정해놓으면 배터리가 연결되어있는 한 다시 설정할 필요가 없다. (최소 5년 유지가능)
  }

  // re-open the file for reading: 
  // test.txt 파일을 읽기 위해 다시 Open한다.
  myFile = SD.open(fileName);
  if (myFile) {
    Serial.println(fileName);

    // read from the file until there's nothing else in it:
    // test.txt 파일의 처음부터 끝까지 read하여 시리얼 모니터에 표시해준다.
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file: 읽기 완료한 파일을 닫는다.
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void loop() {
  DateTime now = rtc.now();
  //now()를 호출하면 DateTime 오브젝트를 반환해준다.
  //DateTime 오브젝트에는 year, month, day, hour, minute, second가 선언되어 있다.
  curtime = millis();   //센서값을 받아오는 주기를 만들기 위해 현재시간을 밀리스로 받는다.

  Wire.beginTransmission(MPU);    //데이터 전송시작
  Wire.write(0x3B);         // register 0x3B (ACCEL_XOUT_H), 큐에 데이터 기록
  Wire.endTransmission(false);    //연결유지
  Wire.requestFrom(MPU, 14, true);  //MPU에 데이터 요청
  //데이터 한 바이트 씩 읽어서 반환

  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  myFile = SD.open(fileName, FILE_WRITE);

  if (curtime - acctime >= interval) {    //interval 0.025초마다 실행됨
    acctime = curtime;

    Serial.println(now.second());

    // if the file opened okay, write to it:
    if (myFile) {
      Serial.print("Writing to ACC.txt...");
      myFile.print(now.year(), DEC); // 년월일시간에 따라 파일안에 받는 센서값 표시
      myFile.print(",");
      myFile.print(now.month(), DEC);
      myFile.print(",");
      myFile.print(now.day(), DEC);
      myFile.print(",");
      myFile.print(daysOfTheWeek[now.dayOfTheWeek()]);
      myFile.print(",");
      myFile.print(now.hour(), DEC);
      myFile.print(",");
      myFile.print(now.minute(), DEC);
      myFile.print(",");
      myFile.print(now.second(), DEC);
      myFile.print(",");
      myFile.print(AcX);
      myFile.print(",");
      myFile.print(AcY);
      myFile.print(",");
      myFile.print(AcZ);
      myFile.print(",");
      myFile.print(Tmp);
      myFile.print(",");
      myFile.print(GyX);
      myFile.print(",");
      myFile.print(GyY);
      myFile.print(",");
      myFile.println(GyZ);
      //test.txt 파일에 문자열을 쓴다.

    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening ACC.txt");
    }
  }
  // close the file: 쓰기 완료한 파일을 닫아준다.
  myFile.close();
  Serial.println("done.");
}
