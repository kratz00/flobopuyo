#include "stdlib.h"
#include "sofont.h"
#include "string.h"

struct _SOFONT
{
	int     height;
	IIM_Surface *picture;
	int    *CharPos;
	int    *Spacing;

	int     max_i, spacew, cursShift;
	Uint32  background;
};

// protected
int     SoFont_DoStartNewChar (SoFont * font, Sint32 x);
void    SoFont_CleanSurface (SoFont * font);


int
SoFont_FontHeight (SoFont * font)
{
	return font->height;
}

int
SoFont_getMinChar (SoFont * font)
{
	return START_CHAR;
}

int
SoFont_getMaxChar (SoFont * font)
{
	return font->max_i;
}


SoFont *
SoFont_new ()
{
	SoFont *font = (SoFont *) malloc (sizeof (SoFont));

	font->picture = NULL;
	font->CharPos = NULL;
	font->Spacing = NULL;
	font->height = 0;
	font->max_i = 0;
	font->spacew = 0;
	font->cursShift = 0;
	font->background = 0;

	return font;
}

void
SoFont_free (SoFont * font)
{
/*	if (font->picture)
		SDL_FreeSurface (font->picture); */
	if (font->CharPos)
		free (font->CharPos);
	if (font->Spacing)
		free (font->Spacing);
}

// SoFontUtilities

static Uint32
SoFontGetPixel (SDL_Surface * Surface, Sint32 X, Sint32 Y)
{
	Uint8  *bits;
	Uint32  Bpp;
	Uint8   r, g, b;

	if (!Surface) {
		fprintf (stderr, "SoFontGetPixel: No surface!\n");
		return 0;
	}
	if ((X < 0) || (X >= Surface->w)) {
		fprintf (stderr, "SoFontGetPixel: X (%d)" " out of range!\n", X);
		return 0;
	}

	Bpp = Surface->format->BytesPerPixel;

	bits = ((Uint8 *) Surface->pixels) + Y * Surface->pitch + X * Bpp;

	switch (Bpp) {
	case 1:
		return *((Uint8 *) Surface->pixels + Y * Surface->pitch + X);
		break;
	case 2:
		return *((Uint16 *) Surface->pixels + Y * Surface->pitch / 2 + X);
		break;
	case 3:
		// Format/endian independent
		r = *((bits) + Surface->format->Rshift / 8);
		g = *((bits) + Surface->format->Gshift / 8);
		b = *((bits) + Surface->format->Bshift / 8);
		return SDL_MapRGB (Surface->format, r, g, b);
		break;
	case 4:
		return *((Uint32 *) Surface->pixels + Y * Surface->pitch / 4 + X);
		break;
	}
	fprintf (stderr, "SoFontGetPixel: Unsupported pixel format!\n");
	return 0;											// David (to get rid of warning)
}

static void
SoFontSetPixel (SDL_Surface * Surface, Sint32 X, Sint32 Y, Uint32 c)
{
	Uint8  *bits;
	Uint32  Bpp;
	Uint8   r, g, b;

	if (!Surface) {
		fprintf (stderr, "SoFontSetPixel: No surface!\n");
		return;
	}
	if ((X < 0) || (X >= Surface->w)) {
		fprintf (stderr, "SoFontSetPixel: X (%d)" " out of range!\n", X);
		return;
	}

	Bpp = Surface->format->BytesPerPixel;

	bits = ((Uint8 *) Surface->pixels) + Y * Surface->pitch + X * Bpp;

	switch (Bpp) {
	case 1:
		*((Uint8 *) Surface->pixels + Y * Surface->pitch + X) = (Uint8) c;
		break;
	case 2:
		*((Uint16 *) Surface->pixels + Y * Surface->pitch / 2 + X) = (Uint16) c;
		break;
	case 3:
		// Format/endian independent
		SDL_GetRGB (c, Surface->format, &r, &g, &b);
		*((bits) + Surface->format->Rshift / 8) = r;
		*((bits) + Surface->format->Gshift / 8) = g;
		*((bits) + Surface->format->Bshift / 8) = b;
		break;
	case 4:
		*((Uint32 *) Surface->pixels + Y * Surface->pitch / 4 + X) = c;
		break;
	}
}

