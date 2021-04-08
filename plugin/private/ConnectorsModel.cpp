#include "ConnectorsModel.h"

#include <QDebug>

#include "IzTree/Node.h"
#include "IzTree/Tree.h"
#include "IzTree/TreeModel.h"

IzTree::ConnectorsModel::ConnectorsModel(Tree* tree, QObject* parent)
	: QAbstractItemModel(parent)
	, m_tree(tree)
{
	connect(m_tree, &Tree::dimensionsCalculated, this, [this]() {
		beginResetModel();
		endResetModel();
	});
}

QModelIndex IzTree::ConnectorsModel::index(int row, int column, const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return createIndex(row, column);
}

QModelIndex IzTree::ConnectorsModel::parent(const QModelIndex& index) const
{
	Q_UNUSED(index)
	return {};
}

int IzTree::ConnectorsModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return m_tree->connectors().size();
}

int IzTree::ConnectorsModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return 1;
}

QVariant IzTree::ConnectorsModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) {
		return {};
	}

	switch (role) {
	case static_cast<int>(ConnectorsModelRoles::ParentX):
		return std::get<0>(m_tree->connectors()[index.row()])->depth();
	case static_cast<int>(ConnectorsModelRoles::ParentY):
		return std::get<0>(m_tree->connectors()[index.row()])->level();
	case static_cast<int>(ConnectorsModelRoles::Length):
		return std::get<1>(m_tree->connectors()[index.row()]);
	default:
		return {};
	}

	return {};
}

QHash<int, QByteArray> IzTree::ConnectorsModel::roleNames() const
{
	QHash<int, QByteArray> rn;
	rn.insert(static_cast<int>(ConnectorsModelRoles::ParentX), QByteArrayLiteral("parentX"));
	rn.insert(static_cast<int>(ConnectorsModelRoles::ParentY), QByteArrayLiteral("parentY"));
	rn.insert(static_cast<int>(ConnectorsModelRoles::Length), QByteArrayLiteral("length"));
	return rn;
}

void IzTree::ConnectorsModel::resetModel()
{
	beginResetModel();
	endResetModel();
}
