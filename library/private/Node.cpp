#include "IzTree/Node.h"

#include <QDebug>

IzTree::Node::Node(int nodeID, const QString& nodeName, const QString& nodeDescription, const QVariantMap& additionalData, QObject* parent)
	: QObject(parent)
	, m_id(nodeID)
	, m_nodeName(nodeName)
	, m_nodeDescription(nodeDescription)
	, m_additionalData(additionalData)
{
	qRegisterMetaType<Node*>("Node*");
	qRegisterMetaType<QList<Node*>>("QList<Node*>");
}

int IzTree::Node::depth() const
{
	return m_depth;
}

void IzTree::Node::setDepth(int depth)
{
	m_depth = depth;
}

int IzTree::Node::level() const
{
	return m_level;
}

void IzTree::Node::setLevel(int level)
{
	m_level = level;
}

int IzTree::Node::id() const
{
	return m_id;
}

int IzTree::Node::parentNodeID() const
{
	if (m_parent != nullptr) {
		return m_parent->id();
	}
	return -1;
}

QString IzTree::Node::nodeName() const
{
	return m_nodeName;
}

void IzTree::Node::setNodeName(const QString& nodeName)
{
	m_nodeName = nodeName;
}

void IzTree::Node::setAdditionalData(const QVariantMap& additionalData)
{
	m_additionalData = additionalData;
}

const QVariantMap& IzTree::Node::additionalData() const
{
	return m_additionalData;
}

int IzTree::Node::childrenCount() const
{
	return m_children.size();
}

bool IzTree::Node::childrenHidden() const
{
	return m_childrenHidden;
}

void IzTree::Node::setChildrenHidden(bool childrenHidden)
{
	m_childrenHidden = childrenHidden;
}

bool IzTree::Node::isVisible() const
{
	if (!hasParent()) {
		return true;
	}
	return !m_parent->childrenHidden();
}

bool IzTree::Node::hasParent() const
{
	return m_parent != nullptr;
}

IzTree::Node* IzTree::Node::parentNode() const
{
	return m_parent;
}

bool IzTree::Node::hasChildren() const
{
	return !m_children.empty();
}

bool IzTree::Node::isRootNode() const
{
	return !hasParent();
}

void IzTree::Node::setParentNode(Node* parent)
{
	if (parent == nullptr) {
		qCritical() << "Got invalid parent.";
		return;
	}

	setParent(parent);
	m_parent = parent;
};

bool IzTree::Node::addChild(Node* child)
{
	if (child == nullptr) {
		qCritical() << "Got invalid child.";

		return false;
	}

	auto res = std::find_if(m_children.begin(), m_children.end(), [&](const auto e) -> bool {
		return e == child;
	});

	if (res != m_children.end()) {
		qCritical() << "Child already in node.";
		return false;
	}

	m_children.push_back(child);

	return true;
}

QList<QObject*> IzTree::Node::children() const
{
	QList<QObject*> res;
	res.reserve(m_children.size());

	for (const auto child : m_children) {
		res.push_back(child);
	}
	return res;
}

bool IzTree::Node::isFiltered() const
{
	return m_isFiltered;
}

void IzTree::Node::setIsFiltered(bool isFiltered)
{
	m_isFiltered = isFiltered;
}

QString IzTree::Node::nodeDescription() const
{
	return m_nodeDescription;
}

void IzTree::Node::setNodeDescription(const QString& nodeDescription)
{
	m_nodeDescription = nodeDescription;
}
