#include "constants.h"
#include "globals.h"
#include "prototypes.h"

void int_gate(unsigned int vec_nr, unsigned int offset, unsigned int dpl_type)
{
    struct gatedesc_s *idp = &idt[vec_nr];
    idp->offset_low = offset & 0xFFFF;
    idp->offset_high = offset >> 16;
    idp->selector = CS_SELECTOR;
    idp->pad = 0;
    idp->p_dpl_type = dpl_type;
}
