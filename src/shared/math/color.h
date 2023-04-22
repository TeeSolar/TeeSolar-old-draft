/*
 * Copyright (C) 2016 necropotame (necropotame@gmail.com)
 * 
 * This file is part of TeeUniverse.
 * 
 * TeeUniverse is free software: you can redistribute it and/or  modify
 * it under the terms of the GNU Affero General Public License, version 3,
 * as published by the Free Software Foundation.
 *
 * TeeUniverse is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with TeeUniverse.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Some parts of this file comes from other projects.
 * These parts are itendified in this file by the following block:
 * 
 * FOREIGN CODE BEGIN: ProjectName *************************************
 * 
 * FOREIGN CODE END: ProjectName ***************************************
 * 
 * If ProjectName is "TeeWorlds", then this part of the code follows the
 * TeeWorlds licence:
 *      (c) Magnus Auvinen. See LICENSE_TEEWORLDS in the root of the
 *      distribution for more information. If you are missing that file,
 *      acquire a complete release at teeworlds.com.
 */

#ifndef __SHARED_MATH_COLOR__
#define __SHARED_MATH_COLOR__

/* FOREIGN CODE BEGIN: TeeWorlds **************************************/

#include "vector.h"

/*
	Title: Color handling
*/

/*
	Function: HueToRgb
		Converts Hue to RGB
*/
inline float HueToRgb(float v1, float v2, float h)
{
	if(h < 0.0f) h += 1;
	if(h > 1.0f) h -= 1;
	if((6.0f * h) < 1.0f) return v1 + (v2 - v1) * 6.0f * h;
	if((2.0f * h) < 1.0f) return v2;
	if((3.0f * h) < 2.0f) return v1 + (v2 - v1) * ((2.0f/3.0f) - h) * 6.0f;
	return v1;
}

/*
	Function: HslToRgb
		Converts HSL to RGB
*/
inline vec3 HslToRgb(vec3 HSL)
{
	if(HSL.s == 0.0f)
		return vec3(HSL.l, HSL.l, HSL.l);
	else
	{
		float v2 = HSL.l < 0.5f ? HSL.l * (1.0f + HSL.s) : (HSL.l+HSL.s) - (HSL.s*HSL.l);
		float v1 = 2.0f * HSL.l - v2;

		return vec3(HueToRgb(v1, v2, HSL.h + (1.0f/3.0f)), HueToRgb(v1, v2, HSL.h), HueToRgb(v1, v2, HSL.h - (1.0f/3.0f)));
	}
}

/*
	Function: HexToRgba
		Converts Hex to Rgba

	Remarks: Hex should be RGBA8
*/
inline vec4 HexToRgba(int hex)
{
	vec4 c;
	c.r = ((hex >> 24) & 0xFF) / 255.0f;
	c.g = ((hex >> 16) & 0xFF) / 255.0f;
	c.b = ((hex >> 8) & 0xFF) / 255.0f;
	c.a = (hex & 0xFF) / 255.0f;

	return c;
}

/*
	Function: HsvToRgb
		Converts Hsv to Rgb
*/
inline vec3 HsvToRgb(vec3 hsv)
{
	int h = int(hsv.x * 6.0f);
	float f = hsv.x * 6.0f - h;
	float p = hsv.z * (1.0f - hsv.y);
	float q = hsv.z * (1.0f - hsv.y * f);
	float t = hsv.z * (1.0f - hsv.y * (1.0f - f));

	vec3 rgb = vec3(0.0f, 0.0f, 0.0f);

	switch(h % 6)
	{
	case 0:
		rgb.r = hsv.z;
		rgb.g = t;
		rgb.b = p;
		break;

	case 1:
		rgb.r = q;
		rgb.g = hsv.z;
		rgb.b = p;
		break;

	case 2:
		rgb.r = p;
		rgb.g = hsv.z;
		rgb.b = t;
		break;

	case 3:
		rgb.r = p;
		rgb.g = q;
		rgb.b = hsv.z;
		break;

	case 4:
		rgb.r = t;
		rgb.g = p;
		rgb.b = hsv.z;
		break;

	case 5:
		rgb.r = hsv.z;
		rgb.g = p;
		rgb.b = q;
		break;
	}

	return rgb;
}

/*
	Function: RgbToHsv
		Converts Rgb to Hsv
*/
inline vec3 RgbToHsv(vec3 rgb)
{
	float h_min = min(min(rgb.r, rgb.g), rgb.b);
	float h_max = max(max(rgb.r, rgb.g), rgb.b);

	// hue
	float hue = 0.0f;

	if(h_max == h_min)
		hue = 0.0f;
	else if(h_max == rgb.r)
		hue = (rgb.g-rgb.b) / (h_max-h_min);
	else if(h_max == rgb.g)
		hue = 2.0f + (rgb.b-rgb.r) / (h_max-h_min);
	else
		hue = 4.0f + (rgb.r-rgb.g) / (h_max-h_min);

	hue /= 6.0f;

	if(hue < 0.0f)
		hue += 1.0f;

	// saturation
	float s = 0.0f;
	if(h_max != 0.0f)
		s = (h_max - h_min)/h_max;

	// lightness
	float l = h_max;

	return vec3(hue, s, l);
}

/* FOREIGN CODE END: TeeWorlds ****************************************/

/*
 * Generate a color with a hue that is that farthest possible from the N previous ones
 */
inline vec4 ColorSequence(int N, float Saturation, float Lightness, float Alpha)
{
	float Hue = 0.0f;
	float Power = 0.5f;
	while(N != 0)
	{
		if(N%2)
			Hue += Power;
		Power /= 2.0f;
		N /= 2;
	}
	
	vec3 Rbg = HsvToRgb(vec3(Hue, Saturation, Lightness));
	
	return vec4(Rbg.r, Rbg.g, Rbg.b, Alpha);
}

#endif
