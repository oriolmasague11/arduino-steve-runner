#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Grafics.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int BOTO_A = 3; 
const int BOTO_B = 2; 
const int LED = 10; 

bool detectA = false; 
bool detectB = false; 

void isr_boto_A() {
  detectA = true; 
}

void isr_boto_B() {
  detectB = true; 
}

void presentacio_adafruit() {
  oled.display(); 
  delay(2000);
}

void presentacio_arduino() {
  oled.clearDisplay();
  oled.drawBitmap((oled.width()-ARDUINO_W)/2, (oled.height()-ARDUINO_H)/2, arduino_logo, ARDUINO_W, ARDUINO_H, WHITE);
  oled.display();
  delay(2000);
}

void presentacio_elegoo() {
  oled.clearDisplay();
  oled.drawBitmap((oled.width()-ELEGOO_W)/2, (oled.height()-ELEGOO_H)/2, elegoo_logo, ELEGOO_W, ELEGOO_H, WHITE);
  oled.display();
  delay(2000);
}

void presentacio_chrome() {
  oled.clearDisplay();
  oled.drawBitmap((oled.width()-CHROME_W)/2, (oled.height()-CHROME_H)/2, chrome_logo, CHROME_W, CHROME_H, WHITE);
  oled.display();
  delay(2000);
}

void show_sponsors() {
  presentacio_adafruit(); 
  presentacio_arduino(); 
  presentacio_elegoo(); 
  presentacio_chrome();
  oled.clearDisplay(); 
  oled.display(); 
  delay(1000);
}

void setup() {
  Serial.begin(9600);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while(1);
  }
  pinMode(BOTO_A,INPUT);
  pinMode(BOTO_B,INPUT);
  pinMode(LED, OUTPUT);
  //Programem les interrupcions
  //attachInterrupt(digitalPinToInterrupt(BOTO_A), isr_boto_A, RISING);
  //attachInterrupt(digitalPinToInterrupt(BOTO_B), isr_boto_B, RISING);
  //Patrocinadors
  show_sponsors(); 

  afegir_nuvols(); 
}

#define XPOS    0
#define YPOS    1
#define ID      2
#define SPRITEX 3
#define ALTX    4     
const int num_obj = 8;    //capacitat per a 8 objectes per pantalla
int obj[num_obj][5] = {
  {-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1},
  {-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1}
};

#define DX      0
#define DY      1
#define DID     2
#define DSPRITE 3
#define DALT    4
int dino[5] = {-1,-1,-1,-1,-1};

int alternar(int x) {
  if(x == -1) return 1; 
  else return -1; 
}

void horitzontal(int y) {
  oled.drawLine(0,y,oled.width(),y,SSD1306_WHITE);
}

bool alive = true; 
const int x0 = 0; 
int y0;
bool air = false; 
int cont = 0; 

const int canvi = 3;    //cada quan canvia de cama el dino 
void drawSteve() { 
  if (!air) {
    if(cont != 0 && cont%canvi == 0) { 
      dino[DSPRITE] += dino[DALT]; 
      dino[DALT] = alternar(dino[DALT]); 
    }
  }
  int did = dino[DID];
  int dsprite = dino[DSPRITE];
  oled.drawBitmap(dino[DX], dino[DY], skins[did][dsprite], info_dino[did][dsprite][WIDTH], info_dino[did][dsprite][HEIGHT], SSD1306_WHITE);
}

void representar_obj() {
  for (int i = 0; i < num_obj; ++i) {
    int id = obj[i][ID];
    if (id != -1) {
      if (info[id][ALT] != 0 && cont%info[id][ALT] == 0) { obj[i][SPRITEX] += obj[i][ALTX]; obj[i][ALTX] = alternar(obj[i][ALTX]); }
      oled.drawBitmap(obj[i][XPOS], obj[i][YPOS], sprite[obj[i][SPRITEX]], info[id][WIDTH], info[id][HEIGHT], SSD1306_WHITE);
    }
  }
}

int raltura(int aux) {
  int x = random(info[aux][MAX], info[aux][MIN]+1)*8 - info[aux][HEIGHT];
  return x; 
}

