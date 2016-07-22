# @authors Avik De <avikde@gmail.com>

# This file is part of koduino <https://github.com/avikde/koduino>

# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation, either
# version 3 of the License, or (at your option) any later version.

# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, see <http://www.gnu.org/licenses/>.
SERIES = STM32F30x
MCU = STM32F303xC
STD_PERIPH_PREFIX = stm32f30x
KODUINO_ISRS = INLINE
EEP_LEN ?= 0
# 4096
EEP_START = 0x0800F000
MAX_PROGRAM_SIZE = 262144
