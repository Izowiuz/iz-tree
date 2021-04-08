#ifndef IZTREE_SQLDATAPROVIDER_H
#define IZTREE_SQLDATAPROVIDER_H

#include "IzTree_Global.h"

#include <future>

#include <QVariant>
#include <QElapsedTimer>

#include "DataProvider.h"

// TODO: komentarze
namespace IzTree
{
	// TODO: dodać możliwośc konfiguracji SQLdbc
	class IZTREESHARED_EXPORT SQLDataProvider : public DataProvider
	{
		Q_OBJECT
		Q_DISABLE_COPY(SQLDataProvider)

	public:
		// ctor
		// nodeIDColumn, parentIDColumn, nodeNameColumn and additionalDataColumns bound SQL columns with node parameters
		explicit SQLDataProvider(QObject* parent                          = nullptr,
								 const QString& nodeIDColumn              = QStringLiteral("nodeID"),
								 const QString& parentIDColumn            = QStringLiteral("parentID"),
								 const QString& nodeNameColumn            = QStringLiteral("nodeName"),
								 const QString& nodeDescriptionColumn     = QStringLiteral(""),
								 const QStringList& additionalDataColumns = {});

		// dtor
		~SQLDataProvider() final = default;

		// m_query getter / setter
		void setQuery(const QString& query);
		QString getQuery() const;

		// DataProvider interface - starts loading data
		void loadData() override;

	private:
		// future for loadData() task
		std::future<bool> m_dataFuture;

		// definitions of sql columns bound to node parameters
		QString m_query;
		QString m_nodeIDColumn;
		QString m_parentIDColumn;
		QString m_nodeNameColumn;
		QString m_nodeDescriptionColumn;
		QStringList m_additionalDataColumns;

		// debug timer
		QElapsedTimer m_loaderTimer;
	};
}   // namespace IzTree

#endif   // IZTREE_SQLDATAPROVIDER_H
