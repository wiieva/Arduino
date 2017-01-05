// Wiieva board demonstration scetch
// *********************************
// 1. IMPORTANT:
// This scetch requires additional resource files to be uploaded to board. This resources are located in data folder
// You have to upload the contents of the data folder with MkSPIFFS Tool ("ESP8266 Sketch Data Upload" in Tools menu in Arduino IDE)
// For uploading data necesary to install the following tool: https://github.com/esp8266/arduino-esp8266fs-plugin
// 2. SPEECH RECOGNITION
// For enabling speech recognition required google cloud API key. https://cloud.google.com/speech/
// Add obtained key to #define UPLOAD_AUDIO_URL

#include <SdFat.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiievaRecorder.h>
#include <WiievaPlayer.h>
#include <WiievaIRSend.h>
#include "gfx.h"

ESP8266WiFiMulti WiFiMulti;
HTTPClient http;
SdFat SD;

#define UPLOAD_AUDIO_URL "http://www.google.com/speech-api/v2/recognize?output=json&lang=en&key=...."

// Common window stuff
// ******************************************************************
extern "C"{
#include "output/gui.c"
}

class Window {
public:
    virtual void init (GHandle _handle,coord_t x,coord_t y,coord_t w,coord_t h) {
        handle = _handle;
        gwinResize(handle,w,h); 
        gwinMove(handle,x,y);
    }
    virtual void onShow () {}
    virtual void onClose () {};
    virtual int handleEvent (GEvent *e) {return 0;}
    void show () {
        if (this == curWindow)
            return;
        if (curWindow) {
            curWindow->onClose ();
            gwinHide (curWindow->handle);
        }
        curWindow = this;
        onShow ();
        gwinShow (handle);
    }
    static void eventLoop(uint32_t timeout) {
        extern Window *winMainMenu;
        GEvent* pe = geventEventWait(&glistener, timeout);
        if (!pe)
            return;
        if (curWindow)
            if (curWindow->handleEvent (pe))
                return;
        switch (pe->type) {
            case GEVENT_GWIN_BUTTON: {
                GEventGWinButton *we = (GEventGWinButton *)pe;
                if (we->gwin == ghButtonClose) {
                    winMainMenu->show();
                }
            }
            break;
        }
    }
protected:
    GHandle handle;
    static Window *curWindow;
};
Window *Window::curWindow = 0;
Window *winMainMenu,*winBrowseFile,*winRemote,*winWeather,*winRecognizer,*winWifiSettings;

// Window: Main menu
// ******************************************************************
class MainMenuWindow : public Window {
public:
    int handleEvent (GEvent *pe) {
        switch (pe->type) {
            case GEVENT_GWIN_BUTTON: {
                GEventGWinButton *we = (GEventGWinButton *)pe;
                if (we->gwin == ghButtonSettings)
                    winWifiSettings->show ();
                else if (we->gwin == ghButtonPlayer)
                    winBrowseFile->show();
                else if (we->gwin == ghButtonRecognize)
                    winRecognizer->show();
                else if (we->gwin == ghButtonWeather)
                    winWeather->show ();
                else if (we->gwin == ghButtonRemote)
                    winRemote->show ();
                else
                    return 0;
                return 1;
            }
            break;
        }
        return 0;
    }
};

// Window: Browse file
// ******************************************************************
class FileBrowseWindow : public Window {
public:
    struct FileRecord {
        bool isDir;
        String name;
    } *files;
    const int  filesMax=256;
    int        filesCnt=0;
    String     curDir;

