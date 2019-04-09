/* 
 * Arduino Sketch File untuk Arduino Mega 2560
 * 
 * Sketch ini akan berisikan kode-kode untuk 
 * - Mmenerima perintah dari NodeMCU DevKit 1.0 melalui Software Serial
 * - Menjalankan aktuator-aktuator sesuai dengan perintah yang diterima
 * 
 * Sketch ini akan dibagi menjadi 4 bagian besar, untuk memudahkan pencarian
 * bisa digunakan fitur Find (Ctrl + F) untuk langsung berpindah ke bagian 
 * yang ingin dicari
 * 
 * - DIRECTIVES SECTION (#define and #include)
 *    $ LCD DIRECTIVES
 *    $ HARDWARE PINS DEFINITION
 *    $ PATH LIGHTS PIN DEFINITION
 *    
 * - GLOBALS SECTION (global variables initiation)
 *    $ PRIMITIVE DATA TYPES
 *    $ CLASSES AND OBJECTS
 *    
 * - METHOD SECTION (Methods and function definitions)
 *    $ STEPPER CONTROLS
 *    $ BMP IMAGE RENDERING
 *    $ COMMUNICATIONS
 *    $ INITIATIONS
 * 
 * - CODE SECTION (setup, loop)
 */


/* DIRECTIVES SECTION */
  #include <ArduinoJson.h>    // Library untuk penyajian packet yang akan dikomunikasikan
  #include <Servo.h>          // Library yang memudahkan pengoperasian Motor Servo

  #define jsonSize      512   // Dalam Byte
  #define serialBaud    9600  // Baudrate untuk Serial Interface Standar
  #define commsBaud     9600  // Baudrate untuk Serial pin yang berkomunikasi dengan NodeMCU

  #define numberOfTurns 3     // Merepresentasikan jumlah putaran yang akan dilakukan stepper motor

  /* LCD DIRECTIVES */
//    #include <SPI.h>          // Library penting untuk komunikasi dengan Micro SD Card
//    #include <SdFat.h>        // Library untuk membaca file di Micro SD
//    #include <Adafruit_GFX.h> // Library untuk hardware (Dibutuhkan untuk library selanjutnya)
    #include <MCUFRIEND_kbv.h>// Library khusus untuk TFT LCD 3.5" Shield yang digunakan
    #include "logo_umn_small.h" //Header file yang berisikan array pixel-by-pixel logo UMN

    #define LOWFLASH (defined(__AVR_ATmega328P__) && defined(MCUFRIEND_KBV_H_))

    #define BLACK   0x0000
    #define BLUE    0x001F
    #define RED     0xF800
    #define GREEN   0x07E0
    #define CYAN    0x07FF
    #define MAGENTA 0xF81F
    #define YELLOW  0xFFE0
    #define WHITE   0xFFFF
    #define GREY    0x8410
    #define ORANGE  0xE880

//    #define PALETTEDEPTH   8  // 2^n dari color palette yang bisa diakses oleh TFT LCD
//    #define BMPIMAGEOFFSET 54
//    #define BUFFPIXEL      20
  /* END OF LCD DIRECTIVES */
  
  /* HARDWARE PINS DEFINITIONS */
    #define fan_pin         13
    #define red_rgb_pin     12
    #define green_rgb_pin   11
    #define blue_rgb_pin    10

    #define curtains_in1    30
    #define curtains_in2    31
    #define curtains_in3    32
    #define curtains_in4    33
    #define door_lock_pin   34

    #define warm_lights_pin 45
    #define main_lights_pin 46

    /* ARDUINO MEGA SOFTWARE SPI PINS */
//      #define mega_miso_pin 50
//      #define mega_mosi_pin 51
//      #define mega_sck_pin  52
//      #define mega_ss_pin   53
    
  /* END OF HARDWARE PINS DEFINITIONS */

  /* PATH LIGHTS PIN DEFINITION */
    /* SENSOR PATH LIGHTS */
      #define ldr_path_pin      40
      #define gas_path_pin      41
      #define pir_path_pin      42
      #define dht22_path_pin    43

    /* ACTUATOR PATH LIGHTS */
      #define lcd_path_pin      38
      #define main_path_pin     47
      #define rgb_path_pin      48
      #define curtain_path_pin  49
  /* END OF PATH LIGHTS PIN DEFINITION */
/* END OF DIRECTIVES SECTION */