#if 0
static void
clipx (SDL_Rect * srcrect, SDL_Rect * dstrect, SDL_Rect * clip)
{
	int     dwx;

	// Use if destination have the same size than source.
	int     dx = clip->x - dstrect->x;

	int     sw = srcrect->w;			// Because SDL_Rect.w are

	// unsigned.
	int     dw = dstrect->w;


	if (dx > 0) {
		srcrect->x += dx;
		dstrect->x += dx;

		sw -= dx;
		dw -= dx;
	}

	dwx = (dstrect->x + dstrect->w) - (clip->x + clip->w);

	if (dwx > 0) {
		sw -= dwx;
		dw -= dwx;
	}

	if (sw > 0)
		srcrect->w = sw;
	else
		srcrect->w = 0;

	if (dw > 0)
		dstrect->w = dw;
	else
		dstrect->w = 0;
}
#endif

static void
sdcRects (SDL_Rect * source, SDL_Rect * destination, SDL_Rect clipping)
{
	int     dwx, dhy;

	// Use if destination have the same size than source &
	// cliping on destination
	int     dx = clipping.x - destination->x;
	int     dy = clipping.y - destination->y;

	int     sw = source->w;
	int     sh = source->h;

	if (dx > 0) {
		source->x += dx;
		destination->x += dx;

		sw -= dx;
		destination->w -= dx;
	}
	if (dy > 0) {
		source->y += dy;
		destination->y += dy;

		sh -= dy;
		destination->h -= dy;
	}

	dwx = (destination->x + destination->w) - (clipping.x + clipping.w);
	dhy = (destination->y + destination->h) - (clipping.y + clipping.h);

	if (dwx > 0) {
		sw -= dwx;
		destination->w -= dwx;
	}
	if (dhy > 0) {
		sh -= dhy;
		destination->h -= dhy;
	}

	if (sw > 0)
		source->w = sw;
	else
		source->w = 0;

	if (sh > 0)
		source->h = sh;
	else
		source->h = 0;

}

// end of SoFontUtilities

int
SoFont_DoStartNewChar (SoFont * font, Sint32 x)
{
	if (!font->picture)
		return 0;
	return SoFontGetPixel (font->picture->surf, x, 0) ==
		SDL_MapRGB (font->picture->surf->format, 255, 0, 255);
}

void
SoFont_CleanSurface (SoFont * font)
{
	int     x = 0, y = 0;
	Uint32  pix;

	if (!font->picture)
		return;

	pix = SDL_MapRGB (font->picture->surf->format, 255, 0, 255);

	while (x < font->picture->w) {
		y = 0;
//Why clean the entire surface? IMHO, S[o]Font should only ever
//touch - or even care about - the very top pixel row.
//    while(y < picture->h)
//    {
		if (SoFontGetPixel (font->picture->surf, x, y) == pix)
			SoFontSetPixel (font->picture->surf, x, y, font->background);
//      y++;
//    }
		x++;
	}
}