    void addFileToList (bool isDir,const String &name) {
        int i,width=0;
        for ( i = 1; i < name.length(); ++i) {
            width = gdispGetStringWidthCount (name.c_str(),ghListFiles->font,i);
            if (width+36 > ghListFiles->width) {
                i--; break;
            }
        }

        int id = gwinListAddItem(ghListFiles, name.substring (0,i).c_str(), TRUE);

        if (isDir)
            gwinListItemSetImage (ghListFiles,id,&folder);
        else if (name.indexOf("mp3")>=0 || name.indexOf("MP3")>=0)
            gwinListItemSetImage (ghListFiles,id,&file_mp3);
        else 
            gwinListItemSetImage (ghListFiles,id,&file_reg);
        
        if (filesCnt < filesMax) {
            files[filesCnt].name = name;
            files[filesCnt++].isDir = isDir;
        }
    }
    int getFileList (const String &dirName) {
        char name[256];
        auto dir = SD.open(dirName);
        if (!dir)
            return 0;

        for (;;) {
            auto entry =  dir.openNextFile();
            if (! entry)
                break;
            if (entry.getName(name,sizeof (name)) && name[0] != '.')
            {
                addFileToList (entry.isDirectory(),name);
                entry.close();
            }
        }
        dir.close();
    }
    void clearFileRecords () {
        delete []files;
        filesCnt = 0;
    }
    void openDir(const String &dir) {

        clearFileRecords ();
        files = new FileRecord [filesMax];
        
        if (gwinListItemCount(ghListFiles))
            gwinListSetSelected(ghListFiles,0,TRUE);
        gwinListDeleteAll(ghListFiles);

        if (dir != "/")
            addFileToList (1,"..");
        curDir = dir;
        getFileList (curDir);
    }
    void onShow () {
        openDir("/");
    }
    void onHide () {
        clearFileRecords ();
    }
    int playProgress (int progress) {
        GEvent* pe = geventEventWait(&glistener, 1);

        if (pe /*&& pe->type == GEVENT_GWIN_BUTTON*/) 
            return 0;
        return 1;
    }
    int playFile (const String &fileName) {

        WiievaPlayer player (10000);
        auto f = SD.open(fileName);

        if (!f) {
            Serial.printf ("Can't open file\n");
            return 0;
        }

        Serial.printf ("Start playing\n");
        player.start (AIO_AUDIO_OUT_MP3);

        while (playProgress (10))
            player.run(f);

        Serial.printf ("Stop playing\n");
        player.stop ();
        return 1;
    }
    void onSelect (int idx) {
        if (idx < filesCnt && idx >= 0) {
            String fullPath = curDir;
            if (files[idx].isDir) {
                if (files[idx].name == "..") {
                    int l = fullPath.lastIndexOf (fullPath.substring (0,fullPath.length ()-1));
                    if (l >= 0)
                        fullPath = fullPath.substring (0,l+1);
                } else
                    fullPath += files[idx].name + "/";
                openDir (fullPath);
            } else {
                playFile(fullPath + files[idx].name);
            }
        }
    }

    int handleEvent (GEvent *pe) {
        switch (pe->type) {
            case GEVENT_GWIN_LIST: {
                GEventGWinList *le = (GEventGWinList *)pe;
                onSelect (le->item);
                return 1;
            }
        }
        return 0;
    }
};

// Window: Voice recognizer
// ******************************************************************
class VoiceRecognizeWindow : public Window {
public:
    enum RecognizeStates { RSRecording, RSSending, RSError, RSDone };

    void parseGoogleJson (const String &json,String &res,String &subres) {
        int pos = 0,pos_end;
        String *r = &res;
        for (;;) {
            if ((pos = json.indexOf ("\"transcript\":\"",pos)) < 0)
                break;
            pos += 14;
            if ((pos_end = json.indexOf ('\"',pos)) < 0)
                break;
            *r += json.substring (pos,pos_end) + ";";
            r = &subres;
        }
    }

    int state = 0;

