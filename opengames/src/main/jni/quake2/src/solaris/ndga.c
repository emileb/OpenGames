#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stropts.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>

#include "nvsession.h"

#define NDGA_IMPLEMENTATION	1
#include "NRegion.h"
#include "ndga.h"

/*
 * newt equivalent of DGA
 *
 * Written by: Jim Hanko
 */

#define NS_PER_SEC	(1000LL * 1000LL * 1000LL)

/* #define DEBUG */

#ifdef DEBUG
#define DBG(x)	x
#else
#define DBG(x)
#endif

int
ndga_get_eventmask(void)
{
	return (VisibilityChangeMask | ExposureMask | StructureNotifyMask);
}

void
ndga_destroy(struct ndga_state * sp)
{
	if (sp) {
		if (sp->session && sp->sent_clip) {
			sp->sent_clip = 0;
			nvSessionClipRegion(sp->session, NULL);
		}
		NDestroyRegion(sp->region);
		NDestroyRegion(sp->clip);
		NDestroyRegion(sp->lastclip);
		NDestroyRegion(sp->screenArea);
		free(sp);
	}
}

struct ndga_state *
ndga_new(Display *display, Window window, nvSession session, 
	int xalign, int yalign)
{
	struct ndga_state *sp;
	Window	       *children;
	int		nchildren;
	XWindowAttributes attr;
	int		retval;

	sp = (struct ndga_state *) malloc(sizeof(*sp));
	if (sp == NULL)
		return (NULL);
	memset(sp, 0, sizeof(*sp));

	sp->display = display;
	sp->window = window;

	sp->xalign = xalign;
	sp->yalign = yalign;

	sp->is_visible = 1;
	sp->is_unobscured = 1;
	sp->is_open = 1;
	sp->was_open = 1;
	sp->was_unobscured = 0;
	sp->is_changed = 1;
	sp->new_clip = 1;
	sp->busy = 0;

	sp->clipEvent = 1;

	sp->session = session;

	XQueryTree(sp->display, sp->window, &sp->root, &sp->parent,
	    &children, &nchildren);

	retval = XGetWindowAttributes(sp->display, sp->root, &attr);
	if (retval == 0) {
		fprintf(stderr, "Can't get window attributes\n");
		free(sp);
		return (NULL);
	}
	sp->region = NCreateRegion();
	sp->clip = NCreateRegion();
	sp->lastclip = NCreateRegion();

	sp->screenArea = NCreateRegion();

	sp->screenRect.x = attr.x;
	sp->screenRect.y = attr.y;
	sp->screenRect.width = attr.width;
	sp->screenRect.height = attr.height;
	DBG(fprintf(stderr, "screen %d,%d %dx%d\n", 
		sp->screenRect.x, sp->screenRect.y,
		sp->screenRect.width, sp->screenRect.height));
	NUnionRectWithRegion(&sp->screenRect, sp->screenArea, sp->screenArea);

	return (sp);
}

static void
get_win_info(struct ndga_state * sp)
{
	int		depth;
	int		xr, yr, wr, hr, border;
	Window		w, root, parent, *children;
	int		nchildren;
	int		x, y;
	XWindowAttributes attr;
	int		retval;


	DBG(fprintf(stderr, "get_win_info()\n"));
	retval = XGetWindowAttributes(sp->display, sp->window, &attr);
	if (retval == 0) {
		fprintf(stderr, "Can't get window attributes\n");
		exit(1);
	}

	sp->is_open = (attr.map_state == IsViewable);
	DBG(fprintf(stderr, "ismapped %d\n", sp->is_open));

	x = attr.x + attr.border_width;
	y = attr.y + attr.border_width;
	sp->w = attr.width;
	sp->h = attr.height;
	if (sp->xalign > 1)
		sp->w -= (sp->w % sp->xalign);
	if (sp->yalign > 1)
		sp->h -= (sp->h % sp->yalign);

	w = sp->parent;
	while (w && w != sp->root) {
		XGetGeometry(sp->display, w, &root, &xr, &yr, &wr, &hr,
		    &border, &depth);
		x += xr + border;
		y += yr + border;
		XQueryTree(sp->display, w, &root, &w, &children, &nchildren);
	}

	sp->x = x;
	sp->y = y;

	sp->visRect.x = sp->x;
	sp->visRect.y = sp->y;
	sp->visRect.width = sp->w;
	sp->visRect.height = sp->h;

	DBG(fprintf(stderr, "X %d Y %d W %d H %d\n", sp->x, sp->y,
		sp->w, sp->h));
}

