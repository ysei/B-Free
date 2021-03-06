/*
   FoRTh Modoki
   fgraph.c ver 1.3.2 B-Free 1.7.1
   Copyright (C) 1999, 2000 Tomohide Naniwa. All rights reserved.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Last modified: Feb. 20, 2000.
*/

#include "frtm.h"
#ifndef BFREE
#include <X11/Xlib.h>
#else
#include <graphic.h>
#endif

#define MAXCOL 16

static int gflag = 0;
#ifndef BFREE
static Display *d;
static Window w0;
static int palette[][MAXCOL] = {
  {0, 0,   0,  0, 42, 42, 42, 42, 21, 21, 21, 21, 63, 63, 63, 63},
  {0, 0,  42, 42,  0,  0, 42, 42, 21, 21, 63, 63, 21, 21, 63, 63},
  {0, 42, 0,  42,  0,  42, 0, 42, 21, 63, 21, 63, 21, 63, 21, 63}
};
static struct {
  Font f;
  int asc, lbe;
} font_dat[2];
static GC gc[MAXCOL];
static long pxl[MAXCOL];
#endif
   
static int cx = 0, cy = 0, color = 15, bgcolor = 0, font = 0;

void f_gm()
{
#ifdef BFREE
  color = 15;
  bgcolor = 0;
  font = 0;
  if (gflag) return;
  graphic_mode(1);
  gflag = 1;
#else
  unsigned long black, white;
  XEvent e;
  Colormap cmap;
  XColor c1;
  Font f;
  XFontStruct *fs;
  int i;
  int asc, lbe;

  color = 15;
  bgcolor = 0;
  font = 0;
  if (gflag) return;
  if ((d = XOpenDisplay(NULL)) == NULL) return;
  gflag = 1;
  black = BlackPixel(d, 0);
  white = WhitePixel(d, 0);
  f = XLoadFont(d, "a14");
  fs = XQueryFont(d, f);
  asc = fs->max_bounds.ascent;
  lbe = fs->max_bounds.lbearing;
  font_dat[0].f = f;
  font_dat[0].asc = asc;
  font_dat[0].lbe = lbe;
  f = XLoadFont(d, "7x14bold");
  fs = XQueryFont(d, f);
  asc = fs->max_bounds.ascent;
  lbe = fs->max_bounds.lbearing;
  font_dat[1].f = f;
  font_dat[1].asc = asc;
  font_dat[1].lbe = lbe;
  w0 = XCreateSimpleWindow(d, RootWindow(d, 0),
			   100, 100, 640, 480, 1, white, black);
/* initialize GC for paint */
  for(i = 0; i < MAXCOL; ++i) {
    gc[i] = XCreateGC(d, w0, 0, 0);
    XSetForeground(d, gc[i], white);
    XSetBackground(d, gc[i], black);
  }
  cmap = DefaultColormap(d, 0);
  for(i = 0; i < MAXCOL; ++i) {
    c1.red = (palette[0][i]+1L)*1024-1;
    c1.green = (palette[1][i]+1L)*1024-1;
    c1.blue = (palette[2][i]+1L)*1024-1;
    XAllocColor(d, cmap, &c1);
    XSetFillStyle(d, gc[i], FillOpaqueStippled);
    XSetForeground(d, gc[i], c1.pixel);
    pxl[i] = c1.pixel;
  }

  XSelectInput(d, w0, ExposureMask);
  XMapWindow(d, w0);
  XFlush(d);

  while(1) {
    XNextEvent(d, &e);
    if (e.type == Expose && e.xexpose.count == 0) break;
  }
  XSelectInput(d, w0, NoEventMask);
  XFlush(d);
#endif
}

void f_tm()
{
#ifdef BFREE
  text_mode(1);
  gflag = 0;
#else
  if (! gflag) return;
  XCloseDisplay(d);
#endif
}

void f_cls()
{
#ifdef BFREE
  fillbox_vga(1, 0, 0, 640, 480, 0);
#else
  if (! gflag) return;
  XClearWindow(d, w0);
  XFlush(d);
#endif
}

void f_moveto()
{
  int x, y;
  y = pop(); EC;
  x = pop(); EC;
  cx = x; cy = y;
}

