#include <INTNode.h>

// ---- buzzer ----
#define speakerOut  5
int A = 440;
int B = 494;
int C = 523;
int D = 587;
int F = 659;
int G = 784;
int frequencies[] = {A, B, C, D, F, G};

// ---- time for games ----
unsigned long timeForShow;
int timeForNextPos = 1000;

// ---- Leds ----
#define RLed 11
#define YLed 10
#define GLed 9
#define BLed 6
#define LENGTH_ARRAY 4
int Leds[] = {RLed, YLed, GLed, BLed};

void OnAllLeds(){
  for(int k = 0; k < LENGTH_ARRAY; k++){
    digitalWrite(Leds[k], HIGH);
  }
}

void OffAllLeds(){
  for(int k = 0; k < LENGTH_ARRAY; k++){
    digitalWrite(Leds[k], LOW);
  }
}

// ---- buttons ------
#define RBtn 12
#define GBtn 4
#define BBtn 3
#define YBtn 2
int Buttons[] = {RBtn, YBtn, GBtn, BBtn};
bool IsPress = false;


int IsPressed(){
 if(digitalRead(Buttons[0]) == LOW){
  return 0;
 }
 if(digitalRead(Buttons[1]) == LOW){
  return 1;
 }
 if(digitalRead(Buttons[2]) == LOW){
  return 2;
 }
 if(digitalRead(Buttons[3]) == LOW){
  return 3;
 }

 return -1;
}



// ---- game manager -----
#define START_GAME 21
#define LEVEL_UP 22
#define SHOW_COLOR 23
#define PLAYER_TURN 24
int currState;
bool endTurn = true;

INTNode* head = NULL;
INTNode* last; 
INTNode* plr;
int level = 0;


void nextLevel(){
  int rnd = random(0,LENGTH_ARRAY);
  if(head == NULL){
     head = new INTNode(rnd, NULL);
     last = head;
  }else{
    last->SetNext(new INTNode(rnd, NULL));
    last = last->GetNext();
    level++;
  }
}

void showLevel(){
  INTNode* pos = head;
  if((level % 5 == 0) && (timeForNextPos > 200)){
    timeForNextPos -= 100;
  }
  
  while(pos != NULL){
    int index = pos->GetVal();
    digitalWrite(Leds[index],HIGH);
    tone(speakerOut,frequencies[index]);
    
    if(millis() - timeForShow > timeForNextPos){
      digitalWrite(Leds[index],LOW);
      noTone(speakerOut);
      delay(100);
      pos = pos->GetNext();
      timeForShow = millis();
    }
  }
}

void setup() {
  Serial.begin(9600);
  for(int k = 0; k < LENGTH_ARRAY; k++){
    pinMode(Leds[k], OUTPUT);
    pinMode(Buttons[k], INPUT_PULLUP);
  }
  randomSeed(analogRead(A6));
  currState = START_GAME;
}

void loop() {
  switch(currState){
    case START_GAME:
      OnAllLeds();
      if(IsPressed() == 2){
        OffAllLeds();
        head = NULL;
        level = 0;
        currState = LEVEL_UP;
      }
      break;
     case LEVEL_UP:
        nextLevel();
        timeForShow = millis();
        currState = SHOW_COLOR;
      break;
     case SHOW_COLOR:
        showLevel();
        endTurn = true;
        currState = PLAYER_TURN;
        
      break;
     case PLAYER_TURN:
     if(endTurn){
      plr = head;
      endTurn = false;
     }
     
     int index = IsPressed();
       if((index >= 0) && (index != plr->GetVal())){
          currState = START_GAME;
       }else if((index >= 0)){
        IsPress = true;
        digitalWrite(Leds[index],HIGH);
        tone(speakerOut,frequencies[index]);
       }else if((IsPress)){
          IsPress = false;
          digitalWrite(Leds[plr->GetVal()],LOW);
          noTone(speakerOut);
          plr = plr->GetNext();
        }
            
      
      if(plr == NULL){
           currState = LEVEL_UP;
         }
      break;
  }
}
