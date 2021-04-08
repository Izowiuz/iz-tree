#ifndef IZTREE_TREEVISIBILITYFILTER_H
#define IZTREE_TREEVISIBILITYFILTER_H

#include <QSortFilterProxyModel>

namespace IzTree
{
	class TreeProxyModel;
	class TreeViewController;

	class TreeVisibilityFilter : public QSortFilterProxyModel
	{
		Q_OBJECT
		Q_DISABLE_COPY(TreeVisibilityFilter)

	public:
		// ctor
		TreeVisibilityFilter(TreeProxyModel* baseProxy, TreeViewController* view, QObject* parent = nullptr);

		// dtor
		~TreeVisibilityFilter() final = default;

		// invalidates filtering and sorting, sets visual dimensions of the controlling view
		void setVisualDimensions(int contentY, int viewHeight, int nodeHeight, int rowSpacing);

		// returns source row for given proxy row
		Q_INVOKABLE int sourceRow(int proxyRow) const;

	private:
		// base proxy model
		TreeProxyModel* m_proxy;

		// tree view controller
		TreeViewController* m_controller;

		// view's visual properties
		int m_contentY{ 0 };
		int m_viewHeight{ 0 };
		int m_nodeHeight{ 0 };
		int m_rowSpacing{ 0 };

	protected:
		// QSortFilterProxyModel interface start

		bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

		// QSortFilterProxyModel interface end
	};
}   // namespace IzTree

#endif   // IZTREE_TREEVISIBILITYFILTER_H
