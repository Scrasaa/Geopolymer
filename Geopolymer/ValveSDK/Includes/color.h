//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef COLOR_H
#define COLOR_H

#ifdef _WIN32
#pragma once
#endif

#include "strtools.h"
#include <format>

//-----------------------------------------------------------------------------
// Purpose: Basic handler for an rgb set of colors
//			This class is fully inline
//-----------------------------------------------------------------------------
struct Color
{
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 0;

	bool operator==(Color other) const
	{
		return r == other.r && g == other.g && b == other.b && a == other.a;
	}

	bool operator!=(Color other) const
	{
		return r != other.r || g != other.g || b != other.b || a != other.a;
	}

	std::string ToHex() const
	{
		return std::format("\x7{:02X}{:02X}{:02X}", r, g, b);
	}

	std::string ToHexA() const
	{
		return std::format("\x8{:02X}{:02X}{:02X}{:02X}", r, g, b, a);
	}

	Color Lerp(Color to, float t) const
	{
		return {
		static_cast<unsigned char>(r + (to.r - r) * t),
		static_cast<unsigned char>(g + (to.g - g) * t),
		static_cast<unsigned char>(b + (to.b - b) * t),
		static_cast<unsigned char>(a + (to.a - a) * t)
		};
	}
};


#endif // COLOR_H