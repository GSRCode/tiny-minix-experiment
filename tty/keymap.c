#include "prototypes.h"

char scan_code_to_char(unsigned char scan_code, int shift, int caps_lock)
{
    switch (scan_code) {

        /* Number row */
        case 2:  return shift ? '!' : '1';
        case 3:  return shift ? '@' : '2';
        case 4:  return shift ? '#' : '3';
        case 5:  return shift ? '$' : '4';
        case 6:  return shift ? '%' : '5';
        case 7:  return shift ? '^' : '6';
        case 8:  return shift ? '&' : '7';
        case 9:  return shift ? '*' : '8';
        case 10: return shift ? '(' : '9';
        case 11: return shift ? ')' : '0';

        /* Top row */
        case 16: return (shift ^ caps_lock) ? 'Q' : 'q';
        case 17: return (shift ^ caps_lock) ? 'W' : 'w';
        case 18: return (shift ^ caps_lock) ? 'E' : 'e';
        case 19: return (shift ^ caps_lock) ? 'R' : 'r';
        case 20: return (shift ^ caps_lock) ? 'T' : 't';
        case 21: return (shift ^ caps_lock) ? 'Y' : 'y';
        case 22: return (shift ^ caps_lock) ? 'U' : 'u';
        case 23: return (shift ^ caps_lock) ? 'I' : 'i';
        case 24: return (shift ^ caps_lock) ? 'O' : 'o';
        case 25: return (shift ^ caps_lock) ? 'P' : 'p';

        /* Home row */
        case 30: return (shift ^ caps_lock) ? 'A' : 'a';
        case 31: return (shift ^ caps_lock) ? 'S' : 's';
        case 32: return (shift ^ caps_lock) ? 'D' : 'd';
        case 33: return (shift ^ caps_lock) ? 'F' : 'f';
        case 34: return (shift ^ caps_lock) ? 'G' : 'g';
        case 35: return (shift ^ caps_lock) ? 'H' : 'h';
        case 36: return (shift ^ caps_lock) ? 'J' : 'j';
        case 37: return (shift ^ caps_lock) ? 'K' : 'k';
        case 38: return (shift ^ caps_lock) ? 'L' : 'l';

        /* Bottom row */
        case 44: return (shift ^ caps_lock) ? 'Z' : 'z';
        case 45: return (shift ^ caps_lock) ? 'X' : 'x';
        case 46: return (shift ^ caps_lock) ? 'C' : 'c';
        case 47: return (shift ^ caps_lock) ? 'V' : 'v';
        case 48: return (shift ^ caps_lock) ? 'B' : 'b';
        case 49: return (shift ^ caps_lock) ? 'N' : 'n';
        case 50: return (shift ^ caps_lock) ? 'M' : 'm';

        /* Punctuation */
        case 12: return shift ? '_' : '-';
        case 13: return shift ? '+' : '=';
        case 26: return shift ? '{' : '[';
        case 27: return shift ? '}' : ']';
        case 39: return shift ? ':' : ';';
        case 40: return shift ? '"' : '\'';
        case 41: return shift ? '~' : '`';
        case 43: return shift ? '|' : '\\';
        case 51: return shift ? '<' : ',';
        case 52: return shift ? '>' : '.';
        case 53: return shift ? '?' : '/';

        case 57: return ' ';
        case 28: return '\n';

        default:
            return 0;
    }
}