static void
rdump(void *a, int x, int y, int w, int h)
{
	struct ndga_state *s = (struct ndga_state *) a;
	fprintf(stderr, "%d,%d %dx%d\n", x, y, w, h);
}

static void
ndga_handle_change(struct ndga_state *sp)
{
	NRectangle	rectangle;

	get_win_info(sp);
	if (sp->is_open && sp->is_unobscured && !sp->was_unobscured) {
		rectangle.x = 0;
		rectangle.y = 0;
		rectangle.width = sp->w;
		rectangle.height = sp->h;
		NDestroyRegion(sp->region);
		sp->region = NCreateRegion();
		NUnionRectWithRegion(&rectangle, sp->region,
		    sp->region);

		DBG(fprintf(stderr, "Full clip\n"));
	} else if (!sp->is_open || !sp->is_visible) {
		NDestroyRegion(sp->region);
		sp->region = NCreateRegion();
	}

	NUnionRegion(sp->region, sp->region, sp->clip);
	NOffsetRegion(sp->clip, sp->x, sp->y);
	NIntersectRegion(sp->clip, sp->screenArea, sp->clip);
	if (sp->xalign > 1 || sp->yalign > 1) {
		NAlignRegion(sp->clip, sp->clip, sp->x, sp->xalign, sp->y, 
			sp->yalign);
	}

	if (sp->x < sp->screenRect.x || 
	    sp->y < sp->screenRect.y || 
	    sp->x + sp->w > sp->screenRect.x + sp->screenRect.width ||
	    sp->y + sp->h > sp->screenRect.y + sp->screenRect.height) {
		(void) NClipBox(sp->clip, &sp->visRect);
	} else {
		sp->visRect.x = sp->x;
		sp->visRect.y = sp->y;
		sp->visRect.width = sp->w;
		sp->visRect.height = sp->h;
	}

	sp->new_clip = 1;
	sp->clipEvent++;
	sp->was_unobscured = sp->is_unobscured;

	DBG(fprintf(stderr, "\n%s: %d - Clip \n", __FILE__, __LINE__));
	DBG(NWalkRegion(sp->clip, rdump, sp));
}

/*
 * Keep the session manager informed
 */
static void
update_clip(struct ndga_state *sp)
{
	if (!NEqualRegion(sp->clip, sp->lastclip)) {
		NUnionRegion(sp->clip, sp->clip, sp->lastclip);
		if (sp->session && sp->sent_clip) {
			nvSessionClipRegion(sp->session, sp->clip);
		}
	}
	sp->new_clip = 0;
}

#ifdef X_VISIBILITY_BUG

int fix_visibility = -1;

/*
 * The current X server will not report visibility events if the
 * window was partially obscured and it become more obscured. So,
 * we periodically poll to get an accurate clip list.
 *
 * Expected to be fixed by Solaris 2.8
 */
static void
ndga_workaround(struct ndga_state * sp)
{
	char *s;

	if (fix_visibility == -1) {
		if ((s = getenv("CORONA_VISIBILITY_FIX")) != NULL && 
		    (*s == 'T' || *s == 't' || *s == '1')) {
			fix_visibility = 1;
		} else {
			fix_visibility = 0;
		}
	}
	if (fix_visibility != 1)
		return;

	if (sp->is_visible && !sp->is_unobscured && sp->sent_clip) {
		hrtime_t now;
		static hrtime_t last;

		now = gethrtime();
		if (now - last > NS_PER_SEC) {
			Window		w;

			DBG(fprintf(stderr, "Poll for clip\n"));
			w = XCreateSimpleWindow(sp->display, sp->window,
			    0, 0, 30000, 30000, 0, 0, 0);
			XMapWindow(sp->display, w);
			XFlush(sp->display);
			XDestroyWindow(sp->display, w);
			XFlush(sp->display);
			/*
			 * No need for sync here
			 */
			sp->clearOnExpose = 1;

			last = gethrtime();
		}
	}
}
#endif /* X_VISIBILITY_BUG */