int
SoFont_load (SoFont * font, IIM_Surface * FontSurface)
{
	int     x = 0, i = 0, p = 0, s = 0;
	int     cursBegin = 0;
	int     cursWidth = 0;

	int     _CharPos[256];
	int     _Spacing[256];

	if (!FontSurface) {
		fprintf (stderr, "SoFont recieved a NULL SDL_Surface\n");
		return 0;
	}
	font->picture = FontSurface;
	font->height = font->picture->h - 1;
	while (x < font->picture->w) {
		if (SoFont_DoStartNewChar (font, x)) {
			if (i)
				_Spacing[i - 1] = 1 + x - s;
			p = x;
			while ((x < font->picture->w - 1) && (SoFont_DoStartNewChar (font, x)))
				x++;
			s = x;
			// CharPos[i++] = (p + x) / 2;
			_CharPos[i++] = (p + x + 1) / 2;	// David, Kobo Deluxe
		}
		x++;
	}
	// Note that spacing is not needed for the last char,
	// as it's just used for the blit width calculation.
	if (i)
		_Spacing[i - 1] = 1 + x - s;
	_Spacing[i] = 0;
	_CharPos[i++] = font->picture->w;

	font->max_i = START_CHAR + i - 1;
	font->background = SoFontGetPixel (font->picture->surf, 0, font->height);
	SDL_SetColorKey (font->picture->surf, SDL_SRCCOLORKEY, font->background);
	SoFont_CleanSurface (font);

	font->CharPos = (int *) malloc (i * sizeof (int));
	font->Spacing = (int *) malloc (i * sizeof (int));
	memcpy (font->CharPos, _CharPos, i * sizeof (int));
	memcpy (font->Spacing, _Spacing, i * sizeof (int));

	// We search for a smart space width:
	// Changed from "a", "A", "0" for Kobo Deluxe.
	// Spaces were *way* to wide! //David
	font->spacew = 0;
	if (!font->spacew)
		font->spacew = SoFont_TextWidth (font, "i") * 3 / 2;
	if (!font->spacew)
		font->spacew = SoFont_TextWidth (font, "I") * 3 / 2;
	if (!font->spacew)
		font->spacew = SoFont_TextWidth (font, ".") * 3 / 2;
	if (!font->spacew)
		font->spacew = font->CharPos[1] - font->CharPos[0];

	// We search for a smart cursor position:
	font->cursShift = 0;
	if ('|' > font->max_i)
		return 1;										// No bar in this font!

	if (font->background ==
			SoFontGetPixel (font->picture->surf, font->CharPos['|' - START_CHAR],
											font->height / 2)) {
		// Up to the first | color
		for (cursBegin = 0; cursBegin <= SoFont_TextWidth (font, "|");
				 cursBegin++)
				if (font->background != SoFontGetPixel (font->picture->surf,
																								font->CharPos['|' -
																															START_CHAR] +
																								cursBegin, font->height / 2))
				break;
		// Up to the end of the | color
		for (cursWidth = 0; cursWidth <= SoFont_TextWidth (font, "|");
				 cursWidth++)
				if (font->background == SoFontGetPixel (font->picture->surf,
																								font->CharPos['|' -
																															START_CHAR] +
																								cursBegin + cursWidth,
																								font->height / 2))
				break;
	}
	else {
		// Up to the end of the | color
		for (cursWidth = 0; cursWidth <= SoFont_TextWidth (font, "|");
				 cursWidth++)
				if (font->background == SoFontGetPixel (font->picture->surf,
																								font->CharPos['|' -
																															START_CHAR] +
																								cursWidth, font->height / 2))
				break;
	}
	font->cursShift = cursBegin + 1;	// cursWidth could be used if
	// image format changes.

	return 1;
}

void
SoFont_Refresh(SoFont * font)
{
	font->background = SoFontGetPixel (font->picture->surf, 0, font->height);
	SDL_SetColorKey (font->picture->surf, SDL_SRCCOLORKEY, font->background);
	SoFont_CleanSurface (font);
}

void
SoFont_PutString (SoFont * font, SDL_Surface * Surface, int x, int y,
									const char *text, SDL_Rect * clip)
{
    int sx = x;
	int     ofs, i = 0;
	SDL_Rect srcrect, dstrect;

	if ((!font->picture) || (!Surface) || (!text))
		return;

	while (text[i] != '\0') {
		if (text[i] == ' ') {
			x += font->spacew;
			i++;
		}
        else if (text[i] == '\n') {
            x = sx;
            y += font->picture->h + 2;
            i++;
        }
		else if ((text[i] >= START_CHAR) && (text[i] <= font->max_i)) {
			ofs = text[i] - START_CHAR;
			srcrect.w = dstrect.w = font->CharPos[ofs + 1] - font->CharPos[ofs];
			srcrect.h = dstrect.h = font->height;
			srcrect.x = font->CharPos[ofs];
			srcrect.y = 1;
			dstrect.x = x;
			dstrect.y = y;
			x += font->Spacing[ofs];
			if (clip)
				sdcRects (&srcrect, &dstrect, *clip);
			SDL_BlitSurface (font->picture->surf, &srcrect, Surface, &dstrect);
			i++;
		}
		else
			i++;											// other chars are ignored
	}
}

void
SoFont_PutStringWithCursor (SoFont * font, SDL_Surface * Surface, int xs,
														int y, const char *text, int cursPos,
														SDL_Rect * clip, int showCurs)
{
	int     ofs, i = 0, x = xs;
	SDL_Rect srcrect, dstrect;

	if ((!font->picture) || (!Surface) || (!text))
		return;
	if ('|' > font->max_i)
		showCurs = 0;

	// We want the cursor to appear under the main text.
	if (showCurs) {
		while (text[i] != '\0')
			if (i == cursPos)
				break;
			else if (text[i] == ' ') {
				x += font->spacew;
				i++;
			}
			else if ((text[i] >= START_CHAR)
							 && (text[i] <= font->max_i)) {
				ofs = text[i] - START_CHAR;
				x += font->Spacing[ofs];
				i++;
			}
			else
				i++;
		ofs = '|' - START_CHAR;

		srcrect.w = dstrect.w = font->CharPos[ofs + 1] - font->CharPos[ofs];
		srcrect.h = dstrect.h = font->height;
		srcrect.x = font->CharPos[ofs];
		srcrect.y = 1;
		dstrect.x = x - font->cursShift;
		dstrect.y = y;
		if (clip)
			sdcRects (&srcrect, &dstrect, *clip);
		SDL_BlitSurface (font->picture->surf, &srcrect, Surface, &dstrect);
	}
	// Then the text:
	SoFont_PutString (font, Surface, xs, y, text, clip);
}


