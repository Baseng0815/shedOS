#include "irqs.h"

#include "../io.h"
#include "../input/keyboard.h"
#include "pic.h"

#include <printk.h>

static enum kb_keycode keycode_map[84] = {
        KB_NONE, KB_ESCAPE, KB_1, KB_2,
        KB_3, KB_4, KB_5, KB_6,
        KB_7, KB_8, KB_9, KB_0,
        KB_MINUS, KB_EQUAL, KB_BACKSPACE, KB_TAB,
        KB_Q, KB_W, KB_E, KB_R,
        KB_T, KB_Y, KB_U, KB_I,
        KB_O, KB_P, KB_LSQUAREB, KB_RSQUAREB,
        KB_RETURN, KB_LCTRL, KB_A, KB_S,
        KB_D, KB_F, KB_G, KB_H,
        KB_J, KB_K, KB_L, KB_SEMICOLON,
        KB_APOSTROPHE, KB_BACKTICK, KB_LSHIFT, KB_BACKSLASH,
        KB_Z, KB_X, KB_C, KB_V,
        KB_B, KB_N, KB_M, KB_COMMA,
        KB_DOT, KB_SLASH, KB_RSHIFT, KB_KP_STAR,
        KB_LALT, KB_SPACE, KB_CAPS, KB_F1,
        KB_F2, KB_F3, KB_F4, KB_F5,
        KB_F6, KB_F7, KB_F8, KB_F9,
        KB_F10, KB_NUMLOCK, KB_SCROLLLOCK, KB_KP_7,
        KB_KP_8, KB_KP_9, KB_KP_MINUS, KB_KP_4,
        KB_KP_5, KB_KP_6, KB_KP_PLUS, KB_KP_1,
        KB_KP_2, KB_KP_3, KB_KP_0, KB_KP_DOT
};

__attribute__((interrupt)) void kb_handle(struct interrupt_frame *iframe)
{
        uint8_t scancode = io_inb(0x60);

        struct kb_event kb_event;
        /* unknown key */
        if (scancode > 0xD8 /* F12 released */)
                return;

        if (scancode > 0x58 /* F12 released */) {
                /* key was released */
                kb_event.code   = keycode_map[scancode - 0x80];
                kb_event.action = KB_RELEASE;
        } else {
                /* we don't care about repeated keys for now */
                kb_event.code   = keycode_map[scancode];
                kb_event.action = KB_PRESS;
        }

        handle_key(&kb_event);

        pic_send_eoi(false);
}
