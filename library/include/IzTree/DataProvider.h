#ifndef IZTREE_DATAPROVIDER_H
#define IZTREE_DATAPROVIDER_H

#include "IzTree_Global.h"

#include <atomic>
#include <memory>

#include <QObject>

namespace IzTree
{
	struct TreeData;

	class IZTREESHARED_EXPORT DataProvider : public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(DataProvider)

	public:
		// ctor
		explicit DataProvider(QObject* parent = nullptr);

		// dtor
		virtual ~DataProvider() = default;

		// loads data for IzTree from external source - has to emit loadStarted() and loadEnded() signals
		virtual void loadData() = 0;

		// returns true if data DataProvider is currently working, controlled by loadStarted() and loadEnded() signals
		bool isWorking() const;

	private:
		// true if data DataProvider is currently working
		std::atomic_bool m_isWorking{ false };

	signals:
		// has to be emited when DataProvider starts loading data
		void loadEnded(bool result, std::shared_ptr<TreeData> data);

		// has to be emited when DataProvider stops loading data
		void loadStarted();
	};
}   // namespace IzTree

#endif   // IZTREE_ZTREEDATAPROVIDER_H
