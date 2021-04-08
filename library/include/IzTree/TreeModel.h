#ifndef IZTREE_TREEMODEL_H
#define IZTREE_TREEMODEL_H

#include "IzTree_Global.h"

#include <QAbstractItemModel>

// TODO: zoptymalizować filtrowanie
// TODO: zaimplementować sortowanie
namespace IzTree
{
	class Tree;

	class IZTREESHARED_EXPORT TreeModel : public QAbstractItemModel
	{
		Q_OBJECT
		Q_DISABLE_COPY(TreeModel)

	public:
		enum class TreeRoleNames : int {
			NodeDepth = Qt::UserRole + 1,
			NodeLevel,
			NodeID,
			HasParent,
			HasChildren,
			ParentNodeID,
			NodeName,
			NodeDescription,
			AdditionalData,
			RootNode,
			ChildrenHidden,
			IsVisible
		};

		// ctor
		explicit TreeModel(Tree* tree, QObject* parent = nullptr);

		// dtor
		~TreeModel() = default;

		// QAbstractItemModel interface start

		QHash<int, QByteArray> roleNames() const override;
		QModelIndex index(int row, int column, const QModelIndex& parent = {}) const override;
		QModelIndex parent(const QModelIndex& index) const override;
		int rowCount(const QModelIndex& parent = {}) const override;
		int columnCount(const QModelIndex& parent = {}) const override;
		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
		bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
		Qt::ItemFlags flags(const QModelIndex& index) const override;
		// TODO: implement
		bool insertRows(int row, int count, const QModelIndex& parent = {}) override;
		// TODO: implement
		bool insertColumns(int column, int count, const QModelIndex& parent = {}) override;
		// TODO: implement
		bool removeRows(int row, int count, const QModelIndex& parent = {}) override;
		// TODO: implement
		bool removeColumns(int column, int count, const QModelIndex& parent = {}) override;

		// QAbstractItemModel interface end

		// QAbstractItemModel's data() wrapper for QML usage
		Q_INVOKABLE QVariant getData(int index, const QString& role) const;

		// returns true if model has currenty any data
		bool hasData() const;

		// m_tree getter
		Tree* tree();

	private:
		// tree handler
		Tree* m_tree{ nullptr };

		// cached role names
		mutable QHash<int, QByteArray> m_cachedRoleNames;
		mutable QHash<QByteArray, int> m_cachedReversedRoleNames;
	};
}   // namespace IzTree

#endif   // IZTREE_TREEMODEL_H
