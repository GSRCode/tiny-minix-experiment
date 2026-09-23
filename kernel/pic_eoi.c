#include "constants.h"
#include "prototypes.h"
void pic_eoi(void) { outb(PIC_MASTER_COMMAND, PIC_EOI); }
