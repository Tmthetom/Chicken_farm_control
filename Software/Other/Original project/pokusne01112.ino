#include <Wire.h>                                     //přidání knihoven pro komunikaci s čidlem a arduinem
#include <AM2320.h>                                   //přidání knihoven pro komunikaci s čidlem a arduinem
#include <LiquidCrystal.h>                            //přidání knihoven pro komunikaci s čidlem a arduinem
AM2320 th;                                            //
LiquidCrystal lcd(A0,A1,A2,5,A3,4);           //
int topeni=13;                                        //nadefinování jednotek 
int vetrak=10;                                        //nadefinování jednotek 
int teplota=0;                                        //nadefinování jednotek 
int vlhkost=0;                                        //nadefinování jednotek 
unsigned long cas;                                    //
int settemp=38;
int sethumi=80;
int setdays=23;
int tlacitko=2;
int tl=0;
bool stav=0;
int menu = 0;
int val;
int encoder0PinB = 3;
int encoder0PinA = 9;
int encoder0Pos = 0;
int encoder0PinALast = LOW;
int n = LOW;

//................................................................................................................
byte stupen[8] = { 
B00000,
B11000,
B11011, 
B00100, 
B00100, 
B00100, 
B00100, 
B00011
};
byte cara[8] = { 
  B00100, 
  B00100, 
  B00100, 
  B00100, 
  B00100, 
  B00100, 
  B00100, 
  B00100 }; 
