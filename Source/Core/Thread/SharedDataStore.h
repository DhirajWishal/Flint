// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mutex>
#include <queue>

namespace Flint
{
	namespace Thread
	{
		inline std::mutex __SharedDataStoreMutex = {};

		template<class Type>
		class SharedDataClient {
		public:
			SharedDataClient() = default;

			void Push(const Type& data) { mQueue.push(data); }
			void Push(Type&& data) { mQueue.push(std::move(data)); }

			std::queue<Type, std::vector<Type>> mQueue;
		};

		template<class Type>
		class SharedDataStore {
		public:
			SharedDataStore() = default;
			~SharedDataStore() { for (auto itr : pClients) delete itr; }

			void Push(UI64 index, const Type& data)
			{
				std::lock_guard<std::mutex> guard{ __SharedDataStoreMutex };
				GetClient(index)->Push(data);
			}

			void Push(UI64 index, Type&& data)
			{
				std::lock_guard<std::mutex> guard{ __SharedDataStoreMutex };
				GetClient(index)->Push(std::move(data));
			}

			std::pair<SharedDataClient<Type>*, UI64> CreateClient()
			{
				INSERT_INTO_VECTOR(pClients, new SharedDataClient<Type>());
				return { pClients.back(), pClients.size() - 1 };
			}

			SharedDataClient<Type>* GetClient(UI64 index) const
			{
				return pClients[index];
			}

		private:
			std::vector<SharedDataClient<Type>*> pClients;
		};
	}
}