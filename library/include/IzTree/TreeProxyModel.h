#ifndef IZTREE_TREEPROXYMODEL_H
#define IZTREE_TREEPROXYMODEL_H

#include <QSet>

#include "IzTree_Global.h"

#include <QFutureWatcher>
#include <QRegularExpression>
#include <QSortFilterProxyModel>

// TODO: porządek z funkcjonalnościami
namespace IzTree
{
	class Tree;
	class TreeModel;

	class IZTREESHARED_EXPORT TreeProxyModel : public QSortFilterProxyModel
	{
		Q_OBJECT
		Q_DISABLE_COPY(TreeProxyModel)

	public:
		// ctor
		explicit TreeProxyModel(Tree* tree, QObject* parent = nullptr);

		// dtor
		~TreeProxyModel() = default;

		// returns source row for given proxy row
		// returns -1 if invalid index was requested
		Q_INVOKABLE int sourceRow(int proxyRow) const;

		// returns proxy row for given source row
		// returns -1 if invalid index was requested
		Q_INVOKABLE int proxyRow(int sourceRow) const;

		// QAbstractItemModel's data() wrapper for QML usage
		Q_INVOKABLE QVariant getData(int proxyRow, const QString& role) const;

		// QAbstractItemModel interface
		bool setData(const QModelIndex& index, const QVariant& value, int role) override;

		// enables filtering using given value
		void enableGlobalFilter(const QString& filterValue);

		// disables glabal filter
		void disableGlobalFilter();

		// returns const reference to filtered indexes
		const QSet<int>& filteredIndexes() const;

		// m_regExpFilteringEnabled getter / setter
		bool regExpFilteringEnabled() const;
		void setRegExpFilteringEnabled(bool regExpFilteringEnabled);

		// m_treeModel getter
		TreeModel *treeModel() const;

	protected:
		// QSortFilterProxyModel interface
		bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

	private:
		// model handler
		TreeModel* m_treeModel;

		// set of filtered, visible indexes
		QSet<int> m_filteredIndexes;

		// true if filter is enabled and proxy model is using m_visibleSourceIndexes to filter source data
		bool m_dataFiltered{ false };

		// string representation of filter that is applied to all nodes in the tree
		QString m_globalFilter;

		// if true proxy will filter data using regular expressions
		bool m_regExpFilteringEnabled{ true };

		// internal RegExp used in filtering with regExpFilteringEnabled set to true
		QRegularExpression m_regExp;

		// filter watcher
		QFutureWatcher<QSet<int>>* m_filterWatcher;

		// called when QFuture returns from its operation
		void onDataFiltered();

		// called when proxy filter changes, used to start QFuture and QFutureWatcher
		void onFilterChangeRequested();

		// called when proxy filter changes, used to start QFuture and QFutureWatcher - RegExp version
		void onFilterChangeRequestedRegExp();

	signals:
		// emited when globalFilter was changed
		void globalFilterChanged(QString filterValue);

		// emited when model starts filtering data
		void filteringStarted();

		// emited when model stops filtering data
		void filteringEnded();

		// used to force proxy to refilter model
		void refilterData();

		// emited when proxy got invalid RegExp for use in filering
		void invalidRegExpObject();
	};
}   // namespace IzTree

#endif   // IZTREE_TREEPROXYMODEL_H
