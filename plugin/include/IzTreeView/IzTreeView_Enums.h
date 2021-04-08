#ifndef IZTREE_IZTREEVIEW_ENUMS_H
#define IZTREE_IZTREEVIEW_ENUMS_H

#include <QtCore>

namespace IzTree {
	Q_NAMESPACE

	namespace IzTreeNodeSize
	{
		Q_NAMESPACE

		enum class IzTreeNodeSize : int {
			Small = 0,
			Medium,
			Large,
			Custom
		};
		Q_DECLARE_FLAGS(IzTreeNodeSizes, IzTreeNodeSize)
		Q_ENUM_NS(IzTreeNodeSize)
		Q_FLAGS(IzTreeNodeSize::IzTreeNodeSizes)

	} // namespace IzTreeNodeSize

} // namespace IzTree

#endif // IZTREE_IZTREEVIEW_ENUMS_H
