#include <WiievaPlayer.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

const char *ssid     = "...";
const char *password = "...";
const char *url      = "http://wiieva.com/files/ichwill_128.mp3";

ESP8266WiFiMulti WiFiMulti;

void setup () {
    Serial.begin (115200);
    Serial.setDebugOutput(true);
    WiFiMulti.addAP(ssid, password);
    Serial.printf ("Press PWR button to start playing\n");
}

void play ()
{
    WiievaPlayer player (0x8000);
    HTTPClient http;

    Serial.printf ("Start playing\n");

    http.begin(url);
    int httpCode = http.GET ();
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if(httpCode != HTTP_CODE_OK)
        return;
    Serial.printf ("Press PWR button to stop playing\n");
    player.start (AIO_AUDIO_OUT_MP3);

    while ( !digitalRead (WIIEVA_KB_PWR))
        player.run(http.getStream());

    Serial.printf ("Stop playing\n");
    player.stop ();

    while (digitalRead (WIIEVA_KB_PWR));
}

void loop () {
    if (digitalRead (WIIEVA_KB_PWR)) {
        while ( digitalRead (WIIEVA_KB_PWR));
        play ();
    }
    WiFiMulti.run();
}
