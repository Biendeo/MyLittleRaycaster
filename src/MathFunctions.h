#pragma once

#include <cmath>

/// <summary>
/// A unit specifying degrees (stored as a double).
/// </summary>
using Degrees = double;
/// <summary>
/// A unit specifying radians (stored as a double).
/// </summary>
using Radians = double;

/// <summary>
/// Returns a value of pi calculated by the atan function.
/// </summary>
/// <returns></returns>
inline double Pi() {
	return std::atan(1.0) * 4.0;
}

/// <summary>
/// Converts an angle represented as degrees to radians.
/// </summary>
/// <param name="degrees"></param>
/// <returns></returns>
inline Radians ToRadians(Degrees degrees) {
	return degrees / 180.0 * Pi();
}

/// <summary>
/// Converts an angle represented as radians to degrees.
/// </summary>
/// <param name="radians"></param>
/// <returns></returns>
inline Degrees ToDegrees(Radians radians) {
	return radians * 180.0 / Pi();
}

/// <summary>
/// Linearly interpolates between two values <see cref="min"/> and <see cref="max"/> with a given coefficient
/// <see cref="t"/>. <see cref="t"/> is ideally between the range of 0 and 1, but can theoretically act beyond.
/// </summary>
/// <param name="min"></param>
/// <param name="max"></param>
/// <param name="t"></param>
/// <returns></returns>
inline double Lerp(double min, double max, double t) {
	return t * min + (1 - t) * max;
}

/// <summary>
/// Determines the coefficient t given a minimum and a maximum value, and the value of a point. This should return the
/// same value as the t value passed into the <see cref="Lerp"/> function.
/// </summary>
/// <param name="min"></param>
/// <param name="max"></param>
/// <param name="val"></param>
/// <returns></returns>
inline double ReverseLerp(double min, double max, double val) {
	return (val - min) / (max - min);
}

/// <summary>
/// Returns whether a value <see cref="val"/> lies between two points <see cref="min"/> and <see cref="max"/>. This
/// works both if <see cref="min"/> is less than <see cref="max"/> or vice versa.
/// </summary>
/// <param name="min"></param>
/// <param name="max"></param>
/// <param name="val"></param>
/// <returns></returns>
inline bool IsBetween(double min, double max, double val) {
	if (min == 0.0 && max == 1.0) {
		return val >= 0.0 && val <= 1.0;
	} else if (min > max) {
		return IsBetween(max, min, val);
	} else {
		return IsBetween(0.0, 1.0, ReverseLerp(min, max, val));
	}
}

/// <summary>
/// Returns the clamped value between the given range. Effectively, if <see cref="val"/> is less than <see cref="min"/>,
/// <see cref="min"/> is returned. If <see cref="val"/> is greater than <see cref="max"/>, <see cref="max"/> is
/// returned. If neither, <see cref="val"/> is returned.
/// </summary>
/// <param name="min"></param>
/// <param name="max"></param>
/// <param name="val"></param>
/// <returns></returns>
inline double Clamp(double min, double max, double val) {
	return val < min ? min : (val > max ? max : val);
}

/// <summary>
/// Returns the minimum of the two objects, given that the less than operator is valid.
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
template <typename T>
inline T& Min(T& a, T& b) {
	return a < b ? a : b;
}

/// <summary>
/// Returns the minimum of the two objects, given that the greater than operator is valid.
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
template <typename T>
inline T& Max(T& a, T& b) {
	return a > b ? a : b;
}

/// <summary>
/// Returns the magnitude of the vector created by the two co-ordinates.
/// This is simply the square root of a^2 + b^2.
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
inline double Pythagoras(double a, double b) {
	return std::sqrt(a * a + b * b);
}

/// <summary>
/// Returns the magnitude of the vector created by the three co-ordinates.
/// This is simply the square root of a^2 + b^2 + c^2.
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="c"></param>
/// <returns></returns>
inline double Pythagoras(double a, double b, double c) {
	return std::sqrt(a * a + b * b + c * c);
}

/// <summary>
/// Returns the absolute value of the given number.
/// </summary>
/// <param name="x"></param>
/// <returns></returns>
inline double Abs(double x) {
	return x < 0.0 ? -x : x;
}