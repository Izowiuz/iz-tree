#include "IzTree/SQLDataProvider.h"

#include <algorithm>

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "IzSQLUtilities/SQLConnector.h"
#include "IzSQLUtilities/SQLErrorEvent.h"

#include "TreeData.h"

IzTree::SQLDataProvider::SQLDataProvider(QObject* parent, const QString& nodeIDColumn, const QString& parentIDColumn, const QString& nodeNameColumn, const QString& nodeDescriptionColumn, const QStringList& additionalDataColumns)
	: DataProvider(parent)
	, m_nodeIDColumn(nodeIDColumn)
	, m_parentIDColumn(parentIDColumn)
	, m_nodeNameColumn(nodeNameColumn)
	, m_nodeDescriptionColumn(nodeDescriptionColumn)
	, m_additionalDataColumns(additionalDataColumns)
{
}

void IzTree::SQLDataProvider::setQuery(const QString& query)
{
	m_query = query;
	// TODO: tu jakąś normalną sanityzację można by zrobić
	m_query.remove(QStringLiteral("DELETE"), Qt::CaseInsensitive);
	m_query.remove(QStringLiteral("UPDATE"), Qt::CaseInsensitive);
	m_query.remove(QStringLiteral("DROP"), Qt::CaseInsensitive);
	m_query.remove(QStringLiteral("CREATE"), Qt::CaseInsensitive);
}

QString IzTree::SQLDataProvider::getQuery() const
{
	return m_query;
}

void IzTree::SQLDataProvider::loadData()
{
	if (isWorking()) {
		qWarning() << "Data provider is currently working.";
		return;
	}

	if (m_query.isEmpty()) {
		qWarning() << "No SQL query set.";
		return;
	}

	m_dataFuture = std::async(std::launch::async, [this]() -> bool {
		emit loadStarted();

		IzSQLUtilities::SqlConnector db(IzSQLUtilities::DatabaseType::MSSQL);
		if (db.getConnection().isOpen()) {
			QSqlQuery q(db.getConnection());
			q.setForwardOnly(true);
			m_loaderTimer.start();

			if (q.exec(m_query)) {
				qDebug() << "Tree SQL query time:" << m_loaderTimer.elapsed() << "msecs.";
				m_loaderTimer.restart();
				QHash<QString, int> additionalDataColumns;

				for (const auto& column : qAsConst(m_additionalDataColumns)) {
					auto columnIdx = q.record().indexOf(column);

					if (columnIdx == -1) {
						qCritical() << "Required column:" << column << "not found in query results.";
						emit loadEnded(false, {});
						return false;
					}

					additionalDataColumns.insert(column, columnIdx);
				}

				auto nodeIDidx = q.record().indexOf(m_nodeIDColumn);
				if (nodeIDidx == -1) {
					qCritical() << "Required column:" << m_nodeIDColumn << " not found in query results.";
					emit loadEnded(false, {});
					return false;
				}

				auto parentIDidx = q.record().indexOf(m_parentIDColumn);
				if (parentIDidx == -1) {
					qCritical() << "Required column:" << m_parentIDColumn << " not found in query results.";
					emit loadEnded(false, {});
					return false;
				}

				auto nodeNameidx = q.record().indexOf(m_nodeNameColumn);
				if (nodeNameidx == -1) {
					qCritical() << "Required column:" << m_nodeNameColumn << "not found in query results.";
					emit loadEnded(false, {});
					return false;
				}

				auto nodeDescriptionIdx = -1;
				if (!m_nodeDescriptionColumn.isEmpty()) {
					nodeDescriptionIdx = q.record().indexOf(m_nodeDescriptionColumn);
					if (nodeDescriptionIdx == -1) {
						qCritical() << "Required column:" << m_nodeDescriptionColumn << "not found in query results.";
						emit loadEnded(false, {});
						return false;
					}
				}

				// temporary data
				auto data = std::make_shared<TreeData>();
				QVariantMap additionalData;
				QHashIterator<QString, int> i(additionalDataColumns);

				int firstNodeID = std::numeric_limits<int>::max();

				while (q.next()) {
					while (i.hasNext()) {
						i.next();
						additionalData.insert(i.key(), q.value(i.value()));
					}

					if (!data->treeData.contains(q.value(nodeIDidx).toInt())) {
						if (nodeDescriptionIdx != -1) {
							data->treeData.insert(q.value(nodeIDidx).toInt(), { q.value(nodeIDidx).toInt(), q.value(parentIDidx).toInt(), q.value(nodeNameidx).toString(), q.value(nodeDescriptionIdx).toString(), additionalData }).key();
						} else {
							data->treeData.insert(q.value(nodeIDidx).toInt(), { q.value(nodeIDidx).toInt(), q.value(parentIDidx).toInt(), q.value(nodeNameidx).toString(), "", additionalData }).key();
						}

						if (q.value(nodeIDidx).toInt() < firstNodeID) {
							firstNodeID = q.value(nodeIDidx).toInt();
						}
					} else {
						qWarning() << "Got duplicated nodeID:" << q.value(nodeIDidx).toInt();
					}

					additionalData.clear();
					i.toFront();
				}


				if (!data->treeData.contains(firstNodeID)) {
					qInfo() << "Tree model is empty.";
					emit loadEnded(true, data);
					return true;
				}

				data->rootNode = &data->treeData[firstNodeID];

				qDebug() << "Tree SQL data parse time:" << m_loaderTimer.elapsed() << "msecs.";
				emit loadEnded(true, data);
				return true;
			}

			qCritical() << q.lastError().text();
			IzSQLUtilities::SQLErrorEvent::postSQLError(q.lastError());
			emit loadEnded(false, {});
			return false;
		}

		IzSQLUtilities::SQLErrorEvent::postSQLError(db.lastError());
		emit loadEnded(false, {});
		return false;
	});
}
