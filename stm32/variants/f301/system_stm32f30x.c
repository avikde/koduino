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
#if defined(CLKSRC_HSE_16M)
#include "hse_16.inc"
#elif defined(CLKSRC_HSE_8M)
#include "hse_8.inc"
#elif defined(CLKSRC_HSE_8M_OC96)
#include "hse_8_oc96.inc"
#elif defined(CLKSRC_HSE_8M_OC112)
#include "hse_8_oc112.inc"
#else
// includes CLKSRC_HSI
#include "hsi.inc"
#endif