    void onShow () {
        gwinProgressbarSetPosition (ghProgressRecording,0);
        gwinSetText(ghButtonStartSpeech,"Start speech",FALSE);
        gwinSetText(ghProgressRecording,"Waiting...",FALSE);
        gwinSetText (ghLabelRecognizeResult,"",FALSE);
    }
    int progress (int state,int progress,const String &result,const String &subresult) {
        switch (state) {
            case RSRecording:
                gwinSetText(ghProgressRecording,"Recording...",FALSE);
                break;
            case RSSending:
                gwinSetText(ghProgressRecording,"Sending...",FALSE);
                break;
            case RSDone:
                gwinSetText(ghProgressRecording,"Done",FALSE);
                gwinSetText (ghLabelRecognizeResult,result.c_str(),TRUE);
                gwinSetText (ghLabelRecognizeResult2,subresult.c_str(),TRUE);
                break;
            case RSError:
                gwinSetText(ghProgressRecording,"http error!",FALSE);
                gwinSetText (ghLabelRecognizeResult,result.c_str(),TRUE);
                break;
        }
        gwinProgressbarSetPosition (ghProgressRecording,progress);

        GEvent* pe = geventEventWait(&glistener, 10);

        if (pe && pe->type == GEVENT_GWIN_BUTTON) {
            GEventGWinButton *we = (GEventGWinButton *)pe;
            if (we->gwin == ghButtonStartSpeech)
                return 0;
        }
        return 1;
    }

    int audioRecognize (int seconds,int vad) {

        WiievaRecorder recorder (2000*seconds);

        Serial.printf ("Start recording\n");
        recorder.start (AIO_AUDIO_IN_SPEEX);

        int res, cb_res;
        int millis_start = millis();
        int vad_pause_cnt = 0;

        do {
            res = recorder.run ();
            cb_res = progress (RSRecording,recorder.recordedSize() * 100 / (seconds*2000),"","");

            // check VAD if need
            if (vad && millis () - millis_start > 1000 && !recorder.checkVad())
                vad_pause_cnt++;
            else
                vad_pause_cnt = 0;
        } while (res && cb_res && vad_pause_cnt < 30);

        recorder.stop ();
        cb_res = progress (RSSending,20,"","");

        http.begin(UPLOAD_AUDIO_URL);
        http.addHeader ("Content-Type","audio/x-speex-with-header-byte; rate=8000");
        int httpCode = http.sendRequest ("POST",&recorder,recorder.recordedSize());

        if(httpCode > 0) {
            Serial.printf("[HTTP] POST... code: %d\n", httpCode);
            String payload = http.getString();
            Serial.println(payload);
            if(httpCode == HTTP_CODE_OK) {
                String res,subres;
                parseGoogleJson(payload,res,subres);
                cb_res = progress (RSDone,100,res,subres);
            } else
                cb_res = progress (RSError,100,payload,"");
        } else {
            Serial.printf("[HTTP] POST... failed, error: %s\r\n", http.errorToString(httpCode).c_str());
            cb_res = progress (RSError,100,http.errorToString(httpCode),"");
        }

        http.end();
        return 0;
    }

    void onRecognize () {
        gwinSetText(ghButtonStartSpeech,"Stop speech",FALSE);
        gwinSetText (ghLabelRecognizeResult,"",FALSE);
        gwinProgressbarSetPosition (ghProgressRecording,0);
        gfxSleepMilliseconds(200);

        audioRecognize (5, gwinCheckboxIsChecked (ghEnableVAD));
        gwinSetText(ghButtonStartSpeech,"Start speech",FALSE);
    }

    int handleEvent (GEvent *pe) {
        switch (pe->type) {
            case GEVENT_GWIN_BUTTON: {
                GEventGWinButton *we = (GEventGWinButton *)pe;
                if (we->gwin == ghButtonStartSpeech) {
                    onRecognize ();
                    return 1;
                }
            }
        }
        return 0;
    }
};

// Window: Remote control
// *********************************
class RemoteControlWindow : public Window {
public:
    enum RCCommands { RCCmdPower, RCCmdVolumePlus, RCCmdVolumeMunus, RCCmdProgPlus, RCCmdProgMinus, RCCmdNext, RCCmdPrev, RCCmdPause };

    void onCommand (int vendor, int cmd) {
        WiievaIRSend IRSend;
        uint32_t code = 0;
        switch (cmd) {
            case RCCmdPower:      code = 0xA90 ; break;
            case RCCmdVolumePlus: code = 0x490 ; break;
            case RCCmdVolumeMunus:code = 0xC90 ; break;
            case RCCmdProgPlus:   code = 0x090 ; break;
            case RCCmdProgMinus:  code = 0x890 ; break;
            case RCCmdNext:       code = 0x0 ; break;
            case RCCmdPrev:       code = 0x0 ; break;
            case RCCmdPause:      code = 0x0 ; break;
        };

        Serial.printf("send ir =%04X\n",code);
        for (int i = 0; i < 5;++i) {
            IRSend.sendSony(code,12);
            yield ();
            delay (45);
        }
    }

