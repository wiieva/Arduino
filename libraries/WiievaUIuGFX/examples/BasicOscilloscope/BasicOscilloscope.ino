#include <WiievaConsole.h>

WiievaConsole con;

void setup() {
  con.begin (WiievaConsole::Normal,WiievaConsole::Landscape);
  con.togglePlotter ();
  con.setPlotterMode (WiievaPlotter::Oscilloscope);
}

void loop() {
  con.print (analogRead (0));
  con.print (" ");
  con.print (analogRead (1));
  con.println ("");
}
