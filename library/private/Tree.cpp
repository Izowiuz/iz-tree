#include "IzTree/Tree.h"

#include <tuple>

#include <QDebug>

#include "IzTree/Node.h"
#include "TreeData.h"

IzTree::Tree::Tree(QObject* parent)
	: QObject(parent)
{
}

int IzTree::Tree::nodesCount() const
{
	return static_cast<int>(m_nodes.size());
}

bool IzTree::Tree::setRootNode(IzTree::Node* rootNode)
{
	if (m_rootNode != nullptr) {
		qCritical() << "Root node already set.";
		return false;
	}

	if (rootNode == nullptr) {
		qCritical() << "Got invalid root node.";
		return false;
	}

	m_rootNode = rootNode;
	m_rootNode->setParent(this);
	m_nodes.push_back(m_rootNode);

	return true;
}

bool IzTree::Tree::addChildNode(IzTree::Node* parent, IzTree::Node* child)
{
	if (parent == nullptr) {
		qCritical() << "Got invalid parent node.";
		return false;
	}

	if (child == nullptr) {
		qCritical() << "Got invalid child node.";
		return false;
	}

	if (parent->addChild(child)) {
		child->setParentNode(parent);
		m_nodes.push_back(child);

		return true;
	}

	qCritical() << "Could not add child for parent:" << parent->nodeName() << "child:" << child->nodeName();
	return false;
}

bool IzTree::Tree::rootIsSet() const
{
	return m_rootNode != nullptr;
}

void IzTree::Tree::clearNodes()
{
	if (m_rootNode != nullptr) {
		// NOTICE: because m_rootNode is a Qt parent to every Node, they will be destroyed automatically
		delete m_rootNode;
		m_rootNode = nullptr;
	}

	m_treeDepth = 0;
	m_treeLevel = 0;

	m_nodes.clear();

	emit dataCleared();
}

int IzTree::Tree::nodeDepth(int index) const
{
	if (index > static_cast<int>(m_nodes.size())) {
		return -1;
	}

	return m_nodes[index]->depth();
}

int IzTree::Tree::nodeLevel(int index) const
{
	if (index > static_cast<int>(m_nodes.size())) {
		return -1;
	}

	return m_nodes[index]->level();
}

int IzTree::Tree::nodeID(int index) const
{
	if (index > static_cast<int>(m_nodes.size())) {
		return -1;
	}

	return m_nodes[index]->id();
}

bool IzTree::Tree::nodeHasHiddenChildren(int index) const
{
	if (index > static_cast<int>(m_nodes.size())) {
		return false;
	}

	return m_nodes[index]->childrenHidden();
}

bool IzTree::Tree::nodeIsVisible(int index) const
{
	if (index > static_cast<int>(m_nodes.size())) {
		return false;
	}

	return m_nodes[index]->isVisible();
}

QString IzTree::Tree::nodeName(int index) const
{
	if (index > static_cast<int>(m_nodes.size())) {
		return {};
	}

	return m_nodes[index]->nodeName();
}

QString IzTree::Tree::nodeDescription(int index) const
{
	if (index > static_cast<int>(m_nodes.size())) {
		return {};
	}

	return m_nodes[index]->nodeDescription();
}

QVariantMap IzTree::Tree::nodeAdditionalData(int index) const
{
	if (index > static_cast<int>(m_nodes.size())) {
		return {};
	}

	return m_nodes[index]->additionalData();
}

IzTree::Node* IzTree::Tree::nodeByIndex(int index) const
{
	if (index > static_cast<int>(m_nodes.size())) {
		return {};
	}

	return m_nodes[index];
}

IzTree::Node* IzTree::Tree::nodeByID(int nodeID) const
{
	auto n = std::find_if(m_nodes.cbegin(), m_nodes.cend(), [&nodeID](auto node) -> bool {
		return node->id() == nodeID;
	});

	if (n != m_nodes.cend()) {
		return (*n);
	}

	return {};
}