/*
 * attempt to start direct io; returns 1 if visible and sets the
 * window's location in xp, yp, wp, hp. is_obscured is set if all
 * parts of the window are visible. clipEvent contains an event
 * counter for changes in the clip list. When a caller observes
 * a change in the counter, it should obtain the new clip.
 *
 * Returns 0 if the window is not visible.
 */
int
ndga_start(struct ndga_state * sp,
    int *xp,			/* x */
    int *yp,			/* y */
    int *wp,			/* width */
    int *hp,			/* height */
    int *is_unobscured,		/* 1 if clipped, 0 if not */
    int *clipEvent		/* clip event count */)
{
	char	       *loc;

	if (!sp)
		return (0);

	if (sp->is_changed) {
		ndga_handle_change(sp);
		sp->is_changed = 0;
	}

	if (sp->new_clip) {
		update_clip(sp);
	}

	/*
	 * If fully visible or if partially visible and the client is
	 * expecting to handle clipping, allow it to go through
	 */
	if (sp->is_open && sp->is_visible && 
	    (sp->is_unobscured || 
	    (clipEvent != NULL && is_unobscured != NULL))) {
		if (sp->session && sp->sent_clip == 0) {
			nvSessionClipRegion(sp->session, sp->clip);
			sp->sent_clip = 1;
		}

		*xp = sp->visRect.x;
		*wp = sp->visRect.width;

		*yp = sp->visRect.y;
		*hp = sp->visRect.height;

		sp->busy = 1;

		if (clipEvent)
			*clipEvent = sp->clipEvent;

		if (is_unobscured)
			*is_unobscured = sp->is_unobscured;

		DBG(fprintf(stderr, "ndga_start() = 1\n"));
		return (1);
	}

	if (sp->is_open && sp->is_visible) {
		/*
		 * Assume it will just go through X; slow it down
		 */
		poll(0, 0, sp->w * sp->h / 450);
	}

	/*
	 * not OK
	 */
	if (sp->session && sp->sent_clip) {
		sp->sent_clip = 0;
		nvSessionClipRegion(sp->session, NULL);
	}

	if (clipEvent)
		*clipEvent = sp->clipEvent;

	if (is_unobscured)
		*is_unobscured = 0;

	return (0);
}

/*
 * update the argument to hold the current clip-list and and return
 * it. If the clip argument is NULL, a new region is allocated. It
 * is the responsibility of the caller to free the clip.
 */
NRegion
ndga_clip(struct ndga_state * sp, NRegion clip)
{
	if (!clip)
		clip = NCreateRegion();

	/*
	 * Use union as copy
	 */
	(void) NUnionRegion(sp->clip, sp->clip, clip);
	return(clip);
}

void
ndga_done(struct ndga_state * sp)
{
	if (!sp)
		return;

	if (sp->is_changed) {
		ndga_handle_change(sp);
		sp->is_changed = 0;
	}

	if (sp->new_clip) {
		update_clip(sp);
	}

	sp->busy = 0;

#ifdef X_VISIBILITY_BUG
	ndga_workaround(sp);
#endif /* X_VISIBILITY_BUG */
}

