#ifndef IZTREE_TREE_H
#define IZTREE_TREE_H

#include "IzTree_Global.h"

#include <memory>
#include <vector>

#include <QElapsedTimer>
#include <QHash>
#include <QObject>

// TODO: zoptymalizowac koszmarne filtrowanie
// TODO: byc może zastąpić raw pointery unique / shared
namespace IzTree
{
	class Node;
	struct TreeData;

	// (int index) -> index of the element in m_nodes
	class IZTREESHARED_EXPORT Tree : public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(Tree)

	public:
		// ctor
		explicit Tree(QObject* parent = nullptr);

		// dtor
		~Tree() = default;

		// returns total node count for this tree
		int nodesCount() const;

		// returns true if root node was properly set
		bool rootIsSet() const;

		// clears all tree data
		void clearNodes();

		// returns current depth of given node - X coordinate on a grid
		int nodeDepth(int index) const;

		// returns current level of given node - Y coordinate on a grid
		int nodeLevel(int index) const;

		// returns ID of the given node
		int nodeID(int index) const;

		// returns true if node is currently visible
		bool nodeHasHiddenChildren(int index) const;

		// returns true if node has hidden children
		bool nodeIsVisible(int index) const;

		// returns name of the given node
		QString nodeName(int index) const;

		// returns description of the given node
		QString nodeDescription(int index) const;

		// returns additional data of the given node
		QVariantMap nodeAdditionalData(int index) const;

		// returns node from std::vector<Node*> index
		Node* nodeByIndex(int index) const;

		// returns node from nodeID index
		Node* nodeByID(int nodeID) const;

		// returns true if node is a parent to given child
		bool hasChild(int parentID, int childID) const;

		// returns true if node is a parent to thee given child
		bool isParent(int nodeID, int childID) const;

		// returns true if given node has parent
		bool nodeHasParent(int index) const;

		// returns true if given node has children
		bool nodeHasChildren(int index) const;

		// returns nodeID of given node's parent
		int parentNodeID(int index) const;

		// returns parent node for given node
		Node* parentNode(int index) const;

		// returns current tree depth - max X coordinate on a grid
		int treeDepth() const;

		// returns current tree level - max Y coordinate on a grid
		int treeLevel() const;

		// returns nodeID for the root node of this tree
		int rootNodeID() const;

		// returns root node for this tree
		Node* rootNode() const;

		// calulates depth and level of this tree and all of its nodes
		void calculateDimensions();

		// returns true if given node is root node
		bool nodeIsRoot(int index) const;

		// returns std::vector<Node*> index for given nodeID
		int indexFromID(int nodeID) const;

		// returns list of all nodes on given depth
		QList<QObject*> nodesOnDepth(int depth) const;

		// returns list of children for diven node
		QList<QObject*> nodeChildren(int nodeID) const;

		// returns count of nodes which has a given [field - value] pair in it's additional data
		int childrenCountWithAdditionalData(int nodeID, const QString& field, const QVariant& value) const;

		// sets m_treeRawData and, if parseData is set to true, parses it
		void setRawData(std::shared_ptr<TreeData> data, bool parseData = true);

		// creates tree data from m_rawTreeData
		void parseRawData();

		// debug function - dumps tree structure with qDebug() function
		void dumpStructure();

		// returns true if tree has nodes
		bool hasNodes() const;

		// returns true if given index is valid
		bool indexIsValid(int index) const;

		// m_connectors getter
		const std::vector<std::tuple<Node*, int>>& connectors() const;

	private:
		// sets given Node as root node
		bool setRootNode(Node* rootNode);

		// adds node to the tree
		bool addChildNode(Node* parent, Node* child);

		// current tree depth - max X coordinate on a grid
		int m_treeDepth{ 0 };

		// current tree level - max Y coordinate on a grid
		int m_treeLevel{ 0 };

		// root node of this tree
		Node* m_rootNode{ nullptr };

		// all node of this tree
		std::vector<Node*> m_nodes;

		// 'connectors' for this tree dimensions
		// [ parent, length ]
		std::vector<std::tuple<Node*, int>> m_connectors;

		// last dataset acquired from data provider
		std::shared_ptr<TreeData> m_rawTreeData;

		// debug timer
		QElapsedTimer m_elapsedTimer;

		// recursively calculates level and depth of given node and all of its children
		int calculateNodeLevelAndDepth(Node* node);

	signals:
		// emited when DataProvider's data parsing started
		void parsingData();

		// emited when DataProvider's data parsing ended
		void dataParsed();

		// emited when tree's data was cleared
		void dataCleared();

		// emited when all dimensions were for tree and its nodes were calculated
		void dimensionsCalculated();
	};
}   // namespace IzTree

#endif   // IZTREE_TREE_H