/* GLOBALS SECTION */
  /* PRIMITIVE DATA TYPES */
    int outputs[] = {red_rgb_pin, green_rgb_pin, blue_rgb_pin, 
                     warm_lights_pin, main_lights_pin,
                     curtains_in1, curtains_in2, curtains_in3, curtains_in4,
                     fan_pin, door_lock_pin, rgb_path_pin,
                     main_path_pin, lcd_path_pin, curtain_path_pin};

    //char target_file[13] =  "logo-umn.bmp";

    // variabel-variabel "prevxxx" digunakan untuk mendeteksi perubahan dan menyalakan 
    // lampu Path apabila terjadi perubahan nilai
    int prevDoor      = 0;
    int prevCurtains  = 0;

    int prevRed       = 0;
    int prevGreen     = 0;
    int prevBlue      = 0;

    int prevMain      = 0;
    int prevWarm      = 0;
    int prevFan       = 0;

    int tempChanged   = 0;
    int gasChanged    = 0;
    int brightChanged = 0;
    int humidChanged  = 0;
    int pirChanged    = 0;

    int door_angle     = 0;
    int curtainsRaised = 0;
    
    int red_intensity  = 0;
    int green_intensity= 0;
    int blue_intensity = 0;

    int main_intensity = 0;
    int warm_intensity = 0;
    int fan_speed      = 0;

  /* END OF PRIMITIVE DATA TYPES */

  /* CLASSES AND OBJECTS */
    Servo door_lock;

    //SdFatSoftSpi<mega_miso_pin, mega_mosi_pin, mega_sck_pin> sd;

    MCUFRIEND_kbv tft;
  /* END OF CLASSES AND OBJECTS */
/* END OF GLOBALS SECTION */

/* METHOD SECTION */
  /* STEPPER CONTROLS */
    void rollCurtain(bool isRaised){
      int Steps = 0;
      int steps_left = (4096 * numberOfTurns) - 1;
      long time;
      unsigned long last_time;
      unsigned long currentMillis;    
  
      while(steps_left > 0){
        
        currentMillis = micros();

        // Mungkin bisa tuning di sini supaya jadi lebih cepat
        if(currentMillis-last_time>=1000){
          

          // Mengendalikan magnet 4 fasa untuk memutar stepper motor
          // Perlu tinjauan, apakah for loop diperlukan atau tidak
          for (int x=0;x<1;x++){
            switch(Steps){
               case 0:
                 digitalWrite(curtains_in1, LOW); 
                 digitalWrite(curtains_in2, LOW);
                 digitalWrite(curtains_in3, LOW);
                 digitalWrite(curtains_in4, HIGH);
               break; 
               case 1:
                 digitalWrite(curtains_in1, LOW); 
                 digitalWrite(curtains_in2, LOW);
                 digitalWrite(curtains_in3, HIGH);
                 digitalWrite(curtains_in4, HIGH);
               break; 
               case 2:
                 digitalWrite(curtains_in1, LOW); 
                 digitalWrite(curtains_in2, LOW);
                 digitalWrite(curtains_in3, HIGH);
                 digitalWrite(curtains_in4, LOW);
               break; 
               case 3:
                 digitalWrite(curtains_in1, LOW); 
                 digitalWrite(curtains_in2, HIGH);
                 digitalWrite(curtains_in3, HIGH);
                 digitalWrite(curtains_in4, LOW);
               break; 
               case 4:
                 digitalWrite(curtains_in1, LOW); 
                 digitalWrite(curtains_in2, HIGH);
                 digitalWrite(curtains_in3, LOW);
                 digitalWrite(curtains_in4, LOW);
               break; 
               case 5:
                 digitalWrite(curtains_in1, HIGH); 
                 digitalWrite(curtains_in2, HIGH);
                 digitalWrite(curtains_in3, LOW);
                 digitalWrite(curtains_in4, LOW);
               break; 
                 case 6:
                 digitalWrite(curtains_in1, HIGH); 
                 digitalWrite(curtains_in2, LOW);
                 digitalWrite(curtains_in3, LOW);
                 digitalWrite(curtains_in4, LOW);
               break; 
               case 7:
                 digitalWrite(curtains_in1, HIGH); 
                 digitalWrite(curtains_in2, LOW);
                 digitalWrite(curtains_in3, LOW);
                 digitalWrite(curtains_in4, HIGH);
               break; 
               default:
                 digitalWrite(curtains_in1, LOW); 
                 digitalWrite(curtains_in2, LOW);
                 digitalWrite(curtains_in3, LOW);
                 digitalWrite(curtains_in4, LOW);
               break; 
            }
            
              if (isRaised) Steps++;
              else if(!isRaised) Steps--;
              
              if(Steps>7) Steps=0;
              else if(Steps<0) Steps=7;
          }
          
          time=time+micros()-last_time;
          last_time=micros();
          steps_left--;
        }
      }
    }
  /* END OF STEPPER CONTROLS */

  /* BMP IMAGE RENDERING */
      void LCDInit(){
        uint16_t ID = tft.readID();
        tft.begin(ID);

        tft.fillScreen(BLACK);

        // Origin = Pojok Kanan Atas
        int start_x = 96;     // Kanan = X positif
        int start_y = 208;    // Bawah = Y Positif

        int width_logo = 128;
        int height_logo = 64;
        
        tft.drawRGBBitmap(start_x, start_y, logo_umn_small, width_logo, height_logo);
      }

  
