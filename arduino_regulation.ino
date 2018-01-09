#pragma region Inicializace a nastaven�

/* Knihovny */

#include <AM2320.h>  // https://github.com/hibikiledo/AM2320
#include <LiquidCrystal.h>

/* Nastaven� p�eddefinovan�ch hodnot */

int setTemperature = 38;
int setHumidity = 80;
int setDays = 23;

/* Nastaven� periferi� */

#define button 2
#define heater 13
#define fan 10

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

/* Nastaven� blik�n� */

#define blinkInterval 200  // Blink in milliseconds

unsigned long previousMillis = 0;  // P�edchoz� po�et milisekund
unsigned long currentMillis;  // Aktu�ln� po�et milisekund

/* Nastaven� editace menu */

#define menuNotSelected -1  // ��dn� menu nevybr�no
#define menuTemperature 0  // ��slo menu
#define menuHumidity 1  // ��slo menu
#define menuDays 2  // ��slo menu

int editedMenu = menuNotSelected;  // Aktu�ln� nastavovan� menu
int lastButtonState = LOW;  // Posledn� stav tla��tka
int currentButtonState;  // Aktu�ln� stav tla��tka
char emptyValue[] = "  ";  // Pr�zdn� hodnota p�i blik�n�
bool isEditedMenuVisible = true;  // Je editovan� menu viditeln�?

#pragma endregion

#pragma region Hlavn� program
// TODO: Displej se nep�ekresluje, pouze v�ci p�episujeme. Bylo by dobr� statick� texty vykreslit pouze jednou.

/* Nastaven� p�ed prvn�m spu�t�n�m */
void setup(){

	// Nastaven� periferi�
	pinMode(heater, OUTPUT);
	pinMode(fan, OUTPUT);
	pinMode(button, INPUT_PULLUP);

	// Nastaven� hygrometru
	hygrometer.begin();  // Inicializace I2C pro AM2320
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
	measure();  // Zm�� aktu�ln� hodnoty
	checkForMenuEdit();  // Pohyb v edita�n�m menu
	showMenu();  // Vykresl� menu
	control();  // Ovl�d�n� periferi� na z�klad� m��en�
}

#pragma endregion

#pragma region Measure

/* Zm�� aktu�ln� hodnoty */
void measure() {

	// Pokud je menu editov�no, odejdi
	if (editedMenu != menuNotSelected) return;
	
	// M��en� aktu�ln�ch hodnot
	hygrometer.measure();
}

#pragma endregion

#pragma region Read Button

/* Pohyb v edita�n�m menu */
void checkForMenuEdit() {
	// TODO: O�et�en� z�kmitu tla��tka (Debounce)

	// Na�ten� aktu�ln�ho stavu tla��tka
	currentButtonState = digitalRead(button);

	// Tla��tko zm��knut�
	if (currentButtonState == LOW && lastButtonState == HIGH) {
		switch (editedMenu) {

		// Editace teploty
		case menuNotSelected:  // Aktu�ln� needitujeme ��dn� menu
			editedMenu = menuTemperature;  // Jdeme editovat teplotu
			break;

		// Editace vlhkosti
		case menuTemperature:  // Aktu�ln� editujeme teplotu
			editedMenu = menuHumidity;  // Jdeme editovat vlhkost
			break;

		// Editace dn�
		case menuHumidity:  // Aktu�ln� editujeme vlhkost
			editedMenu = menuDays;  // Jdeme editovat dny
			break;

		// Konec editace
		case menuDays:  // Aktu�ln� editujeme dny
			editedMenu = menuNotSelected;  // Kon��me s editac�
			isEditedMenuVisible = true;
			break;
		}
	}

	// Aktualizace star�ho stavu
	lastButtonState = currentButtonState;
}

#pragma endregion

#pragma region Show Menu

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

	// Vykreslen� za��tku ��dku
	lcdPrint(positionText, rowOne, "TEMP");  // N�zev ��dku
	lcdPrint(positionActualValue, rowOne, readTemperature());  // Nam��en� hodnota
	lcdPrint(positionUnit, rowOne, (char)0);  // Znak stupn� celsia

	// P�ednastaven� hodnota
	if (editedMenu != menuTemperature) {  // Pokud toto menu nen� zrovna editov�no
		lcdPrint(positionSetValue, rowOne, setTemperature);  // P�ednastaven� hodnota
		return;  // Konec vykreslen�
	}

	// Blik�n� p�enastavovan� hodnoty
	if (isBlinkTime()) {

		// Zhasnut� hodnoty
		if (isEditedMenuVisible) {
			lcdPrint(positionSetValue, rowOne, emptyValue);  // Pr�zdn� hodnota 
			isEditedMenuVisible = false;
		}

		// Uk�zan� hodnoty
		else {
			lcdPrint(positionSetValue, rowOne, setTemperature);  // P�ednastaven� hodnota
			isEditedMenuVisible = true;
		}
	}
	
	// Nastavov�n� p�ednastaven� hodnoty pomoc� potenciometru
	setTemperature += readEncoder();
}

