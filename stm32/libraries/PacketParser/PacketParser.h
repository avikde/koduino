
#ifndef PacketParser_h
#define PacketParser_h

#include <stdint.h>

#define MAX_PACKET_LEN 100
#define MAX_ALIGNMENT_LEN 4

// Helpful data-related functions =======================================

typedef union {
  int16_t s16;
  struct {
    uint8_t h, l;
  } u8;
} s16u8;

// cldoc:begin-category(PacketParser)

// __REV already exists in CMSIS
// Convert an unsigned 16 bit int to reverse order
static inline uint16_t rev16(uint16_t a) {
  return (a >> 8) | (a << 8);
}

enum ChecksumType {
  CHECKSUM_XOR, CHECKSUM_16BIT_BIG_ENDIAN
};
// Callback for a new packet: Should return true to accept the packet and remove that data from the buffer, false to reject the packet (checksum didn't match?) and keep the data in the buffer
typedef bool (*NewPacketFunc)(const uint8_t *buf);

class PacketParser {
protected:
  uint8_t buffer[MAX_PACKET_LEN];
  int len;
  // alignment
  uint8_t alignmentLen; // 1 or 2
  uint8_t alignment[MAX_ALIGNMENT_LEN]; // Needs to come at the END of the packet
  NewPacketFunc packetCallback;

public:
  void init(int len, uint8_t alignmentLen, const uint8_t *alignment, NewPacketFunc f);

  static bool checksum(const uint8_t *buf, int n, ChecksumType type);

  // void attach();
};

// cldoc:end-category()

#endif

