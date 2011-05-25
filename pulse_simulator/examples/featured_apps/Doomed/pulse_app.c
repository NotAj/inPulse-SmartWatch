
/* Copyright (c) 2010, Peter Barrett  
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

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#define abs(_a) ((_a) < 0 ? -(_a) : (_a))

#undef SCREEN_WIDTH
#undef SCREEN_HEIGHT
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 96

void Graphics_OpenBounds()
{
	pulse_set_draw_window(0,0,SCREEN_HEIGHT-1,SCREEN_WIDTH-1);
}

void Graphics_Fill(color24_t c, int n)
{
	while(n--)
		pulse_draw_point24(c);
}

#define _S 1    // Steel Wall
#define _W 2    // Wood wall
#define WD 3    // wood door
#define _H 4    // Hex brick wall

#define _C 5    // Clinker brick wall
#define _A 5    // Ammo
#define _B 7    // Blue wall
#define SD 6    // Steel door

extern const u8 _map[64];
const u8 _map[64] =
{
    _S,_S,_S,_S,_A,_W,_W,_W,
    SD, 0, 0,_S, 0, 0, 0,WD,
    _S,_S, 0,_S, 0,_W,_W,_W,
    _A, 0, 0, 0, 0, 0, 0,_C,
    _H,_B,_B, 0,_B,_B, 0,_C,
    _H, 0, 0, 0,_B,_C, 0,WD,
    WD, 0, 0, 0, 0,SD, 0,_C,
    _H,_H,_H,_H,_H,_C,_C,_C,
};

//  Range if uv is 0..2 in 16:16
//  Always positive

//	Not all that accurate, but fast
long multfix(long a, long b)
{
	u16 aa = a >> 8;
	return aa*b >> 8;
}

long multfix16(u16 a16, long b24)
{
	b24 >>= 8;
	return (a16*b24)>>8;
}

long reciprocalfix(u16 d)
{
    u8 count = 0;
    while (((short)d) > 0)
	{
        ++count;
        d <<= 1;
	}
     
    // Single n-r iteration
    long x = 0x02EA00 - d;
	x -= d;
    x = multfix(x, 0x20000L-multfix16(d,x));
    
    if (count>0)
		x = x << count;    
    return x;  
}

//  Range if uv is 0..2 in 16:16
//  Always positive
long RECIP(u16 uv)
{
    if (uv < 4)
        return 0x3FFFFFFF;
	//return 0x80000000/(uv>>1);   // Long divide to provide 16:16 result
	return reciprocalfix(uv);
}

//  dduv is always positive 16:16 number may be very large, might resonably be trimmed
long MUL8(u8 a, long dduv)
{
    return (dduv >> 8)*a;
}

extern const u8 _trig[64];
const u8 _trig[64] = 
{
    0,6,13,19,25,31,37,44,
    50,56,62,68,74,80,86,92,
    98,103,109,115,120,126,131,136,
    142,147,152,157,162,167,171,176,
    180,185,189,193,197,201,205,208,
    212,215,219,222,225,228,231,233,
    236,238,240,242,244,246,247,249,
    250,251,252,253,254,254,255,255
};

short SIN(u8 angle)
{
    if ((angle & 0x7F) == 0)
        return 0;
    u8 b = angle & 0x3F;
    if (angle & 0x40)
        b = 0x3F - b;
    int i = _trig[b] + 1;
    if (angle & 0x80)
        return -i;
    return i;
}

// a little lerping
// QQTTTTTTFFFF
//	Q: Quadrant
//	T: Table lookup
//	F: Lerp fraction
short SIN16(u16 angle)
{
	short a = SIN(angle >> 4);
	short b = SIN((angle+15) >> 4);
	u8 f = angle & 0x0F;
	return a*(16-f) + b*f;	//+-16
}

short COS16(u16 angle)
{
    return SIN16(angle + (64 << 4));
}

//	globals
short _playerPosX;    
short _playerPosY;

u16 _angle;
signed char _moveRate;
signed char _turnRate;

short _x;
short _y;

u8 _phase;
u8 _allglow;
color24_t _glow;

u8	_pressed;
u8	_dir;
u32	_vibeOff;
        
void Doomed_Init()
{
	_angle = 0x2EBA;	// a cool place on the map
	_playerPosX = 0x3AC;
	_playerPosY = 0x344;
	_moveRate = _turnRate = 0;
	_x = -1;
	_phase = 0;
	_allglow = 0;
	_glow.red = _glow.green = _glow.blue = 0;
	_pressed = _dir = _vibeOff = 0;
}

#define PLAYERWIDTH 0x20
u8 InWall(short dx, short dy)
{
    dx += _playerPosX;
    dy += _playerPosY;
    u8 x0 = (dx-PLAYERWIDTH)>>8;
    u8 y0 = (dy-PLAYERWIDTH)>>8;
    u8 x1 = (dx+PLAYERWIDTH)>>8;
    u8 y1 = (dy+PLAYERWIDTH)>>8;
    while (x0 <= x1)
    {
        for (u8 y = y0; y <= y1; y++)
		{
			u8 t = _map[y * 8 + x0];
			if (t)
				return t;
		}
        x0++;
    }
    return 0;
}

// Add a little acceleration to movement
int damp(int i)
{
	if (i < 0)
		return ++i;
	if (i > 0)
		i--;
	return i;
}

void glow()
{
	_phase += 7;
	int c = 128 + (SIN(_phase++) >> 1);
	if (c > 255)
		c = 255;
	_glow.red = c;
	if (_allglow)
		--_allglow;
}

void move()
{
	if (!_pressed)
	{
		_turnRate = damp(_turnRate);
		_moveRate = damp(_moveRate);
	} else {
		if (_dir & 1)
		{
			if (_turnRate < 50)
				_turnRate += 4;
		}
		else
		{
			if (_moveRate < 50)
				_moveRate += 4;
		}
	}

    if (_turnRate)
        _angle += _turnRate;
    
    //  Rather dumb wall avoidance
    while (_moveRate)
    {
        short dx = ((COS16(_angle) >> 5)*_moveRate) >> 7;
        short dy = ((SIN16(_angle) >> 5)*_moveRate) >> 7;
		u8 t = InWall(dx,dy);
        if (t)
        {
		if (_vibeOff == 0)
		{
			_vibeOff = pulse_get_millis() + 500;	// bang into walls
			pulse_vibe_on();
		}
			if (t == 6)
				_allglow = 0xFF;
            if (!InWall(0,dy))
                dx = 0;
            else if (!InWall(dx,0))
                dy = 0;
        }               
        if (!InWall(dx,dy))
        {
            _playerPosX += dx;
            _playerPosY += dy;           
            break;
        }
        _moveRate = damp(_moveRate);
    }

	if (_vibeOff && pulse_get_millis() > _vibeOff)
	{
		pulse_vibe_off();
		_vibeOff = 0;
	}

}

void MakeColor(u8 h, u8 texture, color24_t* c)
{
	if (texture == SD || _allglow)
		*c = _glow;
	if (!(texture & 0x80))
		h += h >> 1;	// 0..240 (dark is 0..160)
	h += 15;
	
	c->red = (texture & 1) ? h: h >> 2;
	c->green = (texture & 2) ? h : h >> 1;
	c->blue = (texture & 4) ? h : 0;
}

void Doomed_Draw()
{        
    move();
	glow();

    // cast all rays here
	// then draw on a second pass

    short sina = SIN16(_angle) << 2;
    short cosa = COS16(_angle) << 2;
    short u = cosa - sina;          // Range of u/v is +- 2 TODO: Fit in 16 bit
    short v = sina + cosa;
    short du = sina / (SCREEN_WIDTH>>1);     // Range is +- 1/24 - 16:16
    short dv = -cosa / (SCREEN_WIDTH>>1);
    
	Graphics_OpenBounds();

	u8 hs[SCREEN_WIDTH];
	u8 ts[SCREEN_WIDTH];
    for (u8 ray = 0; ray < SCREEN_WIDTH; ray++, u += du, v += dv)
    {           
        long duu = RECIP(abs(u));
        long dvv = RECIP(abs(v));
        char stepx = (u < 0) ? -1 : 1;
        char stepy = (v < 0) ? -8 : 8;

        // Initial position
        u8 mapx = _playerPosX >> 8;
        u8 mapy = _playerPosY >> 8;      
        u8 mx = _playerPosX;
        u8 my = _playerPosY;
        if (u > 0)
            mx = 0xFF-mx;
        if (v > 0)
            my = 0xFF-my;
        long distx = MUL8(mx, duu);
        long disty = MUL8(my, dvv);
            
		u8 map = mapy*8 + mapx;
        u8 texture = 0;
		for (;;)
        {
            if (distx > disty)
			{
                // shorter distance to a hit in constant y line
		map += stepy;
                texture = _map[map];
		if (texture)
			break;
                disty += dvv;
            } else {
                // shorter distance to a hit in constant x line
                map += stepx;
                texture = _map[map];
		if (texture)
			break;
                distx += duu;
            }
        }

		//	Texture loop
        long hitdist;
        if (distx > disty)
		{
			hitdist = disty;
			if (stepy <= 0)
			{
				map >>= 3;
				texture = _map[map+1];
			}
			texture |= 0x80;
        }
		else
		{
			hitdist = distx;
			if (stepx <= 0)
			{
				map &= 7;
				texture = _map[map+1];
			}
        }

        //short hh = SCREEN_WIDTH*2*0x10000L/hitdist;
		short hh = reciprocalfix(hitdist >> 8) >> 16;
		hh >>= 1;
		if (hh > SCREEN_HEIGHT/2)
			hh = SCREEN_HEIGHT/2;
		hs[ray] = hh;
		ts[ray] = texture;
	}

	// Draw
	for (u8 ray = 0; ray < SCREEN_WIDTH; ray++)
	{
        short y1 = 0;
        short y2 = SCREEN_HEIGHT-1;
		u8 hh = hs[ray];
        if (hh < (SCREEN_HEIGHT/2))
        {
            y1 = y2 = SCREEN_HEIGHT/2;
            y1 -= hh;
            y2 += hh;
        }

	color24_t color;
	MakeColor(hh,ts[ray],&color);

	Graphics_Fill(COLOR_BLACK24,y1);
	Graphics_Fill(color,y2-y1);
	Graphics_Fill(COLOR_BLACK24,SCREEN_HEIGHT-y2);
    }
}


void main_app_init()
{
	Doomed_Init();
}

void main_app_handle_button_down()
{
	_pressed = 1;
	_dir++;
}

void main_app_handle_button_up()
{
	_pressed = 0;
}

void main_app_loop()
{
	Doomed_Draw(0);
}

void main_app_handle_doz()
{
}

void main_app_handle_hardware_update(enum PulseHardwareEvent event)
{
}
