// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "PackageEntry.hpp"

#include <string>
#include <filesystem>
#include <unordered_map>

namespace Flint
{
	/**
	 * Package class.
	 * This class can be used to store almost any type of data in a Flint package (*.fpkg) file.
	 *
	 * Flint package files have two main variations. Verbose mode and the compiled mode. Verbose mode will save everything in a json format and will store everything in a readable
	 * format. The compiled version will be compiled down as a binary format.
	 */
	class Package final
	{
		using BinaryString = std::basic_string<std::byte>;

	public:
		/**
		 * Default constructor.
		 */
		Package() = default;

		/**
		 * Default destructor.
		 */
		~Package() = default;

		/**
		 * Store a byte array in the package.
		 *
		 * @param name The name of the data to be stored.
		 * @param bytes The bytes to be added.
		 */
		void set(std::string&& name, std::vector<std::byte>&& bytes);

		/**
		 * Store a string in the package.
		 *
		 * @param name The name of the data to be stored.
		 * @param string The string to set.
		 */
		void set(std::string&& name, std::string&& string);

		/**
		 * Get an entry from the package.
		 *
		 * @param name The name of the entry.
		 * @return The stored entry.
		 */
		[[nodiscard]] PackageEntry get(std::string_view&& name) const;

		/**
		 * Save the package in a readable format (json).
		 *
		 * @param path The path to save everything to.
		 */
		void save(std::filesystem::path&& path) const;

		/**
		 * Compile and save to a file as specified.
		 *
		 * @param path THe path to save everything to.
		 */
		void compile(std::filesystem::path&& path) const;

		/**
		 * Get the verbose output.
		 *
		 * @return The output string.
		 */
		[[nodiscard]] std::string getVerbose() const;

		/**
		 * Get the compiled output.
		 *
		 * @return The output binary.
		 */
		[[nodiscard]] BinaryString getCompiled() const;

	private:
		std::unordered_map<std::string, PackageEntry> m_Entries;
		std::vector<std::byte> m_Pool;
	};
}