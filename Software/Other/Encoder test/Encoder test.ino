/* Nastaven� encoderu */

#define encoderPinA 9  // Pin encoderu A
#define encoderPinB 3  // Pin encoderu B

int readedValue;  // Hodnota p�e�ten� z encoderu
int encoderLastValue = LOW;  // Posledn� hodnota encoderu
int encoderCurrentValue = LOW;  // Aktu�ln� hodnota encoderu

/* Nastaven� p�ed prvn�m spu�t�n�m */
void setup()
{
	Serial.begin(9600);  // Zah�jen� komunikace s po��ta�em
}

/* Hlavn� program */
void loop()
{
	readedValue = readEncoder();  // P�e�ten� hodnoty z encoderu

	if (readedValue == 1) Serial.println("Zm�na: +1");  // Vypi� zm�ny potenciometru
	else if (readedValue == -1) Serial.println("Zm�na: -1");  // Vypi� zm�ny potenciometru

	readedValue = 0;
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