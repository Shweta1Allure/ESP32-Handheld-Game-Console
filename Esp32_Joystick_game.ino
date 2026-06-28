#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_CS   5
#define TFT_RST  4
#define TFT_DC   2

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

// ================= JOYSTICK =================
#define VRX 34
#define VRY 35
#define BTN 32

// ================= STATES =================
#define MENU 0
#define PLAY_DINO 1
#define PLAY_SNAKE 2
#define PLAY_TETRIS 3
#define PLAY_REDLED 4
#define PLAY_TIMETAP 5
#define GAMEOVER_SCREEN 6
#define GAMEOVER_MENU 7



int state = MENU;


// =====================================================
// RED LED GAME
// =====================================================

#define GREEN1 12
#define GREEN2 13
#define GREEN3 14
#define GREEN4 25
#define GREEN5 26
#define REDLED 27

int leds[6] =
{
  GREEN1,
  GREEN2,
  GREEN3,
  GREEN4,
  GREEN5,
  REDLED
};

// =====================================================
// TIMETAP GAME
// =====================================================

//#define TIMETAP_BTN 15

unsigned long timeTapStart;

bool timeTapRunning = false;

float targetTime = 10.00;

// ================= MENU =================
int menuIndex = 0;
bool joyLock = false;

// ================= GAME OVER =================
int overIndex = 0;
int lastScore = 0;

int highDino = 0;
int highSnake = 0;
int highTetris = 0;

int activeGame = 0;

bool gameOverShown = false;

// =====================================================
// DINO VARIABLES
// =====================================================

int dinoX = 20;
int dinoY = 85;

int prevDinoY = 85;

bool jumping = false;

float velocity = 0;

int obsX = 160;
int obsY = 92;

int prevObsX = 160;

int dinoScore = 0;

unsigned long lastDinoTime = 0;

int dinoSpeed = 50;

// =====================================================
// SNAKE VARIABLES
// =====================================================

#define MAX 80
#define CELL 6

int sx[MAX], sy[MAX];

int slen = 5;

int dx = 1;
int dy = 0;

int fx, fy;

int snakeScore = 0;

unsigned long lastSnakeTime = 0;

int snakeSpeed = 120;

// =====================================================
// TETRIS VARIABLES
// =====================================================

#define COLS 10
#define ROWS 16
#define BLOCK 8

int grid[ROWS][COLS];

int blockX = 4;
int blockY = 0;

int tetrisScore = 0;

int lastTetrisScore = -1;

unsigned long lastFall = 0;

int fallSpeed = 400;

// ================= SHAPES =================

byte shapes[5][4][4] = {

{
  {0,1,0,0},
  {0,1,0,0},
  {0,1,0,0},
  {0,1,0,0}
},

{
  {1,1,0,0},
  {1,1,0,0},
  {0,0,0,0},
  {0,0,0,0}
},

{
  {1,0,0,0},
  {1,1,1,0},
  {0,0,0,0},
  {0,0,0,0}
},

{
  {0,0,1,0},
  {1,1,1,0},
  {0,0,0,0},
  {0,0,0,0}
},

{
  {0,1,1,0},
  {1,1,0,0},
  {0,0,0,0},
  {0,0,0,0}
}

};

byte currentShape[4][4];

// =====================================================
// SETUP
// =====================================================

void setup() {
 
for (int i = 0; i < 6; i++)
{
  pinMode(leds[i], OUTPUT);
}
  pinMode(21, OUTPUT);
  pinMode(22, OUTPUT);

  pinMode(BTN, INPUT_PULLUP);
  digitalWrite(21, HIGH);
  digitalWrite(22, HIGH);
  tft.initR(INITR_BLACKTAB);

  tft.setRotation(1);

  tft.fillScreen(ST77XX_BLACK);

  randomSeed(analogRead(34));

  showMenu();
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  if (state == MENU)
    handleMenu();

  else if (state == PLAY_DINO)
    runDino();

  else if (state == PLAY_SNAKE)
    runSnake();

  else if (state == PLAY_TETRIS)
    runTetris();
else if (state == PLAY_REDLED)
  runRedLed();

else if (state == PLAY_TIMETAP)
  runTimeTap();
  else if (state == GAMEOVER_SCREEN)
    handleGameOverScreen();

  else if (state == GAMEOVER_MENU)
    handleGameOverMenu();
}

// =====================================================
// MENU
// =====================================================