bool IzTree::Tree::hasChild(int parentID, int childID) const
{
	auto n = nodeByID(parentID);
	if (n == nullptr) {
		qCritical() << "Got invalid nodeID.";
		return false;
	}

	std::function<bool(Node*)> fn;
	fn = [childID, &fn](Node* node) {
		if (node->id() == childID) {
			return true;
		}

		for (auto it = node->childrenBegin(); it != node->childrenEnd(); ++it) {
			if (fn(*it)) {
				return true;
			}
		}

		return false;
	};

	return fn(n);
}

bool IzTree::Tree::isParent(int nodeID, int childID) const
{
	auto n = nodeByID(childID);
	if (n == nullptr) {
		qCritical() << "Got invalid childID.";
		return false;
	}

	return n->hasParent() && n->parentNode()->id() == nodeID;
}

bool IzTree::Tree::nodeHasParent(int index) const
{
	if (index > static_cast<int>(m_nodes.size())) {
		return false;
	}

	return m_nodes[index]->hasParent();
}

bool IzTree::Tree::nodeHasChildren(int index) const
{
	if (index > static_cast<int>(m_nodes.size())) {
		return false;
	}

	return m_nodes[index]->hasChildren();
}

int IzTree::Tree::parentNodeID(int index) const
{
	if (index > static_cast<int>(m_nodes.size())) {
		return -1;
	}

	return m_nodes[index]->parentNodeID();
}

IzTree::Node* IzTree::Tree::parentNode(int index) const
{
	if (index > static_cast<int>(m_nodes.size())) {
		return {};
	}

	return m_nodes[index];
}

int IzTree::Tree::treeDepth() const
{
	return m_treeDepth;
}

int IzTree::Tree::treeLevel() const
{
	return m_treeLevel;
}

int IzTree::Tree::rootNodeID() const
{
	if (m_rootNode == nullptr) {
		return 0;
	}

	return m_rootNode->id();
}

IzTree::Node* IzTree::Tree::rootNode() const
{
	return m_rootNode;
}

void IzTree::Tree::calculateDimensions()
{
	if (m_rootNode != nullptr) {
		m_treeLevel = calculateNodeLevelAndDepth(m_rootNode) - 1;
		m_treeDepth = 0;

		m_connectors.clear();
		QHash<Node*, int> tmp;

		for (const auto node : m_nodes) {
			if (node->isFiltered() || !node->isVisible()) {
				continue;
			}

			if (node->depth() > m_treeDepth) {
				m_treeDepth = node->depth();
			}

			if (node->hasParent() && node->parentNode()->level() < node->level()) {
				tmp.insert(node->parentNode(), std::abs(node->parentNode()->level() - node->level()));
			}
		}

		QHashIterator<Node*, int> it(tmp);
		while (it.hasNext()) {
			it.next();
			m_connectors.emplace_back(std::make_tuple(it.key(), it.value()));
		}

		emit dimensionsCalculated();
	} else {
		qInfo() << "Root node is not set.";
	}
}

bool IzTree::Tree::nodeIsRoot(int index) const
{
	if (index > static_cast<int>(m_nodes.size())) {
		return false;
	}

	return m_nodes[index]->isRootNode();
}

int IzTree::Tree::indexFromID(int nodeID) const
{
	auto n = std::find_if(m_nodes.cbegin(), m_nodes.cend(), [&nodeID](auto node) -> bool {
		return node->id() == nodeID;
	});

	if (n != m_nodes.cend()) {
		return n - m_nodes.cbegin();
	}

	return -1;
}

QList<QObject*> IzTree::Tree::nodesOnDepth(int depth) const
{
	QList<QObject*> res;

	for (const auto& node : m_nodes) {
		if (node->depth() == depth) {
			res.push_back(node);
		}
	}

	return res;
}

QList<QObject*> IzTree::Tree::nodeChildren(int nodeID) const
{
	for (const auto node : m_nodes) {
		if (node->id() == nodeID) {
			return node->children();
		}
	}
	return {};
}

