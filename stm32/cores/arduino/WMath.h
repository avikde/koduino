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


/** @addtogroup Math Basic and advanced math
 *  @{
 */

// ARDUINO

extern long random( long ) ;
extern long random( long, long ) ;
extern void randomSeed( uint32_t dwSeed ) ;
extern long map( long, long, long, long, long ) ;

extern uint16_t makeWord( uint16_t w ) ;
extern uint16_t makeWord( uint8_t h, uint8_t l ) ;

#define word(...) makeWord(__VA_ARGS__)

/**
 * @brief Map a float (different from Arduino implementation)
 * 
 * @param x Value to map
 * @return x mapped from [in_min, in_max] to [out_min, out_max]
 */
extern float map(float x, float in_min, float in_max, float out_min, float out_max);
/**
 * @brief Linear interpolation between given endpoints
 * @param from Lower endpoint
 * @param to Higher endpoint
 * @param frac float between 0.0 and 1.0
 * @return Interpolated value
 */
extern float interp1(float from, float to, float frac);
extern float interpFrac(uint32_t startTime, uint32_t endTime, uint32_t now);

// Mod functions

/**
 * @brief Mod an angle to between -PI and PI
 * @param f Input angle
 * @return Result between -PI and PI
 */
extern float fmodf_mpi_pi(float f);
/**
 * @brief Mod an angle to between 0 and 2*PI
 * @param f Input angle
 * @return Result between 0 and 2*PI
 */
extern float fmodf_0_2pi(float f);
/**
 * @brief Mod between 0 and 1
 * @param f Input value
 * @return Result between 0 and 1
 */
extern float fmodf_0_1(float f);
/**
 * @brief Mod between -0.5 and +0.5
 * @param f Input value
 * @return Result between -0.5 and +0.5
 */
extern float fmodf_mp5_p5(float f);
/**
 * @brief Change coords to mean and diff on a circle (accounting for wraparound)
 * @param a First angle in rad
 * @param b Second angle in rad
 * @param mean Mean angle
 * @param diff Difference angle
 */
extern void circleMeanDiff(float a, float b, float *mean, float *diff);

/**
 * @brief Swaps the values pointed to be a and b
 */
extern void swapByte(uint8_t *a, uint8_t *b);
/**
 * @brief Convenient function to safely (watch for NULL passed) set an output var
 * @param out Output variable (pointer) which could be NULL
 * @param val Value to assign to output
 */
extern void setOutPtrSafe(float *out, float val);

// Matrix operations (TODO: use Eigen)

/**
 * @brief Use arm_mat_mult_f32 to multiply two matrices
 * @param nr1 Number of rows of M1
 * @param nc1 Number of columns of M1 ( = number of rows of M2)
 * @param nc2 Number of columns of M2
 * @param m1 Pointer to contents of M1 (row major)
 * @param m2 Pointer to contents of M2 (row major)
 * @param mout Preallocated array (nr1*nc2) for contents of M1*M2
 */
void matMult(uint16_t nr1, uint16_t nc1, uint16_t nc2, float *m1, float *m2, float *mout);
void matMult(uint16_t nr1, uint16_t nc1, uint16_t nc2, const float *m1, const float *m2, float *mout);

// Digital low pass filter

enum DLPFType {
  DLPF_ANGRATE,   // Returns a speed for an angle (mods 2*pi) 
  DLPF_RATE,      // Returns a speed
  DLPF_SMOOTH,    // Just smooths the input (no derivative involved)
  DLPF_INTEGRATE  // Integrates once, and decays for drift by smooth
};

/**
 * @brief Digital autoregressive low pass filter
 * @details Use by calling DLPF
 * 
 */
class DLPF {
public:
  // Parameters
  DLPFType type;
  float smooth; // between 0 and 1; high means more smooth
  float freq; // in Hz
  // State
  float oldVal;
  float vel;

  DLPF() : type(DLPF_SMOOTH), smooth(0.5), freq(1000), oldVal(0), vel(0) {}
  /**
   * @brief Initialize filter
   * @param smooth Smoothing factor (0 = no smoothing, 1 = never updates)
   * @param freq Update frequency in Hz
   * @param type One of `DLPF_ANGRATE`, `DLPF_RATE`, `DLPF_SMOOTH`, `DLPF_INTEGRATE`
   */
  virtual void init(float smooth, float freq, DLPFType type) {
    this->smooth = smooth;
    this->freq = freq;
    this->type = type;
  }
  /**
   * @brief Call this at the rate given in freq in init()
   * @param val New measurement
   */
  virtual float update(float val);
};

/**
 * @brief Generic proportional-derivative control (for use in a motor, look at the @ref MotorController instead)
 */
class PD : public DLPF {
public:
  float Kp, Kd;

  PD() {
    // DLPF constructor is automatically called
    type = DLPF_RATE;
  }
  /**
   * @brief Computes the PD control from the given error
   * @param val Tracking error
   * @return - Kp * val - Kd * (filtered velocity)
   */
  virtual float update(float val) {
    return - Kp * val - Kd * DLPF::update(val);
  }

  /**
   * @brief Set gains
   * 
   * @param Kp Proportional gain
   * @param Kd Dissipative (derivative) gain
   */
  void setGain(float Kp, float Kd) {
    this->Kp = Kp;
    this->Kd = Kd;
  }
};

/** @} */ // end of addtogroup

#endif // WMath_h
