// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/FObject.hpp"

namespace Flint
{
	namespace MaterialProperties
	{
		/**
		 * Material type enum.
		 */
		enum class MaterialType : UI8 {
			FLOAT,
			FLOAT_2,
			FLOAT_3,
			FLOAT_4,

			DOUBLE,
			DOUBLE_2,
			DOUBLE_3,
			DOUBLE_4,
		};

		/**
		 * Flint material property class.
		 * This object is the base class to all the supported properties.
		 */
		class Property : public FObject
		{
		public:
			Property(const MaterialType type) : mType(type) {}

			const MaterialType GetType() const { return mType; }

		protected:
			MaterialType mType = MaterialType::FLOAT;
		};

		/**
		 * Single float property.
		 */
		class Float final : public Property
		{
		public:
			/**
			 * Constructor.
			 * 
			 * @param value: The value to store.
			 */
			Float(const float value) : Property(MaterialType::FLOAT), mValue(value) {}

			float mValue = 0.0f;
		};

		/**
		 * Two float property.
		 */
		class Float2 final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param values: The values to store.
			 */
			Float2(const float values[2]) : Property(MaterialType::FLOAT_2)
			{
				mValues[0] = values[0];
				mValues[1] = values[1];
			}

			float mValues[2] = {};
		};

		/**
		 * Three float property.
		 */
		class Float3 final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param values: The values to store.
			 */
			Float3(const float values[3]) : Property(MaterialType::FLOAT_3)
			{
				mValues[0] = values[0];
				mValues[1] = values[1];
				mValues[2] = values[2];
			}

			float mValues[3] = {};
		};

		/**
		 * Four float property.
		 */
		class Float4 final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param values: The values to store.
			 */
			Float4(const float values[4]) : Property(MaterialType::FLOAT_4)
			{
				mValues[0] = values[0];
				mValues[1] = values[1];
				mValues[2] = values[2];
				mValues[3] = values[3];
			}

			float mValues[4] = {};
		};

		/**
		 * Single double property.
		 */
		class Double final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param value: The value to store.
			 */
			Double(const double value) : Property(MaterialType::DOUBLE), mValue(value) {}

			double mValue = 0.0f;
		};

		/**
		 * Two float property.
		 */
		class Double2 final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param values: The values to store.
			 */
			Double2(const double values[2]) : Property(MaterialType::DOUBLE_2)
			{
				mValues[0] = values[0];
				mValues[1] = values[1];
			}

			double mValues[2] = {};
		};

		/**
		 * Three float property.
		 */
		class Double3 final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param values: The values to store.
			 */
			Double3(const double values[3]) : Property(MaterialType::DOUBLE_3)
			{
				mValues[0] = values[0];
				mValues[1] = values[1];
				mValues[2] = values[2];
			}

			double mValues[3] = {};
		};

		/**
		 * Four float property.
		 */
		class Double4 final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param values: The values to store.
			 */
			Double4(const double values[4]) : Property(MaterialType::DOUBLE_4)
			{
				mValues[0] = values[0];
				mValues[1] = values[1];
				mValues[2] = values[2];
				mValues[3] = values[3];
			}

			double mValues[4] = {};
		};
	}
}
