/* Knihovny */

#include <Wire.h>
#include <AM2320.h>
#include <LiquidCrystal.h>

/* Nastaven� p�eddefinovan�ch hodnot */

#define setTemperature 38
#define setHumidity 80
#define setDays 23

/* Nastaven� displeje */

LiquidCrystal lcd(A0, A1, A2, 5, A3, 4);  // Piny displeje
#define lcdColumns 20  // Po�et sloupc�
#define lcdRows 4  // Po�et ��dk�
#define rowHeader 0  // Pozice hlavi�ky textu na displeji
#define rowOne 1  // Pozice prvn�ho ��dku na displeji
#define rowTwo 2  // Pozice druh�ho ��dku na displeji
#define rowThree 3  // Pozice t�et�ho ��dku na displeji
#define positionText 0  // Pozice za��tku textu ��dku
#define positionLineLeft 7  // Pozice lev� odd�lovac� ��ry
#define positionLineRight 13  // Pozice prav� odd�lovac� ��ry
#define positionActualValue 8  // Pozice nam��en� hodnoty
#define positionSetValue 15  // Pozice nastaven� hodnoty
#define positionUnit 12  // Pozice jednotky (celsia, procenta)

#pragma region Vlastn� znaky displeje
byte degreeDesign[8] = {
	B00000,
	B11000,
	B11011,
	B00100,
	B00100,
	B00100,
	B00100,
	B00011
};

byte lineDesign[8] = {
	B00100,
	B00100,
	B00100,
	B00100,
	B00100,
	B00100,
	B00100,
	B00100 
};
#pragma endregion

/* Nastaven� encoderu */

#define encoderPinA 9  // Pin encoderu A
#define encoderPinB 3  // Pin encoderu B
int encoderLastValue = LOW;  // Posledn� hodnota encoderu
int encoderCurrentValue = LOW;  // Aktu�ln� hodnota encoderu

/* Nastaven� hygrometru (teplota a vlkost) */

AM2320 hygrometer;

/* Nastaven� p�ed prvn�m spu�t�n�m */
void setup(){

	// Nastaven� hygrometru
	Wire.begin();  // Inicializace I2C pro AM2320
	pinMode(encoderPinA, INPUT);  // Nastaven� pinu encoderu jako vstupu
	pinMode(encoderPinB, INPUT);  // Nastaven� pinu encoderu jako vstupu

	// Nastaven� displeje
	lcd.begin(lcdColumns, lcdRows);  // Nastaven� rozli�en� displeje
	lcd.createChar(0, degreeDesign);  // Vytvo�en� nov�ho znaku displeje
	lcd.write(byte(0));  // P�id�n� nov�ho znaku do pam�ti displeje
	lcd.createChar(1, lineDesign);  // Vytvo�en� nov�ho znaku displeje
	lcd.write(byte(0));  // P�id�n� nov�ho znaku do pam�ti displeje
}

/* Hlavn� program */
void loop(){
	showMenu();

}


/* Vykreslen� menu */
void showMenu() {
	printDividingLines();  // Odd�lovac� ��ry
	printHeaderText();  // Hlavi�ka
	printTemperature();  // Teplota
	printHumidity();  // Vlhkost
	printDay();  // Dny
}

/* Vykresl� odd�lovac� ��ry */
void printDividingLines() {

	// Lev� ��ra
	lcdPrint(positionLineLeft, 0, (char)1);
	lcdPrint(positionLineLeft, 1, (char)1);
	lcdPrint(positionLineLeft, 2, (char)1);
	lcdPrint(positionLineLeft, 3, (char)1);

	// Prav� ��ra
	lcdPrint(positionLineRight, 0, (char)1);
	lcdPrint(positionLineRight, 1, (char)1);
	lcdPrint(positionLineRight, 2, (char)1);
	lcdPrint(positionLineRight, 3, (char)1);
}

/* Vykresl� hlavi�ku (ACT, SET) */
void printHeaderText() {
	lcdPrint(positionActualValue + 1, rowHeader, "ACT");
	lcdPrint(positionSetValue, rowHeader, "SET");
}

/* Vykresl� teplotu */
void printTemperature() {
	lcdPrint(positionText, rowOne, "TEMP");  // N�zev ��dku
	lcdPrint(positionActualValue, rowOne, readTemperature());  // Nam��en� hodnota
	lcdPrint(positionUnit, rowOne, (char)0);  // Znak stupn� celsia
	lcdPrint(positionSetValue, rowOne, setTemperature);  // P�ednastaven� hodnota
}

/* Vykresl� vlhkost */
void printHumidity() {
	lcdPrint(positionText, rowTwo, "HUMI");  // N�zev ��dku
	lcdPrint(positionActualValue, rowTwo, readHumidity());  // Nam��en� hodnota
	lcdPrint(positionUnit, rowTwo, "%");  // Znak procent
	lcdPrint(positionSetValue, rowTwo, setHumidity);  // P�ednastaven� hodnota
}

/* Vykresl� �as */
void printDay() {
	lcdPrint(positionText, rowThree, "DAY");  // N�zev ��dku
	lcdPrint(positionActualValue, rowThree, getDaysFromStart());  // Po�etn� dn� od zapnut�
	lcdPrint(positionSetValue, rowThree, setDays);  // Maxim�ln� po�et dn�
}

/* P�e�te hodnotu z encoderu a vrac� hodnotu posunu */
/* 1 = Zv��en�, -1 = Zmen�en�, 0 = Bez zm�ny */
int readEncoder() {
	encoderCurrentValue = digitalRead(encoderPinA);
	if ((encoderLastValue == LOW) && (encoderCurrentValue == HIGH)) {
		if (digitalRead(encoderPinB) == LOW) return -1;
		else return 1;
	}
	encoderLastValue = encoderCurrentValue;
	return 0;
}

/* P�e�te a vr�t� hodnotu teplom��u */
int readTemperature() {
	return hygrometer.getTemperature();
}

/* P�e�te a vr�t� hodnotu vlhkom�ru */
int readHumidity() {
	return hygrometer.getHumidity();
}

/* Vr�t� po�et dn� od startu programu */
int getDaysFromStart() {
	return (millis() / 1000 * 43200) / 84400;
}

/* Vyps�n� textu na displej s ur�itou pozic� */
void lcdPrint(int column, int row, String text) {
	lcd.setCursor(column, row);
	lcd.print(text);
}
void lcdPrint(int column, int row, int value) {
	lcdPrint(column, row, String(value));
}