int IzTree::Tree::childrenCountWithAdditionalData(int nodeID, const QString& field, const QVariant& value) const
{
	auto n = nodeByID(nodeID);

	if (n == nullptr) {
		return 0;
	}

	int hits{ 0 };

	auto tmpList = n->children();

	for (const auto child : qAsConst(tmpList)) {
		if (static_cast<IzTree::Node*>(child)->additionalData().value(field) == value) {
			hits++;
		}
	}

	return hits;
}

void IzTree::Tree::setRawData(std::shared_ptr<IzTree::TreeData> data, bool parseData)
{
	m_rawTreeData = std::move(data);
	if (parseData) {
		parseRawData();
	}
}

void IzTree::Tree::parseRawData()
{
	emit parsingData();

	m_elapsedTimer.restart();
	clearNodes();

	// TODO: jakiaś inna optymalizacja tego
	if (!m_rawTreeData->treeData.empty()) {

		// helper structure
		// QHash< nodeID, <Node*, nodeParentID >>
		QHash<int, std::tuple<Node*, int>> nodes;
		nodes.reserve(m_rawTreeData->treeData.size());

		for (const auto& element : m_rawTreeData->treeData) {
			nodes.insert(element.nodeID, { new Node(element.nodeID, element.nodeName, element.nodeDescription, element.additionalData), element.parentNodeID });
		}

		if (m_rawTreeData->rootNode == nullptr) {
			qCritical() << "Root node data is not specified. Tree is invalid.";
		} else {
			auto rootNode = std::get<0>(nodes.value(m_rawTreeData->rootNode->nodeID));

			if (!setRootNode(rootNode)) {
				qCritical() << "Could not set root node. Tree is invalid.";
				delete rootNode;
				return;
			}

			QHashIterator<int, std::tuple<Node*, int>> it(nodes);
			while (it.hasNext()) {
				it.next();

				if (it.key() == rootNode->id()) {
					continue;
				}

				if (auto n = std::get<0>(nodes.value(std::get<1>(it.value())))) {
					addChildNode(n, std::get<0>(it.value()));
				} else {
					qCritical() << "Got node with invalid parent, name:"
								<< std::get<0>(it.value())->nodeName()
								<< ", id:" << std::get<0>(it.value())->id()
								<< ", parent id:" << std::get<1>(it.value());
				}
			}
		}
	} else {
		qInfo() << "Raw tree data is empty";
	}

	qDebug() << "Tree data parse time:" << m_elapsedTimer.elapsed() << "msecs.";

	emit dataParsed();
}

void IzTree::Tree::dumpStructure()
{
	for (const auto& node : m_nodes) {
		qDebug() << "nodeID" << node->id() << "childrenCount" << node->childrenCount() << "depth" << node->depth() << "level" << node->level();
	}
}

bool IzTree::Tree::hasNodes() const
{
	return !m_nodes.empty();
}

bool IzTree::Tree::indexIsValid(int index) const
{
	// TODO: cache size()?
	return index >= 0 && index < static_cast<int>(m_nodes.size());
}

const std::vector<std::tuple<IzTree::Node*, int>>& IzTree::Tree::connectors() const
{
	return m_connectors;
}

int IzTree::Tree::calculateNodeLevelAndDepth(IzTree::Node* node)
{
	if (node->hasChildren()) {
		int currentLevel = node->level();
		bool hasVisibleChild{ false };
		int invisibleChildren{ 0 };

		for (auto it = node->childrenBegin(); it != node->childrenEnd(); ++it) {
			if (!(*it)->isFiltered()) {
				if (!(*it)->isVisible()) {
					invisibleChildren++;
				}

				(*it)->setDepth(node->depth() + 1);
				(*it)->setLevel(currentLevel);
				currentLevel += calculateNodeLevelAndDepth((*it));
				hasVisibleChild = true;
			}
		}

		if (!hasVisibleChild) {
			return 1;
		}

		if (invisibleChildren == 0) {
			return currentLevel - node->level();
		}

		return currentLevel - node->level() - invisibleChildren + 1;
	}

	return 1;
}
