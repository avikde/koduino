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
  
/**
  * \file syscalls.h
  *
  * Implementation of newlib syscall.
  *
  */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

// cldoc:begin-category(Helper)

extern caddr_t _sbrk( int incr ) ;

extern int link( char *cOld, char *cNew ) ;

extern int _close( int file ) ;

extern int _fstat( int file, struct stat *st ) ;

extern int _isatty( int file ) ;

extern int _lseek( int file, int ptr, int dir ) ;

extern int _read(int file, char *ptr, int len) ;

extern int _write( int file, char *ptr, int len ) ;

// cldoc:end-category()

#ifdef __cplusplus
}
#endif

