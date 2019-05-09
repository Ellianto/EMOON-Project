/* 
 * Arduino Sketch File untuk NodeMCU DevKit 1.0
 * 
 * Sketch ini akan berisikan kode-kode untuk 
 * - Mengirimkan perintah ke Arduino Mega melalui Software Serial
 * - Berkomunikasi dengan Blynk
 *    -> Menerima perintah dari Aplikasi Blynk
 *    -> Mengirimkan hasil bacaan sensor ke Blynk
 * 
 * Sketch ini akan dibagi menjadi 4 bagian besar, untuk memudahkan pencarian
 * bisa digunakan fitur Find (Ctrl + F) untuk langsung berpindah ke bagian 
 * yang ingin dicari
 * 
 * - DIRECTIVES SECTION (#define and #include)
 *    $ VIRTUAL PINS DEFINITION
 *    $ HARDWARE PINS DEFINITION
 *    
 * - GLOBALS SECTION (global variables initiation)
 *    $ PRIMITIVE DATA TYPES
 *    $ CLASSES AND OBJECTS
 *    
 * - METHOD SECTION (Methods and function definitions)
 *    $ PARSERS
 *    $ COMMUNICATIONS
 *    $ TIMERS
 *    $ INITIATIONS
 * 
 * - CODE SECTION (setup, loop, BLYNK functions)
 *    $ BLYNK_WRITE ACTUATORS
 *    $ BLYNK_WRITE AUTOMATORS
 *    $ MAIN CODE
 */

/* DIRECTIVES SECTION */

  #include <ESP8266WiFi.h>        // Library untuk method-method koneksi WiFi di ESP8266
  #include <ArduinoJson.h>        // Library untuk penyajian packet yang akan dikomunikasikan
  #include <SoftwareSerial.h>     // Library untuk "membuat" serial pin
  #include "DHT.h"                // Library untuk sensor temperatur dan kelembaban
  #include <BlynkSimpleEsp8266.h> // Library untuk integrasi Blynk dengan NodeMCU
  #include <TimeLib.h>            // Library untuk penggunaan jam dan waktu
  #include <WidgetRTC.h>          // Library untuk penggunaan RTC untuk sinkronisasi waktu
  
  #define BLYNK_PRINT Serial      // Perintah ini digunakan untuk mempermudah debugging dari sisi fungsionalitas Blynk
  // #define BLYNK_NO_BUILTIN     // Perintah ini dapat di uncomment (dan perintah di atas di comment) untuk menghemat memory
  #define DHTTYPE DHT22           // Sesuaikan dengan tipe sensor suhu dan kelembaban yang digunakan

  #define jsonSize    512  // Dalam Byte
  #define serialBaud  9600 // Baudrate untuk Serial Interface Standar
  #define softBaud    9600 // Baudrate untuk Software Serial (yang berkomunikasi dengan Arduino Mega)
  
  /* VIRTUAL PIN DEFINITIONS */
    //Actuators Section : Menerima perintah dari Blynk
    #define v_rgb_pin               V0
    #define v_main_lights_pin       V1
    #define v_warm_lights_pin       V2
    #define v_fan_pin               V3
    #define v_curtain_pin           V4
    #define v_door_lock_pin         V5
    
    //Sensors Section   : Mengirimkan data ke Blynk
    #define v_gas_chart             V10
    #define v_brightness_chart      V11
    #define v_humidity_chart        V12
    #define v_temperature_chart     V13
    
    #define v_pir_pin               V14
    #define v_gas_pin               V15
    #define v_brightness_pin        V16
    #define v_humidity_pin          V17
    #define v_temperature_pin       V18
  
    //Control Section   : Dipakai untuk automasi
    #define v_automate              V20
    #define v_humidity_threshold    V22
    #define v_temperature_threshold V23
  /* END OF VIRTUAL PIN DEFINITIONS */
  
  /* HARDWARE PIN DEFINITIONS */
    //Sensors
    #define ldr_pin     D1   // D1 = GPIO5
    #define gas_pin     D2   // D2 = GPIO4
    #define dht22_pin   D3   // D3 = GPIO0
    #define pir_pin     D4   // D4 = GPIO2

    //Serial Comms
    #define tx_pin      D7  // D7 = GPIO13
    #define rx_pin      D8  // D8 = GPIO15
  /* END OF HARDWARE PIN DEFINITIONS */
