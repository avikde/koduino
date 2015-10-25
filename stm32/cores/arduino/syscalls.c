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
#include "syscalls.h"
#include "chip.h"
#include <core_cm4.h>
#include <core_cmFunc.h>

#ifdef __cplusplus
extern "C" {
#endif

char *__env[1] = { 0 };
char **environ = __env;

/*----------------------------------------------------------------------------
 *        Exported variables
 *----------------------------------------------------------------------------*/

// #undef errno
// extern int errno ;
// extern int  _end ;

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
extern void _exit( int status ) ;
extern void _kill( int pid, int sig ) ;
extern int _getpid ( void ) ;


extern int link( char *cOld, char *cNew )
{
    return -1 ;
}

extern int _write(int file, char *ptr, int len)
{
  // if(bDeviceState == CONFIGURED) {
  //   uint32_t timeout=180000;

  //   while((!packet_sent) && timeout--) {}

  //   // if loop quit without timeout, success
  //   if(timeout) {

  //     if(len>63) {
  //       uint8_t packetNo, finalPacket, i;
  //       packetNo    = len/64;
  //       finalPacket = len;
  //       for(i=0;i<packetNo;i++) {
  //         CDC_Send_DATA((uint8_t*)ptr, 63);
  //         finalPacket -= 63;
  //         ptr += 63;
  //         timeout=180000;
  //         while(packet_sent==0 && timeout--);
  //         if(timeout==0)
  //         {
  //           bDeviceState = UNCONNECTED;
  //           return len;
  //         }
  //       }
  //       CDC_Send_DATA((uint8_t*)ptr,finalPacket);
  //       return len;
  //     }
  //     else {
  //       CDC_Send_DATA((uint8_t*)ptr,len);
  //       return len;
  //     }
  //   }
  //   // loop quit because of timeout, fail
  //   else {
  //     return -1;
  //   }
  // }
  // else {
  //   return len;
  // }
  return 0;
}


extern caddr_t _sbrk(int incr) {
  extern char _end; // Defined by the linker
  static char *heap_end;
  char *prev_heap_end;
  if(heap_end == 0) {heap_end = &_end;}
  prev_heap_end = heap_end;
  char * stack = (char*) __get_MSP();
  if (heap_end+incr >  stack)
  {
    _write(1, (char *)"Heap and stack collision\n", 25);
    return  (caddr_t) -1;
  }
  heap_end += incr;
  return (caddr_t) prev_heap_end;
}


extern int _read(int file, char *ptr, int len) {
  // uint8_t rLen=0;
  // uint8_t *temp;
  // while(rLen<len)
  // {
  //   while(!packet_receive);
  //   CDC_Receive_DATA();
  //   temp = Receive_Buffer;
  //   for(u8 i=0; i<Receive_length; i++)
  //   {
  //     *ptr = *temp;
  //     ptr++;
  //     temp++;
  //   }
  //   rLen += Receive_length;
  //   printf("%u %d %x\r\n", (unsigned int)Receive_length, len, *Receive_Buffer);
  // }
  // return len;
  return 0;
}


extern int _close(int file) {
  return -1;
}

extern int _fstat( int file, struct stat *st )
{
    // st->st_mode = S_IFCHR ;

    return 0 ;
}

extern int _isatty( int file )
{
    return 1 ;
}

extern int _lseek( int file, int ptr, int dir )
{
    return 0 ;
}

extern void _exit( int status )
{
  for ( ; ; ) ;
}

extern void _kill( int pid, int sig )
{
  return ;
}

extern int _getpid ( void )
{
  return -1 ;
}

#ifdef __cplusplus
}
#endif


