#include <WiievaConsole.h>
#include <WiievaIRSend.h>

WiievaConsole con;
WiievaIRSend IRSend;

void setup() {
  con.begin (WiievaConsole::FontBig);
  con.toggleKeyboard ();
  con.bold().red().println ("IR control demo");
  con.normal().println ("0 - Power");
  con.println ("1 - Volume Up");
  con.println ("2 - Volume Down");
  con.println ("3 - Prog Up");
  con.println ("4 - Prog Down");
  con.white().bold().println ("Press key:");
}

void loop() {
  delay (10);
  if (con.available  ()) {
    int code = 0;
    switch (con.read ()) {
      case '0': code = 0xA90; break;
      case '1': code = 0x490; break;
      case '2': code = 0xC90; break;
      case '3': code = 0x090; break;
      case '4': code = 0x890; break;
    }
    for (int i = 0; i < 5;++i) {
      IRSend.sendSony(code,12);
      yield ();
      delay (45);
    }
  }
}
