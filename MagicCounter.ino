
#include <LiquidCrystal.h>

#define LCD_RS 8
#define LCD_E 9
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7

#define P_BUTTON A0

#define UP_K 1
#define DOWN_K 2
#define LEFT_K 3
#define RIGHT_K 4
#define SWITCH_K 5

#define UP button == UP_K
#define DOWN button == DOWN_K
#define LEFT button == LEFT_K
#define RIGHT button == RIGHT_K
#define SWITCH button == SWITCH_K

#define MAX_PLAYERS 4

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

enum GameModeType {
  duel,
  troll,
  arene,
  edh
};

enum EditModeType {
  pv, ps, pg
};

typedef struct {
  int pv; // PV
  int ps; // Poison
  int pg; // Degats de generaux
} PlayerType;

GameModeType game = duel;
EditModeType edit = pv;
PlayerType players[MAX_PLAYERS];
int nbPlayer = 2;
int currentPlayer = 0; // utilisé que pour le multi
int button = 0;

void setup() {
  
  lcd.begin(16,2);

  byte heart[8] = { B00000, B01010, B11111, B11111, B01110, B00100, B00000, B00000 };
  lcd.createChar(0, heart);
  byte phyrerian[8] = { B00100, B01110, B10101, B10101, B10101, B01110, B00100, B00000 };
  lcd.createChar(1, phyrerian);
  byte skull[8] = { B00000, B01110, B10101, B11011, B01110, B01010, B00000, B00000 };
  lcd.createChar(2, skull);

  selectGameMode();
  if (game == edh || game == arene)
    selectNbPlayer();
  displayStartingPlayer();

  int pv = 20;
  if (game == edh || game == troll) {
    pv = 30;
  }
  if (nbPlayer > 2 && game == edh) {
    pv = 40;
  }
  for (int i=0;i<nbPlayer;i++) {
    players[i].pv = pv;
    players[i].ps = 0;
    players[i].pg = 0;
  }
}

void selectGameMode() {
  boolean selected = false;
  while (!selected) {
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Game Mode");
    switch(game) {
      case duel : 
        lcd.setCursor(5,1);
        lcd.print("Duel");
        break;
      case troll :
        lcd.setCursor(5,1);
        lcd.print("Troll");
        break;
      case arene :
        lcd.setCursor(5,1);
        lcd.print("Arene");
        break;
      case edh : 
        lcd.setCursor(6,1);
        lcd.print("EDH");
        break;
    }
    onePressed();
    if (UP || RIGHT) {
      game = game == edh ? duel : (GameModeType)((int)game+1);
    }
    if (DOWN || LEFT) {
      game = game == duel ? edh : (GameModeType)((int)game-1);
    }
    if (SWITCH) {
      selected = true;
    }
    allReleased();
  }
}

void selectNbPlayer() {
  boolean selected = false;
  while (!selected) {
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Players");
    lcd.setCursor(6,1);
    lcd.print(nbPlayer);
    onePressed();
    if (UP || RIGHT) {
      nbPlayer = nbPlayer == MAX_PLAYERS ? 2 : nbPlayer+1;
    }
    if (DOWN || LEFT) {
      nbPlayer = nbPlayer == 2 ? MAX_PLAYERS : nbPlayer-1;
    }
    if (SWITCH) {
      selected = true;
    }
    allReleased();
  }
}

void displayStartingPlayer() {
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Start ");
  if (game == troll)
    lcd.print("Team ");
  else 
    lcd.print("Player ");
  lcd.print(random(nbPlayer)+1);
  onePressed();
  allReleased();
}

void loop() {
  lcd.clear();
  if (nbPlayer == 2) {
      printTwoPlayers();
  } else {
      printManyPlayers();
  }
  updateKeys();
}

void printTwoPlayers() {
  for (int i=0;i<nbPlayer;i++) {
    lcd.setCursor(0,i);
    lcd.print(game == troll ? "T" : "P");
    lcd.print(i+1);
    lcd.setCursor(3,i);
    lcd.print(players[i].pv);
    lcd.write(byte(0));
    if (players[i].ps != 0) {
      lcd.setCursor(7,i);
      lcd.print(players[i].ps);
      lcd.write(byte(1));
    }
    if (players[i].pg != 0) {
      lcd.setCursor(11,i);
      lcd.print(players[i].pg);
      lcd.write(byte(2));
    }
  }
  lcd.setCursor(15,0);
  lcd.write((byte)edit); 
  lcd.setCursor(15,0);
  lcd.blink();
}

void printManyPlayers() {
  lcd.setCursor(0,0);
  lcd.print('P');
  lcd.print(currentPlayer+1);
  lcd.setCursor(3,0);
  lcd.print(players[currentPlayer].pv);
  lcd.write(byte(0));
  if (players[currentPlayer].ps != 0) {
    lcd.setCursor(7,0);
    lcd.print(players[currentPlayer].ps);
    lcd.write(byte(1));
  }
  if (players[currentPlayer].pg != 0) {
    lcd.setCursor(11,0);
    lcd.print(players[currentPlayer].pg);
    lcd.write(byte(2));
  }
  
  for (int i=0;i<nbPlayer;i++) {
    if (i!=currentPlayer) {
      lcd.setCursor(i*6 - (i>currentPlayer?6:0) ,1);
      lcd.print(i+1);
      lcd.print(':');
      lcd.print(players[i].pv);
    }
  }
  lcd.setCursor(15,0);
  lcd.write((byte)edit); 
  lcd.setCursor(15,0);
  lcd.blink();
}

void updateKeys() {
  onePressed();
  if (SWITCH) {
    edit = edit == (game==edh?pg:ps) ? pv : (EditModeType)((int)edit+1);
  }

  if (nbPlayer == 2) {
    if (UP) {
      upPlayer(0);
    }
    if (DOWN) {
      downPlayer(0);
    }
    if (LEFT) {
      upPlayer(1);
    }
    if (RIGHT) {
      downPlayer(1);
    }
  } else {
    if (UP) {
      upPlayer(currentPlayer);
    }
    if (DOWN) {
      downPlayer(currentPlayer);
    }
    if (RIGHT) {
      currentPlayer = (currentPlayer+1) % nbPlayer;
    }
    if (LEFT) {
      currentPlayer = (currentPlayer+nbPlayer-1) % nbPlayer;
    }
  }
  allReleased();
}

void upPlayer(int p) {
  if (edit == pv) 
    players[p].pv++;
  else if (edit == ps) 
    players[p].ps++;
  else 
    players[p].pg++;
}

void downPlayer(int p) {
  if (edit == pv) 
    players[p].pv--;
  else if (edit == ps && players[p].ps > 0) 
    players[p].ps--;
  else if (edit == pg && players[p].pg > 0) 
    players[p].pg--;
}
void onePressed() {
  refreshKeys();
  while (!button) { 
    delay(1);
    refreshKeys();
  };
}

void allReleased() {
  refreshKeys();
  while (button) { 
    delay(1);
    refreshKeys();
  };
}

void refreshKeys() {
  random();
  int value = analogRead(A0);
  if (value < 50) button = RIGHT_K; 
  else if (value < 195) button = UP_K;
  else if (value < 380) button = DOWN_K;
  else if (value < 555) button = LEFT_K;
  else if (value < 790) button = SWITCH_K;  
  else button = 0;
}

