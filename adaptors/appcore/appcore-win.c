/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>


static Atom a_pid;
static Atom a_active_win;

static pid_t GetWinPid(Display *d, Window win)
{
  int r;
  pid_t pid;

  Atom a_type;
  int format;
  unsigned long nitems;
  unsigned long bytes_after;
  unsigned char *prop_ret;
  XWindowAttributes attr;

  if( d == NULL || !a_pid )
  {
    return -1;
  }

  if( !XGetWindowAttributes(d, win, &attr) )
  {
    return -1;
  }

  if( attr.override_redirect || attr.class == InputOnly )
  {
    return -1;
  }

  prop_ret = NULL;
  r = XGetWindowProperty(d, win, a_pid, 0, 1, False, XA_CARDINAL,
                         &a_type, &format, &nitems, &bytes_after,
                         &prop_ret);
  if( r != Success || prop_ret == NULL )
  {
    return -1;
  }

  if( a_type == XA_CARDINAL && format == 32 )
  {
    pid = *(unsigned long *)prop_ret;
  }
  else
  {
    pid = -1;
  }

  XFree(prop_ret);

  return pid;
}

static int FindWin(Display *d, Window *win, pid_t pid)
{
  int r;
  pid_t p;
  unsigned int n;
  Window root, parent, *child;

  p = GetWinPid(d, *win);
  if( p == pid )
    return 1;

  r = XQueryTree(d, *win, &root, &parent, &child, &n);
  if( r )
  {
    int i;
    int found = 0;

    for (i = 0; i < n; i++)
    {
      found = FindWin(d, &child[i], pid);
      if( found )
      {
        *win = child[i];
        break;
      }
    }
    XFree(child);

    if( found )
    {
      return 1;
    }
  }
  return 0;
}

static int RaiseWin(Display *d, Window win)
{
  XEvent xev;
  Window root;

  if( !a_active_win )
  {
    a_active_win = XInternAtom(d, "_NET_ACTIVE_WINDOW", False);
  }

  root = XDefaultRootWindow(d);

  xev.xclient.type = ClientMessage;
  xev.xclient.display = d;
  xev.xclient.window = win;
  xev.xclient.message_type = a_active_win;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = 1;
  xev.xclient.data.l[1] = CurrentTime;
  xev.xclient.data.l[2] = 0;
  xev.xclient.data.l[3] = 0;
  xev.xclient.data.l[4] = 0;
  XSendEvent(d, root, False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);

  return 0;
}

int AppcoreRaiseWin(pid_t pid)
{

  int r;
  int found;
  Display *d;
  Window win;

  if( pid < 1 )
  {
    errno = EINVAL;
    return -1;
  }

  r = kill(pid, 0);
  if( r == -1 )
  {
    errno = ESRCH;  /* No such process */
    return -1;
  }

  d = XOpenDisplay(NULL);
  if(d == NULL)
  {
    return -1;
  }

  win = XDefaultRootWindow(d);

  if( !a_pid )
  {
    a_pid = XInternAtom(d, "_NET_WM_PID", True);
  }

  found = FindWin(d, &win, pid);
  if( !found )
  {
    XCloseDisplay(d);
    errno = ENOENT;
    return -1;
  }

  r = RaiseWin(d, win);

  XCloseDisplay(d);

  return r;
}
