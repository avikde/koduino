
#include "Board.h"

void Board::errorStop(const char *msg) {
  while (1) {
    digitalWrite(led, LOW);
    delay(50);
    digitalWrite(led, HIGH);
    Serial1 << msg << "\n";
    delay(1000);
  }
}