void showMenu() {

  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_GREEN);

  tft.setTextSize(1);

  tft.setCursor(35, 10);

  tft.print("GAME MENU");

  drawMenu();
}

void drawMenu()
{
  tft.fillRect(0, 40, 160, 90, ST77XX_BLACK);

  tft.setCursor(20, 40);
  tft.setTextColor(menuIndex == 0 ? ST77XX_YELLOW : ST77XX_WHITE);
  tft.print(menuIndex == 0 ? "> Dino" : "  Dino");

  tft.setCursor(20, 55);
  tft.setTextColor(menuIndex == 1 ? ST77XX_YELLOW : ST77XX_WHITE);
  tft.print(menuIndex == 1 ? "> Snake" : "  Snake");

  tft.setCursor(20, 70);
  tft.setTextColor(menuIndex == 2 ? ST77XX_YELLOW : ST77XX_WHITE);
  tft.print(menuIndex == 2 ? "> Tetris" : "  Tetris");

  tft.setCursor(20, 85);
  tft.setTextColor(menuIndex == 3 ? ST77XX_YELLOW : ST77XX_WHITE);
  // tft.print(menuIndex == 3 ? "> Red Led" : "  Red Led");

  // tft.setCursor(20, 100);
  // tft.setTextColor(menuIndex == 4 ? ST77XX_YELLOW : ST77XX_WHITE);
  tft.print(menuIndex == 3 ? "> TimeTap" : "  TimeTap");
}

void handleMenu()
{
  int y = analogRead(VRY);

  if (y > 3000 && !joyLock)
  {
    menuIndex++;

    if (menuIndex > 3)
      menuIndex = 0;

    drawMenu();

    joyLock = true;
  }

  if (y < 1000 && !joyLock)
  {
    menuIndex--;

    if (menuIndex < 0)
      menuIndex = 3;

    drawMenu();

    joyLock = true;
  }

  if (y > 1500 && y < 2500)
    joyLock = false;

  if (digitalRead(BTN) == LOW)
  {
    delay(200);

    if (menuIndex == 0)
      startDino();

    else if (menuIndex == 1)
      startSnake();

    else if (menuIndex == 2)
      startTetris();

    else if (menuIndex == 3)
    //   startRedLed();

    // else if (menuIndex == 4)
      startTimeTap();
  }
}

// =====================================================
// DINO GAME
// =====================================================

void startDino() {

  state = PLAY_DINO;

  dinoY = 85;

  prevDinoY = 85;

  obsX = 160;

  prevObsX = 160;

  dinoScore = 0;

  jumping = false;

  velocity = 0;

  tft.fillScreen(ST77XX_BLACK);

  tft.drawLine(0, 112, 160, 112, ST77XX_WHITE);
}

void runDino() {

  if (millis() - lastDinoTime < dinoSpeed)
    return;

  lastDinoTime = millis();

  tft.fillRect(dinoX, prevDinoY, 24, 24, ST77XX_BLACK);

  tft.fillRect(prevObsX, obsY, 12, 20, ST77XX_BLACK);

  tft.drawLine(0, 112, 160, 112, ST77XX_WHITE);

  if (digitalRead(BTN) == LOW && !jumping) {

    jumping = true;

    velocity = -12;
  }

  if (jumping) {

    dinoY += velocity;

    velocity += 1;

    if (dinoY >= 85) {

      dinoY = 85;

      jumping = false;
    }
  }

  obsX -= 2;

  if (obsX < -12) {

    obsX = 160;

    dinoScore++;
  }

  if (obsX < dinoX + 20 &&
      obsX + 12 > dinoX &&
      obsY < dinoY + 20 &&
      obsY + 20 > dinoY) {

    lastScore = dinoScore;

    activeGame = PLAY_DINO;

    if (dinoScore > highDino)
      highDino = dinoScore;

    gameOverShown = false;

    state = GAMEOVER_SCREEN;

    return;
  }

  drawDino(dinoX, dinoY);

  drawCactus(obsX, obsY);

  tft.fillRect(0, 0, 80, 10, ST77XX_BLACK);

  tft.setCursor(2, 2);

  tft.setTextColor(ST77XX_WHITE);

  tft.print("Score:");

  tft.print(dinoScore);

  prevDinoY = dinoY;

  prevObsX = obsX;
}

