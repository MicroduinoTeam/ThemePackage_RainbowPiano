/*****************************************
   彩虹钢琴/光弦琴应用程序
 ****************************************/

#include <Microduino_ColorLED.h>  //彩灯库
#include <Microduino_Key.h>       //按键库
#include <Microduino_AudioPro.h>  //audioPro库

#define LED_NUM 7                 //定义彩灯数量
#define PIN_LED SDA               //定义彩灯端口号

/***********定义彩灯颜色*/
const uint32_t colorArray[10] = {
  COLOR_NONE,       //0x000000, 彩灯关闭，colorLED OFF
  COLOR_RED,        //0xff0000, 红色，Red
  COLOR_ORANGE,     //0xea8b17, 橙色，Orange
  COLOR_YELLOW,     //0xffff00, 黄色，Yellow
  COLOR_GREEN,      //0x00ff00, 绿色，Green
  COLOR_CYAN,       //0x00ffff, 青色，Cyan
  COLOR_BLUE,       //0x0000ff, 蓝色，Blue
  COLOR_PURPLE,     //0xff00ff, 紫色，Purple
  COLOR_WARM,       //0x9b6400, 暖色，Warm
  COLOR_COLD,       //0x648264, 冷色，Cold
};

ColorLED strip = ColorLED(LED_NUM, PIN_LED);

AudioPro midiPlayer;

DigitalKey touchButton[7] = {   //touchButton引脚分配
  DigitalKey(4), DigitalKey(5), DigitalKey(6), DigitalKey(8), DigitalKey(9), DigitalKey(A0), DigitalKey(A6)
};

uint8_t toneNum[7] = {60, 62, 64, 65, 67, 69, 71};   //钢琴音阶

void setup() {
  /***********串口初始化*/
  Serial.begin(115200);

  /***********触摸按键初始化*/
  for (int a = 0; a < 7; a++) {
    touchButton[a].begin();
  }

  /*************彩灯初始化*/
  strip.begin();
  strip.setBrightness(80);
  strip.show(); // Initialize all pixels to 'off'

  /************audioPro初始化*/
  if (! midiPlayer.begin()) { // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1);
  }
  Serial.println(F("VS1053 found"));
  midiPlayer.applyPatch(MIDIPatch, sizeof(MIDIPatch) / sizeof(uint16_t));
  midiPlayer.midiSetVolume(0, 127);
  midiPlayer.midiSetBank(0, VS1053_BANK_DEFAULT);
  midiPlayer.midiSetInstrument(0, VS1053_GM1_ELECTRIC_GRAND_PIANO);
  delay(200);

  /**********彩灯与音乐逐渐点亮与响起*/
  for (int a = 0; a < 7; a++) {
    midiPlayer.noteOn(0, toneNum[a], 120);     //播放相应音级
    strip.setPixelColor(a, colorArray[a + 1]); //设置相应灯的颜色
    strip.show();                              //将灯点亮成新的颜色
    delay(100);
    midiPlayer.noteOff(0, toneNum[a], 120);    //关闭相应音级
    strip.setPixelColor(a, colorArray[0]);     //设置相应灯的颜色
    strip.show();                              //将灯点亮成新的颜色
  }
  delay(500);

  /***********彩灯与音乐同时点亮与响起*/
  for (int a = 0; a < 7; a++) {
    midiPlayer.noteOn(0, toneNum[a], 120);      //播放相应音级
    strip.setPixelColor(a, colorArray[a + 1]);  //设置相应灯的颜色
    strip.show();                               //将灯点亮成新设置的颜色
  }
  delay(1000);

  /************关闭彩灯与音乐*/
  for (int a = 0; a < 7; a++) {
    midiPlayer.noteOff(0, toneNum[a], 120);     //关闭相应音级
    strip.setPixelColor(a, colorArray[0]);      //设置相应灯的颜色
    strip.show();                               //将灯点亮成新设置的颜色
  }

  Serial.println(F("VS1053 found"));
}

void loop() {
  for (int a = 0; a < 7; a++) {
    uint8_t b = touchButton[a].readVal();       //获取touchButton状态
    if (b == KEY_PRESSING) {                    //按下touchButton
      midiPlayer.noteOn(0, toneNum[a], 120);    //播放相应音级
      strip.setPixelColor(a, colorArray[a + 1]);//设置相应灯的颜色
    }
    else if (b == KEY_RELEASING) {              //松开touchButton
      midiPlayer.noteOff(0, toneNum[a], 120);   //关闭相应音级
      strip.setPixelColor(a, colorArray[0]);    //设置相应灯的颜色
    }
    strip.show();                               //将灯点亮成新设置的颜色
  }
  delay(1);                                     //延时1ms
}

