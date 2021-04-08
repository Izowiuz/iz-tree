#ifndef IZTREE_NODEDATA_H
#define IZTREE_NODEDATA_H

#include "IzTree/IzTree_Global.h"

#include <QString>
#include <QVariant>
#include <vector>

namespace IzTree
{
	struct IZTREESHARED_EXPORT NodeData {
		// id of node
		int32_t nodeID{ -1 };

		// parent id of node
		int32_t parentNodeID{ -1 };

		// name of node
		QString nodeName{ "undefined" };

		// description of node
		QString nodeDescription{ "" };

		// additional data of node
		QVariantMap additionalData;
	};
}   // namespace IzTree

#endif   // IZTREE_NODEDATA_H
