/* Nastaven� encoderu */

#define encoderPinA 9  // Pin encoderu A
#define encoderPinB 3  // Pin encoderu B

int readedValue;  // Hodnota p�e�ten� z encoderu
int encoderLastValue = LOW;  // Posledn� hodnota encoderu
int encoderCurrentValue = LOW;  // Aktu�ln� hodnota encoderu

int value = 200;

/* Nastaven� p�ed prvn�m spu�t�n�m */
void setup()
{
	Serial.begin(9600);  // Zah�jen� komunikace s po��ta�em
}

/* Hlavn� program */
void loop()
{
	readedValue = readEncoder();  // P�e�ten� hodnoty z encoderu
	value += readedValue;

	if (readedValue == 1) {
		Serial.print(value);
		Serial.println(" [ + 1]");
	}

	else if (readedValue == -1) {
		Serial.print(value);
		Serial.println(" [ - 1]");
	}
}

/* P�e�te hodnotu z encoderu a vrac� hodnotu posunu */
/* 1 = Zv��en�, -1 = Zmen�en�, 0 = Bez zm�ny */
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