/* END OF DIRECTIVES SECTION */

/* GLOBALS SECTION */
  /* PRIMITIVE DATA TYPES */
    char auth[] = "07c853e56e2d4c40b8eb50e5f3caf101"; // Diambil dari aplikasi Blynk

    char ssid[] = "Hmmmm";                            // Untuk saat ini, masih menggunakan Hotspot dari HP
    char pass[] = "otnaille";                         // Perlu dicari cara untuk koneksi ke WiFi luar
    
    bool automate = false;                            // Flag yang digunakan untuk menentukan mode kerja sistem
    bool door_locked = false;                         // Flag yang menandakan terkuncinya pintu
    bool curtains_raised = false;                     // Flag yang menandakan status gorden
    
    int fan_speed = 0;                                // Value yang merepresentasikan kecepatan dari kipas
    int rgb_value[] = {0,0,0};                        // Array yang merepresentasikan value dari intensitas RGB
    int main_light = 0;                               // Value yang merepresentasikan value dari intensitas Lampu Utama
    int warm_light = 0;                               // Value yang merepresentasikan value dari intensitas Lampu Kuning
  
    float temperature_threshold = 0.0;                // Value yang menyimpan batasan temperatur yang diatur dari aplikasi
    float humidity_threshold = 0.0;                   // Value yang menyimpan batasan kelembaban yang diatur dari aplikasi

    float temperature = 0.0;
    float humidity    = 0.0;
    int brightness = 0;
    int gas_concentration = 0;    
    int pir_state = 0;

    bool temperatureChange  = false;
    bool humidityChange     = false;
    bool brightnessChange   = false;
    bool gasChange          = false;
    bool pirChange          = false;

    bool send_curtain = false;
  
    // Array yang berisikan pin-pin yang ingin diubah modenya menjadi INPUT
    int inputs[] = {ldr_pin, gas_pin, dht22_pin, pir_pin, rx_pin};
  
    // Array yang berisikan pin-pin yang ingin diubah modenya menjadi OUTPUT
    int outputs[] = {tx_pin};
  /* END OF PRIMITIVE DATA TYPES */
  
  /* CLASSES AND OBJECTS */
    BlynkTimer timer;                                 // Untuk mengontrol interval pengiriman data ke Blynk
    DHT dht22(dht22_pin, DHTTYPE);                    // Inisiasi objek yang merepresentasikan sensor DHT22
    SoftwareSerial soft_serial(rx_pin, tx_pin);       // Membuat "pin-pin untuk komunikasi Serial" yang baru
    WidgetTerminal pir_log(v_pir_pin);                // Inisiasi widget Terminal yang ada di Blynk untuk dimanipulasi
    WidgetRTC rtc;
  /* END OF CLASSES AND OBJECTS */
/* END OF GLOBALS SECTION */

