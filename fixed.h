/* 
   Copyright (C) 2005 Benjamin C Meyer <ben+ksearch@meyerhome.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


/*********************************************************************
 * fixed - a fixed point number implementation. Meant to replace
 *         floats on systems lacking a real FPU.
 *
 *     Copyright © 2002, David Hedbor - david@eongames.com
 *
 * This code may be freely used for any purpose, either personal
 * or commercial, provided the authors copyright notice remains
 * intact.
 *
 *********************************************************************/

#ifndef HAVE_FIXED_HPP
    #define HAVE_FIXED_HPP

// Adjust for higher or lower precision. Higher number gives better
// precision but allows for smaller numbers. 
    #define NUM_POINT_BITS 15

// Macros to convert to/from ints and floats.
    #define I_TO_FIXED(x)   (static_cast<long long>(x)<<NUM_POINT_BITS)
    #define F_TO_FIXED(x)   static_cast<long>(x*I_TO_FIXED(1))
    #define FROM_FIXED_I(x) (x>>NUM_POINT_BITS)
    #define FROM_FIXED_F(x) (static_cast<float>(x) / I_TO_FIXED(1))

class fixed
{
    long my_val;
public:
    // Initialize to zero since even though a real float doesn't behave
    // like that. I'm originally a Pike programmer and like my variables
    // to be initialized per default (almost never would you not want that).
    fixed() : my_val(0)
    {
        
    }

    // Not much to say about the below constructors really. Note the lack of
    // long / long long here. Reason being that the fixed point class can't
    // store those values correctly anyway.
    fixed(float d) : my_val(F_TO_FIXED(d))
    {
        
    }  
    fixed(double d) : my_val(F_TO_FIXED(d))
    {
        
    }  
    fixed(int d) : my_val(I_TO_FIXED(d))
    {
        
    }

    // Operators to implicity convert a fixed to a int, unsigned int or float.
    operator int()     const
    {
        return (int)FROM_FIXED_I(my_val);
    }
    operator unsigned int() const
    {
        return (unsigned int)FROM_FIXED_I(my_val);
    }
    operator float()   const
    {
        return FROM_FIXED_F(my_val);
    }

    // This just returns the internal value representation. Not really
    // all that useful.
    long val() const
    {
        return my_val;
    }

    // Simply returns a copy of myself.
    const fixed  operator+() const
    {
        return *this;
    }
    // Return a negative copy of myself
    const fixed  operator-() const
    {
        fixed n; n.my_val = -my_val; return n;
    }

    // Below are the various assignment operators. 
    const fixed &operator=  (const fixed& rhs)
    {
        my_val = rhs.my_val;
        return *this;
    }
    const fixed &operator+= (const fixed& rhs)
    {
        my_val += rhs.my_val;
        return *this;
    }
    const fixed &operator-= (const fixed& rhs)
    {
        my_val -= rhs.my_val;
        return *this;
    }
    const fixed &operator*= (const fixed& rhs)
    {
        my_val = FROM_FIXED_I(static_cast<long long>(my_val) * rhs.my_val);
        return *this;
    }
    const fixed &operator/= (const fixed& rhs)
    {
        my_val = I_TO_FIXED(my_val) / rhs.my_val;
        return *this;
    }

    // The following 12 methods are friends so that we don't get ambiguous
    // matches when doing the various operations. theoretically they could
    // be member methods, but in reality that doesn't work.
    friend const fixed operator+ (const fixed& lhs, const fixed& rhs);
    friend const fixed operator- (const fixed& lhs, const fixed& rhs);
    friend const fixed operator* (const fixed& lhs, const fixed& rhs);
    friend const fixed operator/ (const fixed& lhs, const fixed& rhs);

    friend const fixed operator+ (const fixed& lhs, float rhs);
    friend const fixed operator- (const fixed& lhs, float rhs);
    friend const fixed operator* (const fixed& lhs, float rhs);
    friend const fixed operator/ (const fixed& lhs, float rhs);

    friend const fixed operator+ (const fixed& lhs, int rhs);
    friend const fixed operator- (const fixed& lhs, int rhs);
    friend const fixed operator* (const fixed& lhs, int rhs);
    friend const fixed operator/ (const fixed& lhs, int rhs);

    // These has to be non-member methods since the first argument is
    // not a fixed and C++ doesn't explicity do such conversions.
    friend const fixed operator+ (const float, const fixed&);
    friend const fixed operator- (const float, const fixed&);
    friend const fixed operator* (const float, const fixed&);
    friend const fixed operator/ (const float, const fixed&);
    friend const fixed operator+ (const int, const fixed&);
    friend const fixed operator- (const int, const fixed&);
    friend const fixed operator* (const int, const fixed&);
    friend const fixed operator/ (const int, const fixed&);
};


// fixed / fixed operators
inline const fixed operator+ (const fixed& lhs, const fixed& rhs)
{
    fixed n;
    n.my_val = lhs.my_val + rhs.my_val; 
    return n;
}
inline const fixed operator- (const fixed& lhs, const fixed& rhs)
{
    fixed n;
    n.my_val = lhs.my_val - rhs.my_val; 
    return n;
}
inline const fixed operator* (const fixed& lhs, const fixed& rhs)
{
    fixed n;
    n.my_val = FROM_FIXED_I(static_cast<long long>(lhs.my_val) * rhs.my_val);
    return n;
}
inline const fixed operator/ (const fixed& lhs, const fixed& rhs)
{
    fixed n;
    n.my_val = I_TO_FIXED(lhs.my_val) / rhs.my_val;
    return n;
}

// fixed / float operators
inline const fixed operator+ (const fixed& lhs, float rhs)
{
    return lhs + fixed(rhs);
}
inline const fixed operator- (const fixed& lhs, float rhs)
{
    return lhs - fixed(rhs);
}
inline const fixed operator* (const fixed& lhs, float rhs)
{
    return lhs * fixed(rhs);
}
inline const fixed operator/ (const fixed& lhs, float rhs)
{
    return lhs / fixed(rhs);
}

// fixed / int operators
inline const fixed operator+ (const fixed& lhs, int rhs)
{
    return lhs + fixed(rhs);
}
inline const fixed operator- (const fixed& lhs, int rhs)
{
    return lhs - fixed(rhs);
}
inline const fixed operator* (const fixed& lhs, int rhs)
{
    return lhs * fixed(rhs);
}
inline const fixed operator/ (const fixed& lhs, int rhs)
{
    return lhs / fixed(rhs);
}

// float / fixed operators
inline const  fixed operator+ (const float lhs, const fixed& rhs)
{
    return fixed(lhs) + rhs;
}
inline const  fixed operator- (const float lhs, const fixed& rhs)
{
    return fixed(lhs) - rhs;
}
inline const  fixed operator* (const float lhs, const fixed& rhs)
{
    return fixed(lhs) * rhs;
}
inline const  fixed operator/ (const float lhs, const fixed& rhs)
{
    return fixed(lhs) / rhs;
}


// int / fixed operators
inline const  fixed operator+ (const int lhs, const fixed& rhs)
{
    return fixed(lhs) + rhs;
}
inline const  fixed operator- (const int lhs, const fixed& rhs)
{
    return fixed(lhs) - rhs;
}
inline const  fixed operator* (const int lhs, const fixed& rhs)
{
    return fixed(lhs) * rhs;
}
inline const  fixed operator/ (const int lhs, const fixed& rhs)
{
    return fixed(lhs) / rhs;
}

#endif
