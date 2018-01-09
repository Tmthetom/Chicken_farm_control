/* Knihovny */

#include <Wire.h>
#include <AM2320.h>
#include <LiquidCrystal.h>

/* Nastavení pøeddefinovaných hodnot */

#define setTemperature 38
#define setHumidity 80
#define setDays 23

/* Nastavení periferií */

#define button 2
#define heater 13
#define fan 10

/* Nastavení displeje */

LiquidCrystal lcd(A0, A1, A2, 5, A3, 4);  // Piny displeje
#define lcdColumns 20  // Poèet sloupcù
#define lcdRows 4  // Poèet øádkù
#define rowHeader 0  // Pozice hlavièky textu na displeji
#define rowOne 1  // Pozice prvního øádku na displeji
#define rowTwo 2  // Pozice druhého øádku na displeji
#define rowThree 3  // Pozice tøetího øádku na displeji
#define positionText 0  // Pozice zaèátku textu øádku
#define positionLineLeft 7  // Pozice levé oddìlovací èáry
#define positionLineRight 13  // Pozice pravé oddìlovací èáry
#define positionActualValue 8  // Pozice namìøené hodnoty
#define positionSetValue 15  // Pozice nastavené hodnoty
#define positionUnit 12  // Pozice jednotky (celsia, procenta)

#pragma region Vlastní znaky displeje
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

/* Nastavení encoderu */

#define encoderPinA 9  // Pin encoderu A
#define encoderPinB 3  // Pin encoderu B
int encoderLastValue = LOW;  // Poslední hodnota encoderu
int encoderCurrentValue = LOW;  // Aktuální hodnota encoderu

/* Nastavení hygrometru (teplota a vlkost) */

AM2320 hygrometer;

/* Nastavení pøed prvním spuštìním */
void setup(){

	// Nastavení periferií
	pinMode(heater, OUTPUT);
	pinMode(fan, OUTPUT);
	pinMode(button, INPUT_PULLUP);

	// Nastavení hygrometru
	Wire.begin();  // Inicializace I2C pro AM2320
	pinMode(encoderPinA, INPUT);  // Nastavení pinu encoderu jako vstupu
	pinMode(encoderPinB, INPUT);  // Nastavení pinu encoderu jako vstupu

	// Nastavení displeje
	lcd.begin(lcdColumns, lcdRows);  // Nastavení rozlišení displeje
	lcd.createChar(0, degreeDesign);  // Vytvoøení nového znaku displeje
	lcd.write(byte(0));  // Pøidání nového znaku do pamìti displeje
	lcd.createChar(1, lineDesign);  // Vytvoøení nového znaku displeje
	lcd.write(byte(0));  // Pøidání nového znaku do pamìti displeje
}

/* Hlavní program */
void loop(){
	//handleButton();
	showMenu();
	control();
}

#pragma region Handle Button

/* Pøeète hodnotu z encoderu a vrací hodnotu posunu */
/* 1 = Zvýšení, -1 = Zmenšení, 0 = Bez zmìny */
int readEncoder() {
	encoderCurrentValue = digitalRead(encoderPinA);
	if ((encoderLastValue == LOW) && (encoderCurrentValue == HIGH)) {
		if (digitalRead(encoderPinB) == LOW) return -1;
		else return 1;
	}
	encoderLastValue = encoderCurrentValue;
	return 0;
}

#pragma endregion

#pragma region Show Menu

/* Vykreslení menu */
void showMenu() {
	printDividingLines();  // Oddìlovací èáry
	printHeaderText();  // Hlavièka
	printTemperature();  // Teplota
	printHumidity();  // Vlhkost
	printDay();  // Dny
}

/* Vykreslí oddìlovací èáry */
void printDividingLines() {

	// Levá èára
	lcdPrint(positionLineLeft, 0, (char)1);
	lcdPrint(positionLineLeft, 1, (char)1);
	lcdPrint(positionLineLeft, 2, (char)1);
	lcdPrint(positionLineLeft, 3, (char)1);

	// Pravá èára
	lcdPrint(positionLineRight, 0, (char)1);
	lcdPrint(positionLineRight, 1, (char)1);
	lcdPrint(positionLineRight, 2, (char)1);
	lcdPrint(positionLineRight, 3, (char)1);
}

/* Vykreslí hlavièku (ACT, SET) */
void printHeaderText() {
	lcdPrint(positionActualValue + 1, rowHeader, "ACT");
	lcdPrint(positionSetValue, rowHeader, "SET");
}

/* Vykreslí teplotu */
void printTemperature() {
	lcdPrint(positionText, rowOne, "TEMP");  // Název øádku
	lcdPrint(positionActualValue, rowOne, readTemperature());  // Namìøená hodnota
	lcdPrint(positionUnit, rowOne, (char)0);  // Znak stupnì celsia
	lcdPrint(positionSetValue, rowOne, setTemperature);  // Pøednastavená hodnota
}

/* Vykreslí vlhkost */
void printHumidity() {
	lcdPrint(positionText, rowTwo, "HUMI");  // Název øádku
	lcdPrint(positionActualValue, rowTwo, readHumidity());  // Namìøená hodnota
	lcdPrint(positionUnit, rowTwo, "%");  // Znak procent
	lcdPrint(positionSetValue, rowTwo, setHumidity);  // Pøednastavená hodnota
}

/* Vykreslí èas */
void printDay() {

	// Název øádku
	lcdPrint(positionText, rowThree, "DAY");
	
	// Poèetní dní od zapnutí
	if (setDays < 10) lcdPrint(positionActualValue + 1, rowThree, getDaysFromStart());  // Jednotky
	else lcdPrint(positionActualValue, rowThree, getDaysFromStart());  // Desítky

	// Nastavený poèet dní
	if (setDays < 10) lcdPrint(positionSetValue + 1, rowThree, setDays);  // Jednotky
	else lcdPrint(positionSetValue, rowThree, setDays);  // Desítky
}

/* Pøeète a vrátí hodnotu teplomìøu */
int readTemperature() {
	return hygrometer.getTemperature();
}

/* Pøeète a vrátí hodnotu vlhkomìru */
int readHumidity() {
	return hygrometer.getHumidity();
}

/* Vrátí poèet dní od startu programu */
int getDaysFromStart() {
	return (millis() / 1000 * 43200) / 84400;
}

/* Vypsání textu na displej s urèitou pozicí */
void lcdPrint(int column, int row, String text) {
	lcd.setCursor(column, row);
	lcd.print(text);
}
void lcdPrint(int column, int row, int value) {
	lcdPrint(column, row, String(value));
}

#pragma endregion

#pragma region Control

/* Ovládání periferií na základì mìøení */
void control() {

	// Reguluje
	if (getDaysFromStart() <= setDays){

		// Topení
		if (readTemperature() < setTemperature) digitalWrite(heater, HIGH);  // Topí
		else digitalWrite(heater, LOW);  // Netopí

		// Vìtrání
		if (readHumidity() < setHumidity) digitalWrite(fan, HIGH);  // Vìtrá
		else digitalWrite(fan, LOW);  // Nevìtrá
	}

	// Nereguluje (poèet dní pøesažen)
	else {
		digitalWrite(heater, LOW);  // Netopí
		digitalWrite(fan, LOW);  // Nevìtrá
	}
}

#pragma endregion
