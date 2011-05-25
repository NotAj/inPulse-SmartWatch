
/* Copyright (c) 2010,2011, Peter Barrett  
**  
** Permission to use, copy, modify, and/or distribute this software for  
** any purpose with or without fee is hereby granted, provided that the  
** above copyright notice and this permission notice appear in all copies.  
**  
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL  
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED  
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR  
** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES  
** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,  
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS  
** SOFTWARE.  
*/

#include <pulse_os.h>
#include <pulse_types.h>


//=========================================================================
//	Raycasting image of a nice lattice
//	If the types looks fussy its because this code runs on 8 bit cpus too

typedef unsigned char u8;
#define abs(_a) ((_a) < 0 ? -(_a) : (_a))

int _delta;
u8 _sample;
color24_t _palette[128];

color24_t ToColor(int p)
{
	int i = p >> 5;	// color
	p &= 0x1F;		// luminance
	p = (p << 3) | (p >> 2);

	color24_t c = { p,p,p,p };
	switch (i)
	{
		case 1:	c.green = 0;	break;
		case 2:	c.red = 0;		break;
		case 3:	c.blue = 0;		break;
	}
	return c;
}

int Probe(short* o, short* d)
{
	u8 stepshift = 7;
	u8 hit = 0;
	u8 lasthit = 0;
	u8 i;
	u8 it = 0;
	for (it = 0; it < 100; it++)
	{
		//_gits++;
		//_ss += stepshift;

		//	step
		for (i = 0; i < 3; i++)
		{
			short dd = d[i] >> stepshift;
			if (hit)
				dd = -dd;
			o[i] += dd;
		}

		short test = 0x3000;		// at 6
		long blowup = 0x8C00 >> stepshift;
		//u16 blowup = _hitRange[stepshift];

		// hit
		long ax = abs(o[0]);	// only need to be long because of integer type promotion
		long ay = abs(o[1]);
		long az = abs(o[2]);
		hit = 0;

	//	octohedron
		long t = ax+ay+az;
		if ((t - test*2) < blowup)
			hit = 1;

	//	bar
		t = abs(ax-az) + abs(ay-ax) + abs(az-ay);
	//		t = o[0]*o[0] + o[1]*o[1] + o[2]*o[2];	// ballz etc
		if (t - test < blowup)
		{
			hit = 2;
			if ((o[0] ^ o[1] ^ o[2]) & 0x8000)
				hit = 3;
		}

		if (hit)
		{
			stepshift++;		// finer
			t = 8 - (it >> 3);
			if (stepshift >= t)
			{
				int c = 31-it+stepshift;
				if (c < 0)
					c = 0;
				if (c > 31)
					c = 31;
				return c | (hit << 5);
			}
			it--;
		} else {
			if (lasthit == 0 && stepshift > 1)
				stepshift--;	// coarser
		}
		lasthit = hit;
	}
	return 0;
}

void Lattice_Init()
{
	_delta = 17000;
	for (int i = 0; i < 128; i++)
		_palette[i] = ToColor(i);
	_sample = 16;
}

void Graphics_OpenBounds()
{
	pulse_set_draw_window(0,0,SCREEN_WIDTH-1,SCREEN_HEIGHT-1);
}

void Graphics_BlitIndexed(u8* buf, color24_t* palette, int n)
{
	while (n--)
		pulse_draw_point24(palette[*buf++]);
}

void Lattice_Draw(int step)
{
	_delta += 0x1000;
	int xx,yy;
	short d[3];
	u8 buf[SCREEN_WIDTH];

	int WIDTH = SCREEN_HEIGHT;
	int HEIGHT = SCREEN_WIDTH;
//	int dx = 205;
//	int dy = 191;
	int dx = 167;
	int dy = 163;
	if (WIDTH < 240)
	{
		// 128x128, 160x128
		dx += dx;
		dy += dy;
	}
	dx *= step;
	dy *= step;

	Graphics_OpenBounds();

	u8 s;
	d[0] = -32767;	// x
	for (xx = 0; xx < WIDTH; xx += step)
	{
		d[1] = -24576;	// y
		long x = d[0];
		short x2 = x*x >> 16;
		for (yy = 0; yy < HEIGHT;)
		{
			long y = d[1];
			short y2 = y*y >> 16;
			d[2] = 21168 - x2 - y2;

			short o[3];
			o[0] = _delta+0;
			o[1] = _delta+45056;
			o[2] = _delta+24576;	// origin

			u8 pixel = Probe(o,d);
			s = step;
			while (s--)
				buf[yy++] = pixel;
			d[1] += dy;
		}
		s = step;
		while (s--)
			Graphics_BlitIndexed(buf,_palette,HEIGHT);
		d[0] += dx;
	}
}

//=========================================================================
//=========================================================================

void main_app_init()
{
	Lattice_Init();
}

void main_app_handle_button_down()
{
    _sample++;
}

void main_app_handle_button_up()
{
}

void main_app_loop()
{
	Lattice_Draw(_sample);
	if (_sample > 1)
		_sample--;
}

void main_app_handle_doz()
{
}

void main_app_handle_hardware_update(enum PulseHardwareEvent event)
{
}