int buscar_pos_lliure(int i0) {
  for (int i = i0; i < num_obj; i = i+2) {
    if(obj[i][ID] == -1) return i; 
  }
  return -1; 
}

void afegir_obj(int i, int id, int xpos, int ypos, int spritex) {
  obj[i][ID] = id; 
  obj[i][XPOS] = xpos; 
  obj[i][YPOS] = ypos; 
  obj[i][SPRITEX] = spritex; 
  if (info[id][ALT] != 0) obj[i][ALTX] = 1; 
  else obj[i][ALTX] = 0; 
}

int vel = 13;
const int vel_up = 13; 
const int vel_down = 0; 
const int g = 9.81/4;

int parabola(int y0, int v0, int count, int cont0) {
  int y = y0 - v0*(count-cont0) + 1/double(2)*g*pow(count-cont0,2); 
  return y; 
}

void configurar_dino(int dx, int dy, int did, int dsprite, int dalt) {
  dino[DX] = dx; 
  dino[DY] = dy;   
  dino[DID] = did; 
  dino[DSPRITE] = dsprite; 
  dino[DALT] = dalt; 
}

int HI = 00000; 

void drawHighScore() {
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(oled.width() - 6*(5+1+5+1+2),0);
  oled.println("HI ");
  imprimir_num(oled.width() - 6*(5+1),0,HI,1);
}

const int PRE = 2;  //PRESCALER

int contar_digits(int num) {
  int i = 0; 
  while(num != 0 && i < 5) {
    num = num/10; 
    ++i; 
  }
  return i; 
}

void imprimir_num(int xf, int y, int num, int size) {
  int dig = contar_digits(num);
  oled.setTextSize(size);
  oled.setTextColor(WHITE);
  oled.setCursor(xf - 6*dig,y);
  if(num != 0) oled.print(num,DEC);
  for(int j = dig+1; j <= 5; ++j) { 
    oled.setCursor(xf - 6*j,y);
    oled.print(0,DEC);
  }
}

int point; 
int cntdown = 0;  

int checkpoint(int num) {
  cntdown = 7; 
  num = num/100;  // reduim el numero
  point = num*100; 
  return point; 
}

bool on = true; 

int calcular_score() {
  return cont/PRE;
}

//falta canviar a que surti el score real si mor el dino
void drawScore() {
  int score = calcular_score();
  if(score != 0 && score%100 == 0)    //arribes a un multiple de 100 
    score = checkpoint(score);  
  else if(cntdown != 0) {             //encara estas a un multiple de 100
    score = point;                                          
    if(cont%5 == 0) { 
      --cntdown; 
      if(cntdown != 0) on = !on;
    }  
  } else { on = true; }   //no estas a checkpoint
  if(on) imprimir_num(oled.width(),0,score,1);
  drawHighScore(); 
}

int coordenada_y0(int ordenada, int sprite) {
  int did = dino[DID];
  int h = info_dino[did][sprite][HEIGHT];
  return ordenada - h;
}

void generar_objectes(int i0, int numA, int numB) {
  int id = random(numA, numA + numB);
  int i = buscar_pos_lliure(i0);
  if(i != -1) afegir_obj(i,id,oled.width(),raltura(id),info[id][SPRITE]);
}

int jump(int cont0) {
  cont0 = cont-1; 
  vel = vel_up;
  configurar_dino(x0,dino[DY],dino[DID],UP,0);
  dino[DY] = parabola(y0, vel, cont, cont0);
  return cont0; 
}

void crouch() {
  int did = dino[DID];
  y0 = coordenada_y0(oled.height(),DOWN);
  configurar_dino(x0,y0,did,DOWN,0);   //aqui tmb podria alternar
}

void stand() {
  int did =  dino[DID];
  y0 = coordenada_y0(oled.height(),LEFT);
  configurar_dino(x0,y0,did,LEFT,1);      //aqui podria fer una configuracio per defecte sense parametres
}

bool continguts(int x0a, int x0b, int x1a, int x1b) {
  return (x0a < x0b && x0b < x1a) || (x0b < x0a && x0a < x1b);
}