void f_lineto()
{
  int x, y;
  y = pop(); EC;
  x = pop(); EC;
#ifdef BFREE
  line_vga(1, cx, cy, x, y, color, 0);
#else
  if (! gflag) return;
  XDrawLine(d, w0, gc[color], cx, cy, x, y);
  XFlush(d);
#endif
  cx = x; cy = y;
}

void f_rmoveto()
{
  int x, y;
  y = pop(); EC;
  x = pop(); EC;
  cx += x; cy += y;
}

void f_rlineto()
{
  int x, y;
  y = pop(); EC;
  x = pop(); EC;
#ifdef BFREE
  line_vga(1, cx, cy, cx+x, cy+y, color, 0);
#else
  if (! gflag) return;
  XDrawLine(d, w0, gc[color], cx, cy, cx+x, cy+y);
  XFlush(d);
#endif
  cx += x; cy += y;
}

void f_rect()
{
  int x, y, w, h;
  h = pop(); EC;
  w = pop(); EC;
  y = pop(); EC;
  x = pop(); EC;
#ifdef BFREE
  box_vga(1, x, y, w, h, color);
#else
  if (! gflag) return;
  XDrawRectangle(d, w0, gc[color], x, y, w, h);
  XFlush(d);
#endif
}

void f_rectf()
{
  int x, y, w, h;
  h = pop(); EC;
  w = pop(); EC;
  y = pop(); EC;
  x = pop(); EC;
#ifdef BFREE
  fillbox_vga(1, x, y, w, h, color);
#else
  if (! gflag) return;
  XFillRectangle(d, w0, gc[color], x, y, w, h);
  XFlush(d);
#endif
}

void f_point()
{
  int x, y;
  y = pop(); EC;
  x = pop(); EC;
#ifdef BFREE
  point_vga(1, x, y, color, 0);
#else
  if (! gflag) return;
  XDrawPoint(d, w0, gc[color], x, y);
  XFlush(d);
#endif
}

void f_ellips()
{
  int x, y, w, h;
  h = pop(); EC;
  w = pop(); EC;
  y = pop(); EC;
  x = pop(); EC;
#ifdef BFREE
  ellipse_vga(1, x, y, w, h, color);
#else
  if (! gflag) return;
  XDrawArc(d, w0, gc[color], x, y, w, h, 0, 360*64);
  XFlush(d);
#endif
}

void f_ellipsf()
{
  int x, y, w, h;
  h = pop(); EC;
  w = pop(); EC;
  y = pop(); EC;
  x = pop(); EC;
#ifdef BFREE
  fillellipse_vga(1, x, y, w, h, color);
#else
  if (! gflag) return;
  XFillArc(d, w0, gc[color], x, y, w, h, 0, 360*64);
  XFlush(d);
#endif
}

void f_setcolor()
{
  int c;
  c = pop(); EC;
  if (c >= 0) color = c % MAXCOL;
}

void f_setbgc()
{
  int c;
  c = pop(); EC;
  if (c >= 0) bgcolor = c % MAXCOL;
#ifndef BFREE
  for(c = 0; c < MAXCOL; ++c) {
    XSetBackground(d, gc[c], pxl[bgcolor]);
  }
#endif
}

void f_setfont()
{
  int c;
  c = pop(); EC;
  if (c >= 0) font = c % 2;
}

#ifdef BFREE
/*
   This xbm font data is made from a14(7x14.pcf) font of X11.
*/

