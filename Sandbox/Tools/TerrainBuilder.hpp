// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>

/**
 * Entity type enum.
 */
enum class EntityType : uint8_t
{
	Grass,
	Tree,
	Water
};

/**
 * Block structure.
 * This contains information of a single block (1m x 1m).
 */
struct Block final
{
	uint8_t m_Height = 0;
	EntityType m_Entity = EntityType::Grass;
};

/**
 * 2D Vector structure.
 */
struct Vec2 final
{
	float m_X = 0;
	float m_Y = 0;
};

/**
 * Terrain builder class.
 * This class generates a terrain using a 64 bit seed.
 * 
 * This object uses the Perlin Noise algorithm (https://en.wikipedia.org/wiki/Perlin_noise).
 */
class TerrainBuilder final
{
public:
	/**
	 * Explicit constructor.
	 *
	 * @param seed The seed to generate the terrain with.
	 */
	explicit TerrainBuilder(uint64_t seed) : m_Seed(seed) {}

	/**
	 * Update the terrain and return the block information for the point.
	 *
	 * @param x The X-Axis value.
	 * @param y The Y-Axis value.
	 * @param The block information to that position.
	 */
	[[nodiscard]] Block update(int32_t x, int32_t y);

private:
	/**
	 * Linearly interpolate between a0 and a1.
	 * Weight should be in the range of 0.0f - 1.0f.
	 *
	 * @param a0 The first parameter.
	 * @param a1 The second parameter.
	 * @param weight The weight value.
	 * @return The interpolated value.
	 */
	[[nodiscard]] float interpolate(float a0, float a1, float weight) const noexcept;

	/**
	 * Generate a random gradient value.
	 *
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @return The generated gradient.
	 */
	[[nodiscard]] Vec2 generateRandomGradient(int32_t x, int32_t y) const noexcept;

	/**
	 * Compute the dot product of the distance and gradient vectors.
	 */
	[[nodiscard]] float dotGridGradient(int32_t ix, int32_t iy, int32_t x, int32_t y) const noexcept;

private:
	const uint64_t m_Seed = 0;
};