bool check_colision(int rectA[5], int rectB[5]) {
  int x0a = rectA[DX]; 
  int x1a = x0a + info_dino[rectA[DID]][rectA[DSPRITE]][WIDTH]; 
  int y0a = rectA[DY];
  int y1a = y0a + info_dino[rectA[DID]][rectA[DSPRITE]][HEIGHT];

  int x0b = rectB[XPOS]; 
  int x1b = x0b + info[rectB[ID]][WIDTH]; 
  int y0b = rectB[YPOS]; 
  int y1b = y0b + info[rectB[ID]][HEIGHT];   

  bool horitz, vertic; 
  horitz = continguts(x0a,x0b,x1a,x1b);
  if(horitz) vertic = continguts(y0a,y0b,y1a,y1b);
  return horitz && vertic; 
}

void writetxt(int x, int y, char* text, int size) {
  oled.setTextSize(size);
  oled.setTextColor(WHITE);
  oled.setCursor(x,y);
  oled.println(text);
}

void drawEndGame() {
  writetxt((oled.width()-6*8)/2, oled.height()/2-8*2,"GAME OVER",1);
  oled.drawBitmap((oled.width()-REPLAY_W)/2, (oled.height()-REPLAY_H)/2+8*1, replay, REPLAY_W, REPLAY_H, WHITE);
}

void actualitzar_HI() {
  int score = calcular_score(); 
  if(score > HI) HI = score; 
}

void dead() {
  int did =  dino[DID];
  int aux = dino[DY] + info_dino[did][dino[DSPRITE]][HEIGHT];
  y0 = coordenada_y0(aux,DEAD);
  configurar_dino(x0,y0,did,DEAD,0); 
  alive = false;  
}

void desfer_canvis(int k) {
  for(int i = k; i >= 0; --i) {
    if(obj[i][ID] != -1) {
      obj[i][XPOS] += info[ obj[i][ID] ][DELTAX];
    }
  }
}

void finalitzar() {
  oled.clearDisplay();
  drawEndGame(); 
  dead(); 
  representar_obj(); 
  horitzontal(oled.height()-1);
  drawSteve();  
  on = true; cntdown = 0; 
  actualitzar_HI(); 
  drawScore(); 
  oled.display(); 
}

void tractament_colisions(int object[5], int k) {
  int sprite = object[SPRITE];
  if(sprite >= OBJ_TRANS) {
    desfer_canvis(k); 
    finalitzar(); 
  }
}

void coordinar_objectes() {
  for(int i = 0; i < num_obj; ++i) {
    if(obj[i][ID] != -1) {
      obj[i][XPOS] -= info[ obj[i][ID] ][DELTAX];   //alguns avançaran mes rapid //aqui falta un factor multiplicador
      // Comprovem les colisions
      if(check_colision(dino,obj[i])) tractament_colisions(obj[i],i); 
      // If snowflake is off the bottom of the screen...
      if (obj[i][XPOS] <= -info[ obj[i][ID] ][WIDTH]) {
        afegir_obj(i,-1,-1,-1,-1);
      }
    }
  }
}

void desactivar_objectes(int i0) {
  for (int i = i0; i < num_obj; i = i+2) {
    if(obj[i][ID] != -1) 
      afegir_obj(i,-1,-1,-1,-1); 
  }
} 

void config_ini() {
  detectA = false; 
  detectB = false; 
  air = false; 
  cont = 0; 
  point = 0; 
  cntdown = 0; 
  on = true; 
  alive = true; 
  desactivar_objectes(0);
}

int generar_obj(int next, int dmin, int dmax, int i0, int numA, int numB) {
  --next;
  if (next == 0) {
    next = random(dmin, dmax);
    generar_objectes(i0,numA,numB); 
  }
  return next;
}

const int dminObst = 16;
const int dmaxObst = 32;
const int dminTrans = 32; 
const int dmaxTrans = 64;  