static unsigned char a14xbm[][14] = {
  {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00},
  {
   0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08, 0x08,
   0x00, 0x00},
  {
   0x00, 0x36, 0x24, 0x24, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00},
  {
   0x00, 0x00, 0x50, 0x50, 0x50, 0x7e, 0x28, 0x28, 0x7e, 0x14, 0x14, 0x14,
   0x00, 0x00},
  {
   0x00, 0x00, 0x10, 0x3c, 0x52, 0x52, 0x14, 0x38, 0x50, 0x52, 0x52, 0x3c,
   0x10, 0x00},
  {
   0x00, 0x00, 0x4c, 0x52, 0x32, 0x1c, 0x10, 0x08, 0x38, 0x4c, 0x4a, 0x32,
   0x00, 0x00},
  {
   0x00, 0x00, 0x18, 0x24, 0x24, 0x24, 0x18, 0x4c, 0x52, 0x22, 0x32, 0x4c,
   0x00, 0x00},
  {
   0x00, 0x18, 0x10, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00},
  {
   0x00, 0x40, 0x20, 0x10, 0x10, 0x08, 0x08, 0x08, 0x08, 0x08, 0x10, 0x10,
   0x20, 0x40},
  {
   0x00, 0x02, 0x04, 0x08, 0x08, 0x10, 0x10, 0x10, 0x10, 0x10, 0x08, 0x08,
   0x04, 0x02},
  {
   0x00, 0x00, 0x10, 0x54, 0x38, 0x10, 0x38, 0x54, 0x10, 0x00, 0x00, 0x00,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x08, 0x00,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x08,
   0x08, 0x04},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38,
   0x10, 0x00},
  {
   0x00, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04,
   0x02, 0x02},
  {
   0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x24, 0x18,
   0x00, 0x00},
  {
   0x00, 0x00, 0x10, 0x18, 0x14, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x42, 0x42, 0x40, 0x20, 0x20, 0x10, 0x08, 0x04, 0x7e,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x42, 0x42, 0x40, 0x38, 0x40, 0x40, 0x42, 0x42, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x20, 0x30, 0x28, 0x28, 0x24, 0x24, 0x22, 0x7e, 0x20, 0x20,
   0x00, 0x00},
  {
   0x00, 0x00, 0x7e, 0x02, 0x02, 0x3e, 0x42, 0x40, 0x40, 0x42, 0x42, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x38, 0x44, 0x42, 0x02, 0x3a, 0x46, 0x42, 0x42, 0x42, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x7e, 0x42, 0x22, 0x20, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x42, 0x42, 0x24, 0x18, 0x24, 0x42, 0x42, 0x42, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x62, 0x5c, 0x40, 0x42, 0x22, 0x1c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x10, 0x00, 0x00, 0x10, 0x38, 0x10,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x10, 0x10,
   0x08, 0x00},
  {
   0x00, 0x00, 0x00, 0x20, 0x10, 0x08, 0x04, 0x02, 0x04, 0x08, 0x10, 0x20,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x02, 0x04, 0x08, 0x10, 0x20, 0x10, 0x08, 0x04, 0x02,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x42, 0x42, 0x20, 0x10, 0x10, 0x10, 0x00, 0x10, 0x10,
   0x00, 0x00},
  {
   0x00, 0x00, 0x38, 0x44, 0x72, 0x4a, 0x4a, 0x4a, 0x4a, 0x72, 0x04, 0x78,
   0x00, 0x00},
  {
   0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x42, 0x7e, 0x42, 0x42, 0x42, 0x42,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3e, 0x44, 0x44, 0x44, 0x3c, 0x44, 0x44, 0x44, 0x44, 0x3e,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x42, 0x42, 0x02, 0x02, 0x02, 0x02, 0x42, 0x42, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3e, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x3e,
   0x00, 0x00},
  {
   0x00, 0x00, 0x7e, 0x02, 0x02, 0x02, 0x1e, 0x02, 0x02, 0x02, 0x02, 0x7e,
   0x00, 0x00},
  {
   0x00, 0x00, 0x7e, 0x02, 0x02, 0x02, 0x1e, 0x02, 0x02, 0x02, 0x02, 0x02,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x42, 0x42, 0x02, 0x02, 0x72, 0x42, 0x42, 0x62, 0x5c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x7e, 0x42, 0x42, 0x42, 0x42, 0x42,
   0x00, 0x00},
  {
   0x00, 0x00, 0x7c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x70, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x22, 0x1c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x42, 0x22, 0x12, 0x0a, 0x06, 0x0a, 0x12, 0x22, 0x42, 0x42,
   0x00, 0x00},
  {
   0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x7e,
   0x00, 0x00},
  {
   0x00, 0x00, 0x42, 0x66, 0x66, 0x5a, 0x5a, 0x42, 0x42, 0x42, 0x42, 0x42,
   0x00, 0x00},
  {
   0x00, 0x00, 0x42, 0x42, 0x46, 0x46, 0x4a, 0x52, 0x62, 0x62, 0x42, 0x42,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3e, 0x42, 0x42, 0x42, 0x42, 0x3e, 0x02, 0x02, 0x02, 0x02,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x4e, 0x52, 0x62, 0x3c,
   0x20, 0x40},
  {
   0x00, 0x00, 0x3e, 0x42, 0x42, 0x42, 0x42, 0x3e, 0x12, 0x22, 0x42, 0x42,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x42, 0x42, 0x02, 0x0c, 0x30, 0x40, 0x42, 0x42, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x7f, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
   0x00, 0x00},
  {
   0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x24, 0x24, 0x24, 0x18, 0x18, 0x18,
   0x00, 0x00},
  {
   0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x5a, 0x5a, 0x66, 0x66, 0x42,
   0x00, 0x00},
  {
   0x00, 0x00, 0x42, 0x42, 0x24, 0x24, 0x18, 0x18, 0x24, 0x24, 0x42, 0x42,
   0x00, 0x00},
  {
   0x00, 0x00, 0x22, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x08, 0x08, 0x08,
   0x00, 0x00},
  {
   0x00, 0x00, 0x7e, 0x40, 0x20, 0x10, 0x10, 0x08, 0x04, 0x04, 0x02, 0x7e,
   0x00, 0x00},
  {
   0x00, 0x78, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
   0x08, 0x78},
  {
   0x00, 0x02, 0x02, 0x04, 0x04, 0x08, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20,
   0x40, 0x40},
  {
   0x00, 0x1e, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
   0x10, 0x1e},
  {
   0x00, 0x18, 0x24, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x7e},
  {
   0x00, 0x30, 0x10, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x70, 0x4c, 0x42, 0x62, 0x5c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x02, 0x02, 0x02, 0x3a, 0x46, 0x42, 0x42, 0x42, 0x46, 0x3a,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x02, 0x02, 0x02, 0x42, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x40, 0x40, 0x40, 0x5c, 0x62, 0x42, 0x42, 0x42, 0x62, 0x5c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x7e, 0x02, 0x42, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x30, 0x48, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x5c, 0x22, 0x22, 0x22, 0x1c, 0x04, 0x3a,
   0x42, 0x3c},
  {
   0x00, 0x00, 0x02, 0x02, 0x02, 0x3a, 0x46, 0x42, 0x42, 0x42, 0x42, 0x42,
   0x00, 0x00},
  {
   0x00, 0x00, 0x10, 0x10, 0x00, 0x18, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x20, 0x20, 0x00, 0x30, 0x20, 0x20, 0x20, 0x20, 0x20, 0x22,
   0x22, 0x1c},
  {
   0x00, 0x00, 0x02, 0x02, 0x02, 0x22, 0x12, 0x0a, 0x0e, 0x12, 0x22, 0x42,
   0x00, 0x00},
  {
   0x00, 0x00, 0x18, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x22,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 0x46, 0x42, 0x42, 0x42, 0x42, 0x42,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 0x46, 0x42, 0x42, 0x42, 0x46, 0x3a,
   0x02, 0x02},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x5c, 0x62, 0x42, 0x42, 0x42, 0x62, 0x5c,
   0x40, 0x40},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 0x46, 0x42, 0x02, 0x02, 0x02, 0x02,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x04, 0x18, 0x20, 0x42, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x08, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x48, 0x30,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x62, 0x5c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x2a, 0x2a, 0x2a, 0x2a, 0x14,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x24, 0x18, 0x24, 0x42, 0x42,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x62, 0x5c, 0x40,
   0x42, 0x3c},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x20, 0x10, 0x08, 0x08, 0x04, 0x7e,
   0x00, 0x00},
  {
   0x00, 0x60, 0x10, 0x10, 0x10, 0x10, 0x10, 0x08, 0x10, 0x10, 0x10, 0x10,
   0x10, 0x60},
  {
   0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
   0x08, 0x08},
  {
   0x00, 0x06, 0x08, 0x08, 0x08, 0x08, 0x08, 0x10, 0x08, 0x08, 0x08, 0x08,
   0x08, 0x06},
  {
   0x00, 0x04, 0x4a, 0x52, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00}
};

