/*
 * Nope: Simple FPS game on Nokia
 * By giangvinhloc610
 */

/* System headers */

// C standard libraries
#include <math.h>

// MRE libraries
#include "vmsys.h"    // things releated to system
#include "vmio.h"     // things releated to I/O operation
#include "vmgraph.h"  // graphic
#include "vmchset.h"  // convert between charsets
#include "vmstdlib.h" // implementations of some standard C functions
#include "ResID.h"    // resource library
#include "vm4res.h"   // resource library
#include "vmres.h"    // resource library
#include "vmtimer.h"  // timer library
#include "string.h"   // string operations

/* Local headers */

/* Macros */

#define FPS 60 // Frames per second

/* Global variables */

VMINT layer_hdl[1];  // Layer buffer handler
VMUINT16* layer_buf; // Pointer to layer buffer

VMUINT8* font;       // Font buffer

int scr_w, scr_h;    // Screen width & height

VMINT timer_h = -1;  // Main game loop's timer id

int prev_tick;       // Previous tick when visit loop() function, to calculate FPS

/* Function prototype */

// Event handlers
void handle_sysevt(VMINT message, VMINT param);
void handle_keyevt(VMINT event, VMINT keycode);
void handle_penevt(VMINT event, VMINT x, VMINT y);

// Debug helper
void draw_text(int x, int y, unsigned char* str, int length);

// Main game loop
void loop(int tid);

/*
 * vm_main: main entry point of VXP
 */

void vm_main() {
    /* Set the initial tick count */
    prev_tick = vm_get_tick_count();

    /* Get the screen size */
    scr_w = vm_graphic_get_screen_width();
    scr_h = vm_graphic_get_screen_height();

    /* Init graphic layer */
    layer_hdl[0] = -1;

    /* register system events handler */
    vm_reg_sysevt_callback(handle_sysevt);

    /* register keyboard events handler */
    vm_reg_keyboard_callback(handle_keyevt);
    
    /* register pen events handler */
    vm_reg_pen_callback(handle_penevt);

    /* Init resource */
    vm_res_init();

    /* Disable power saving mode (to keep screen always on) */
    vm_switch_power_saving_mode(turn_off_mode);

    /* Load the font */
    VMINT size;
    VMUINT8* res = vm_load_resource("ProFont6x11.gif", &size);
    font = vm_graphic_get_canvas_buffer(vm_graphic_load_image(res, size));
    vm_free(res); // free the buffer
}

/*
 * handle_sysevt: system events handler
 */

void handle_sysevt(VMINT message, VMINT param) {
    switch (message) {
        case VM_MSG_CREATE:
        case VM_MSG_ACTIVE:
            // If app is in active mode or it's just created
            // Create layer buffer
            layer_hdl[0] = vm_graphic_create_layer(0, 0, scr_w, scr_h, VM_COLOR_888_TO_565(0, 0, 0));
            vm_graphic_set_clip(0, 0, scr_w, scr_h);
            layer_buf = (VMUINT16*)vm_graphic_get_layer_buffer(layer_hdl[0]);

            // If timer not init -> init timer
            if (timer_h <= 0)
                timer_h = vm_create_timer(1000/FPS, loop);
            break;

        case VM_MSG_PAINT:
            // We refresh ourself, so just ignore this message
            break;

        case VM_MSG_INACTIVE:
            // If app in inactive mode (eg when the phone receive info popup about charging / incoming call,...)
            // We should handle this carefully, or the app will cause the phone to restart
            // Delete timer
            vm_delete_timer(timer_h);
            timer_h = -1;

            // Delete graphic layer
            if (layer_hdl[0] != -1)
                vm_graphic_delete_layer(layer_hdl[0]);

            break;
        case VM_MSG_QUIT:
            // Receive quit even (eg when pressing power key or function vm_exit_app() is called)
            // Delete timer
            vm_delete_timer(timer_h);
            timer_h = -1;

            // Delete graphic layer
            if (layer_hdl[0] != -1)
                vm_graphic_delete_layer(layer_hdl[0]);

            // Deinit resouces
            vm_res_deinit();
            break;
    }
}

/*
 * handle_keyevt: key events handler
 */

void handle_keyevt(VMINT event, VMINT keycode) {

}

/*
 * handle_penevt: touch events handler
 */

void handle_penevt(VMINT event, VMINT x, VMINT y) {

}

/*
 * loop: game's main loop
 * Clear the screen and redraw the buffer
 */

void loop(int tid) {
    /* Clear the screen */
    vm_graphic_clear_layer_bg(layer_hdl[0]);

    /* Calculate FPS */
    int delta = vm_get_tick_count() - prev_tick; // Get current tick count

    char str_buffer[100]; // Create a temp string buffer
    sprintf(str_buffer, "%d FPS, %d ms", 1000/delta, delta);
    draw_text(2, 2, (unsigned char*)str_buffer, strlen(str_buffer)); // Print to screen

    prev_tick = vm_get_tick_count(); // Get new tick count

    /* Refresh the screen */
    vm_graphic_flush_layer(layer_hdl, 1);
}

/*
 * draw_text: draw text on screen with the font in "font" variable
 */

void draw_text(int x, int y, unsigned char* str, int length) {
	int i; // Because GCC don't like it to be in the for loop :)
    
	for (i = 0; i < length; ++i) {
        vm_graphic_blt((VMBYTE*)layer_buf, x + (i * 6), y, font, (str[i] % 16) * 6, str[i] / 16 * 11, 6, 11, 1);
    }
}
