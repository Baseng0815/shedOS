#include "keyboard.h"

#include <printk.h>


/* 0 means no ASCII representation */
char kb_asciimap[84] = {
        '0', '1', '2', '3', '4',
        '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e',
        'f', 'g', 'h', 'i', 'j',
        'k', 'l', 'm', 'n', 'o',
        'p', 'q', 'r', 's', 't',
        'u', 'v', 'w', 'x', 'y',
        'z', 'u', 'v', 'w', 'x',
        'y', 'z', '=', '-', '[',
        ']', '/', '.', '\'', ';',
        '`', '\\', ',', 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        '0', '1', '2', '3', '4',
        '5', '6', '7', '8', '9',
        '-', '+', '*', '.', 0,
        0, '\t', '\r', 0, 0,
        0, ' ', 0, 0, 0,
        0, 0
};

void handle_key(struct kb_event *event)
{
        if (event->action == KB_PRESS)
                printk(KMSG_LOGLEVEL_NONE, "%c", kb_asciimap[event->code]);
}
