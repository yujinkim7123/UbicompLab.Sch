float resistance ;
float conductance;

float sensorvalue ;
float voltage;

void setup() {
  
  Serial.begin(9600);

}

void loop() {
  //sensorvalue 占쎈돗�겫占� 占쏙옙占쎈퉮占쎌벥 揶쏅�れ뵠占쎈빍繹먲옙.
  sensorvalue = analogRead(A2);
  
  voltage = sensorvalue * (5.0 / 1023.0);
 
  conductance = 2 * (voltage - 0.5 / 10000);

 resistance = map(sensorvalue, 0, 675, 50 , 500);
 
//  resistance =(((1024 + (2 * sensorvalue))*10000) / (675-sensorvalue));

 // resistance = 1 / resistance;
  
  Serial.println(voltage);
  Serial.println(sensorvalue);
  Serial.println(resistance);
  delay(1000);
 
}