void testanimate(int did) {
  int nextTrans = random(dminTrans, dmaxTrans);
  while(1) {
    config_ini(); 
    configurar_dino(x0,y0,did,0,1); 
    y0 = coordenada_y0(oled.height(),LEFT);
    int y = y0; 
    int cont0 = cont;  
    //inicialitzem el primer objecte
    int aux = 1;    //cactus; 
    afegir_obj(0,aux,oled.width(),raltura(aux),info[aux][SPRITE]);
    int nextObst = random(dminObst, dmaxObst);
    while(alive) {
      oled.clearDisplay(); 
      representar_obj(); 
      horitzontal(oled.height()-1);
      air = dino[DY] < y0; 
      if(air) {
        dino[DY] = parabola(y0, vel, cont, cont0);
        if(dino[DY] >= y0) configurar_dino(x0,y0,dino[DID],LEFT,1);
      } else if (dino[DSPRITE] == UP) configurar_dino(x0,y0,dino[DID],LEFT,1); //ojo amb aquesta y0

      if(digitalRead(BOTO_B) == HIGH || detectB) {
        detectB = false; 
        if(air) vel = vel_down;  //descendre 
        else crouch(); 
      } else if(dino[DSPRITE] == DOWN) stand();
      else if (!air && (digitalRead(BOTO_A) == HIGH || detectA)) { 
        detectA = false; 
        cont0 = jump(cont0); 
      }

      drawSteve();
      drawScore(); 
      oled.display(); // Show the display buffer on the screen
      //delay(200);        // Pause for 1/10 second
      //Then update coordinates of each flake...
      coordinar_objectes(); 
      nextObst = generar_obj(nextObst,dminObst,dmaxObst,0,OBJ_TRANS,OBJ_OBST);
      nextTrans = generar_obj(nextTrans,dminTrans,dmaxTrans,1,0,OBJ_TRANS);
      //augmentem el contador intern
      ++cont; 
    }
    delay(500);
    while(digitalRead(BOTO_A) != HIGH);
  } 

}

void drawGuio(int x) {
  x = 3 + x*2;
  int rect_w = 11;
  int a0 = (oled.width()-6*rect_w)/2; 
  int b0 = 8*x;
  int b1 = b0+6;
  int b2 = (b0+b1)/2;
  oled.fillTriangle(a0,b0,a0,b1,a0+6,b2,WHITE);
}

void drawTittleScreen() {
  oled.clearDisplay();
  writetxt(0,0,"   STEVE",2);
  writetxt((oled.width()-6*4)/2,8*3,"PLAY",1);
  int rect_w = 12;
  oled.drawRect((oled.width()-6*rect_w)/2, 8*3-4, rect_w*6, 14, WHITE);
  writetxt((oled.width()-6*7)/2,8*5,"MY DINO",1);
  oled.drawRect((oled.width()-6*rect_w)/2, 8*5-4, rect_w*6, 14, WHITE);
  drawSteve();
  horitzontal(oled.height()-1);
  representar_obj(); 
}

void afegir_nuvols() {
  int nuvol = 0; 
  int a = -5; 
  afegir_obj(1,nuvol,a,raltura(nuvol),info[nuvol][SPRITE]);
  int b = a + random(dminTrans, dmaxTrans);
  afegir_obj(3,nuvol,b,raltura(nuvol),info[nuvol][SPRITE]);
  int c = b + random(dminTrans, dmaxTrans); 
  afegir_obj(5,nuvol,c,raltura(nuvol),info[nuvol][SPRITE]);
  int d = c + random(dminTrans, dmaxTrans); 
  afegir_obj(7,nuvol,d,raltura(nuvol),info[nuvol][SPRITE]);
}

#define PUJADA  1
#define BAIXADA 0 
bool detectEdge(int antRead, int actRead, int edge) {
  if(edge == PUJADA) {
    if (antRead == LOW && actRead == HIGH) return true; 
    else return false; 
  } else {  //edge == BAIXADA
    if (antRead == HIGH && actRead == LOW) return true; 
    else return false; 
  }
}

int nextTrans = random(dminTrans, dmaxTrans);