    int handleEvent (GEvent *event) {
    switch (event->type) {
            case GEVENT_GWIN_BUTTON: {
                GEventGWinButton *we = (GEventGWinButton *)event;
                if (we->gwin == ghButtonRemoteVolumeMinus)
                    onCommand (0,RCCmdVolumeMunus);
                else if (we->gwin == ghButtonRemoteVolumePlus)
                    onCommand (0,RCCmdVolumePlus);
                else if (we->gwin == ghButtonRemotePower)
                    onCommand (0,RCCmdPower);
                else if (we->gwin == ghButtonRemoteProgPlus)
                    onCommand (0,RCCmdProgPlus);
                else if (we->gwin == ghButtonRemoteProgMinus)
                    onCommand (0,RCCmdProgMinus);
                else if (we->gwin == ghButtonRemoteNext)
                    onCommand (0,RCCmdNext);
                else if (we->gwin == ghButtonRemoteNext)
                    onCommand (0,RCCmdPrev);
                else
                    return 0;
                return 1;
            }
            break;
        }
        return 0;
    }
};

// Window: Weather
// ******************************************************************
class WeatherWindow : public Window {
public:
};
// Window: Wifi  settings
// *********************************
class WifiSettingsWindow : public Window {
public:
};


void guiInit () {
    gfxInit();
    gdispSetBacklight(100);

    geventListenerInit(&glistener);
    geventAttachSource(&glistener, ginputGetKeyboard(0), 0);
    gwinAttachListener(&glistener);

    guiCreate();

    coord_t y = ghContainerStatus->height;
    coord_t height = ghContainerMainFrame->height - y - ghContainerNavkeys->height;
    coord_t width = ghContainerMainFrame->width;

    (winMainMenu = new MainMenuWindow)->init (ghContainerMainMenu,0,y,width,height);
    (winBrowseFile = new FileBrowseWindow)->init (ghContainerBrowseFile,0,y,width,height);
    (winWifiSettings = new WifiSettingsWindow)->init (ghContainerWifiSettings,0,y,width,height);
    (winRecognizer = new VoiceRecognizeWindow)->init (ghContainerRecognizer,0,y,width,height);
    (winRemote = new RemoteControlWindow)->init (ghContainerRemoteControl,0,y,width,height);
    (winWeather = new WeatherWindow)->init (ghContainerWeather,0,y,width,height);

    winMainMenu->show();
}

void setup () {
    Serial.begin (115200);
    Serial.setDebugOutput(true);
    wdt_disable ();

    WiFiMulti.addAP("...", "...");

    guiInit ();

    if (!SD.begin(WIIEVA_SD_CS))
        Serial.println("Error init microsd card!");
}

void loop () {
    Window::eventLoop(2);
    WiFiMulti.run();
}

extern "C" void gwinButtonDraw_ImageText(GWidgetObject *gw, void *param) {
    coord_t sy;

    const GColorSet* colors;
    if (!gwinGetEnabled((GHandle)gw)) {
        colors = &gw->pstyle->disabled;
        sy = 2 * gw->g.height;
    } else if ((gw->g.flags & GBUTTON_FLG_PRESSED)) {
        colors = &gw->pstyle->pressed;
        sy = 0;
    } else {
        colors = &gw->pstyle->enabled;
        sy = 0;
    }

    gdispImage *img = (gdispImage*)param;

    int x = gdispGImageDraw(gw->g.display, img, gw->g.x + (gw->g.width-img->width)/2, gw->g.y+1, gw->g.width, gw->g.height, 0, sy);

    gdispGDrawStringBox(gw->g.display, gw->g.x+1, gw->g.y+gw->g.height-15 , gw->g.width-2, 15, gw->text, gw->g.font, colors->text, justifyCenter);
}
