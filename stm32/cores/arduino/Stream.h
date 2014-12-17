
#ifndef Stream_h
#define Stream_h


//#include "Print.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "WString.h"
#include "Print.h"
  
// compatability macros for testing
/*
#define   getInt()            parseInt()
#define   getInt(skipChar)    parseInt(skipchar)
#define   getFloat()          parseFloat()
#define   getFloat(skipChar)  parseFloat(skipChar)
#define   getString( pre_string, post_string, buffer, length)
readBytesBetween( pre_string, terminator, buffer, length)
*/

/**
 * @brief Stream library inherited by Wire, Serial, etc.
 * @details 
 */
class Stream : public Print
{
protected:
  uint32_t _timeout;      // number of milliseconds to wait for the next char before aborting timed read
  uint32_t _startMillis;  // used for timeout measurement
  int timedRead();    // private method to read stream with timeout
  int timedPeek();    // private method to peek stream with timeout
  int peekNextDigit(); // returns the next numeric digit in the stream or -1 if timeout
  
  int parseInt(char skipChar); // as above but the given skipChar is ignored
  // as above but the given skipChar is ignored
  // this allows format characters (typically commas) in values to be ignored

  float parseFloat(char skipChar);  // as above but the given skipChar is ignored

public:
  /**
   * @brief Return the number of bytes in the receive buffer
   * @details 
   * @return 
   */
  virtual int available() = 0;

  /**
   * @brief Reads a single byte from the receive buffer
   * @details Increments the buffer pointer
   * @return 
   */
  virtual int read() = 0;

  /**
   * @brief Reads a single byte from the receive buffer
   * @details Does not increment the buffer pointer
   * @return 
   */
  virtual int peek() = 0;

  /**
   * @brief Waits till the previous write operation is finished
   * @details 
   */
  virtual void flush() = 0;

  Stream() {_timeout=1000;}

// parsing methods

  /**
   * @brief Sets maximum milliseconds to wait for stream data
   * @details 
   * 
   * @param timeout Default is 1 second
   */
  void setTimeout(uint32_t timeout);

  /**
   * @brief Reads data from the stream until the target string is found
   * @details 
   * 
   * @param target 
   * @return true if target string is found, false if timed out (see setTimeout)
   */
  bool find(char *target);

  /**
   * @brief Reads data from the stream until the target string of given length is found
   * @details 
   * 
   * @param target 
   * @param length 
   * 
   * @return true if target string is found, false if timed out
   */
  bool find(char *target, size_t length);

  /**
   * @brief Same as find() but search ends if the terminator string is found
   * @details 
   * 
   * @param target 
   * @param terminator 
   * 
   * @return 
   */
  bool findUntil(char *target, char *terminator);


  bool findUntil(char *target, size_t targetLen, char *terminate, size_t termLen);

  /**
   * @brief Returns the first valid (long) integer value from the current position
   * @details Initial characters that are not digits (or the minus sign) are skipped. Integer is terminated by the first character that is not a digit.
   * @return 
   */
  int parseInt();

  /**
   * @brief Float version of parseInt()
   * @details 
   * @return 
   */
  float parseFloat();

  /**
   * @brief Read chars from stream into buffer
   * @details Terminates if length characters have been read or timeout (see setTimeout())
   * 
   * @param buffer 
   * @param length 
   * 
   * @return The number of characters placed in the buffer (0 means no valid data found)
   */
  size_t readBytes( char *buffer, size_t length);

  /**
   * @brief Same as readBytes() with terminator character
   * @details Terminates if length characters have been read, timeout, or if the terminator character  detected
   * 
   * @param terminator 
   * @param buffer 
   * @param length 
   * @return the number of characters placed in the buffer (0 means no valid data found)
   */
  size_t readBytesUntil( char terminator, char *buffer, size_t length);

  // Arduino String functions to be added here
  String readString();
  String readStringUntil(char terminator);
};

#endif
