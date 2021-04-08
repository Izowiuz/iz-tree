#include "TreeVisibilityFilter.h"

#include <QDebug>
#include <QQuickItem>

#include "IzTree/Node.h"
#include "IzTree/Tree.h"
#include "IzTree/TreeModel.h"
#include "IzTree/TreeProxyModel.h"

#include "TreeViewController.h"

IzTree::TreeVisibilityFilter::TreeVisibilityFilter(TreeProxyModel* baseProxy, TreeViewController* view, QObject* parent)
	: QSortFilterProxyModel(parent)
	, m_proxy(baseProxy)
	, m_controller(view)
{
	setSourceModel(baseProxy);
}

void IzTree::TreeVisibilityFilter::setVisualDimensions(int contentY, int viewHeight, int nodeHeight, int rowSpacing)
{
	m_contentY   = contentY;
	m_viewHeight = viewHeight;
	m_nodeHeight = nodeHeight;
	m_rowSpacing = rowSpacing;
	invalidate();
}

int IzTree::TreeVisibilityFilter::sourceRow(int proxyRow) const
{
	qDebug() << "TreeVisibilityFilter" << proxyRow << mapToSource(index(proxyRow, 0));
	return mapToSource(index(proxyRow, 0)).row();
}

bool IzTree::TreeVisibilityFilter::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
	Q_UNUSED(source_parent)
	auto node = m_proxy->treeModel()->tree()->nodeByIndex(m_proxy->sourceRow(source_row));
	return node->level() * (m_nodeHeight + m_rowSpacing) >= m_contentY - (m_viewHeight) && node->level() * (m_nodeHeight + m_rowSpacing) <= m_contentY + m_viewHeight + (m_viewHeight);
}
