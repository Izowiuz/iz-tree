#ifndef IZTREE_CONNECTORSMODEL_H
#define IZTREE_CONNECTORSMODEL_H

#include <tuple>
#include <vector>

#include <QAbstractItemModel>

namespace IzTree
{
	class Tree;

	class ConnectorsModel : public QAbstractItemModel
	{
		Q_OBJECT
		Q_DISABLE_COPY(ConnectorsModel)

	public:
		// ConnectorsModel role names
		enum class ConnectorsModelRoles {
			ParentX,
			ParentY,
			Length
		};

		explicit ConnectorsModel(Tree* tree, QObject* parent = nullptr);

		// QAbstractItemModel interface start

		QModelIndex index(int row, int column, const QModelIndex& parent = {}) const override;
		QModelIndex parent(const QModelIndex& index) const override;
		int rowCount(const QModelIndex& parent = QModelIndex()) const override;
		int columnCount(const QModelIndex& parent = QModelIndex()) const override;
		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
		QHash<int, QByteArray> roleNames() const override;

		// QAbstractItemModel interface end

		// used to reset model
		Q_INVOKABLE void resetModel();

	private:
		// tree handler
		Tree* m_tree;
	};
}   // namespace IzTree

#endif   // IZTREE_CONNECTORSMODEL_H
