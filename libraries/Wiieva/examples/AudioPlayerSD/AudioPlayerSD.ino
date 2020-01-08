
#include <WiievaPlayer.h>
#include <SD.h>

const char *file      = "ichwill.mp3";

void setup () {
    Serial.begin (115200);
    Serial.setDebugOutput (true);

    if (!SD.begin(WIIEVA_SD_CS))
       Serial.println("Error init microsd card!");

    Serial.printf ("Press PWR button to start playing\n");
}

void play () {
    WiievaPlayer player (0x2000);
    File f = SD.open(file);
    if (!f) {
        Serial.printf ("Can't open file\n");
        return;
    }

    Serial.printf ("Start playing\n");
    player.start (AIO_AUDIO_OUT_MP3);

    while (!digitalRead (WIIEVA_KB_PWR))
        player.run(f);

    Serial.printf ("Stop playing\n");
    player.stop ();

    while (digitalRead (WIIEVA_KB_PWR));
}

void loop () {
    if (digitalRead (WIIEVA_KB_PWR)) {
        while ( digitalRead (WIIEVA_KB_PWR));
        play ();
    }
}