/* Vykresl� vlhkost */
void printHumidity() {

	// Vykreslen� za��tku ��dku
	lcdPrint(positionText, rowTwo, "HUMI");  // N�zev ��dku
	lcdPrint(positionActualValue, rowTwo, readHumidity());  // Nam��en� hodnota
	lcdPrint(positionUnit, rowTwo, "%");  // Znak procent

	// P�ednastaven� hodnota
	if (editedMenu != menuHumidity) {  // Pokud toto menu nen� zrovna editov�no
		lcdPrint(positionSetValue, rowTwo, setHumidity);  // P�ednastaven� hodnota
		return;  // Konec vykreslen�
	}

	// Blik�n� p�enastavovan� hodnoty
	if (isBlinkTime()) {

		// Zhasnut� hodnoty
		if (isEditedMenuVisible) {
			lcdPrint(positionSetValue, rowTwo, emptyValue);  // Pr�zdn� hodnota 
			isEditedMenuVisible = false;
		}

		// Uk�zan� hodnoty
		else {
			lcdPrint(positionSetValue, rowTwo, setHumidity);  // P�ednastaven� hodnota
			isEditedMenuVisible = true;
		}
	}

	// Nastavov�n� p�ednastaven� hodnoty pomoc� potenciometru
	setHumidity += readEncoder();  // Zm�na vybran�ho menu
}

/* Vykresl� �as */
void printDay() {

	// N�zev ��dku
	lcdPrint(positionText, rowThree, "DAY");
	
	// Po�etn� dn� od zapnut�
	if (setDays < 10) lcdPrint(positionActualValue + 1, rowThree, getDaysFromStart());  // Jednotky
	else lcdPrint(positionActualValue, rowThree, getDaysFromStart());  // Des�tky

	// Nastaven� po�et dn�
	if (editedMenu != menuDays) {  // Pokud toto menu nen� zrovna editov�no
		if (setDays < 10) lcdPrint(positionSetValue + 1, rowThree, setDays);  // Jednotky
		else lcdPrint(positionSetValue, rowThree, setDays);  // Des�tky
		return;  // Konec vykreslen�
	}

	// Blik�n� p�enastavovan� hodnoty
	if (isBlinkTime()) {

		// Zhasnut� hodnoty
		if (isEditedMenuVisible) {
			lcdPrint(positionSetValue, rowThree, emptyValue);  // Pr�zdn� hodnota 
			isEditedMenuVisible = false;
		}

		// Uk�zan� hodnoty
		else {
			if (setDays < 10) lcdPrint(positionSetValue + 1, rowThree, setDays);  // Jednotky
			else lcdPrint(positionSetValue, rowThree, setDays);  // Des�tky
			isEditedMenuVisible = true;
		}
	}

	// Nastavov�n� p�ednastaven� hodnoty pomoc� potenciometru
	setDays += readEncoder();  // Zm�na vybran�ho menu
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

/* Je �as bliknout hodnotou v menu? */
bool isBlinkTime() {
	currentMillis = millis();

	if (currentMillis - previousMillis >= blinkInterval) {  // Nastal �as
		previousMillis = currentMillis;  // P�ep�eme �as posledn�ho blinkut�
		return true;  // Ozn�m�me �e je �as na bliknut�
	}

	return false;  // Ozn�m�me �e nen� �as na bliknut�
}

/* Vyps�n� textu na displej s ur�itou pozic� */
void lcdPrint(int column, int row, String text) {
	lcd.setCursor(column, row);
	lcd.print(text);
}
void lcdPrint(int column, int row, int value) {
	lcdPrint(column, row, String(value));
}
void lcdPrint(int column, int row, char value) {
	lcd.setCursor(column, row);
	lcd.print(value);
}

#pragma endregion

#pragma region Control

/* Ovl�d�n� periferi� na z�klad� m��en� */
void control() {

	// Reguluje
	if (getDaysFromStart() <= setDays){

		// Topen�
		if (readTemperature() < setTemperature) digitalWrite(heater, HIGH);  // Top�
		else digitalWrite(heater, LOW);  // Netop�

		// V�tr�n�
		if (readHumidity() < setHumidity) digitalWrite(fan, HIGH);  // V�tr�
		else digitalWrite(fan, LOW);  // Nev�tr�
	}

	// Nereguluje (po�et dn� p�esa�en)
	else {
		digitalWrite(heater, LOW);  // Netop�
		digitalWrite(fan, LOW);  // Nev�tr�
	}
}

#pragma endregion
