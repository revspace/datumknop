#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;

IPAddress printer(10,4,2,25);
IPAddress myip(10,4,2,11);
IPAddress mymask(255,255,255,0);
IPAddress gw(10,4,2,1);
IPAddress ntp(10,4,2,1);
IPAddress dns(10,4,2,1);
NTPClient ntpclient(ntpUDP, ntp);

void setup() {
	Serial.begin(115200);
	Serial.println("Hallo wereld!");
	WiFi.config(myip, dns, gw, mymask);
	WiFi.begin("revspace-dingen", "");
	setenv("TZ","CET-1CEST,M3.5.0/2,M10.5.0/3",1);
	tzset();
}

void loop() {
	while (WiFi.status() != WL_CONNECTED ) {
		Serial.println("Connecting to WiFi...");
		delay(10);
	}
	ntpclient.update();
	unsigned long time = ntpclient.getEpochTime();

	struct tm *ptm = localtime((time_t *) &time);

	char date[11];
	snprintf(date, sizeof(date), "%04d-%02d-%02d",
		ptm->tm_year + 1900,
		ptm->tm_mon + 1,
		ptm->tm_mday
	);

	Serial.println("Connecting to printer...");

	WiFiClient client;
	if (client.connect(printer, 9100)) {
		client.print("\eia"); client.write('\0');
		client.print(
			"\e@"
			"\ea\x01"
			"\ek\x0b"
		);
		client.print("\eX0\x43"); client.write('\0');
		client.print("gratis meuk\n");
		client.print("\eX0\x64"); client.write('\0');
		client.print(date);
		client.print("\f");
		client.stop();
		Serial.println("Date sent to printer.");
	}

	Serial.println("Halting.");

	for(;;);
}