void drawDino(int x, int y) {

  tft.fillRect(x + 8, y, 10, 8, ST77XX_GREEN);

  tft.fillRect(x + 5, y + 8, 12, 10, ST77XX_GREEN);

  tft.fillRect(x, y + 12, 5, 3, ST77XX_GREEN);

  tft.fillRect(x + 6, y + 18, 3, 6, ST77XX_GREEN);

  tft.fillRect(x + 13, y + 18, 3, 6, ST77XX_GREEN);
}

void drawCactus(int x, int y) {

  tft.fillRect(x + 3, y, 4, 18, ST77XX_RED);
}

// =====================================================
// SNAKE GAME
// =====================================================

void startSnake() {

  state = PLAY_SNAKE;

  slen = 5;

  snakeScore = 0;

  for (int i = 0; i < slen; i++) {

    sx[i] = 60 - i * CELL;

    sy[i] = 60;
  }

  dx = 1;
  dy = 0;

  spawnFood();

  tft.fillScreen(ST77XX_BLACK);
}

void spawnFood() {

  fx = random(2, 24) * CELL;

  fy = random(2, 18) * CELL;
}

void runSnake() {

  // ================= SPEED CONTROL =================
  if (millis() - lastSnakeTime < snakeSpeed)
    return;

  lastSnakeTime = millis();

  // ================= CLEAR OLD SNAKE =================
  // ================= CLEAR OLD SNAKE =================
for (int i = 0; i < slen; i++) {

  tft.fillCircle(
    sx[i] + 3,
    sy[i] + 3,
    4,
    ST77XX_BLACK
  );
}

  // ================= JOYSTICK INPUT =================
  int x = analogRead(VRX);

  int y = analogRead(VRY);

  // LEFT
  if (x < 1000 && dx != 1) {

    dx = -1;
    dy = 0;
  }

  // RIGHT
  else if (x > 3000 && dx != -1) {

    dx = 1;
    dy = 0;
  }

  // UP
  else if (y < 1000 && dy != 1) {

    dx = 0;
    dy = -1;
  }

  // DOWN
  else if (y > 3000 && dy != -1) {

    dx = 0;
    dy = 1;
  }

  // ================= MOVE BODY =================
  for (int i = slen - 1; i > 0; i--) {

    sx[i] = sx[i - 1];

    sy[i] = sy[i - 1];
  }

  // ================= MOVE HEAD =================
  sx[0] += dx * CELL;

  sy[0] += dy * CELL;

  // ================= WALL COLLISION =================
  if (sx[0] < 0 || sx[0] > 154 ||
      sy[0] < 0 || sy[0] > 122) {

    lastScore = snakeScore;

    activeGame = PLAY_SNAKE;

    if (snakeScore > highSnake)
      highSnake = snakeScore;

    gameOverShown = false;

    state = GAMEOVER_SCREEN;

    return;
  }

  // ================= SELF COLLISION =================
  for (int i = 1; i < slen; i++) {

    if (sx[0] == sx[i] &&
        sy[0] == sy[i]) {

      lastScore = snakeScore;

      activeGame = PLAY_SNAKE;

      if (snakeScore > highSnake)
        highSnake = snakeScore;

      gameOverShown = false;

      state = GAMEOVER_SCREEN;

      return;
    }
  }

  // ================= FOOD EAT =================
  if (sx[0] == fx &&
      sy[0] == fy) {

    // ADD NEW PART
    sx[slen] = sx[slen - 1];

    sy[slen] = sy[slen - 1];

    slen++;

    snakeScore++;

    spawnFood();
  }

  // ================= DRAW FOOD =================
  tft.fillCircle(
    fx + 3,
    fy + 3,
    3,
    ST77XX_RED
  );

  // ================= DRAW SNAKE =================
  for (int i = 0; i < slen; i++) {

    // HEAD
    if (i == 0) {

      tft.fillCircle(
        sx[i] + 3,
        sy[i] + 3,
        3,
        ST77XX_YELLOW
      );

      // EYES
      tft.drawPixel(
        sx[i] + 2,
        sy[i] + 2,
        ST77XX_BLACK
      );

      tft.drawPixel(
        sx[i] + 4,
        sy[i] + 2,
        ST77XX_BLACK
      );
    }

    // BODY
    else {

      tft.fillCircle(
        sx[i] + 3,
        sy[i] + 3,
        3,
        ST77XX_GREEN
      );
    }
  }

  // ================= SCORE =================
  tft.fillRect(0, 0, 70, 10, ST77XX_BLACK);

  tft.setCursor(2, 2);

  tft.setTextColor(ST77XX_WHITE);

  tft.setTextSize(1);

  tft.print("S:");

  tft.print(snakeScore);
}

