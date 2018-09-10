// Copyright 2017-2018 Pharap
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "Details.h"

FIXED_POINTS_BEGIN_NAMESPACE

//
// Declaration
//

template< unsigned Integer, unsigned Fraction >
class UFixed
{
public:
	constexpr const static uintmax_t IntegerSize = Integer;
	constexpr const static uintmax_t FractionSize = Fraction;
	constexpr const static uintmax_t LogicalSize = IntegerSize + FractionSize;
	constexpr const static uintmax_t Scale = UINTMAX_C(1) << FractionSize;

public:
	static_assert(LogicalSize <= FIXED_POINTS_DETAILS::BitSize<uintmax_t>::Value, "Platform does not have a native type large enough for UFixed.");

public:
	using IntegerType = FIXED_POINTS_DETAILS::LeastUInt<IntegerSize>;
	using FractionType = FIXED_POINTS_DETAILS::LeastUInt<FractionSize>;
	using InternalType = FIXED_POINTS_DETAILS::LeastUInt<LogicalSize>;

	constexpr const static uintmax_t InternalSize = FIXED_POINTS_DETAILS::BitSize<InternalType>::Value;
	
	using ShiftType = FIXED_POINTS_DETAILS::LeastUInt<LogicalSize>;
	using MaskType = FIXED_POINTS_DETAILS::LeastUInt<LogicalSize>;
		
public:
	constexpr const static ShiftType IntegerShift = FractionSize;
	constexpr const static ShiftType FractionShift = 0;
	
	constexpr const static MaskType IntegerMask = FIXED_POINTS_DETAILS::IdentityMask<IntegerSize>::Value;
	constexpr const static MaskType FractionMask = FIXED_POINTS_DETAILS::IdentityMask<FractionSize>::Value;
	
	constexpr const static MaskType IdentityMask = (IntegerMask << IntegerShift) | (FractionMask << FractionShift);
	
	constexpr const static MaskType MidpointMask = FIXED_POINTS_DETAILS::MsbMask<FractionSize>::Value;
	constexpr const static MaskType LesserMidpointMask = MidpointMask - 1;

protected:
	class RawType
	{
	private:
		const InternalType value;

	public:
		constexpr inline explicit RawType(const InternalType & value) : value(value) {}
		constexpr inline explicit operator InternalType(void) const { return this->value; }
	};

protected:
	InternalType value;

protected:
	constexpr UFixed(const RawType & value);
	
public:
	constexpr UFixed(void);
	constexpr UFixed(const IntegerType & integer, const FractionType & fraction);
	constexpr UFixed(const char & value);
	constexpr UFixed(const unsigned char & value);
	constexpr UFixed(const signed char & value);
	constexpr UFixed(const unsigned short int & value);
	constexpr UFixed(const signed short int & value);
	constexpr UFixed(const unsigned int & value);
	constexpr UFixed(const signed int & value);
	constexpr UFixed(const unsigned long int & value);
	constexpr UFixed(const signed long int & value);
	constexpr UFixed(const unsigned long long int & value);
	constexpr UFixed(const signed long long int & value);
	constexpr UFixed(const double & value);
	constexpr UFixed(const float & value);
	constexpr UFixed(const long double & value);
	
public:
	constexpr InternalType getInternal(void) const;
	constexpr IntegerType getInteger(void) const;
	constexpr FractionType getFraction(void) const;

	constexpr explicit operator IntegerType(void) const;
	constexpr explicit operator float(void) const;
	constexpr explicit operator double(void) const;
	constexpr explicit operator long double(void) const;

	template< unsigned IntegerOut, unsigned FractionOut >
	constexpr explicit operator UFixed<IntegerOut, FractionOut>(void) const;

	constexpr static UFixed fromInternal(const InternalType & value);

	UFixed & operator ++(void);
	UFixed & operator --(void);
	UFixed & operator +=(const UFixed & other);
	UFixed & operator -=(const UFixed & other);
	UFixed & operator *=(const UFixed & other);
	UFixed & operator /=(const UFixed & other);
	
public:
	constexpr const static UFixed Epsilon = UFixed::fromInternal(1);
	constexpr const static UFixed MinValue = UFixed::fromInternal(0);
	constexpr const static UFixed MaxValue = UFixed::fromInternal(~0);
	
	// 40 digits is probably enough for these
	constexpr const static UFixed Pi = 3.1415926535897932384626433832795028841971;
	constexpr const static UFixed E = 2.718281828459045235360287471352662497757;
	constexpr const static UFixed Phi = 1.6180339887498948482045868343656381177203;
	constexpr const static UFixed Tau = 6.2831853071795864769252867665590057683943;
};


//
// Free functions
//

template< unsigned Integer, unsigned Fraction >
constexpr UFixed<Integer * 2, Fraction * 2> multiply(const UFixed<Integer, Fraction> & left, const UFixed<Integer, Fraction> & right);


//
// Basic Logic Operations
//

template< unsigned Integer, unsigned Fraction >
constexpr bool operator ==(const UFixed<Integer, Fraction> & left, const UFixed<Integer, Fraction> & right);

