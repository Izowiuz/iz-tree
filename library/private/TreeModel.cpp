#include "IzTree/TreeModel.h"

#include <functional>

#include <QDebug>

#include "IzTree/Node.h"
#include "IzTree/Tree.h"

IzTree::TreeModel::TreeModel(Tree* tree, QObject* parent)
	: QAbstractItemModel(parent)
	, m_tree(tree)
{
	connect(m_tree, &Tree::dataCleared, this, [this]() {
		beginResetModel();
		endResetModel();
	});

	connect(m_tree, &Tree::dataParsed, this, [this]() {
		beginResetModel();
		m_tree->calculateDimensions();
		endResetModel();
	});

	connect(m_tree, &Tree::dimensionsCalculated, this, [this]() {
		this->dataChanged(index(0, 0), index(m_tree->nodesCount() - 1, 0), { static_cast<int>(TreeRoleNames::NodeDepth), static_cast<int>(TreeRoleNames::NodeLevel) });
	});

	qRegisterMetaType<IzTree::TreeModel*>("IzTree::TreeModel*");
}

QHash<int, QByteArray> IzTree::TreeModel::roleNames() const
{
	m_cachedRoleNames.clear();
	m_cachedReversedRoleNames.clear();

	QHash<int, QByteArray> rn;
	rn.insert(static_cast<int>(TreeRoleNames::NodeDepth), QByteArrayLiteral("nodeDepth"));
	rn.insert(static_cast<int>(TreeRoleNames::NodeLevel), QByteArrayLiteral("nodeLevel"));
	rn.insert(static_cast<int>(TreeRoleNames::NodeID), QByteArrayLiteral("nodeID"));
	rn.insert(static_cast<int>(TreeRoleNames::HasParent), QByteArrayLiteral("hasParent"));
	rn.insert(static_cast<int>(TreeRoleNames::HasChildren), QByteArrayLiteral("hasChildren"));
	rn.insert(static_cast<int>(TreeRoleNames::ParentNodeID), QByteArrayLiteral("parentNodeID"));
	rn.insert(static_cast<int>(TreeRoleNames::NodeName), QByteArrayLiteral("nodeName"));
	rn.insert(static_cast<int>(TreeRoleNames::NodeDescription), QByteArrayLiteral("nodeDescription"));
	rn.insert(static_cast<int>(TreeRoleNames::AdditionalData), QByteArrayLiteral("additionalData"));
	rn.insert(static_cast<int>(TreeRoleNames::RootNode), QByteArrayLiteral("rootNode"));
	rn.insert(static_cast<int>(TreeRoleNames::ChildrenHidden), QByteArrayLiteral("childrenHidden"));
	rn.insert(static_cast<int>(TreeRoleNames::IsVisible), QByteArrayLiteral("isVisible"));
	m_cachedReversedRoleNames.insert(QByteArrayLiteral("nodeDepth"), static_cast<int>(TreeRoleNames::NodeDepth));
	m_cachedReversedRoleNames.insert(QByteArrayLiteral("nodeLevel"), static_cast<int>(TreeRoleNames::NodeLevel));
	m_cachedReversedRoleNames.insert(QByteArrayLiteral("nodeID"), static_cast<int>(TreeRoleNames::NodeID));
	m_cachedReversedRoleNames.insert(QByteArrayLiteral("hasParent"), static_cast<int>(TreeRoleNames::HasParent));
	m_cachedReversedRoleNames.insert(QByteArrayLiteral("hasChildren"), static_cast<int>(TreeRoleNames::HasChildren));
	m_cachedReversedRoleNames.insert(QByteArrayLiteral("parentNodeID"), static_cast<int>(TreeRoleNames::ParentNodeID));
	m_cachedReversedRoleNames.insert(QByteArrayLiteral("nodeName"), static_cast<int>(TreeRoleNames::NodeName));
	m_cachedReversedRoleNames.insert(QByteArrayLiteral("nodeDescription"), static_cast<int>(TreeRoleNames::NodeDescription));
	m_cachedReversedRoleNames.insert(QByteArrayLiteral("additionalData"), static_cast<int>(TreeRoleNames::AdditionalData));
	m_cachedReversedRoleNames.insert(QByteArrayLiteral("rootNode"), static_cast<int>(TreeRoleNames::RootNode));
	m_cachedReversedRoleNames.insert(QByteArrayLiteral("childrenHidden"), static_cast<int>(TreeRoleNames::ChildrenHidden));
	m_cachedReversedRoleNames.insert(QByteArrayLiteral("isVisible"), static_cast<int>(TreeRoleNames::IsVisible));
	m_cachedRoleNames = rn;

	return rn;
}