//    LCDInit berikut ini digunakan untuk membaca data dari Micro SD Card
//    Akan tetapi karena tidak konsisten, maka akan digantikan dengan
//    LCDInit yang hard-coded
  /* END OF BMP IMAGE RENDERING */

  /* COMMUNICATIONS */
    void readCommands(){
      if(digitalRead(ldr_path_pin)) digitalWrite(ldr_path_pin, LOW);
      if(digitalRead(gas_path_pin)) digitalWrite(gas_path_pin, LOW);
      if(digitalRead(pir_path_pin)) digitalWrite(pir_path_pin, LOW);
      if(digitalRead(dht22_path_pin)) digitalWrite(dht22_path_pin, LOW);
      
      DynamicJsonDocument jsonBuf(jsonSize);
      auto err = deserializeJson(jsonBuf, Serial1);
  
      if(!err){
//        serializeJsonPretty(jsonBuf, Serial);

        if(jsonBuf["door"]) door_lock.write(90);
        else door_lock.write(0);

        
//        if(jsonBuf["curtains"]) curtainsRaised = 1
//        else curtains_raised = 0;

        if(jsonBuf.containsKey("curtains")){
          digitalWrite(curtain_path_pin, HIGH);
          if(jsonBuf["curtains"]) rollCurtain(true);
          else rollCurtain(false);
          digitalWrite(curtain_path_pin, LOW);
        }
  
        // Output PWM menggunakan analogWrite()
          // Control Fans
//          fan_speed       = jsonBuf["fans"]; 
          analogWrite(fan_pin, jsonBuf["fans"]);
  
          // Control RGB
//          red_intensity   = jsonBuf["rgb"][0]; 
//          green_intensity = jsonBuf["rgb"][1]; 
//          blue_intensity  = jsonBuf["rgb"][2]; 

          if(red_intensity != jsonBuf["rgb"][0] || green_intensity != jsonBuf["rgb"][1] || blue_intensity != jsonBuf["rgb"][2]){
            digitalWrite(rgb_path_pin, HIGH);           
            red_intensity = jsonBuf["rgb"][0];
            green_intensity = jsonBuf["rgb"][1];
            blue_intensity = jsonBuf["rgb"][2];
            
            analogWrite(red_rgb_pin, red_intensity);
            analogWrite(green_rgb_pin, green_intensity);
            analogWrite(blue_rgb_pin, blue_intensity);
            
            delay(500);
            digitalWrite(rgb_path_pin, LOW);
          }
  
          // Control Main Lights
          if(main_intensity != jsonBuf["mainLights"]){
             main_intensity  = jsonBuf["mainLights"];

             digitalWrite(main_path_pin, HIGH);           

             analogWrite(main_lights_pin, main_intensity);   

             delay(500);
             digitalWrite(main_path_pin, LOW);
          }
         
  
          // Control Warm Lights
          analogWrite(warm_lights_pin, jsonBuf["warmLights"]);

          
          // Control Sensor Path Lights
          if(jsonBuf["temp_change"] || jsonBuf["humid_change"]){
             digitalWrite(dht22_path_pin, HIGH);
             delay(250);
             digitalWrite(dht22_path_pin, LOW);
          }

          if(jsonBuf["gas_change"] && !digitalRead(gas_path_pin)){
             digitalWrite(gas_path_pin, HIGH);
             delay(250);
             digitalWrite(gas_path_pin, LOW);
          }

          if(jsonBuf["pir_change"] && !digitalRead(pir_path_pin)){
             digitalWrite(pir_path_pin, HIGH);
             delay(250);
             digitalWrite(pir_path_pin, LOW);
          }

          if(jsonBuf["bright_change"] && !digitalRead(ldr_path_pin)){
             digitalWrite(ldr_path_pin, HIGH);
             delay(250);
             digitalWrite(ldr_path_pin, LOW);
          }
      }
      else{
        Serial.println("Failed to read JSON");
      }
    }
  /* END OF COMMUNICATIONS */

  /* INITIATIONS */
    void outputInitiation(){
      int arr_len = sizeof(outputs);
      int ctr = 0;
      
      for(;ctr < arr_len; ctr++){
        pinMode(outputs[ctr], OUTPUT);
      }

      // Khusus servo, butuh inisiasi tambahan
      door_lock.attach(door_lock_pin);
      door_lock.write(0);

      digitalWrite(lcd_path_pin, HIGH);
    }
  /* END OF INITIATIONS */
/* END OF METHOD SECTION */

/* CODE SECTION */
  void setup() {
    Serial.begin(serialBaud);
    Serial1.begin(commsBaud);
    
    outputInitiation();
    // Tidak ada Input Initiation dikarenakan tidak ada pin Input
    // Pengecualian untuk TFT LCD
    // Komunikasi menggunakan pin khusus untuk komunikasi Serial 
    // yang merupakan bawaan dari Arduino Mega 2560

    LCDInit();
  }
  
  void loop() {    
      
    
//    Serial.println("Reading Commands");
    readCommands();                    // Baca instruksi yang dikirim NodeMCU

//    Serial.println("Executing Commands");
//    execCommands();                    // Jalankan perintah

//    Serial.println("Adjusting Lighting");
//    pathLighting();                    // Nyalakan path_light yang bersesuaian
  }
/* END OF CODE SECTION */
