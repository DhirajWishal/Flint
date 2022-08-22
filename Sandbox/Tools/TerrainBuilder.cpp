// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "TerrainBuilder.hpp"

#include <cmath>

Block TerrainBuilder::update(int32_t x, int32_t y)
{
	// Determine grid cell coordinates
	const auto x0 = static_cast<int32_t>(floor(x));
	const auto x1 = x0 + 1;
	const auto y0 = static_cast<int32_t>(floor(y));
	const auto y1 = y0 + 1;

	// Determine interpolation weights
	// Could also use higher order polynomial/s-curve here
	const auto sx = x - static_cast<float>(x0);
	const auto sy = y - static_cast<float>(y0);

	// Interpolate between grid point gradients
	float n0, n1, ix0, ix1;

	n0 = dotGridGradient(x0, y0, x, y);
	n1 = dotGridGradient(x1, y0, x, y);
	ix0 = interpolate(n0, n1, sx);

	n0 = dotGridGradient(x0, y1, x, y);
	n1 = dotGridGradient(x1, y1, x, y);
	ix1 = interpolate(n0, n1, sx);

	const auto value = (interpolate(ix0, ix1, sy) * 0.5f) + 0.5f;

	Block block;
	block.m_Height = static_cast<uint8_t>(value * 255);

	return block;
}

float TerrainBuilder::interpolate(float a0, float a1, float weight) const noexcept
{
	if (0.0 > weight) return a0;
	if (1.0 < weight) return a1;
	return (a1 - a0) * ((weight * (weight * 6.0 - 15.0) + 10.0) * weight * weight * weight) + a0;
}

Vec2 TerrainBuilder::generateRandomGradient(int32_t x, int32_t y) const noexcept
{
	constexpr auto weight = 8 * sizeof(uint32_t);
	constexpr auto rotationWidth = weight / 2;

	x *= 3284157443; y ^= x << rotationWidth | x >> weight - rotationWidth;
	y *= 1911520717; x ^= y << rotationWidth | y >> weight - rotationWidth;
	x *= 2048419325;

	const float random = x * (3.14159265 / ~(~0u >> 1));

	Vec2 vetor;
	vetor.m_X = std::cos(random);
	vetor.m_Y = std::sin(random);

	return vetor;
}

float TerrainBuilder::dotGridGradient(int32_t ix, int32_t iy, int32_t x, int32_t y) const noexcept
{
	// Get gradient from integer coordinates
	const auto gradient = generateRandomGradient(ix, iy);

	// Compute the distance vector
	const auto dx = x - static_cast<float>(ix);
	const auto dy = y - static_cast<float>(iy);

	// Compute the dot-product
	return (dx * gradient.m_X + dy * gradient.m_Y);
}