/* METHOD SECTION */
  /* PARSERS */
    void parseTemperature(){
      if(temperature != dht22.computeHeatIndex(dht22.readTemperature(), dht22.readHumidity(), false)){
        temperature = dht22.computeHeatIndex(dht22.readTemperature(), dht22.readHumidity(), false);
        temperatureChange = true;
      }
      else temperatureChange = false;
    }
  
    void parseHumidity(){
      if(humidity != dht22.readHumidity()){
        humidity = dht22.readHumidity();
        humidityChange = true;
      }
      else humidityChange = false;
    }
  
    String parseGas(){
      if(gas_concentration != digitalRead(gas_pin)){
        gas_concentration = digitalRead(gas_pin);
        gasChange = true;
      }
      else gasChange = false;
      
      if(gas_concentration) return "Normal";
      else return "High";
    }
  
    String parseBrightness(){
      if(brightness != digitalRead(ldr_pin)){
        brightness = digitalRead(ldr_pin);
        brightnessChange = true;
      }
      else brightnessChange = false;
      
      if(brightness) return "Dim";
      else return "Bright";
    }

    void logMovement(){
      // Mau ditambahkan Timestamp, kemungkinan menggunakan RTC Module dari Blynk
      if(pir_state != digitalRead(pir_pin)){
        pir_state = digitalRead(pir_pin);
        pirChange = true;
      }
      else pirChange = false;
      
      if(pir_state){
        //Pembentukan Timestamp
        String currentTime = String(hour()) + ":" + minute() + ":" + second() + " > ";
        String currentDate = String(day()) + "-" + month() + "-" + year() + " ";
  
        pir_log.print(currentDate);
        pir_log.print(currentTime);
        pir_log.println("Movement Detected!");
        pir_log.flush();
      }
    }
  /* END OF PARSERS */

  /* COMMUNICATIONS */
    void sendCommands(){
      //Pemanfaatan JSON Document untuk mengirimkan perintah
      DynamicJsonDocument jsonBuf(jsonSize);
  
      //Kondisi pintu diatur dengan tombol di aplikasi
      if(door_locked){
        jsonBuf["door"] = 1;
      }
      else jsonBuf["door"] = 0;
  
      //Kondisi Gorden diatur dengan tombol di aplikasi
      if(send_curtain){
        send_curtain = false;

        if(curtains_raised) jsonBuf["curtains"] = 1;
        else jsonBuf["curtains"] = 0;
      }
  
      //Intensitas cahaya dari lampu diatur sesuai dengan slider di aplikasi
      jsonBuf["mainLights"] = main_light;
      jsonBuf["warmLights"] = warm_light;
  
      JsonArray rgb = jsonBuf.createNestedArray("rgb");
      rgb.add(rgb_value[0]);
      rgb.add(rgb_value[1]);
      rgb.add(rgb_value[2]);
  
      //Untuk Kipas, bisa manual bisa otomatis
      if(!automate){ // Manual Mode
        jsonBuf["fans"] = fan_speed;
      }
      else{         // Automatic Mode
        if(temperature > temperature_threshold && humidity > humidity_threshold)
          jsonBuf["fans"] = 255;
        else jsonBuf["fans"] = 0;
      }

      if(temperatureChange) jsonBuf["temp_change"] = 1;
      else jsonBuf["temp_change"] = 0;

      if(gasChange) jsonBuf["gas_change"] = 1;
      else jsonBuf["gas_change"] = 0;

      if(brightnessChange) jsonBuf["bright_change"] = 1;
      else jsonBuf["bright_change"] = 0;

      if(humidityChange) jsonBuf["humid_change"] = 1;
      else jsonBuf["humid_change"] = 0;

      if(pirChange) jsonBuf["pir_change"] = 1;
      else jsonBuf["pir_change"] = 0;
  
      serializeJson(jsonBuf, soft_serial);
      serializeJsonPretty(jsonBuf, Serial);
    }
  /* END OF COMMUNICATIONS */

  /* TIMERS */
    void sendUptime(){
      parseTemperature();
      parseHumidity();
      
      String light_state = parseBrightness();
      String gas_state = parseGas();
      
      // Pengiriman Data ke SuperChart DataStreams
      Blynk.virtualWrite(v_temperature_chart, temperature);
      Blynk.virtualWrite(v_humidity_chart, humidity);
      Blynk.virtualWrite(v_brightness_chart, digitalRead(ldr_pin));
      Blynk.virtualWrite(v_gas_chart, digitalRead(gas_pin));
  
      // Pengiriman Data ke Displays
      Blynk.virtualWrite(v_temperature_pin, temperature);
      Blynk.virtualWrite(v_humidity_pin, humidity);
      Blynk.virtualWrite(v_brightness_pin, light_state);
      Blynk.virtualWrite(v_gas_pin, gas_state);
      logMovement();  // Dan ke terminal
      yield();
      
      sendCommands();

//      Serial.print("Temperature : ");
//      Serial.println(temperature);
//      Serial.print("Humidity  : ");
//      Serial.println(humidity);
//
//      Serial.print("Temp Thres : ");
//      Serial.println(temperature_threshold);
//      Serial.print("Humid Thres  : ");
//      Serial.println(humidity_threshold);
//
//      Serial.print("PIR : ");
//      Serial.println(digitalRead(pir_pin));
    }
  /* END OF TIMERS */

  /* INITIATIONS */
    void outputInitiation(){
      int arr_len = sizeof(outputs);
      int ctr = 0;
      
      for(;ctr < arr_len; ctr++){
        pinMode(outputs[ctr], OUTPUT);
      }
    }
  
    void inputInitiation(){
      int arr_len = sizeof(inputs);
      int ctr = 0;
      
      for(;ctr < arr_len; ctr++){
        pinMode(inputs[ctr], INPUT);
      }
    }
  /* END OF INITIATIONS */