// =====================================================
// TETRIS GAME
// =====================================================

void startTetris() {

  state = PLAY_TETRIS;

  memset(grid, 0, sizeof(grid));

  tetrisScore = 0;

  lastTetrisScore = -1;

  spawnBlock();

  tft.fillScreen(ST77XX_BLACK);

  drawBorder();
}

void drawBorder() {

  tft.drawRect(10, 0,
               COLS * BLOCK + 2,
               ROWS * BLOCK + 2,
               ST77XX_WHITE);

  tft.setCursor(100, 10);

  tft.setTextColor(ST77XX_WHITE);

  tft.print("Score");
}

void spawnBlock() {

  int s = random(0, 5);

  for (int r = 0; r < 4; r++) {

    for (int c = 0; c < 4; c++) {

      currentShape[r][c] = shapes[s][r][c];
    }
  }

  blockX = 4;

  blockY = 0;

  drawBlock();
}



void rotateShape() {

  byte temp[4][4];

  // CREATE ROTATED SHAPE
  for (int r = 0; r < 4; r++) {

    for (int c = 0; c < 4; c++) {

      temp[r][c] = currentShape[3 - c][r];
    }
  }

  // REMOVE OLD SHAPE
  eraseBlock();

  // COPY ROTATED SHAPE
  for (int r = 0; r < 4; r++) {

    for (int c = 0; c < 4; c++) {

      currentShape[r][c] = temp[r][c];
    }
  }

  // DRAW NEW SHAPE
  drawBlock();
}


void runTetris() {

  handleTetrisInput();

  if (millis() - lastFall > fallSpeed) {

    lastFall = millis();

    if (canMove(blockX, blockY + 1)) {

      eraseBlock();

      blockY++;

      drawBlock();
    }

    else {

      mergeBlock();

      clearLines();

      spawnBlock();

      if (!canMove(blockX, blockY)) {

        lastScore = tetrisScore;

        activeGame = PLAY_TETRIS;

        if (tetrisScore > highTetris)
          highTetris = tetrisScore;

        gameOverShown = false;

        state = GAMEOVER_SCREEN;
      }
    }
  }
}

void drawBlock() {

  for (int r = 0; r < 4; r++) {

    for (int c = 0; c < 4; c++) {

      if (currentShape[r][c]) {

        tft.fillRect(
          11 + (blockX + c) * BLOCK,
          1 + (blockY + r) * BLOCK,
          BLOCK - 1,
          BLOCK - 1,
          ST77XX_CYAN
        );
      }
    }
  }
}

void eraseBlock() {

  for (int r = 0; r < 4; r++) {

    for (int c = 0; c < 4; c++) {

      if (currentShape[r][c]) {

        tft.fillRect(
          11 + (blockX + c) * BLOCK,
          1 + (blockY + r) * BLOCK,
          BLOCK - 1,
          BLOCK - 1,
          ST77XX_BLACK
        );
      }
    }
  }
}

bool canMove(int nx, int ny) {

  for (int r = 0; r < 4; r++) {

    for (int c = 0; c < 4; c++) {

      if (currentShape[r][c]) {

        int gx = nx + c;
        int gy = ny + r;

        if (gx < 0 || gx >= COLS || gy >= ROWS)
          return false;

        if (gy >= 0 && grid[gy][gx])
          return false;
      }
    }
  }

  return true;
}

void mergeBlock() {

  for (int r = 0; r < 4; r++) {

    for (int c = 0; c < 4; c++) {

      if (currentShape[r][c]) {

        int gx = blockX + c;
        int gy = blockY + r;

        if (gy >= 0)
          grid[gy][gx] = 1;
      }
    }
  }

  redrawGrid();
}

void redrawGrid() {

  for (int r = 0; r < ROWS; r++) {

    for (int c = 0; c < COLS; c++) {

      tft.fillRect(
        11 + c * BLOCK,
        1 + r * BLOCK,
        BLOCK - 1,
        BLOCK - 1,
        grid[r][c] ? ST77XX_GREEN : ST77XX_BLACK
      );
    }
  }
}

