#ifndef IZTREE_TREEDATA_H
#define IZTREE_TREEDATA_H

#include "IzTree/IzTree_Global.h"

#include <QMap>
#include <QString>

#include "NodeData.h"

namespace IzTree
{
	struct IZTREESHARED_EXPORT TreeData {
		// hash of data required to construct node
		// QHash< nodeID, nodeData >
		QHash<int, NodeData> treeData;

		// root node data
		NodeData* rootNode{ nullptr };
	};
}   // namespace IzTree

#endif   // IZTREE_TREEDATA_H