/* END OF METHOD SECTION */

/* CODE SECTION */
  /* BLYNK_WRITE ACTUATORS */
    BLYNK_WRITE(v_rgb_pin){
      rgb_value[0] = param[0].asInt();
      rgb_value[1] = param[1].asInt();
      rgb_value[2] = param[2].asInt();
    }
  
    BLYNK_WRITE(v_main_lights_pin){
      main_light = param.asInt();
    }
    
    BLYNK_WRITE(v_warm_lights_pin){
      warm_light = param.asInt();
    }
  
    BLYNK_WRITE(v_fan_pin){
      fan_speed = param.asInt();
    }  
  
    BLYNK_WRITE(v_curtain_pin){
      int temp = param.asInt();
  
      if(temp == 1) curtains_raised = true;
      else if (temp == 0) curtains_raised = false;

      send_curtain = true;
    }
  
    BLYNK_WRITE(v_door_lock_pin){
      int temp = param.asInt();
  
      if(temp == 1) door_locked = true;
      else if (temp == 0) door_locked = false;
    }
  /* END OF BLYNK_WRITE ACTUATORS */

  /* BLYNK_WRITE AUTOMATORS */
    BLYNK_WRITE(v_automate){
      int temp = param.asInt();
  
      if(temp == 1) automate = true;
      else if (temp == 0) automate = false;
    }

    BLYNK_WRITE(v_humidity_threshold){
      humidity_threshold = param.asFloat();
    }

    BLYNK_WRITE(v_temperature_threshold){
      temperature_threshold = param.asFloat();
    }
  /* END OF BLYNK_WRITE AUTOMATORS */

  /* MAIN CODE */
    BLYNK_CONNECTED(){
      Blynk.syncAll();      // Menyesuaikan value Pin (virtual/digital/analog) dengan value yang disimpan Blynk Server
                            // Akan memanggil semua BLYNK_WRITE() dari virtual pin
      rtc.begin();          // Sync Time on app connected
    }
    
    void setup(){                             // Hanya dijalankan 1x
      Serial.begin(serialBaud);
      soft_serial.begin(softBaud);

      // Resolves WDT Resets
      ESP.wdtDisable();
      ESP.wdtEnable(WDTO_8S);
  
      /*
       * Ketika Blynk.begin() dijalankan, akan memanggil method yang:
       * -> Melakukan koneksi ke WiFi sesuai dengan parameter
       * -> Memanggil Blynk.config() yang akan melakukan setup Auth Token dan Server Address
       * -> Mencoba koneksi ke Server Blynk
       */
       
      Blynk.begin(auth, ssid, pass);          
      setSyncInterval(10 * 60);               // RTC akan sync setiap 10 menit
      timer.setInterval(3000L, sendUptime);   // Function sendUpTime akan dijalankan setiap 1 detik
  
      dht22.begin();                          // Inisiasi awal object DHT
      pir_log.clear();                        // Membersihkan Widget Terminal di aplikasi Blynk
  
      outputInitiation();                     // Mengatur mode dari pin-pin tertentu menjadi OUTPUT
      inputInitiation();                      // Mengatur mode dari pin-pin tertentu menjadi INPUT
    }
    
    void loop(){    // Dijalankan berkali-kali
      Blynk.run();  // Menjalankan method-method yang menjaga koneksi ke Blynk Server (terima perintah + kirim data)
      timer.run();  // Menjalankan Timer
    }
  /* END OF MAIN CODE */
/* END OF CODE SECTION */