void clearLines() {

  for (int r = 0; r < ROWS; r++) {

    bool full = true;

    for (int c = 0; c < COLS; c++) {

      if (!grid[r][c]) {

        full = false;
        break;
      }
    }

    if (full) {

      for (int rr = r; rr > 0; rr--) {

        for (int cc = 0; cc < COLS; cc++) {

          grid[rr][cc] = grid[rr - 1][cc];
        }
      }

      for (int cc = 0; cc < COLS; cc++)
        grid[0][cc] = 0;

      tetrisScore++;

      if (tetrisScore != lastTetrisScore) {

        tft.fillRect(100, 25, 40, 10, ST77XX_BLACK);

        tft.setCursor(100, 25);

        tft.print(tetrisScore);

        lastTetrisScore = tetrisScore;
      }

      redrawGrid();
    }
  }
}

void handleTetrisInput() {

  int x = analogRead(VRX);

  int y = analogRead(VRY);

  // LEFT
  if (x < 1000) {

    if (canMove(blockX - 1, blockY)) {

      eraseBlock();

      blockX--;

      drawBlock();
    }

    delay(120);
  }

  // RIGHT
  if (x > 3000) {

    if (canMove(blockX + 1, blockY)) {

      eraseBlock();

      blockX++;

      drawBlock();
    }

    delay(120);
  }

  // DOWN FAST
  if (y > 3000) {

    if (canMove(blockX, blockY + 1)) {

      eraseBlock();

      blockY++;

      drawBlock();
    }

    delay(60);
  }

  // ROTATE SHAPE
  if (digitalRead(BTN) == LOW) {

    rotateShape();

    delay(200);
  }
}
// =====================================================
// GAME OVER
// =====================================================

void handleGameOverScreen() {

  if (!gameOverShown) {

    tft.fillScreen(ST77XX_BLACK);

    tft.setTextColor(ST77XX_RED);

    tft.setTextSize(2);

    tft.setCursor(15, 30);

    tft.print("GAME OVER");

    tft.setTextSize(1);

    tft.setTextColor(ST77XX_WHITE);

    tft.setCursor(20, 75);

    tft.print("Score:");

    tft.print(lastScore);

   tft.setCursor(20, 95);

tft.print("High:");

if (activeGame == PLAY_DINO)
  tft.print(highDino);

else if (activeGame == PLAY_SNAKE)
  tft.print(highSnake);

else
  tft.print(highTetris);

tft.setCursor(20, 115);

tft.print("Press Joystick");

    gameOverShown = true;
  }

  if (digitalRead(BTN) == LOW) {

    delay(250);

    state = GAMEOVER_MENU;

    overIndex = 0;

    drawGameOverMenu();
  }
}

void drawGameOverMenu() {

  tft.fillScreen(ST77XX_BLACK);

  tft.setCursor(20, 50);

  tft.setTextColor(overIndex == 0 ?
                   ST77XX_YELLOW : ST77XX_WHITE);

  tft.print(overIndex == 0 ?
            "> Play Again" : "  Play Again");

  tft.setCursor(20, 80);

  tft.setTextColor(overIndex == 1 ?
                   ST77XX_YELLOW : ST77XX_WHITE);

  tft.print(overIndex == 1 ?
            "> Menu" : "  Menu");
}

void handleGameOverMenu() {

  int y = analogRead(VRY);

  if (y < 1000 && !joyLock) {

    overIndex = 1 - overIndex;

    joyLock = true;

    drawGameOverMenu();
  }

  if (y > 3000 && !joyLock) {

    overIndex = 1 - overIndex;

    joyLock = true;

    drawGameOverMenu();
  }

  if (y > 1500 && y < 2500)
    joyLock = false;

  if (digitalRead(BTN) == LOW) {

    delay(250);

   if (overIndex == 0) {

  if (activeGame == PLAY_DINO)
    startDino();

  else if (activeGame == PLAY_SNAKE)
    startSnake();

  else if (activeGame == PLAY_TETRIS)
    startTetris();

  else if (activeGame == PLAY_REDLED)
    startRedLed();

  else if (activeGame == PLAY_TIMETAP)
    startTimeTap();
}

    else {

      state = MENU;

      showMenu();
    }
  }
} 


// =====================================================
// RED LED GAME
// =====================================================

void allLEDOff()
{
  for (int i = 0; i < 6; i++)
  {
    digitalWrite(leds[i], LOW);
  }
}

void startRedLed()
{
  state = PLAY_REDLED;

  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_CYAN);

  tft.setTextSize(2);

  tft.setCursor(10, 20);

  tft.println("RED LED GAME");

  tft.setCursor(20, 50);

  //tft.println("GAME");

  delay(1000);

  tft.fillScreen(ST77XX_BLACK);
}

