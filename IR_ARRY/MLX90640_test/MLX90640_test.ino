#include <SD.h>
#include <Adafruit_MLX90640.h>
Adafruit_MLX90640 mlx;
File myFile;

float frame[32*24]; // buffer for full frame of temperatures
int state = 0;
int v = 2;
String str;
// uncomment *one* of the below
//#define PRINT_TEMPERATURES
#define PRINT_ASCIIART

void setup() {
  while (!Serial) delay(10);
  Serial.begin(115200);
  delay(100);
  
  pinMode(6,INPUT_PULLDOWN);
  pinMode(10, OUTPUT);
  
  if (!SD.begin(4)) {
   Serial.println("initialization failed!");
                return;
        }
        Serial.println("initialization done.");

  Serial.println("Adafruit MLX90640 Simple Test");
  if (! mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
    Serial.println("MLX90640 not found!");
    while (1) delay(10);
  }
  Serial.println("Found Adafruit MLX90640");

  Serial.print("Serial number: ");
  Serial.print(mlx.serialNumber[0], HEX);
  Serial.print(mlx.serialNumber[1], HEX);
  Serial.println(mlx.serialNumber[2], HEX);
  
  //mlx.setMode(MLX90640_INTERLEAVED);
  mlx.setMode(MLX90640_CHESS);
  Serial.print("Current mode: ");
  if (mlx.getMode() == MLX90640_CHESS) {
    Serial.println("Chess");
  } else {
    Serial.println("Interleave");    
  }

  mlx.setResolution(MLX90640_ADC_18BIT);
  Serial.print("Current resolution: ");
  mlx90640_resolution_t res = mlx.getResolution();
  switch (res) {
    case MLX90640_ADC_16BIT: Serial.println("16 bit"); break;
    case MLX90640_ADC_17BIT: Serial.println("17 bit"); break;
    case MLX90640_ADC_18BIT: Serial.println("18 bit"); break;
    case MLX90640_ADC_19BIT: Serial.println("19 bit"); break;
  }

  mlx.setRefreshRate(MLX90640_4_HZ);
  Serial.print("Current frame rate: ");
  mlx90640_refreshrate_t rate = mlx.getRefreshRate();
  switch (rate) {
    case MLX90640_0_5_HZ: Serial.println("0.5 Hz"); break;
    case MLX90640_1_HZ: Serial.println("1 Hz"); break; 
    case MLX90640_2_HZ: Serial.println("2 Hz"); break;
    case MLX90640_4_HZ: Serial.println("4 Hz"); break;
    case MLX90640_8_HZ: Serial.println("8 Hz"); break;
    case MLX90640_16_HZ: Serial.println("16 Hz"); break;
    case MLX90640_32_HZ: Serial.println("32 Hz"); break;
    case MLX90640_64_HZ: Serial.println("64 Hz"); break;
  }
}

void loop() {
 delay(500);
 
 String iStr(v);
 str = "SD" + iStr + ".txt" ;
 int btn = digitalRead(6);
 myFile = SD.open(str, FILE_WRITE);
 
if(btn == HIGH){
 if(state == 1){
  state = 0;
 }
 else if(state == 0){
  state = 1;
 }
}
  if(state == 0){
  if (mlx.getFrame(frame) != 0) {
    Serial.println("Failed");
    return;
  } 
  for (uint8_t h=0; h<24; h++) {
    for (uint8_t w=0; w<32; w++) {
      float t = frame[h*32 + w];
      Serial.print(frame[h*32 + w],2); 
      myFile.print(frame[h*32 + w]);
      Serial.print(",");
      if((h*32 + w) < 767){
      myFile.print(",");
      
      } 
     }
    }
    Serial.println("");
    myFile.println("");
  }  
   myFile.close();
   
 if(state == 1){
   Serial.print("1");
     v++;
     }    

}