int pantalla_inicial(int did) {
  cont = 0; 
  configurar_dino(0,oled.height()-info_dino[did][LEFT][HEIGHT],did,LEFT,0); 
  drawTittleScreen(); 
  int i = 0; 
  drawGuio(i); 
  oled.display();
  int antReadA = digitalRead(BOTO_A);
  int actReadA = digitalRead(BOTO_A);  
  int antReadB = LOW; 
  while(not detectEdge(antReadA, actReadA,PUJADA)) {
    antReadA = actReadA; 

    int actReadB = digitalRead(BOTO_B);  
    if(detectEdge(antReadB, actReadB,PUJADA)) {
      ++i; 
      if(i >= 2) i = 0; 
    }
    //delay(50);   //estrategia anti rebots
    drawTittleScreen();
    drawGuio(i);
    oled.display();
    coordinar_objectes(); 
    nextTrans = generar_obj(nextTrans,dminTrans,dmaxTrans,1,0,OBJ_TRANS);
    antReadB = actReadB; 

    actReadA = digitalRead(BOTO_A);
  }
  return i; 
}

void draw_arrow(int x, int y, int tipe) {
  oled.drawBitmap(x,y,arrow[tipe],ARROW_W, ARROW_H, WHITE);
} 

char* name(int did) {
  if(did == 0) return "STEVE";
  else if (did == 1) return "BIG STEVE";
}

int strlen(char* txt) {
  int i = 0; 
  while(txt[i] != '\0') ++i;
  return i; 
}

int chose_character(int did) {
  int y0a = 0 - info_dino[did][UP][HEIGHT];
  configurar_dino(0,y0a,did,UP,0); 
  y0 = coordenada_y0(oled.height(),LEFT);
  cont = 0;  // el cont no hauria de ser global
  int cont0 = 0; 
  int antReadB = LOW;
  int antReadA = digitalRead(BOTO_A);
  int actReadA = digitalRead(BOTO_A);  
  int tipe = NPRESSED; 
  char* txt = name(did);
  int txt_len = strlen(txt);
  while(not detectEdge(antReadA, actReadA,PUJADA)) {
    antReadA = actReadA; 

    int actReadB = digitalRead(BOTO_B);  
    oled.clearDisplay(); 
    air = dino[DY] < y0; 
    if(air) {
      dino[DY] = parabola(0, 0, cont, cont0);
      if(dino[DY] >= y0) configurar_dino(x0,y0,dino[DID],LEFT,1);
    } else if (dino[DSPRITE] == UP) configurar_dino(x0,y0,dino[DID],LEFT,1); //ojo amb aquesta y0
    horitzontal(oled.height()-1);
    representar_obj(); 
    drawSteve();
    writetxt((oled.width() - 6*txt_len)/2,1*8,txt,1);
    draw_arrow(oled.width()-ARROW_W-6,(oled.height()-ARROW_H)/2,tipe);
    oled.display(); 
    if(digitalRead(BOTO_B) == HIGH) tipe = PRESSED; 
    if(detectEdge(antReadB, actReadB,BAIXADA)) {
      ++did; 
      did = did%num_skins; 
      y0a = 0 - info_dino[did][UP][HEIGHT];
      configurar_dino(0,y0a,did,UP,0); 
      cont0 = cont; 
      y0 = coordenada_y0(oled.height(),LEFT);
      tipe = NPRESSED; 
      txt = name(did);
      txt_len = strlen(txt);
    } 

    coordinar_objectes(); 
    nextTrans = generar_obj(nextTrans,dminTrans,dmaxTrans,1,0,OBJ_TRANS);
    antReadB = actReadB; 
    ++cont; 

    actReadA = digitalRead(BOTO_A);
  }
  return did; 
}

int didaux = 0; 

int AntReadA = LOW;
void loop() {
  int ActReadA = digitalRead(BOTO_A); 

  int mode = pantalla_inicial(didaux); 
  //if(detectEdge(antReadB, actReadB,BAIXADA))
  if (mode == 0) testanimate(didaux); // Animate bitmaps
  else if(mode == 1) didaux = chose_character(didaux); 

  AntReadA = ActReadA; 
}