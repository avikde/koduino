/**
 * @authors Arduino team, Avik De <avikde@gmail.com>

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
#ifndef WMath_h
#define WMath_h

// ARDUINO

extern long random( long ) ;
extern long random( long, long ) ;
extern void randomSeed( uint32_t dwSeed ) ;
extern long map( long, long, long, long, long ) ;

extern uint16_t makeWord( uint16_t w ) ;
extern uint16_t makeWord( uint8_t h, uint8_t l ) ;

#define word(...) makeWord(__VA_ARGS__)

// NEW

extern float map(float x, float in_min, float in_max, float out_min, float out_max);
extern float interp1(float from, float to, float frac);
extern float interpFrac(uint32_t startTime, uint32_t endTime, uint32_t now);

// Mod functions

// Returns between -pi and pi
extern float fmodf_mpi_pi(float f);
extern float fmodf_0_2pi(float f);
// Returns between and 0 and 1
extern float fmodf_0_1(float f);
// Returns between and -0.5 and 0.5
extern float fmodf_mp5_p5(float f);
// Change coords to mean and diff on a circle
extern void circleMeanDiff(float a, float b, float *mean, float *diff);

//

extern void swapByte(uint8_t *a, uint8_t *b);
// Convenient function to safely (watch for NULL passed) set an output var
extern void setOutPtrSafe(float *out, float val);

// Matrix operations (TODO: use Eigen)

// mout will be a matrix of size nr1 * nc2. Note, nc1 = nr2
void matMult(uint16_t nr1, uint16_t nc1, uint16_t nc2, float *m1, float *m2, float *mout);
void matMult(uint16_t nr1, uint16_t nc1, uint16_t nc2, const float *m1, const float *m2, float *mout);

// Digital low pass filter

enum DLPFType {
  DLPF_ANGRATE,   // Returns a speed for an angle (mods 2*pi) 
  DLPF_RATE,      // Returns a speed
  DLPF_SMOOTH,    // Just smooths the input (no derivative involved)
  DLPF_INTEGRATE  // Integrates once, and decays for drift by smooth
};

class DLPF {
public:
  // Parameters
  DLPFType type;
  float smooth; // between 0 and 1; high means more smooth
  float freq; // in Hz
  // State
  float oldVal;
  float vel;

  // default params
  DLPF() : type(DLPF_SMOOTH), smooth(0.5), freq(1000), oldVal(0), vel(0) {}
  // can reset params
  virtual void init(float smooth, float freq, DLPFType type) {
    this->smooth = smooth;
    this->freq = freq;
    this->type = type;
  }
  virtual float update(float val);
};

// PD controller

class PD : public DLPF {
public:
  float Kp, Kd;

  PD() {
    // DLPF constructor is automatically called
    type = DLPF_RATE;
  }
  // val is the tracking error
  virtual float update(float val) {
    return - Kp * val - Kd * DLPF::update(val);
  }

  void setGain(float Kp, float Kd) {
    this->Kp = Kp;
    this->Kd = Kd;
  }
};



#endif // WMath_h
