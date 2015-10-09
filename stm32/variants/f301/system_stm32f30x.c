
#if defined(CLKSRC_HSE_16M)
#include "hse_16.inc"
#elif defined(CLKSRC_HSE_8M)
#include "hse_8.inc"
#else
// includes CLKSRC_HSI
#include "hsi.inc"
#endif