#include <cstring>
#include <cstdlib>
#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <assert.h>
#include <unistd.h>
#include "config.h"

using namespace std;
#define NIL (0)

int main(int argc, char* argv[]) {
    // Open the display
    Display *dpy = XOpenDisplay(NIL);
    assert(dpy);

    // Parse CLI args
    int urgency = 1;
    for (int i = 2; i < argc; i++) {
        switch (argv[i][0]) {
            case 't':
                timeout = atoi(&argv[i][1]);
                break;
            case 'u':
                urgency = atoi(&argv[i][1]);
                break;
            default:
                break;
        }
    }
    
    // Get the background and border colors
    XColor bg_color;
    XParseColor(dpy, DefaultColormap(dpy, 0), bg_col, &bg_color);
    XAllocColor(dpy, DefaultColormap(dpy, 0), &bg_color);

    XColor bord_color;
    switch (urgency) {
        case 0:
            XParseColor(dpy, DefaultColormap(dpy, 0), border_unimportant, &bord_color);
            break;
        case 1:
            XParseColor(dpy, DefaultColormap(dpy, 0), border_normal, &bord_color);
            break;
        case 2:
            XParseColor(dpy, DefaultColormap(dpy, 0), border_urgent, &bord_color);
            break;
        default:
            XParseColor(dpy, DefaultColormap(dpy, 0), border_normal, &bord_color);
            break;
    }

    XAllocColor(dpy, DefaultColormap(dpy, 0), &bord_color);

    // Set window attributes
    XSetWindowAttributes attributes;
    attributes.override_redirect = True;
    attributes.background_pixel = bg_color.pixel;
    attributes.border_pixel = bord_color.pixel;
    
    // Create the window
    Window w = XCreateWindow(dpy, XDefaultRootWindow(dpy), x_pos, y_pos, width, height, border_size, CopyFromParent, InputOutput, CopyFromParent, CWBackPixel | CWOverrideRedirect | CWBorderPixel, &attributes);
    XSelectInput(dpy, w, StructureNotifyMask);
    XMapWindow(dpy, w);
    
    // Set the font and text color
    XftColor fg_color;
    Visual *visual = DefaultVisual(dpy, 0);
    Colormap cmap = DefaultColormap(dpy, 0);
    XftFont *font = XftFontOpenName(dpy, 0, font_name);
    if (!font) {
        cerr << "Xft: failed to load font " << font_name << endl;
        exit(1);
    }

    if (!XftColorAllocName(dpy, visual, cmap, fg_col, &fg_color)) {
        cerr << "Xft: Failed to allocate color " << fg_col << endl;
        exit(1);
    }

    XftDraw *draw = XftDrawCreate(dpy, w, visual, cmap);
    
	// Wait for the window to be mapped
    for (;;) {
		XEvent e;
		XNextEvent(dpy, &e);
		if (e.type == MapNotify) { break; }
    }

    // Draw the info to the notification
    XftDrawStringUtf8(draw, &fg_color, font, 3, 11, (const FcChar8 *)argv[1], strlen(argv[1]));
    XFlush(dpy);
    sleep(timeout);
    XClearWindow(dpy, w);
}
