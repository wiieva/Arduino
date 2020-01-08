#include <WiievaRecorder.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

const char *ssid     = "...";
const char *password = "...";
const char *url      = "http://www.google.com/speech-api/v2/recognize?output=json&lang=en&key=...";

ESP8266WiFiMulti WiFiMulti;

void setup () {
    Serial.begin (115200);
    Serial.setDebugOutput(true);
    WiFiMulti.addAP(ssid, password);
    Serial.printf ("Press PWR button to start speaking\n");
}

void recognize () {
    HTTPClient http;
    WiievaRecorder recorder (2000*5);
    recorder.start (AIO_AUDIO_IN_SPEEX);
    Serial.printf ("Start recording\n");

    int res;
    int millisStart = millis(),millisEnd=0;
    int vadPauseCnt = 0;
    do {
        res = recorder.run ();
        if (millis () - millisStart < 1000 || recorder.checkVad())
            millisEnd = millis ();
    } while (res && (!millisEnd || millis () - millisEnd < 500));

    http.begin(url);
    http.addHeader ("Content-Type","audio/x-speex-with-header-byte; rate=8000");
    int httpCode = http.sendRequest ("POST",&recorder,recorder.recordedSize());

    if(httpCode > 0) {
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
        String payload = http.getString();
        Serial.println(payload);
    }
    http.end();
}

void loop () {
    if (digitalRead (WIIEVA_KB_PWR)) {
        while ( digitalRead (WIIEVA_KB_PWR));
        recognize ();
    }
    WiFiMulti.run();
}
