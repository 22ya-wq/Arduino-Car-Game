#include "LedControl.h"

int J_VRX = A0;
int J_VRY = A1;
int SPEAKER = A5;

int leds[] = {0, 1, 2, 3, 4, 5, 6, 7};
LedControl lc = LedControl(8, 10, 9, 1);

int sp = 0;
int objRow = 0;

uint32_t timer;
uint32_t timer2;

int gameBoard[8][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

uint64_t IMAGES[] = {
  0x0000000000000000,
  0x0000000000400000,
  0x0000000000600000,
  0x0000000000700000,
  0x0000000000780000,
  0x0000000008780000,
  0x0000000808780000,
  0x0000080808780000,
  0x00000808087c0000,
  0x00000808087e0000,
  0x000008080a7e0000,
  0x0000080a0a7e0000,
  0x00000a0a0a7e0000
};

int IMAGES_LEN = sizeof(IMAGES) / 8;

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 5);
  lc.clearDisplay(0);

  pinMode(SPEAKER, OUTPUT);

  randomSeed(analogRead(A1) + analogRead(A2) + micros());

  timer = millis();
  timer2 = millis();

  objsSpawn();
  objsFall();
}

void updateBoard() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (gameBoard[i][j] == 0) {
        lc.setLed(0, leds[i], leds[j], false);
      }
      if (gameBoard[i][j] == 1) {
        lc.setLed(0, leds[i], leds[j], true);
        gameBoard[i][j] = 0;
      }
      if (gameBoard[i][j] == 2) {
        lc.setLed(0, leds[i], leds[j], true);
      }
    }
  }
}

void displayImage(uint64_t image) {
  for (int i = 0; i < 8; i++) {
    byte row = (image >> (i * 8)) & 0xFF;
    lc.setRow(0, i, row);
  }
}

void (* resetFunc) (void) = 0;

void objsSpawn() {
  objRow = random(0, 8);
  gameBoard[objRow][7] = 2;
}

void dieChecking() {
  if (gameBoard[sp][1] == 2 || gameBoard[sp][2] == 2) {
    tone(SPEAKER, 40, 60);
    for (int i = 0; i < IMAGES_LEN; i++) {
      displayImage(IMAGES[i]);
      delay(150);
    }
    delay(500);
    resetFunc();
  }else {
    gameBoard[sp][1] = 1;
    gameBoard[sp][2] = 1;
  }
}

void objsFall() {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      if (gameBoard[i][j] == 2) {
        gameBoard[i][j] = 0;
        if (j != 0) {
        gameBoard[i][j - 1] = 2;
        }else {
          gameBoard[i][j] = 0;
        } 
      }
    }
  }
  tone(SPEAKER, 500, 60);
}

void loop() {
  int x = map(analogRead(J_VRX), 0, 1023, -1, 2);
  x = constrain(x, -1, 1);
  int speed = x;

  if (millis() - timer >= 500) {
    objsSpawn();
    timer = millis();
  }
  if (millis() - timer2 >= 500) {
    objsFall();
    timer2 = millis();
  }   

  sp -= speed;
  sp = constrain(sp, 0, 7);
  
  delay(50);
  updateBoard();
  dieChecking();
}
