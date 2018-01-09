/* Nastavení encoderu */

#define encoderPinA 9  // Pin encoderu A
#define encoderPinB 3  // Pin encoderu B

int readedValue;  // Hodnota pøeètená z encoderu
int encoderLastValue = LOW;  // Poslední hodnota encoderu
int encoderCurrentValue = LOW;  // Aktuální hodnota encoderu

/* Nastavení pøed prvním spuštìním */
void setup()
{
	Serial.begin(9600);  // Zahájení komunikace s poèítaèem
}

/* Hlavní program */
void loop()
{
	readedValue = readEncoder();  // Pøeètení hodnoty z encoderu

	if (readedValue == 1) Serial.println("Zmìna: +1");  // Vypiš zmìny potenciometru
	else if (readedValue == -1) Serial.println("Zmìna: -1");  // Vypiš zmìny potenciometru

	readedValue = 0;
}

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