/*
   This xbm font data is made from 7x14bold(7x14B.pcf) font of X11.
*/

static unsigned char a14Bxbm[][14] = {
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00},
  {
   0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18,
   0x00, 0x00},
  {
   0x7e, 0x6c, 0x6c, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00},
  {
   0x00, 0x00, 0x78, 0x78, 0x78, 0x7e, 0x3c, 0x3c, 0x7e, 0x1e, 0x1e, 0x1e,
   0x00, 0x00},
  {
   0x00, 0x00, 0x18, 0x3c, 0x5a, 0x5a, 0x1c, 0x38, 0x58, 0x5a, 0x5a, 0x3c,
   0x18, 0x00},
  {
   0x00, 0x00, 0x6c, 0x76, 0x36, 0x1c, 0x10, 0x08, 0x38, 0x6c, 0x6e, 0x36,
   0x00, 0x00},
  {
   0x00, 0x00, 0x38, 0x6c, 0x6c, 0x6c, 0x38, 0x6c, 0x76, 0x36, 0x36, 0x6c,
   0x00, 0x00},
  {
   0x1c, 0x18, 0x18, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00},
  {
   0x00, 0x60, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x18, 0x18,
   0x30, 0x60},
  {
   0x00, 0x06, 0x0c, 0x18, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x18, 0x18,
   0x0c, 0x06},
  {
   0x00, 0x00, 0x5a, 0x5a, 0x3c, 0x18, 0x3c, 0x5a, 0x5a, 0x00, 0x00, 0x00,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x18, 0x00,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x18,
   0x18, 0x0c},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18,
   0x00, 0x00},
  {
   0x00, 0x60, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x18, 0x0c, 0x0c, 0x06,
   0x06, 0x06},
  {
   0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x18, 0x1c, 0x1e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x66, 0x66, 0x60, 0x30, 0x30, 0x18, 0x0c, 0x06, 0x7e,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x66, 0x66, 0x60, 0x38, 0x60, 0x60, 0x66, 0x66, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x20, 0x30, 0x30, 0x38, 0x3c, 0x34, 0x36, 0x7e, 0x20, 0x20,
   0x00, 0x00},
  {
   0x00, 0x00, 0x7e, 0x06, 0x06, 0x3e, 0x66, 0x60, 0x60, 0x66, 0x66, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x38, 0x6c, 0x66, 0x06, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x7e, 0x66, 0x36, 0x30, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x66, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0x66, 0x66, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x66, 0x36, 0x1c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x1c, 0x18, 0x18,
   0x0c, 0x00},
  {
   0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x0c, 0x18, 0x30, 0x60,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x00, 0x7e, 0x7e, 0x00, 0x00, 0x00,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0c, 0x06,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x66, 0x66, 0x30, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18,
   0x00, 0x00},
  {
   0x00, 0x00, 0x38, 0x6c, 0x76, 0x5e, 0x5e, 0x5e, 0x5e, 0x76, 0x0c, 0x78,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3e, 0x66, 0x66, 0x26, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x3e,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x66, 0x66, 0x06, 0x06, 0x06, 0x06, 0x66, 0x66, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x1e, 0x36, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x36, 0x1e,
   0x00, 0x00},
  {
   0x00, 0x00, 0x7e, 0x06, 0x06, 0x06, 0x3e, 0x06, 0x06, 0x06, 0x06, 0x7e,
   0x00, 0x00},
  {
   0x00, 0x00, 0x7e, 0x06, 0x06, 0x06, 0x3e, 0x06, 0x06, 0x06, 0x06, 0x06,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x66, 0x66, 0x06, 0x06, 0x76, 0x66, 0x66, 0x66, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x66,
   0x00, 0x00},
  {
   0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e,
   0x00, 0x00},
  {
   0x00, 0x00, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x66, 0x36, 0x1c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x66, 0x36, 0x1e, 0x0e, 0x0e, 0x1e, 0x1e, 0x36, 0x66, 0x66,
   0x00, 0x00},
  {
   0x00, 0x00, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x7e,
   0x00, 0x00},
  {
   0x00, 0x00, 0x42, 0x66, 0x66, 0x7e, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x66,
   0x00, 0x00},
  {
   0x00, 0x00, 0x66, 0x66, 0x6e, 0x6e, 0x6e, 0x76, 0x76, 0x76, 0x66, 0x66,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x06, 0x06, 0x06, 0x06,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x6e, 0x76, 0x66, 0x3c,
   0x60, 0x40},
  {
   0x00, 0x00, 0x3e, 0x66, 0x66, 0x66, 0x3e, 0x36, 0x66, 0x66, 0x66, 0x66,
   0x00, 0x00},
  {
   0x00, 0x00, 0x3c, 0x66, 0x66, 0x0c, 0x18, 0x18, 0x30, 0x66, 0x66, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
   0x00, 0x00},
  {
   0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x3c, 0x18,
   0x00, 0x00},
  {
   0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7e, 0x7e, 0x7e, 0x7e, 0x24,
   0x00, 0x00},
  {
   0x00, 0x00, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x18, 0x3c, 0x3c, 0x66, 0x66,
   0x00, 0x00},
  {
   0x00, 0x00, 0x66, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x18,
   0x00, 0x00},
  {
   0x00, 0x00, 0x7e, 0x60, 0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x06, 0x7e,
   0x00, 0x00},
  {
   0x00, 0x7c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
   0x0c, 0x7c},
  {
   0x00, 0x06, 0x06, 0x06, 0x0c, 0x0c, 0x18, 0x18, 0x18, 0x30, 0x30, 0x60,
   0x60, 0x60},
  {
   0x00, 0x3e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
   0x30, 0x3e},
  {
   0x18, 0x3c, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x7e, 0x7e},
  {
   0x38, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x70, 0x6c, 0x66, 0x66, 0x7c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x06, 0x06, 0x06, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3e,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x06, 0x06, 0x06, 0x66, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x60, 0x60, 0x60, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x66, 0x7e, 0x06, 0x66, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x30, 0x78, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x5c, 0x76, 0x36, 0x36, 0x1c, 0x04, 0x3e,
   0x66, 0x3c},
  {
   0x00, 0x00, 0x06, 0x06, 0x06, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
   0x00, 0x00},
  {
   0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
   0x00, 0x00},
  {
   0x00, 0x00, 0x30, 0x30, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
   0x36, 0x1c},
  {
   0x00, 0x00, 0x06, 0x06, 0x06, 0x26, 0x36, 0x1e, 0x1e, 0x36, 0x66, 0x46,
   0x00, 0x00},
  {
   0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3e,
   0x06, 0x06},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c,
   0x60, 0x60},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x66, 0x66, 0x06, 0x06, 0x06, 0x06,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x0c, 0x18, 0x30, 0x66, 0x3c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x18, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x70,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x18,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x7e, 0x7e, 0x7e, 0x7e, 0x24,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0x66,
   0x00, 0x00},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x6c, 0x6c, 0x38, 0x38, 0x30,
   0x36, 0x1c},
  {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x7e,
   0x00, 0x00},
  {
   0x00, 0x70, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0c, 0x18, 0x18, 0x18, 0x18,
   0x18, 0x70},
  {
   0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
   0x18, 0x18},
  {
   0x00, 0x0e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x30, 0x18, 0x18, 0x18, 0x18,
   0x18, 0x0e},
  {
   0x00, 0x04, 0x7e, 0x7e, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00}
};
#endif

