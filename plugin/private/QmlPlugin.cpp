#include "IzTreeView/QmlPlugin.h"

#include <QtQml>

#include "IzTree/Node.h"
#include "IzTreeView/IzTreeView_Enums.h"

#include "ConnectorsModel.h"
#include "TreeViewController.h"
#include "TreeVisibilityFilter.h"

void IzTree::IzTreeQmlPlugin::registerTypes(const char* uri)
{
	Q_ASSERT(uri == QLatin1String("IzTreeView"));
	qmlRegisterUncreatableMetaObject(IzTree::IzTreeNodeSize::staticMetaObject,
									 uri,
									 1,
									 0,
									 "IzTreeNodeSize",
									 QStringLiteral("Error: only enums"));
	qmlRegisterType<TreeViewController>(uri, 1, 0, "IzTreeViewController");
	qmlRegisterType(QUrl(QStringLiteral("qrc:/include/IzTreeView/QML/IzTreeView.qml")), uri, 1, 0, "IzTreeView");

	qRegisterMetaType<IzTree::TreeVisibilityFilter*>("IzTree::TreeVisibilityFilter*");
	qRegisterMetaType<IzTree::ConnectorsModel*>("IzTree::ConnectorsModel*");
	qRegisterMetaType<IzTree::Node*>("IzTree::Node*");
}
