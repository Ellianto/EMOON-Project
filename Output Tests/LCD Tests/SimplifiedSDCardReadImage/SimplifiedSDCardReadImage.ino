  /* LCD DIRECTIVES */
    #include <SPI.h>          // Library penting untuk komunikasi dengan Micro SD Card
    #include <SdFat.h>        // Library untuk membaca file di Micro SD
    #include <Adafruit_GFX.h> // Library untuk hardware (Dibutuhkan untuk library selanjutnya)
    #include <MCUFRIEND_kbv.h>// Library khusus untuk TFT LCD 3.5" Shield yang digunakan

    #define PALETTEDEPTH   8
    #define BMPIMAGEOFFSET 54
    #define BUFFPIXEL      20

    /* ARDUINO MEGA SOFTWARE SPI PINS */
      #define mega_miso_pin 50
      #define mega_mosi_pin 51
      #define mega_sck_pin  52
      #define mega_ss_pin   53
  /* END OF LCD DIRECTIVES */

  char target_file[13] = "logo-umn.bmp";

  SdFatSoftSpi<mega_miso_pin, mega_mosi_pin, mega_sck_pin> sd;

  MCUFRIEND_kbv tft;
  
  /* BMP IMAGE RENDERING */
    void LCDInit(){
      uint16_t ID = tft.readID();
      // Harusnya ID yang benar adalah 9486
      if (ID == 0x0D3D3) ID = 0x9481;
      tft.begin(ID);

      Serial.println(ID, HEX);
      
      bool ok = sd.begin(mega_ss_pin);
      if(ok){
        Serial.println("Test");
        if(sd.exists(target_file)){
          tft.fillScreen(0);
          
          uint8_t ret =  showBMP(target_file, 0, 120);

          switch (ret) {
            case 0:
                Serial.println("Image Render Successful!"); break;
            case 1:
                Serial.println(F("bad position")); break;
            case 2:
                Serial.println(F("bad BMP ID")); break;
            case 3:
                Serial.println(F("wrong number of planes")); break;
            case 4:
                Serial.println(F("unsupported BMP format")); break;
            case 5:
                Serial.println(F("unsupported palette")); break;
            default:
                Serial.println(F("unknown"));break;
          }
        }
      }
    }
  
    uint16_t read16(File& f) {
        uint16_t result;         // read little-endian
        f.read(&result, sizeof(result));
        return result;
    }
    
    uint32_t read32(File& f) {
        uint32_t result;
        f.read(&result, sizeof(result));
        return result;
    }
      
    uint8_t showBMP(char *nm, int x, int y)
    {
        File bmpFile;
        int bmpWidth, bmpHeight;    // W+H in pixels
        uint8_t bmpDepth;           // Bit depth (currently must be 24, 16, 8, 4, 1)
        uint32_t bmpImageoffset;    // Start of image data in file
        uint32_t rowSize;           // Not always = bmpWidth; may have padding
        uint8_t sdbuffer[3 * BUFFPIXEL];    // pixel in buffer (R+G+B per pixel)
        uint16_t lcdbuffer[(1 << PALETTEDEPTH) + BUFFPIXEL], *palette = NULL;
        uint8_t bitmask, bitshift;
        boolean flip = true;        // BMP is stored bottom-to-top
        int w, h, row, col, lcdbufsiz = (1 << PALETTEDEPTH) + BUFFPIXEL, buffidx;
        uint32_t pos;               // seek position
        boolean is565 = false;      //
    
        uint16_t bmpID;
        uint16_t n;                 // blocks read
        uint8_t ret;
    
        if ((x >= tft.width()) || (y >= tft.height()))
            return 1;               // off screen
    
        bmpFile = sd.open(nm);      // Parse BMP header
        bmpID = read16(bmpFile);    // BMP signature
        (void) read32(bmpFile);     // Read & ignore file size
        (void) read32(bmpFile);     // Read & ignore creator bytes
        bmpImageoffset = read32(bmpFile);       // Start of image data
        (void) read32(bmpFile);     // Read & ignore DIB header size
        bmpWidth = read32(bmpFile);
        bmpHeight = read32(bmpFile);
        n = read16(bmpFile);        // # planes -- must be '1'
        bmpDepth = read16(bmpFile); // bits per pixel
        pos = read32(bmpFile);      // format
        if (bmpID != 0x4D42) ret = 2; // bad ID
        else if (n != 1) ret = 3;   // too many planes
        else if (pos != 0 && pos != 3) ret = 4; // format: 0 = uncompressed, 3 = 565
        else if (bmpDepth < 16 && bmpDepth > PALETTEDEPTH) ret = 5; // palette 
        else {
            bool first = true;
            is565 = (pos == 3);               // ?already in 16-bit format
            // BMP rows are padded (if needed) to 4-byte boundary
            rowSize = (bmpWidth * bmpDepth / 8 + 3) & ~3;
            if (bmpHeight < 0) {              // If negative, image is in top-down order.
                bmpHeight = -bmpHeight;
                flip = false;
            }
    
            w = bmpWidth;
            h = bmpHeight;
            if ((x + w) >= tft.width())       // Crop area to be loaded
                w = tft.width() - x;
            if ((y + h) >= tft.height())      //
                h = tft.height() - y;
    
            if (bmpDepth <= PALETTEDEPTH) {   // these modes have separate palette
                bmpFile.seek(BMPIMAGEOFFSET); //palette is always @ 54
                bitmask = 0xFF;
                if (bmpDepth < 8)
                    bitmask >>= bmpDepth;
                bitshift = 8 - bmpDepth;
                n = 1 << bmpDepth;
                lcdbufsiz -= n;
                palette = lcdbuffer + lcdbufsiz;
                for (col = 0; col < n; col++) {
                    pos = read32(bmpFile);    //map palette to 5-6-5
                    palette[col] = ((pos & 0x0000F8) >> 3) | ((pos & 0x00FC00) >> 5) | ((pos & 0xF80000) >> 8);
                }
            }
    
            // Set TFT address window to clipped image bounds
            tft.setAddrWindow(x, y, x + w - 1, y + h - 1);
            for (row = 0; row < h; row++) { // For each scanline...
                // Seek to start of scan line.  It might seem labor-
                // intensive to be doing this on every line, but this
                // method covers a lot of gritty details like cropping
                // and scanline padding.  Also, the seek only takes
                // place if the file position actually needs to change
                // (avoids a lot of cluster math in SD library).
                uint8_t r, g, b, *sdptr;
                int lcdidx, lcdleft;
                if (flip)   // Bitmap is stored bottom-to-top order (normal BMP)
                    pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
                else        // Bitmap is stored top-to-bottom
                    pos = bmpImageoffset + row * rowSize;
                if (bmpFile.position() != pos) { // Need seek?
                    bmpFile.seek(pos);
                    buffidx = sizeof(sdbuffer); // Force buffer reload
                }
    
                for (col = 0; col < w; ) {  //pixels in row
                    lcdleft = w - col;
                    if (lcdleft > lcdbufsiz) lcdleft = lcdbufsiz;
                    for (lcdidx = 0; lcdidx < lcdleft; lcdidx++) { // buffer at a time
                        uint16_t color;
                        // Time to read more pixel data?
                        if (buffidx >= sizeof(sdbuffer)) { // Indeed
                            bmpFile.read(sdbuffer, sizeof(sdbuffer));
                            buffidx = 0; // Set index to beginning
                            r = 0;
                        }
                        switch (bmpDepth) {          // Convert pixel from BMP to TFT format
                            case 24:
                                b = sdbuffer[buffidx++];
                                g = sdbuffer[buffidx++];
                                r = sdbuffer[buffidx++];
                                color = tft.color565(r, g, b);
                                break;
                            case 16:
                                b = sdbuffer[buffidx++];
                                r = sdbuffer[buffidx++];
                                if (is565)
                                    color = (r << 8) | (b);
                                else
                                    color = (r << 9) | ((b & 0xE0) << 1) | (b & 0x1F);
                                break;
                            case 1:
                            case 4:
                            case 8:
                                if (r == 0)
                                    b = sdbuffer[buffidx++], r = 8;
                                color = palette[(b >> bitshift) & bitmask];
                                r -= bmpDepth;
                                b <<= bmpDepth;
                                break;
                        }
                        lcdbuffer[lcdidx] = color;
    
                    }
                    tft.pushColors(lcdbuffer, lcdidx, first);
                    first = false;
                    col += lcdidx;
                }           // end cols
            }               // end rows
            tft.setAddrWindow(0, 0, tft.width() - 1, tft.height() - 1); //restore full screen
            ret = 0;        // good render
        }
        bmpFile.close();
        return (ret);
    }

  /* END OF BMP IMAGE RENDERING */

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  LCDInit();
}

void loop() {
  // put your main code here, to run repeatedly:

}