//....................................................//nastavovací smyčka
void setup() {                                        //
 pinMode(topeni, OUTPUT);                             //
 pinMode(vetrak, OUTPUT);                             //
 pinMode(tlacitko, INPUT_PULLUP);
 Wire.begin();                                        //
  Serial.begin(9600);                                 //          
  lcd.begin(20, 4);                                   //nadefinování psaní na určitou pozici na řádku
 // teplota=28;                                         //hodnota pro teplotu která se nesmí překročit 
 // vlhkost=50;                                         //hodnota pro vlhkost která se nesmí překročit
  lcd.createChar(0, stupen); 
  lcd.write(byte(0));                                                 //                                                                               
  lcd.createChar(1, cara); 
  lcd.write(byte(0));   
  pinMode (encoder0PinA, INPUT);
  pinMode (encoder0PinB, INPUT);                                              
 
attachInterrupt(digitalPinToInterrupt(encoder0PinB), blink, RISING);
attachInterrupt(digitalPinToInterrupt(tlacitko), protlacitko, CHANGE);
}                                                     //
//....................................................//
void loop() { 
  
Serial.print ("menu");
Serial.println (menu);
Serial.print ("stav");
Serial.println (stav);
Serial.print ("tlacitko");
Serial.println (tl);
delay(200);
  th.measure();    //
  delay(50);                                          //zpoždění na "nastartování" senzoru
  int day=cas/84400;                                  //definování jednotky day
//....................................................//sekce pro měření a sekce pro promítnutí na display základních veličin (teplota, vlhkost)
lcd.setCursor(15, 1);                            //nastavení kurzoru pro napsání "humi."
      lcd.print(settemp);                           //promítnutí nápisu "humi" na display 
lcd.setCursor(15, 2);                            //nastavení kurzoru pro napsání "humi."
      lcd.print(sethumi); 
lcd.setCursor(15, 3);                            //nastavení kurzoru pro napsání "humi."
      lcd.print(setdays); 
//..............................................................................
      cas = (millis() /1000*43200);                         //čas v rámci funkce milis vydělit 1000 abych měl sekundy86400
      Serial.println(cas);                            //
      lcd.setCursor(0, 1);                            //nastavení kurzoru pro napsání "temp."
      lcd.print("TEMP");                             //promítnutí nápisu "temp" na display
      lcd.setCursor(8,1);                             //nastavení kurzoru na promítnutí teploty
      lcd.print(th.getTemperature());                               //promítnutí teploty na display

      lcd.setCursor(13, 0);                           //nastavení kurzoru pro promítnutí znaku stupně 
      lcd.print((char)1);    
      lcd.setCursor(13, 1);                           //nastavení kurzoru pro promítnutí znaku stupně 
      lcd.print((char)1);
	  lcd.setCursor(13, 2);                           //nastavení kurzoru pro promítnutí znaku stupně 
      lcd.print((char)1);
      lcd.setCursor(13, 3);                           //nastavení kurzoru pro promítnutí znaku stupně 
      lcd.print((char)1);

	  lcd.setCursor(7, 0);                           //nastavení kurzoru pro promítnutí znaku stupně 
      lcd.print((char)1);
	  lcd.setCursor(7, 1);                           //nastavení kurzoru pro promítnutí znaku stupně 
      lcd.print((char)1);
	  lcd.setCursor(7, 2);                           //nastavení kurzoru pro promítnutí znaku stupně 
      lcd.print((char)1);
	  lcd.setCursor(7, 3);                           //nastavení kurzoru pro promítnutí znaku stupně 
      lcd.print((char)1);


lcd.setCursor(9, 0);                            //nastavení kurzoru pro napsání "humi."
      lcd.print("ACT"); 
      lcd.setCursor(12, 1);                           //nastavení kurzoru pro promítnutí znaku stupně 
      lcd.print((char)0);                             //promítnutí stupně na display                     
      lcd.setCursor(0, 2);                            //nastavení kurzoru pro napsání "humi."
      lcd.print("HUMI");                           //promítnutí nápisu "humi" na display 
      lcd.setCursor(8, 2);                            //nastavení kurzoru pro promítnutí vlhkosti 
      lcd.print(th.getHumidity());                             //promítnutí vlhkosti na display 
      lcd.setCursor(12, 2);                           //nastavení kurzoru pro promítnutí znaku "%"
      lcd.print("%");                                 //promítnutí znaku "%" ma display
      lcd.setCursor(0, 3);                            //nastavení kurzoru pro napsání "humi."
      lcd.print("DAY");                             //promítnutí nápisu "humi" na display     
     lcd.setCursor(15, 0);                            
      lcd.print("SET"); 
//....................................................//vypisování dnů na display
if (day<10)                                           //pokud je den menší jak 10
 {    lcd.setCursor(9,3 );                           //začni to číslo psát na pozici 15,1 
      lcd.print(day);                                 //promítni to na display
    }
else                                                  //nebo 
  { 
 lcd.setCursor(8,3);                                //začni vypisovat číslo na pozici 14,1
      lcd.print(day);                                 //promítni to na display
      }  
      Serial.println(day);                            //promítni day do vnitřku programu                                                                              
//..............................................................................................................//smyčka 1.    
    if( day <= setdays){                             //když je čas menší jak 1814400   
//..............................................................................................................//smyčka 2.  
      if(th.getTemperature() < settemp)                              //když je moc chladno
      {
        digitalWrite(topeni, HIGH);                   //topení pustit
         Serial.println("topi");                      //napsat do vnitřního programu "topí"
      }
      else                                            //nebo
      {
        digitalWrite(topeni, LOW);                    //topení vypnout
         Serial.println("netopi");                    //napsat do vnitřního programu "netopí"
      }
             if(th.getHumidity() < sethumi)                       //když je vlhkost menší
      {
        digitalWrite(vetrak, LOW);                    //vypni větrák
         Serial.println("nevetra");                   //promítni do vnitřního programu "nevětrá" 
      }
      else                                            //nebo
      {
        digitalWrite(vetrak, HIGH);                   //zapni větrák
         Serial.println("vetra");                     //napsat do vnitřního programu "větrá"
      
      }
      
 


       
//................................................................................................................//smyčka 1.
      }
      else                                            //nebo
     {
      digitalWrite(topeni, LOW);                      //topení vypnuto 
      digitalWrite(vetrak, LOW);                      //větrání vypnuto
      Serial.println("vypnuto");                      //vypisování vypnuto
      }
      delay(1000);                                   //doba čekání
//.......................................................................................................
switch (menu){
case 1:
lcd.setCursor(15, 1);                            //nastavení kurzoru pro napsání "humi."
      lcd.print ("  ");
   //promítnutí nápisu "humi" na display 
break; //po provedení této části konstrukce switch končí
case 2:
lcd.setCursor(15, 2);                            //nastavení kurzoru pro napsání "humi."
      lcd.print ("  "); 
           //promítnutí nápisu "humi" na display 
break;
case 3:
lcd.setCursor(15, 3);                            //nastavení kurzoru pro napsání "humi."
      lcd.print ("  ");  
                        //promítnutí nápisu "humi" na display 
      break;
case 4:
      break; 
}
 
 
 

}


void blink() {
	n = digitalRead(encoder0PinA);
	if ((encoder0PinALast == LOW) && (n == HIGH)) {

		// Snížení hodnoty
		if (digitalRead(encoder0PinB) == LOW) {
			if (menu == 1){
				settemp--;
			}
			else if(menu == 2){
				sethumi--;
			}
			else if(menu == 3){
				setdays--;
			}
		} 

		// Zvýšení hodnoty
		else {
			if (menu == 1){
				settemp++;
			}
			else if(menu == 2){
				sethumi++;
			}
			else if(menu == 3){
				setdays++;
			}
		}
	}
	encoder0PinALast = n;
}

void protlacitko() {
	tl = digitalRead(tlacitko);
	while ( tl == 0 && stav == 0 ){
		++menu;
		stav=1;
	}
	while( tl == 1 && stav == 1 ){
		stav = 0;
	}
	while (menu >=5){
		menu = 1;
	}
}  