int
SoFont_TextWidth_MinMax (SoFont * font, const char *text, int min, int max)
{
	int     ofs, x = 0, i = min;

	if (!font->picture)
		return 0;
	while ((text[i] != '\0') && (i < max)) {
		if (text[i] == ' ') {
			x += font->spacew;
			i++;
		}
		else if ((text[i] >= START_CHAR) && (text[i] <= font->max_i)) {
			ofs = text[i] - START_CHAR;
			x += font->Spacing[ofs];
			i++;
		}
		else
			i++;
	}
	return x;
}

int
SoFont_TextWidth (SoFont * font, const char *text)
{
	return SoFont_TextWidth_MinMax (font, text, 0, 255);
}


void
SoFont_XCenteredString (SoFont * font, SDL_Surface * Surface, int y,
												const char *text, SDL_Rect * clip)
{
	if (!font->picture)
		return;
	SoFont_PutString (font, Surface,
										Surface->w / 2 - SoFont_TextWidth (font, text) / 2, y,
										text, clip);
}

void
SoFont_CenteredString_XY (SoFont * font, SDL_Surface * Surface, int x, int y,
													const char *text, SDL_Rect * clip)
{
	if (!font->picture)
		return;
	SoFont_PutString (font, Surface, x - SoFont_TextWidth (font, text) / 2,
										y - font->height / 2, text, clip);
}

void
SoFont_CenteredString (SoFont * font, SDL_Surface * Surface, const char *text,
											 SDL_Rect * clip)
{
	if (!font->picture)
		return;
	SoFont_CenteredString_XY (font, Surface, Surface->clip_rect.w / 2,
														Surface->clip_rect.h / 2, text, clip);
}

void
SoFont_PutStringCleverCursor (SoFont * font, SDL_Surface * Surface,
															const char *text, int cursPos, SDL_Rect * r,
															SDL_Rect * clip, int showCurs)
{
	int     w1, w2;

	if ((!font->picture) || (!text))
		return;

	w1 = SoFont_TextWidth_MinMax (font, text, 0, cursPos);
	w2 = SoFont_TextWidth (font, text);

	if ((w2 < r->w) || (w1 < r->w / 2))
		SoFont_PutStringWithCursor (font, Surface, r->x,
																r->y + (r->h - font->height) / 2, text,
																cursPos, clip, showCurs);
	else if (w1 < w2 - r->w / 2)
		SoFont_PutStringWithCursor (font, Surface, r->x - w1 + r->w / 2,
																r->y + (r->h - font->height) / 2, text,
																cursPos, clip, showCurs);
	else
		SoFont_PutStringWithCursor (font, Surface, r->x - w2 + r->w,
																r->y + (r->h - font->height) / 2, text,
																cursPos, clip, showCurs);
}

int
SoFont_TextCursorAt (SoFont * font, const char *text, int px)
{
	int     ofs, x = 0, i = 0, ax = 0;

	if (!font->picture)
		return 0;

	if (px <= 0)
		return 0;

	while (text[i] != '\0') {
		if (text[i] == ' ') {
			x += font->spacew;
			i++;
		}
		else if ((text[i] >= START_CHAR) && (text[i] <= font->max_i)) {
			ofs = text[i] - START_CHAR;
			x += font->Spacing[ofs];
			i++;
		}
		else
			i++;

		if (px < (ax + x) / 2)
			return (i - 1);
		ax = x;
	}
	return i;
}

int
SoFont_CleverTextCursorAt (SoFont * font, const char *text, int px,
													 int cursPos, SDL_Rect * r)
{
	int     w1, w2;

	if ((!font->picture) || (!text))
		return 0;
	w1 = SoFont_TextWidth_MinMax (font, text, 0, cursPos);
	w2 = SoFont_TextWidth (font, text);
	if ((w2 < r->w) || (w1 < r->w / 2))
		return SoFont_TextCursorAt (font, text, px);
	else if (w1 < w2 - r->w / 2)
		return SoFont_TextCursorAt (font, text, px + w1 - (r->w / 2));
	else
		return SoFont_TextCursorAt (font, text, px + w2 - r->w);
}