void runRedLed()
{
  for (int i = 0; i < 6; i++)
  {
    allLEDOff();

    digitalWrite(leds[i], HIGH);

    unsigned long st = millis();

    while (millis() - st < 120)
    {
      if (digitalRead(BTN) == LOW)
      {
        delay(150);

        allLEDOff();

        tft.fillScreen(ST77XX_BLACK);

        tft.setTextSize(2);

        if (i == 5)
        {
          tft.setTextColor(ST77XX_GREEN);

          tft.setCursor(25, 40);

          tft.println("YOU WIN");
        }
        else
        {
          tft.setTextColor(ST77XX_RED);

          tft.setCursor(20, 40);

          tft.println("YOU LOSE");
        }

        tft.setTextSize(1);

        tft.setCursor(10, 90);

        tft.println("PRESS JOYSTICK");

        while (digitalRead(BTN) == HIGH)
        {
        }

        delay(300);

        activeGame = PLAY_REDLED;

        state = GAMEOVER_MENU;

        overIndex = 0;

        drawGameOverMenu();

        return;
      }
    }
  }
}

// =====================================================
// TIMETAP GAME
// =====================================================

void drawTimeTapScreen()
{
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_CYAN);

  tft.setTextSize(2);

  tft.setCursor(25, 20);

  tft.println("TIMETAP");

  tft.setTextSize(1);

  tft.setCursor(15, 90);

  tft.println("PRESS BUTTON");

  tft.setCursor(30, 105);

  tft.println("TO START");
}

void timeTapCountdown()
{
  String txt[4] = {"3", "2", "1", "GO!"};

  for (int i = 0; i < 4; i++)
  {
    tft.fillRect(0, 20, 160, 80, ST77XX_BLACK);

    tft.setTextColor(ST77XX_RED);

    if (i < 3)
    {
      tft.setTextSize(5);

      tft.setCursor(60, 35);
    }
    else
    {
      tft.setTextSize(3);

      tft.setCursor(35, 45);
    }

    tft.print(txt[i]);

    delay(700);
  }

  tft.fillScreen(ST77XX_BLACK);
}

void startTimeTap()
{
  state = PLAY_TIMETAP;

  timeTapRunning = false;

  drawTimeTapScreen();
}

void runTimeTap()
{
  if (!timeTapRunning)
  {
    if (digitalRead(BTN) == LOW)
    {
      delay(250);

      timeTapCountdown();

      timeTapStart = millis();

      timeTapRunning = true;
    }
  }

  else
  {
    float current =
      (millis() - timeTapStart) / 1000.0;

    tft.fillRect(0, 40, 160, 50, ST77XX_BLACK);

    if (current >= 9.5)
      tft.setTextColor(ST77XX_RED);

    else
      tft.setTextColor(ST77XX_GREEN);

    tft.setTextSize(3);

    tft.setCursor(20, 45);

    tft.print(current, 2);

    tft.setTextSize(1);

    tft.setTextColor(ST77XX_WHITE);

    tft.setCursor(30, 10);

    tft.print("STOP AT 10.00");

    if (digitalRead(BTN) == LOW)
    {
      delay(250);

      timeTapRunning = false;

      float diff = abs(10.00 - current);

      tft.fillScreen(ST77XX_BLACK);

      tft.setTextSize(2);

      if (diff <= 0.03)
      {
        tft.setTextColor(ST77XX_GREEN);

        tft.setCursor(20, 20);

        tft.println("PERFECT!");
      }

      else if (diff <= 0.15)
      {
        tft.setTextColor(ST77XX_YELLOW);

        tft.setCursor(15, 20);

        tft.println("GOOD TRY");
      }

      else
      {
        tft.setTextColor(ST77XX_RED);

        tft.setCursor(20, 20);

        tft.println("YOU LOSE");
      }

      tft.setTextSize(1);

      tft.setTextColor(ST77XX_WHITE);

      tft.setCursor(10, 70);

      tft.print("Your Time: ");

      tft.print(current, 2);

      tft.setCursor(10, 90);

      tft.print("Difference: ");

      tft.print(diff, 2);

      tft.println(" sec");

      tft.setCursor(10, 115);

      tft.println("PRESS JOYSTICK");

      while (digitalRead(BTN) == HIGH)
      {
      }

      delay(300);

      activeGame = PLAY_TIMETAP;

      state = GAMEOVER_MENU;

      overIndex = 0;

      drawGameOverMenu();
    }
  }
}

