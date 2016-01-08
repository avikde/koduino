/**
 * @authors Avik De <avikde@gmail.com>

  This file is part of koduino <https://github.com/avikde/koduino>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DxlMotor_h
#define DxlMotor_h

#include <DxlNode.h>
#include <Motor.h>

class DxlMotor : public Motor {
public:
  // Constructor (sets defaults)
  DxlMotor() :  id(0), rawPos(0), lastTX(0) {}

  // "Constructors"
  void init(DxlNode *master_, uint8_t id_, float zero, int8_t dir, float gearRatio);
  void init(DxlNode *master_, uint8_t id_, float zero, int8_t dir) {
    init(master_, id_, zero, dir, 1.0);
  }
  // From base class
  void enable(bool flag) {enableFlag = flag;}
  float getRawPosition() {return rawPos;}
  // choose sendOpenLoop to actually do the comms, and store the rawPosition
  void sendOpenLoop(float val);
  // waits till master is free after the previous update()
  // MUST CALL THIS BEFORE USING THE SAME MASTER AGAIN
  bool updated();

  // new
  float getCurrent() {return rawCurrent;}

  uint8_t id;
protected:
  DxlNode *master;
  float rawPos;
  float rawCurrent;
  uint32_t lastTX;
};

#endif