void f_drawstring()
{
  int x, y, i, len;
  static char str[LINE];

  pop_string(str); EC;
  y = pop(); EC;
  x = pop(); EC;
  len = strlen(str);
#ifdef BFREE
  for(i = 0; i < len; ++i) {
    int j;
    j = str[i];
    if ((j < 0x20) || (j > 0x7e)) j = 0x20;
    switch(font) {
    case 0:
      spl_bitmap(1, x, y, 7, 14, 14, a14xbm[j-0x20], color);
      break;
    case 1:
      spl_bitmap(1, x, y, 7, 14, 14, a14Bxbm[j-0x20], color);
      break;
    }
    x += 7;
  }
#else
  if (! gflag) return;
  XSetFont(d, gc[color], font_dat[font].f);
  XDrawString(d, w0, gc[color],
	      x+font_dat[font].lbe, y+font_dat[font].asc, str, len);
  XFlush(d);
#endif
}

void f_drawistring()
{
  int x, y, i, len;
  static char str[LINE];

  pop_string(str); EC;
  y = pop(); EC;
  x = pop(); EC;
  len = strlen(str);
#ifdef BFREE
  for(i = 0; i < len; ++i) {
    int j;
    j = str[i];
    if ((j < 0x20) || (j > 0x7e)) j = 0x20;
    switch(font) {
    case 0:
      set_bitmap(1, x, y, 7, 14, 14, a14xbm[j-0x20], color, bgcolor);
      break;
    case 1:
      set_bitmap(1, x, y, 7, 14, 14, a14Bxbm[j-0x20], color, bgcolor);
      break;
    }
    x += 7;
  }
#else
  if (! gflag) return;
  XSetFont(d, gc[color], font_dat[font].f);
  XDrawImageString(d, w0, gc[color],
		   x+font_dat[font].lbe, y+font_dat[font].asc, str, len);
  XFlush(d);
#endif
}

