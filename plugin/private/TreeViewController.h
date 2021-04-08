#ifndef IZTREE_IZTREEVIEWCONTROLLER_H
#define IZTREE_IZTREEVIEWCONTROLLER_H

#include <atomic>
#include <memory>

#include <QQuickItem>

class QTimer;

// TODO: komentarze!
namespace IzTree
{
	class Node;
	class Tree;
	struct TreeData;
	class TreeProxyModel;
	class TreeVisibilityFilter;
	class ConnectorsModel;

	class TreeViewController : public QQuickItem
	{
		Q_OBJECT
		Q_DISABLE_COPY(TreeViewController)

		Q_PROPERTY(QQuickItem* flickable READ flickable WRITE setFlickable NOTIFY flickableChanged FINAL)
		Q_PROPERTY(IzTree::ConnectorsModel* connectorsModel MEMBER m_connectors CONSTANT FINAL)
		Q_PROPERTY(IzTree::TreeVisibilityFilter* visualModel MEMBER m_visibilityFilter CONSTANT FINAL)
		Q_PROPERTY(IzTree::TreeProxyModel* model MEMBER m_model CONSTANT FINAL)
		Q_PROPERTY(int treeDepth READ treeDepth NOTIFY treeDimensionsChanged FINAL)
		Q_PROPERTY(int treeLevel READ treeLevel NOTIFY treeDimensionsChanged FINAL)
		Q_PROPERTY(int dataSize READ modelSize NOTIFY modelSizeChanged FINAL)
		Q_PROPERTY(bool hasData READ hasData NOTIFY modelSizeChanged FINAL)
		Q_PROPERTY(bool isWorking MEMBER m_isWorking NOTIFY isWorkingChanged FINAL)
		Q_PROPERTY(bool isFilteringData MEMBER m_isFilteringData NOTIFY filteringStateChanged FINAL)
		Q_PROPERTY(bool regExpFilteringEnabled READ regExpFilteringModeSet WRITE setRegExpFilteringMode NOTIFY filteringModeChanged FINAL)
		Q_PROPERTY(int nodeWidth READ nodeWidth WRITE setNodeWidth NOTIFY nodeWidthChanged FINAL)
		Q_PROPERTY(int nodeHeight READ nodeHeight WRITE setNodeHeight NOTIFY nodeHeightChanged FINAL)
		Q_PROPERTY(int columnSpacing READ columnSpacing WRITE setColumnSpacing NOTIFY columnSpacingChanged FINAL)
		Q_PROPERTY(int rowSpacing READ rowSpacing WRITE setRowSpacing NOTIFY rowSpacingChanged FINAL)

		Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged FINAL)
		Q_PROPERTY(IzTree::Node* currentNode READ currentNode NOTIFY currentIndexChanged FINAL)
		Q_PROPERTY(int currentNodeID READ currentNodeID NOTIFY currentIndexChanged FINAL)
		Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY currentIndexChanged FINAL)

	public:
		// ctor
		explicit TreeViewController(QQuickItem* parent = nullptr);

		// dtor
		~TreeViewController() = default;

		// removes all current nodes
		Q_INVOKABLE void clearNodes();

		// returns node by its ID
		Q_INVOKABLE Node* nodeByID(int nodeID) const;

		// returns node by its internal proxy index
		Q_INVOKABLE Node* nodeByIndex(int visualProxyIndex) const;

		// transforms internal index to nodeID
		Q_INVOKABLE int indexFromID(int nodeID) const;

		// returns data under given role for indicated node
		Q_INVOKABLE QVariant getTreeData(int visualProxyIndex, const QString& role) const;

		// returns ID for root node
		Q_INVOKABLE int rootNodeID() const;

		// returns list of nodes on given depth
		Q_INVOKABLE QList<QObject*> nodesOnDepth(int depth) const;

		// returns list of nodes on given level
		Q_INVOKABLE QList<QObject*> nodeChildren(int nodeID) const;

		// returns list of all active QQuickItem nodes
		Q_INVOKABLE QList<QQuickItem*> itemModes() const;

		// returns count of nodes for which there exists a key-value pair in node additionalData
		Q_INVOKABLE int childrenCountWithAdditionalData(int nodeID, const QString& field, const QVariant& value) const;

		// returns true if node is a parent to given child
		Q_INVOKABLE bool hasChild(int parentID, int childID) const;

		// returns true if node is a parent to thee given child
		Q_INVOKABLE bool isParent(int nodeID, int childID) const;

		// loads data from preconfigured SQL database
		Q_INVOKABLE void loadDataFromSQL(const QString& sqlQuery,
										 const QString& nodeIDColumn              = QStringLiteral("nodeID"),
										 const QString& parentIDColumn            = QStringLiteral("parentID"),
										 const QString& nodeNameColumn            = QStringLiteral("nodeName"),
										 const QString& nodeDescriptionColumn     = QStringLiteral(""),
										 const QStringList& additionalDataColumns = {});

		// sets global filter value to a given vaue
		Q_INVOKABLE void setGlobalFilter(const QString& filterValue);

		// returns current indef of tree
		int currentIndex() const;

		// sets current index of the tree
		void setCurrentIndex(int currentIndex);

		// returns max depth - X coordinate - of the tree
		int treeDepth() const;

		// returns max level - max Y coordinate - of the tree
		int treeLevel() const;

		// returns current size of the source model
		int modelSize() const;

		// returns false if source model has data
		bool hasData() const;

		// returns true if tree currently has selection
		bool hasSelection() const;

		// changes node visibility for given node
		Q_INVOKABLE void changeNodeVisibility(int visualProxyIndex, bool isVisible);

		// called when node is created on the QML scene - creates nodeID -> QQuickItem connection
		Q_INVOKABLE void visualNodeCreated(int nodeID, QQuickItem* item);

		// called when node is destroyed from the QML scene - destroys nodeID -> QQuickItem connection
		Q_INVOKABLE void visualNodeDestroyed(int nodeID);

		// returns handler to the visual node from node ID
		Q_INVOKABLE QQuickItem* visualNodeFromID(int nodeID) const;

		// regExpFilteringMode getter / setter
		bool regExpFilteringModeSet();
		void setRegExpFilteringMode(bool regExpFilteringEnabled);

		// m_flickable getter / setter
		QQuickItem* flickable() const;
		void setFlickable(QQuickItem* flickable);

		// m_nodeWidth getter / setter
		int nodeWidth() const;
		void setNodeWidth(int nodeWidth);

		// m_nodeHeight getter / setter
		int nodeHeight() const;
		void setNodeHeight(int nodeHeight);

		// m_columnSpacing getter / setter
		int columnSpacing() const;
		void setColumnSpacing(int columnSpacing);

		// m_rowSpacing getter / setter
		int rowSpacing() const;
		void setRowSpacing(int rowSpacing);

		// returns currently selected node in tree
		Node* currentNode() const;

		// maps index from visiblity filter to tree model
		QModelIndex mapVisibilityFilterToTreeModel(int visualProxyRow, int visualProxyColumn) const;

		// maps index from visiblity filter to tree proxy model
		QModelIndex mapVisibilityFilterToTreeProxyModel(int visualProxyRow, int visualProxyColumn) const;

		// returns currentNodeID for this tree
		int currentNodeID() const;

	private:
		// caled when DataProvider finishes loading data
		void onLoadEnded(bool result, std::shared_ptr<TreeData> data);

		// internal Tree handler
		Tree* m_tree;

		// internal model handler
		TreeProxyModel* m_model;

		// visibility filter
		TreeVisibilityFilter* m_visibilityFilter;

		// connectors model
		ConnectorsModel* m_connectors;

		// flickable attached to this view
		QQuickItem* m_flickable{ nullptr };

		// current index of the tree
		int m_currentIndex{ -1 };

		// state of the tree - true indicates that tree is currently parsing / loaing data
		bool m_isWorking{ false };

		// true if tree is currentl filtering data
		bool m_isFilteringData{ false };

		// NodeID -> QQuickItem projection
		QHash<int, QQuickItem*> m_nodeItemConnections;

		// called when contentY of the flickable was changed
		Q_INVOKABLE void recalculateVisibleNodes();

		// visibility check timer timer
		QTimer* m_visibilityCheckTimer;

		// height of the node
		int m_nodeWidth{ 0 };

		// width of the node
		int m_nodeHeight{ 0 };

		// view's column spacing
		int m_columnSpacing{ 0 };

		// view's row spacing
		int m_rowSpacing{ 0 };

		// view's current node id
		int m_currentNodeID{ -1 };

	signals:
		// Q_PROPERTY *Changed signals
		void flickableChanged();
		void treeDimensionsChanged();
		void isWorkingChanged();
		void currentIndexChanged();
		void modelSizeChanged();
		void filteringModeChanged();
		void rowSpacingChanged();
		void columnSpacingChanged();
		void nodeHeightChanged();
		void nodeWidthChanged();

		// emited when load operation ends
		void loadEnded(bool result);

		// emited when load operation starts
		void loadStarted();

		// emited when tree data was parsed
		void dataGenerated();

		// emited when tree data was cleared
		void dataCleared();

		// emited when filtering state was changed
		void filteringStateChanged();

		// emited when invlid ReqExp was set in proxy model
		void invalidRegExpSet();

		// emited when visual properties of the view changed
		void viewChanged();
	};
}   // namespace IzTree

#endif   // IZTREE_IZTREEVIEWCONTROLLER_H
