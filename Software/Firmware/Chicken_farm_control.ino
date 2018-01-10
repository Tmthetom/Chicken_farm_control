#pragma region Inicializace a nastavení

/* Knihovny */

#include <AM2320.h>  // https://github.com/hibikiledo/AM2320
#include <LiquidCrystal.h>

/* Nastavení pøeddefinovaných hodnot */

int setTemperature = 38;  // Ideální teplota
int setHumidity = 80;  // Ideální vlhkost
int setDays = 23;  // Maximální poèet dní, které mùže zaøízení bìžet

/* Nastavení pinù periferií */

#define button 2  // Tlaèítko pro ovládání menu
#define heater 13  // Topení pro ohøívání kurníku
#define fan 10  // Vìtrák pro ochlazování kurníku

/* Nastavení displeje */

LiquidCrystal lcd(A0, A1, A2, 5, A3, 4);  // Piny displeje
#define lcdColumns 20  // Poèet sloupcù
#define lcdRows 4  // Poèet øádkù

/* Nastavení menu displeje */

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

/* Nastavení encoderu pro nastavování hodnot v menu (digitálním potenciometrem) */

#define encoderPinA 9  // Pin encoderu A
#define encoderPinB 3  // Pin encoderu B

int encoderLastValue = LOW;  // Poslední hodnota encoderu
int encoderCurrentValue = LOW;  // Aktuální hodnota encoderu

/* Nastavení hygrometru (teplota a vlkost) */

AM2320 hygrometer;

/* Nastavení blikání právì upravovaných hodnot */

#define blinkInterval 200  // Interval bliknutí v milisekundách

unsigned long previousMillis = 0;  // Pøedchozí poèet milisekund
unsigned long currentMillis;  // Aktuální poèet milisekund

/* Nastavení editaèního menu */

#define menuNotSelected -1  // Žádné menu nevybráno
#define menuTemperature 0  // Èíslo menu teploty
#define menuHumidity 1  // Èíslo menu vlhkosti
#define menuDays 2  // Èíslo menu poètu dnù

int editedMenu = menuNotSelected;  // Aktuálnì nastavované menu
int lastButtonState = LOW;  // Poslední stav tlaèítka
int currentButtonState;  // Aktuální stav tlaèítka
char emptyValue[] = "  ";  // Prázdná hodnota pøi blikání
bool isEditedMenuVisible = true;  // Je editované menu viditelné?

#pragma endregion

#pragma region Hlavní program - Volání jednotlivých funkcí
// TODO: Displej se nepøekresluje, pouze vìci pøepisujeme. Bylo by dobré statické texty vykreslit pouze jednou.

/* Nastavení pøed prvním spuštìním */
void setup() {

	// Nastavení periferií
	pinMode(heater, OUTPUT);
	pinMode(fan, OUTPUT);
	pinMode(button, INPUT_PULLUP);

	// Nastavení hygrometru
	hygrometer.begin();  // Inicializace I2C pro AM2320
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
void loop() {
	measure();  // Zmìø aktuální hodnoty
	checkForMenuEdit();  // Pohyb v editaèním menu
	showMenu();  // Vykreslí menu
	control();  // Ovládání periferií na základì mìøení
}

#pragma endregion

#pragma region Mìøení - Teploty a vlhkosti

/* Zmìø aktuální hodnoty */
void measure() {

	// Pokud je menu editováno, odejdi
	if (editedMenu != menuNotSelected) return;

	// Mìøení aktuálních hodnot
	hygrometer.measure();  // Zmìøí všechny hodnoty, které se pak v kodu vyèítají
}

#pragma endregion

#pragma region Editace - Pohyb v editaèním menu pomocí tlaèítka

/* Pohyb v editaèním menu */
void checkForMenuEdit() {
	// TODO: Ošetøení zákmitu tlaèítka (Debounce)

	// Naètení aktuálního stavu tlaèítka
	currentButtonState = digitalRead(button);

	// Tlaèítko zmáèknuté
	if (currentButtonState == LOW && lastButtonState == HIGH) {
		switch (editedMenu) {

			// Editace teploty
		case menuNotSelected:  // Aktuálnì needitujeme žádné menu
			editedMenu = menuTemperature;  // Jdeme editovat teplotu
			break;

			// Editace vlhkosti
		case menuTemperature:  // Aktuálnì editujeme teplotu
			editedMenu = menuHumidity;  // Jdeme editovat vlhkost
			break;

			// Editace dnù
		case menuHumidity:  // Aktuálnì editujeme vlhkost
			editedMenu = menuDays;  // Jdeme editovat dny
			break;

			// Konec editace
		case menuDays:  // Aktuálnì editujeme dny
			editedMenu = menuNotSelected;  // Konèíme s editací
			isEditedMenuVisible = true;
			break;
		}
	}

	// Aktualizace starého stavu
	lastButtonState = currentButtonState;
}

#pragma endregion

#pragma region Menu - Vykreslení menu na displej

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

	// Vykreslení zaèátku øádku
	lcdPrint(positionText, rowOne, "TEMP");  // Název øádku
	lcdPrint(positionActualValue, rowOne, readTemperature());  // Namìøená hodnota
	lcdPrint(positionUnit, rowOne, (char)0);  // Znak stupnì celsia

											  // Pøednastavená hodnota
	if (editedMenu != menuTemperature) {  // Pokud toto menu není zrovna editováno
		lcdPrint(positionSetValue, rowOne, setTemperature);  // Pøednastavená hodnota
		return;  // Konec vykreslení
	}

	// Blikání pøenastavované hodnoty
	if (isBlinkTime()) {

		// Zhasnutí hodnoty
		if (isEditedMenuVisible) {
			lcdPrint(positionSetValue, rowOne, emptyValue);  // Prázdná hodnota 
			isEditedMenuVisible = false;
		}

		// Ukázaní hodnoty
		else {
			lcdPrint(positionSetValue, rowOne, setTemperature);  // Pøednastavená hodnota
			isEditedMenuVisible = true;
		}
	}

	// Nastavování pøednastavené hodnoty pomocí potenciometru
	setTemperature += readEncoder();
}

