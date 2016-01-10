#ifndef NDGA_H
#define NDGA_H

#include "nvsession.h"

#ifndef NDGA_IMPLEMENTATION
/*
 * For public consumption, this is an opaque pointer
 */
typedef void *ndga_t;

#else 
/*
 * This is implementaion private
 *
 * Newt DGA state record
 */
struct ndga_state {
	/*
	 * all private
	 */
	int is_visible;
	int is_unobscured;
	int is_open;
	int is_changed;
	int new_clip;
	int sent_clip;
	int busy;

	int was_open;
	int was_unobscured;

	int x, y;
	int w, h;

	int xalign;
	int yalign;
	nvSession session;

	void (* on_open)();
	void (* on_close)();

	Display *display;
	Window window;
	Window parent;
	Window root;

	NRegion region;
	NRegion clip;
	NRegion lastclip;
	NRegion screenArea;
	NRectangle screenRect;
	NRectangle visRect;

	int	clipEvent;

	int	clearOnExpose;
};

typedef struct ndga_state *ndga_t;

#endif /* NDGA_IMPLEMENTATION */


ndga_t ndga_new(Display *display, Window window, nvSession session, 
	int xalign, int yalign);

void ndga_destroy(ndga_t sp);

int ndga_start(ndga_t sp,
	int *xp,	/* x */
	int *yp,	/* y */
	int *wp,	/* width */
	int *hp,	/* height */
	int *is_unobscured, /* 1 if unobscured 0 if clipped */
	int *clipEvent	/* clip event number */
	);

/*
 * update the argument to hold the current clip-list and and return
 * it. If the clip argument is NULL, a new region is allocated. It
 * is the responsibility of the caller to free the clip.
 */
NRegion ndga_clip(ndga_t sp, NRegion clip);

void ndga_done(ndga_t sp);

int ndga_isvisible(ndga_t sp);

int ndga_get_eventmask(void);
void ndga_process_event(ndga_t sp, XEvent *event);

#endif
