void idle(void)
{
    for (;;) {
        __asm__ volatile ("hlt");
    }
}