QModelIndex IzTree::TreeModel::index(int row, int column, const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return createIndex(row, column);
}

QModelIndex IzTree::TreeModel::parent(const QModelIndex& index) const
{
	if (m_tree->indexIsValid(index.row())) {
		auto node = m_tree->nodeByIndex(index.row());
		if (node->hasParent()) {
			// TODO: można byłoby zapisywać 'parentIndex' w node przy generacji danych
			return createIndex(m_tree->indexFromID(node->parentNodeID()), 0);
		}
	}

	return {};
}

int IzTree::TreeModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	if (!parent.isValid()) {
		return m_tree->nodesCount();
	}

	// TODO: można byłoby zapisywać 'parentIndex' w node przy generacji danych
	return m_tree->nodeByID(m_tree->nodeByIndex(parent.row())->parentNodeID())->childrenCount();
}

int IzTree::TreeModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)

	return 1;
}

QVariant IzTree::TreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) {
		return {};
	}

	switch (static_cast<TreeRoleNames>(role)) {
	case TreeRoleNames::NodeDepth:
		return m_tree->nodeDepth(index.row());
	case TreeRoleNames::NodeLevel:
		return m_tree->nodeLevel(index.row());
	case TreeRoleNames::NodeID:
		return m_tree->nodeID(index.row());
	case TreeRoleNames::HasParent:
		return m_tree->nodeHasParent(index.row());
	case TreeRoleNames::HasChildren:
		return m_tree->nodeHasChildren(index.row());
	case TreeRoleNames::ParentNodeID:
		return m_tree->parentNodeID(index.row());
	case TreeRoleNames::NodeName:
		return m_tree->nodeName(index.row());
	case TreeRoleNames::NodeDescription:
		return m_tree->nodeDescription(index.row());
	case TreeRoleNames::AdditionalData:
		return m_tree->nodeAdditionalData(index.row());
	case TreeRoleNames::RootNode:
		return m_tree->nodeIsRoot(index.row());
	case TreeRoleNames::ChildrenHidden:
		return m_tree->nodeHasHiddenChildren(index.row());
	case TreeRoleNames::IsVisible:
		return m_tree->nodeIsVisible(index.row());
	}
	return {};
}

bool IzTree::TreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (index.isValid() && data(index, role) != value) {
		switch (static_cast<TreeRoleNames>(role)) {
		case TreeRoleNames::ChildrenHidden: {
			auto n = m_tree->nodeByIndex(index.row());
			n->setChildrenHidden(value.toBool());

			std::function<void(Node*, const QVariant&)> fn;
			fn = [this, &fn](Node* node, const QVariant& value) {
				for (auto it = node->childrenBegin(); it != node->childrenEnd(); ++it) {
					(*it)->setChildrenHidden(value.toBool());
					fn(*it, value);
				}
			};

			fn(n, value);
			// to trzebabyłoby zdecydowanie podoptymalizować
			m_tree->calculateDimensions();

			emit dataChanged(this->index(0, 0), this->index(rowCount() - 1, 0), QVector<int>() << role << static_cast<int>(TreeRoleNames::IsVisible) << static_cast<int>(TreeRoleNames::NodeLevel));
			break;
		}
		default:
			qWarning() << "Unsupported role in setData function:" << role;
		}

		return true;
	}

	return false;
}

QVariant IzTree::TreeModel::getData(int index, const QString& role) const
{
	return data(createIndex(index, 0), m_cachedReversedRoleNames.value(role.toLocal8Bit()));
}

bool IzTree::TreeModel::hasData() const
{
	return m_tree->hasNodes();
}

IzTree::Tree* IzTree::TreeModel::tree()
{
	return m_tree;
}

Qt::ItemFlags IzTree::TreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsEditable;
}

bool IzTree::TreeModel::insertRows(int row, int count, const QModelIndex& parent)
{
	beginInsertRows(parent, row, row + count - 1);
	endInsertRows();

	return false;
}

bool IzTree::TreeModel::insertColumns(int column, int count, const QModelIndex& parent)
{
	beginInsertColumns(parent, column, column + count - 1);
	endInsertColumns();

	return false;
}

bool IzTree::TreeModel::removeRows(int row, int count, const QModelIndex& parent)
{
	beginRemoveRows(parent, row, row + count - 1);
	endRemoveRows();

	return false;
}

bool IzTree::TreeModel::removeColumns(int column, int count, const QModelIndex& parent)
{
	beginRemoveColumns(parent, column, column + count - 1);
	endRemoveColumns();

	return false;
}