/* Vykreslí vlhkost */
void printHumidity() {

	// Vykreslení zaèátku øádku
	lcdPrint(positionText, rowTwo, "HUMI");  // Název øádku
	lcdPrint(positionActualValue, rowTwo, readHumidity());  // Namìøená hodnota
	lcdPrint(positionUnit, rowTwo, "%");  // Znak procent

										  // Pøednastavená hodnota
	if (editedMenu != menuHumidity) {  // Pokud toto menu není zrovna editováno
		lcdPrint(positionSetValue, rowTwo, setHumidity);  // Pøednastavená hodnota
		return;  // Konec vykreslení
	}

	// Blikání pøenastavované hodnoty
	if (isBlinkTime()) {

		// Zhasnutí hodnoty
		if (isEditedMenuVisible) {
			lcdPrint(positionSetValue, rowTwo, emptyValue);  // Prázdná hodnota 
			isEditedMenuVisible = false;
		}

		// Ukázaní hodnoty
		else {
			lcdPrint(positionSetValue, rowTwo, setHumidity);  // Pøednastavená hodnota
			isEditedMenuVisible = true;
		}
	}

	// Nastavování pøednastavené hodnoty pomocí potenciometru
	setHumidity += readEncoder();  // Zmìna vybraného menu
}

/* Vykreslí èas */
void printDay() {

	// Název øádku
	lcdPrint(positionText, rowThree, "DAYS");

	// Poèetní dní od zapnutí
	if (setDays < 10) lcdPrint(positionActualValue + 1, rowThree, getDaysFromStart());  // Jednotky
	else lcdPrint(positionActualValue, rowThree, getDaysFromStart());  // Desítky

																	   // Nastavený poèet dní
	if (editedMenu != menuDays) {  // Pokud toto menu není zrovna editováno
		if (setDays < 10) lcdPrint(positionSetValue + 1, rowThree, setDays);  // Jednotky
		else lcdPrint(positionSetValue, rowThree, setDays);  // Desítky
		return;  // Konec vykreslení
	}

	// Blikání pøenastavované hodnoty
	if (isBlinkTime()) {

		// Zhasnutí hodnoty
		if (isEditedMenuVisible) {
			lcdPrint(positionSetValue, rowThree, emptyValue);  // Prázdná hodnota 
			isEditedMenuVisible = false;
		}

		// Ukázaní hodnoty
		else {
			if (setDays < 10) lcdPrint(positionSetValue + 1, rowThree, setDays);  // Jednotky
			else lcdPrint(positionSetValue, rowThree, setDays);  // Desítky
			isEditedMenuVisible = true;
		}
	}

	// Nastavování pøednastavené hodnoty pomocí potenciometru
	setDays += readEncoder();  // Zmìna vybraného menu
}

/* Pøeète a vrátí hodnotu teplomìøu */
float readTemperature() {
	return hygrometer.getTemperature();
}

/* Pøeète a vrátí hodnotu vlhkomìru */
float readHumidity() {
	return hygrometer.getHumidity();
}

/* Vrátí poèet dní od startu programu */
int getDaysFromStart() {
	// TODO: Value is not actually days from start, but something like minutes, need to be changed...
	return (millis() / 1000 * 43200) / 84400;
}

/* Pøeète hodnotu z encoderu a vrací hodnotu posunu */
/* 1 = Zvýšení, -1 = Zmenšení, 0 = Bez zmìny */
int readEncoder() {
	encoderCurrentValue = digitalRead(encoderPinA);
	if ((encoderLastValue == LOW) && (encoderCurrentValue == HIGH)) {
		encoderLastValue = encoderCurrentValue;
		if (digitalRead(encoderPinB) == LOW) return -1;
		else return 1;
	}
	encoderLastValue = encoderCurrentValue;
	return 0;
}

/* Je èas bliknout hodnotou v menu? */
bool isBlinkTime() {
	currentMillis = millis();

	if (currentMillis - previousMillis >= blinkInterval) {  // Nastal èas
		previousMillis = currentMillis;  // Pøepíšeme èas posledního blinkutí
		return true;  // Oznámíme že je èas na bliknutí
	}

	return false;  // Oznámíme že není èas na bliknutí
}

/* Vypsání textu na displej s urèitou pozicí */
void lcdPrint(int column, int row, String text) {
	lcd.setCursor(column, row);
	lcd.print(text);
}
void lcdPrint(int column, int row, int value) {
	lcdPrint(column, row, String(value));
}
void lcdPrint(int column, int row, float value) {
	lcdPrint(column, row, String(value));
}
void lcdPrint(int column, int row, char value) {
	lcd.setCursor(column, row);
	lcd.print(value);
}

#pragma endregion

#pragma region Výstup - Ovládání periferií na základì mìøení

/* Ovládání periferií na základì mìøení */
void control() {

	// Reguluje
	if (getDaysFromStart() <= setDays) {

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