void
ndga_process_event(struct ndga_state * sp, XEvent * event)
{
	NRectangle	rectangle;

	if (!sp)
		return;

	if (event->xany.type == Expose) {
#ifdef X_VISIBILITY_BUG
		if (sp->clearOnExpose) {
			NDestroyRegion(sp->region);
			sp->region = NCreateRegion();
			sp->clearOnExpose = 0;
		}
#endif /* X_VISIBILITY_BUG */
		rectangle.x = event->xexpose.x;
		rectangle.y = event->xexpose.y;
		rectangle.width = event->xexpose.width;
		rectangle.height = event->xexpose.height;
		NUnionRectWithRegion(&rectangle, sp->region, sp->region);

		DBG(fprintf(stderr, "Expose: %d,%d %dx%d\n",
			event->xexpose.x,
			event->xexpose.y,
			event->xexpose.width,
			event->xexpose.height));
		if (event->xexpose.count == 0) {
			/*
			 * Handle the last in a sequence of expose events.
			 */
			sp->is_changed = 1;
		}
	} else if (event->xany.type == VisibilityNotify) {
		sp->is_visible =
		    (event->xvisibility.state != VisibilityFullyObscured);
		sp->is_unobscured =
		    (event->xvisibility.state == VisibilityUnobscured);
		sp->is_changed = 1;
		DBG(fprintf(stderr, "Visiblility: %s\n",
			(event->xvisibility.state == VisibilityUnobscured) ?
			"Unobscured" :
			((event->xvisibility.state == VisibilityFullyObscured) ?
			    "FullyObscured" : "PartuallyObscured")));
		if (event->xvisibility.state == VisibilityPartiallyObscured) {
			Window		w;
			DBG(fprintf(stderr, "Request full clip\n"));

			NDestroyRegion(sp->region);
			sp->region = NCreateRegion();

			w = XCreateSimpleWindow(sp->display, sp->window,
			    0, 0, 30000, 30000, 0, 0, 0);
			XMapWindow(sp->display, w);
			XFlush(sp->display);
			XDestroyWindow(sp->display, w);
			XFlush(sp->display);
			/*
			 * sync so we will get expose events all together
			 */ 
			XSync(sp->display, False);
		}
	} else if (event->xany.type == UnmapNotify) {
		DBG(fprintf(stderr, "Unmap\n"));
		sp->is_changed = 1;
		NDestroyRegion(sp->region);
		sp->region = NCreateRegion();
	} else if (event->xany.type == ConfigureNotify) {
		DBG(fprintf(stderr, "Configure\n"));
		sp->is_changed = 1;
	} else if (event->xany.type == CirculateNotify) {
		DBG(fprintf(stderr, "Circulate\n"));
	} else if (event->xany.type == MapNotify) {
		DBG(fprintf(stderr, "Map\n"));
	} else {
		DBG(fprintf(stderr, "Event %d\n", event->xany.type));
	}

	if (XPending(sp->display) != 0)
		return;
	DBG(fprintf(stderr, "Last Event\n\n"));

	if ((sp->on_open || sp->on_close) && sp->is_changed) {
		if (sp->is_open != sp->was_open) {
			if (!sp->is_open) {
				DBG(fprintf(stderr, "I-CLOSED\n"));
				if (sp->on_close)
					(*sp->on_close) ();
			} else {
				DBG(fprintf(stderr, "I-OPEN\n"));
				if (sp->on_open)
					(*sp->on_open) ();
			}

			sp->was_open = sp->is_open;
		}
	}

	if (sp->is_changed) {
		ndga_handle_change(sp);
		if (!sp->busy)
			sp->is_changed = 0;
	}

	if (!sp->busy && sp->new_clip) {
		update_clip(sp);
	}
}

int
ndga_isvisible(ndga_t sp)
{
	if (!sp) {
		/*
		 * Safest to say yes?
		 */
		return (1);
	}

	if (!sp->busy && sp->is_changed) {
		ndga_handle_change(sp);
		sp->is_changed = 0;
	}

	if (!sp->busy && sp->new_clip) {
		update_clip(sp);
	}

#ifdef X_VISIBILITY_BUG
	ndga_workaround(sp);
#endif /* X_VISIBILITY_BUG */

	DBG(fprintf(stderr, "is_visible %d\n",  sp->is_open && sp->is_visible));
	return (sp->is_open && sp->is_visible);
}
