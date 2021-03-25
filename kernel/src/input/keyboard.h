#ifndef _KEYBOARD_H
#define _KEYBOARD_H

enum kb_keycode {
        /* numbers */
        KB_0, KB_1, KB_2, KB_3, KB_4, KB_5, KB_6, KB_7, KB_8, KB_9,
        /* letters */
        KB_A, KB_B, KB_C, KB_D, KB_E, KB_F, KB_G, KB_H, KB_I,
        KB_J, KB_K, KB_L, KB_M, KB_N, KB_O, KB_P, KB_Q, KB_R, KB_S,
        KB_T, KB_U, KB_V, KB_W, KB_X, KB_Y, KB_Z,
        /* other characters */
        KB_EQUAL, KB_MINUS, KB_LSQUAREB, KB_RSQUAREB, KB_SLASH, KB_DOT,
        KB_APOSTROPHE, KB_SEMICOLON, KB_BACKTICK, KB_BACKSLASH, KB_COMMA,
        /* function keys */
        KB_F1, KB_F2, KB_F3, KB_F4, KB_F5, KB_F6,
        KB_F7, KB_F8, KB_F9, KB_F10, KB_F11, KB_F12,
        /* keypad */
        KB_KP_0, KB_KP_1, KB_KP_2, KB_KP_3, KB_KP_4,
        KB_KP_5, KB_KP_6, KB_KP_7, KB_KP_8, KB_KP_9,
        KB_KP_MINUS, KB_KP_PLUS, KB_KP_STAR, KB_KP_DOT,
        /* other stuff */
        KB_ESCAPE, KB_BACKSPACE, KB_TAB, KB_RETURN, KB_LCTRL,
        KB_LSHIFT, KB_RSHIFT, KB_SPACE, KB_LALT, KB_CAPS, KB_NUMLOCK,
        KB_SCROLLLOCK,

        KB_NONE
};

enum kb_action {
        KB_PRESS, KB_RELEASE, KB_REPEAT
};

struct kb_event {
        enum kb_keycode code;
        enum kb_action  action;
};

extern char kb_asciimap[];

void handle_key(struct kb_event *event);

#endif
