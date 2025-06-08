#include <X11/X.h>
#include <X11/Xlib.h>
#define LF_X11
#define LF_RUNARA
#include <leif/color.h>
#include <leif/ez_api.h>
#include <leif/layout.h>
#include <leif/widget.h>
#include <leif/win.h>
#include <podvig/podvig.h>

#include <X11/extensions/XInput2.h>
void searchbar(lf_ui_state_t* ui) {
  lf_div(ui);
  lf_div(ui);
  lf_widget_set_layout(lf_crnt(ui), LF_LAYOUT_HORIZONTAL);
  lf_style_widget_prop_color(ui, lf_crnt(ui), color, lf_color_from_hex(0x333333));
  lf_widget_set_fixed_width_percent(ui, lf_crnt(ui), 100);

  lf_style_widget_prop(ui, lf_crnt(ui), corner_radius_percent, 25);
  lf_widget_set_padding(ui, lf_crnt(ui), 15);
  lf_div_end(ui);
  lf_div_end(ui);
}

pv_widget_t* widget;

int xi_opcode;
void windowingcb(void* xev, lf_ui_state_t* ui) {
  XEvent ev = *(XEvent*)xev;
  Display* dpy = lf_win_get_x11_display();
  if (ev.xcookie.type == GenericEvent && ev.xcookie.extension == 
    xi_opcode &&
    XGetEventData(dpy, &ev.xcookie)) {

    XIRawEvent *rawev = (XIRawEvent *)ev.xcookie.data;

    if (rawev->evtype == XI_RawKeyPress) {
      int keycode = rawev->detail;  
      // On most systems, Escape is keycode 9
      if (keycode == 9) {
        widget->ui->running = false;
      }
    }

    XFreeEventData(dpy, &ev.xcookie);
  }
}
int main(void) {
  pv_state_t* s = pv_init();

  lf_container_t mon = pv_monitor_by_idx(s, pv_monitor_focused_idx(s));

  widget = pv_widget(
    s, 
    "nari", searchbar,
    (mon.size.x - 400) / 2.0f,
    (mon.size.y - 400) / 2.0f,
    400, 400
  );

  Display *dpy = lf_win_get_x11_display(); 
  int event, error;
  if (!XQueryExtension(dpy, "XInputExtension", &xi_opcode, &event, &error)) {
    fprintf(stderr, "XInput2 not available\n");
    return 1;
  }

  Window root = DefaultRootWindow(dpy);

  XIEventMask evmask;
  unsigned char mask[(XI_LASTEVENT + 7)/8] = {0};
  evmask.deviceid = XIAllDevices;
  evmask.mask_len = sizeof(mask);
  evmask.mask = mask;
  XISetMask(mask, XI_RawKeyPress);

  XISelectEvents(dpy, root, &evmask, 1);
  XFlush(dpy);
  XSetInputFocus(dpy, widget->ui->win, RevertToPointerRoot, CurrentTime);

  lf_win_set_event_cb(widget->ui->win, windowingcb);

  pv_run(s);
}
