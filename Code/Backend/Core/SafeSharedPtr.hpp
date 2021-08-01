// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <atomic>
#include <memory>

namespace Flint
{
	template<class Type>
	class SafeSharedPtr
	{
	public:
		constexpr SafeSharedPtr() = default;
		constexpr SafeSharedPtr(std::nullptr_t) {}
		constexpr SafeSharedPtr(Type* pPointer) { pAtomicPointer.store(std::shared_ptr<Type>(pPointer)); }
		constexpr SafeSharedPtr(const std::shared_ptr<Type>& pPointer) { pAtomicPointer.store(pPointer); }
		constexpr SafeSharedPtr(const SafeSharedPtr<Type>& pPointer) { pAtomicPointer.store(pPointer.pAtomicPointer); }

		template<class Type2>
		constexpr SafeSharedPtr(const SafeSharedPtr<Type2>& pPointer) { pAtomicPointer.store(pPointer.pAtomicPointer); }

		template<class Type2>
		constexpr SafeSharedPtr(const std::shared_ptr<Type2>& pPointer) { pAtomicPointer.store(pPointer); }

		constexpr std::atomic<std::shared_ptr<Type>>& get_atomic_ptr() { return pAtomicPointer; }
		constexpr const std::atomic<std::shared_ptr<Type>>& get_atomic_ptr() const { return pAtomicPointer; }

		constexpr std::shared_ptr<Type> get_shared_ptr() const { return pAtomicPointer.load(); }
		constexpr void store_shared_ptr(std::shared_ptr<Type> pPointer) { pAtomicPointer.store(pPointer); }

		constexpr Type* get() const { return pAtomicPointer.load().get(); }

	public:
		constexpr SafeSharedPtr<Type>& operator=(const SafeSharedPtr& other) { pAtomicPointer.store(other.pAtomicPointer); return *this; }
		constexpr SafeSharedPtr<Type>& operator=(const std::atomic<std::shared_ptr<Type>>& other) { pAtomicPointer.store(other); return *this; }
		constexpr SafeSharedPtr<Type>& operator=(const std::shared_ptr<Type>& other) { pAtomicPointer.store(other); return *this; }

		constexpr operator std::atomic<std::shared_ptr<Type>>& () { return get_atomic_ptr(); }
		constexpr operator const std::atomic<std::shared_ptr<Type>>& () const { return get_atomic_ptr(); }

		constexpr operator std::shared_ptr<Type>() { return get_shared_ptr(); }

		template<class Type2>
		constexpr operator std::shared_ptr<Type2>() { return get_shared_ptr(); }

		constexpr Type* operator->() const { return get(); }

		constexpr Type& operator*() { return *pAtomicPointer.load().get(); }
		constexpr const Type operator*() const { return *pAtomicPointer.load().get(); }

		constexpr explicit operator bool() const { return pAtomicPointer.load().get() != nullptr; }

	private:
		std::atomic<std::shared_ptr<Type>> pAtomicPointer;
	};
}