template< unsigned Integer, unsigned Fraction >
constexpr bool operator !=(const UFixed<Integer, Fraction> & left, const UFixed<Integer, Fraction> & right);

template< unsigned Integer, unsigned Fraction >
constexpr bool operator <(const UFixed<Integer, Fraction> & left, const UFixed<Integer, Fraction> & right);

template< unsigned Integer, unsigned Fraction >
constexpr bool operator >(const UFixed<Integer, Fraction> & left, const UFixed<Integer, Fraction> & right);

template< unsigned Integer, unsigned Fraction >
constexpr bool operator <=(const UFixed<Integer, Fraction> & left, const UFixed<Integer, Fraction> & right);

template< unsigned Integer, unsigned Fraction >
constexpr bool operator >=(const UFixed<Integer, Fraction> & left, const UFixed<Integer, Fraction> & right);

//
// Inter-size Logic Operations
//

template< unsigned IntegerLeft, unsigned FractionLeft, unsigned IntegerRight, unsigned FractionRight >
constexpr bool operator ==(const UFixed<IntegerLeft, FractionLeft> & left, const UFixed<IntegerRight, FractionRight> & right);

template< unsigned IntegerLeft, unsigned FractionLeft, unsigned IntegerRight, unsigned FractionRight >
constexpr bool operator !=(const UFixed<IntegerLeft, FractionLeft> & left, const UFixed<IntegerRight, FractionRight> & right);

template< unsigned IntegerLeft, unsigned FractionLeft, unsigned IntegerRight, unsigned FractionRight >
constexpr bool operator <(const UFixed<IntegerLeft, FractionLeft> & left, const UFixed<IntegerRight, FractionRight> & right);

template< unsigned IntegerLeft, unsigned FractionLeft, unsigned IntegerRight, unsigned FractionRight >
constexpr bool operator >(const UFixed<IntegerLeft, FractionLeft> & left, const UFixed<IntegerRight, FractionRight> & right);

template< unsigned IntegerLeft, unsigned FractionLeft, unsigned IntegerRight, unsigned FractionRight >
constexpr bool operator <=(const UFixed<IntegerLeft, FractionLeft> & left, const UFixed<IntegerRight, FractionRight> & right);

template< unsigned IntegerLeft, unsigned FractionLeft, unsigned IntegerRight, unsigned FractionRight >
constexpr bool operator >=(const UFixed<IntegerLeft, FractionLeft> & left, const UFixed<IntegerRight, FractionRight> & right);

//
// Basic Arithmetic Operations
//

template< unsigned Integer, unsigned Fraction >
constexpr UFixed<Integer, Fraction> operator +(const UFixed<Integer, Fraction> & left, const UFixed<Integer, Fraction> & right);

template< unsigned Integer, unsigned Fraction >
constexpr UFixed<Integer, Fraction> operator -(const UFixed<Integer, Fraction> & left, const UFixed<Integer, Fraction> & right);

template< unsigned Integer, unsigned Fraction >
constexpr UFixed<Integer, Fraction> operator *(const UFixed<Integer, Fraction> & left, const UFixed<Integer, Fraction> & right);

template< unsigned Integer, unsigned Fraction >
constexpr UFixed<Integer, Fraction> operator /(const UFixed<Integer, Fraction> & left, const UFixed<Integer, Fraction> & right);

//
// Inter-size Arithmetic Operations
//

template< unsigned IntegerLeft, unsigned FractionLeft, unsigned IntegerRight, unsigned FractionRight >
constexpr auto operator +(const UFixed<IntegerLeft, FractionLeft> & left, const UFixed<IntegerRight, FractionRight> & right)
	-> FIXED_POINTS_DETAILS::LargerType< UFixed<IntegerLeft, FractionLeft>, UFixed<IntegerRight, FractionRight> >;

template< unsigned IntegerLeft, unsigned FractionLeft, unsigned IntegerRight, unsigned FractionRight >
constexpr auto operator -(const UFixed<IntegerLeft, FractionLeft> & left, const UFixed<IntegerRight, FractionRight> & right)
	-> FIXED_POINTS_DETAILS::LargerType< UFixed<IntegerLeft, FractionLeft>, UFixed<IntegerRight, FractionRight> >;

template< unsigned IntegerLeft, unsigned FractionLeft, unsigned IntegerRight, unsigned FractionRight >
constexpr auto operator *(const UFixed<IntegerLeft, FractionLeft> & left, const UFixed<IntegerRight, FractionRight> & right)
	-> FIXED_POINTS_DETAILS::LargerType< UFixed<IntegerLeft, FractionLeft>, UFixed<IntegerRight, FractionRight> >;

template< unsigned IntegerLeft, unsigned FractionLeft, unsigned IntegerRight, unsigned FractionRight >
inline constexpr auto operator /(const UFixed<IntegerLeft, FractionLeft> & left, const UFixed<IntegerRight, FractionRight> & right)
	-> FIXED_POINTS_DETAILS::LargerType< UFixed<IntegerLeft, FractionLeft>, UFixed<IntegerRight, FractionRight> >;
	
FIXED_POINTS_END_NAMESPACE

#include "UFixedMemberFunctions.h"
#include "UFixedFreeFunctions.h"