#ifdef BFREE
#ifdef notdef
static unsigned char ktest[] = {
   0x08, 0x01, 0x10, 0x01, 0xd0, 0x0f, 0x42, 0x08, 0x44, 0x08, 0xc4, 0x0f,
   0x40, 0x08, 0xc8, 0x0f, 0x48, 0x11, 0x44, 0x12, 0x44, 0x0a, 0x42, 0x04,
   0xc2, 0x09, 0x32, 0x30};

void f_ktest()
{
  set_bitmap(1, 100, 100, 14, 14, sizeof(ktest), ktest, color, bgcolor);
}
#endif

#define MAGIC "BFB"
static int k_fd = -1;
static struct font_header {
  char magic[4];
  short fnt_size;   /* bytes per one character bit patern */
  short fnt_height; /* font height */
  short fnt_width;   /* font width */
  unsigned short top_code;
  unsigned short end_code;
} fheader;
static int has_kfont = 0;

void f_kfopen()
{
  static char name[LINE];

  pop_string(name); EC;
  if ((k_fd = open(name, 4, 0)) < 0) {
    printf("cant open file %s\n", name);
    return;
  }
  read(k_fd, &fheader, sizeof(struct font_header));
  if (strcmp(fheader.magic, MAGIC) != 0) {
    printf("opened file is not %s font", MAGIC);
    close(k_fd);
    return;
  }
  printf("size = %d top_code = %x end_code = %x\n",
	 fheader.fnt_size, fheader.top_code, fheader.end_code);
  has_kfont = 1;
}

void f_kfclose()
{
  if (! has_kfont) return;
  close(k_fd);
  k_fd = -1;
  has_kfont = 0;
}

static int cal_offset(code)
     int code;
{
  int k1, k2;
  if (code > 256) {
    k2 = code & 0xFF;
    k1 = (code >> 8) & 0xFF;
    if (k1 < 0x29) {
      return((k1 - 0x21)*94 + k2-0x21);
    }
    else {
      return((k1 - 0x30)*94 + k2-0x21) + 752;
    }
  }
  else {
    return(code);
  }
}

void f_drawkch()
{
  int x, y, code, oft;
  char buf[255];
  
  code = pop(); EC;
  y = pop(); EC;
  x = pop(); EC;
  
  if (! has_kfont) return;
  if (code < fheader.top_code || code > fheader.end_code) return;
  oft = cal_offset(code);
  lseek(k_fd, 1024 + oft*fheader.fnt_size, 0);
  read(k_fd, buf, fheader.fnt_size);
  set_bitmap(1, x, y, fheader.fnt_width, fheader.fnt_height, fheader.fnt_size,
	     buf, color, bgcolor);
}

void f_winmod()
{
  int mode;

  mode = pop(); EC;
  winmod(1, mode);
}
#endif
