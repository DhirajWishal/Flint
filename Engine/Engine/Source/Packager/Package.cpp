// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/Packager/Package.hpp"

#include <sstream>

namespace Flint
{
	void Package::set(std::string&& name, std::vector<std::byte>&& bytes)
	{
		m_Entries[name] = PackageEntry(std::move(bytes));
	}

	void Package::set(std::string&& name, std::string&& string)
	{
		m_Entries[name] = PackageEntry(std::move(string));
	}

	Flint::PackageEntry Package::get(std::string_view&& name) const
	{
		return m_Entries.at(name.data());
	}

	void Package::save(std::filesystem::path&& path) const
	{
	}

	void Package::compile(std::filesystem::path&& path) const
	{
	}

	std::string Package::getVerbose() const
	{
		std::stringstream jsonStream;
		jsonStream << "{" << std::endl;

		for (const auto& [name, entry] : m_Entries)
		{
			jsonStream << name << ": {" << std::endl;

			jsonStream << "type: " << static_cast<uint32_t>(entry.getType()) << std::endl;

			jsonStream << "}" << std::endl;
		}

		jsonStream << "}" << std::endl;

		return jsonStream.str();
	}

	Flint::Package::BinaryString Package::getCompiled() const
	{
		return BinaryString();
	}
}