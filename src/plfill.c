/* $Id$
   $Log$
   Revision 1.2  1992/05/22 16:39:05  mjl
   Fixed bug in qsort call.  I can't see how it managed to avoid a core dump
   before.  This may explain some of our many problems with this routine.

 * Revision 1.1  1992/05/20  21:34:25  furnish
 * Initial checkin of the whole PLPLOT project.
 *
*/

/*	plfill.c

	Polygon pattern fill.
*/

#include "plplot.h"
#include <math.h>

#ifdef PLSTDC
#include <stdlib.h>
#ifdef INCLUDE_STDDEF
#include <stddef.h>
#endif
#ifdef INCLUDE_MALLOC
#include <malloc.h>
#endif

#else
extern char *malloc();
extern char *realloc();
extern void free();
#define size_t	int
#endif

#define DTOR            0.0174533
#define BINC            50

struct point {
    PLINT x, y;
};
static PLINT bufferleng, buffersize, *buffer;

void 
c_plfill(n, x, y)
PLINT n;
PLFLT *x, *y;
{
    PLINT i, level;
    PLINT xmin, ymin, x1, y1, x2, y2, x3, y3;
    PLINT k, dinc;
    PLFLT ci, si, thetd;
    PLINT *inclin, *delta, nps;
    PLFLT xpmm, ypmm;
    short swap;

#ifdef PLSTDC
    void tran(PLINT * a, PLINT * b, PLFLT c, PLFLT d);
    void buildlist( PLINT, PLINT, PLINT, PLINT, PLINT, PLINT, PLINT );
#else
    void tran();
    void buildlist();
    void qsort();
#endif
    int compar();

    glev(&level);
    if (level < 3)
	plexit("Please set up window before calling plfill.");
    if (n < 3)
	plexit("Not enough points in plfill object!");

    buffersize = 2 * BINC;
    buffer = (PLINT *) malloc((size_t) buffersize * sizeof(PLINT));
    if (!buffer)
	plexit("Out of memory in plfill.");

    gpat(&inclin, &delta, &nps);
    gpixmm(&xpmm, &ypmm);
    for (k = 0; k < nps; k++) {
	bufferleng = 0;

	if (ABS(inclin[k]) <= 450) {
	    thetd = atan(tan(DTOR * inclin[k] / 10.) * ypmm / xpmm);
	    swap = 0;
	}
	else {
	    thetd = atan(tan(DTOR * SIGN(inclin[k]) *
			     (ABS(inclin[k]) - 900) / 10.) * xpmm / ypmm);
	    swap = 1;
	}
	ci = cos(thetd);
	si = sin(thetd);
	if (swap)
	    dinc = delta[k] * SSQR(xpmm * ABS(ci), ypmm * ABS(si)) / 1000.;
	else
	    dinc = delta[k] * SSQR(ypmm * ABS(ci), xpmm * ABS(si)) / 1000.;

	xmin = wcpcx(x[0]);
	ymin = wcpcy(y[0]);
	for (i = 1; i < n; i++) {
	    xmin = MIN(xmin, wcpcx(x[i]));
	    ymin = MIN(ymin, wcpcy(y[i]));
	}

	x1 = wcpcx(x[0]) - xmin;
	y1 = wcpcy(y[0]) - ymin;
	tran(&x1, &y1, (PLFLT) ci, (PLFLT) si);
	x2 = wcpcx(x[1]) - xmin;
	y2 = wcpcy(y[1]) - ymin;
	tran(&x2, &y2, (PLFLT) ci, (PLFLT) si);
	for (i = 2; i < n; i++) {
	    x3 = wcpcx(x[i]) - xmin;
	    y3 = wcpcy(y[i]) - ymin;
	    tran(&x3, &y3, (PLFLT) ci, (PLFLT) si);
	    if (swap)
		buildlist(y1, x1, y2, x2, y3, x3, dinc);
	    else
		buildlist(x1, y1, x2, y2, x3, y3, dinc);
	    x1 = x2;
	    y1 = y2;
	    x2 = x3;
	    y2 = y3;
	}
	x3 = wcpcx(x[0]) - xmin;
	y3 = wcpcy(y[0]) - ymin;
	tran(&x3, &y3, (PLFLT) ci, (PLFLT) si);
	if (swap)
	    buildlist(y1, x1, y2, x2, y3, x3, dinc);
	else
	    buildlist(x1, y1, x2, y2, x3, y3, dinc);

	x1 = x2;
	y1 = y2;
	x2 = x3;
	y2 = y3;
	x3 = wcpcx(x[1]) - xmin;
	y3 = wcpcy(y[1]) - ymin;
	tran(&x3, &y3, (PLFLT) ci, (PLFLT) si);
	if (swap)
	    buildlist(y1, x1, y2, x2, y3, x3, dinc);
	else
	    buildlist(x1, y1, x2, y2, x3, y3, dinc);

	/* Sort list by y then x */
	qsort((char *) buffer, (size_t) bufferleng / 2, 
				sizeof(struct point), &compar);

	/* OK, now do the hatching */
	i = 0;
	while (i < bufferleng) {
	    if (swap) {
		x1 = buffer[i + 1];
		y1 = buffer[i];
	    }
	    else {
		x1 = buffer[i];
		y1 = buffer[i + 1];
	    }
	    i += 2;
	    x2 = x1;
	    y2 = y1;
	    tran(&x1, &y1, (PLFLT) ci, (PLFLT) (-si));
	    movphy(x1 + xmin, y1 + ymin);
	    if (swap) {
		x1 = buffer[i + 1];
		y1 = buffer[i];
	    }
	    else {
		x1 = buffer[i];
		y1 = buffer[i + 1];
	    }
	    i += 2;
	    if ((swap && x2 != x1) || (!swap && y2 != y1))
		continue;	/* Uh oh we're lost */
	    tran(&x1, &y1, (PLFLT) ci, (PLFLT) (-si));
	    draphy(x1 + xmin, y1 + ymin);
	}
    }
    free((VOID *) buffer);
}

