#include <HPDL1414.h>

/*

  Segment numeration example

*/

const byte dataPins[7] = {9, 7, 5, 3, 43, 46, 1}; // Segment data pins: D0 - D6
//const byte dataPins[7] = {1, 3, 5, 7, 43, 9, 46}; // Segment data pins: D0 - D6
const byte addrPins[2] = {40, 41};               // Segment address pins: A0, A1
const byte wrenPins[]  = {42, 44};                   // Write Enable pins (left to right)

HPDL1414 hpdl(dataPins, addrPins, wrenPins, sizeof(wrenPins));

void setup()
{
  hpdl.begin();
  hpdl.printOverflow(true);
  hpdl.clear();
  delay(1000);
  hpdl.print("DTSTDTST");
  delay(1000);
  hpdl.clear();

  /* List all connected segments */
  for (uint8_t a = 0; a < hpdl.segments(); a++)
    hpdl.print("SEG" + String(a + 1));


}



void loop()
{
}