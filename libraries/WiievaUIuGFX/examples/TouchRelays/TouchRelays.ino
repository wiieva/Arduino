// Example of uGFX usage.
#include "gfx.h"


// GPIO pins, where relays are connected
// https://www.seeedstudio.com/Relay-Shield-v3.0-p-2440.html
int relayGpios[] = {4 ,5 ,6, 7};

// Number of controlled relays
const int numRelays = sizeof (relayGpios) /sizeof (relayGpios[0]);
// Current state of relays
int relayStates[numRelays] = {0};

GHandle ghContainerMain,ghButton1,ghButton2,ghButton3,ghButton4,ghButtonAll;
GListener glistener;

void guiCreate(void)
{
    gfxInit();

    // Add GUI event listener for handling events
    geventListenerInit(&glistener);
    geventAttachSource(&glistener, ginputGetKeyboard(0), 0);
    gwinAttachListener(&glistener);

    // Setup defaults for GUI
    gwinSetDefaultFont(gdispOpenFont("DejaVuSans16"));
    gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
    gwinSetDefaultColor(HTML2COLOR(0x000000));
    gwinSetDefaultBgColor(HTML2COLOR(0xFFFFFF));

    // Create GUI elements
    GWidgetInit wi; gwinWidgetClearInit(&wi);
    wi.g.x = 0; wi.g.y = 0; wi.g.width = 176; wi.g.height = 220; wi.g.show = TRUE;
    ghContainerMain = gwinContainerCreate(0, &wi, 0);
    wi.g.parent = ghContainerMain;

    wi.g.width = 88; wi.g.height = 73; wi.text = "Relay 1"; ghButton1 = gwinButtonCreate(0, &wi);
    wi.g.x = 88; wi.g.y = 0; wi.text = "Relay 2"; ghButton2 = gwinButtonCreate(0, &wi);
    wi.g.x = 0; wi.g.y = 73; wi.text = "Ralay 3"; ghButton3 = gwinButtonCreate(0, &wi);
    wi.g.x = 88; wi.g.y = 73; wi.text = "Relay 4"; ghButton4 = gwinButtonCreate(0, &wi);
    wi.g.x = 0; wi.g.y = 146; wi.text = "All"; ghButtonAll = gwinButtonCreate(0, &wi);
}

void setup() {

    Serial.begin (115200);
    Serial.setDebugOutput(true);
    for (int relay = 0; relay < numRelays; relay++)
        pinMode (relayGpios[relay],OUTPUT);

    guiCreate ();
}

void loop() {
    int relay = -1;

    // Check and handle events
    GEvent* pe = geventEventWait(&glistener, 2);
    if (pe && pe->type == GEVENT_GWIN_BUTTON) {
        GEventGWinButton *we = (GEventGWinButton *)pe;
        if (we->gwin == ghButton1) relay = 0;
        if (we->gwin == ghButton2) relay = 1;
        if (we->gwin == ghButton3) relay = 2;
        if (we->gwin == ghButton4) relay = 3;
        if (we->gwin == ghButtonAll) {
             int s = !relayStates[0];
             for (relay =0; relay < numRelays; ++relay){
                relayStates[relay] = s;
                digitalWrite (relayGpios[relay],relayStates[relay]);
                Serial.printf ("Changing relay state %d => %d\n",relay,relayStates[relay]);
             }
        }
    }
    if (relay >= 0 && relay < numRelays) {
        relayStates[relay] = !relayStates[relay];
        digitalWrite (relayGpios[relay],relayStates[relay]);
        Serial.printf ("Changing relay state %d => %d\n",relay,relayStates[relay]);
    }
    delay (10);
}

