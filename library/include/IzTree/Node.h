#ifndef IZTREE_NODE_H
#define IZTREE_NODE_H

#include "IzTree/IzTree_Global.h"

#include <algorithm>
#include <memory>

#include <QList>
#include <QObject>
#include <QVariantMap>
#include <QVector>

// TODO: wersja dynamicznie edytowalna
namespace IzTree
{
	class IZTREESHARED_EXPORT Node : public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(Node)

		Q_PROPERTY(int nodeID READ id CONSTANT)
		Q_PROPERTY(int nodeDepth READ depth CONSTANT)
		Q_PROPERTY(int nodeLevel READ level CONSTANT)
		Q_PROPERTY(int childrenCount READ childrenCount CONSTANT)
		Q_PROPERTY(bool hasChildren READ hasChildren CONSTANT)
		Q_PROPERTY(bool hasParent READ hasParent CONSTANT)
		Q_PROPERTY(bool isRootNode READ isRootNode CONSTANT)
		Q_PROPERTY(QString nodeName READ nodeName CONSTANT)
		Q_PROPERTY(QString nodeDescription READ nodeDescription CONSTANT)
		Q_PROPERTY(QVariantMap additionalData READ additionalData CONSTANT)

	public:
		// ctor
		explicit Node(int nodeID, const QString& nodeName, const QString& nodeDescription, const QVariantMap& additionalData = {}, QObject* parent = nullptr);

		// dtor
		~Node() = default;

		// m_depth getter / setter
		int depth() const;
		void setDepth(int depth);

		// m_level getter / setter
		int level() const;
		void setLevel(int level);

		// m_id getter
		int id() const;

		// returns ID of this node's parent or -1 if no parent exists
		int parentNodeID() const;

		// m_nodeName getter / setter
		QString nodeName() const;
		void setNodeName(const QString& nodeName);

		// m_nodeDescription setter / getter
		QString nodeDescription() const;
		void setNodeDescription(const QString& nodeDescription);

		// m_additionalData setter / getter
		const QVariantMap& additionalData() const;
		void setAdditionalData(const QVariantMap& additionalData);

		// returns children count for this node
		int childrenCount() const;

		// m_childrenHidden getter / setter
		bool childrenHidden() const;
		void setChildrenHidden(bool childrenHidden);

		// returns true if this node is visible - when its parent has visible children or no parent is set
		bool isVisible() const;

		// returns true if this node has parent
		bool hasParent() const;

		// returns parent of this node
		Node* parentNode() const;

		// returns true if this node has children
		bool hasChildren() const;

		// returns true if this node is root of tree
		bool isRootNode() const;

		// sets parent for this node
		void setParentNode(Node* parent);

		// adds child for this node
		bool addChild(Node* child);

		// returns list of children for this node as QList, by value
		QList<QObject*> children() const;

		// const iterators for children of this node
		auto childrenCBegin() const
		{
			return m_children.cbegin();
		}
		auto childrenCEnd() const
		{
			return m_children.cend();
		}

		// iterators for children of this node
		auto childrenBegin() const
		{
			return m_children.begin();
		}

		auto childrenEnd() const
		{
			return m_children.end();
		}

		// m_isFiltered getter / setter
		bool isFiltered() const;
		void setIsFiltered(bool isFiltered);

	private:
		// vector of node's children
		QVector<Node*> m_children;

		// node's parent
		Node* m_parent{ nullptr };

		// depth of this node, X coordinate on a grid
		int m_depth{ 0 };

		// level of this node, Y coordinate on a grid
		int m_level{ 0 };

		// unique id of node in tree
		int m_id;

		// true if node has hidden children
		bool m_childrenHidden{ false };

		// true if this node is filtered
		bool m_isFiltered{ false };

		// name of this node
		QString m_nodeName;

		// description of this node
		QString m_nodeDescription;

		// additional parameters for this node, can be anything representable by the QVariant
		QVariantMap m_additionalData;
	};
}   // namespace IzTree

#endif   // IZTREE_NODE_H