#ifdef PLSTDC
void 
tran(PLINT * a, PLINT * b, PLFLT c, PLFLT d)
#else
void 
tran(a, b, c, d)
PLINT *a, *b;
PLFLT c, d;
#endif
{
    PLINT ta, tb;

    ta = *a;
    tb = *b;

    *a = ROUND(ta * c + tb * d);
    *b = ROUND(tb * c - ta * d);
}

void 
buildlist(x1, y1, x2, y2, x3, y3, dinc)
PLINT x1, y1, x2, y2, x3, y3, dinc;
{
    PLINT i;
    PLINT dx, dy, cstep, nstep, lines, ploty, plotx;
    void addcoord();

    dx = x2 - x1;
    dy = y2 - y1;

    if (dy == 0)
	return;

    cstep = (y2 > y1 ? 1 : -1);
    nstep = (y3 > y2 ? 1 : -1);
    if (y3 == y2)
	nstep = 0;

    /* Build coordinate list */
    lines = ABS(dy) / dinc + 1;
    if (cstep == 1 && y1 > 0)
	ploty = (y1 / dinc + 1) * dinc;
    else if (cstep == -1 && y1 < 0)
	ploty = (y1 / dinc - 1) * dinc;
    else
	ploty = (y1 / dinc) * dinc;

    for (i = 0; i < lines; i++) {
	if (!BETW(ploty, y1, y2))
	    break;
	plotx = x1 + ROUND(((float) (ploty - y1) * dx) / dy + .5);
	/* Check for extremum at end point, otherwise add to coord list */
	if (!((ploty == y1) || (ploty == y2 && nstep != cstep)))
	    addcoord(plotx, ploty);
	ploty += cstep * dinc;
    }
}

void 
addcoord(x1, y1)
PLINT x1, y1;
{
    PLINT *temp;

    if (bufferleng + 2 > buffersize) {
	buffersize += 2 * BINC;
	temp = (PLINT *) realloc((VOID *) buffer, 
			(size_t) buffersize * sizeof(PLINT));
	if (!temp) {
	    free((VOID *) buffer);
	    plexit("Out of memory in plfill!");
	}
	buffer = temp;
    }

    buffer[bufferleng++] = x1;
    buffer[bufferleng++] = y1;
}

int 
compar(pnum1, pnum2)
char *pnum1, *pnum2;
{
    struct point *pnt1, *pnt2;

    pnt1 = (struct point *) pnum1;
    pnt2 = (struct point *) pnum2;

    if (pnt1->y < pnt2->y)
	return (-1);
    else if (pnt1->y > pnt2->y)
	return (1);

    /* Only reach here if y coords are equal, so sort by x */
    if (pnt1->x < pnt2->x)
	return (-1);
    else if (pnt1->x > pnt2->x)
	return (1);

    return (0);
}
