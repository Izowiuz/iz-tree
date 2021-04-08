#ifndef IZTREE_IZTREEVIEWPLUGIN_H
#define IZTREE_IZTREEVIEWPLUGIN_H

#include <QQmlExtensionPlugin>

namespace IzTree
{
	class IzTreeQmlPlugin : public QQmlExtensionPlugin
	{
		Q_OBJECT
		// WARNING: bez CMake można skorzystać z DEFINE: QQmlExtensionInterface_iid
		Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0")

	public:
		// QQmlTypesExtensionInterface interface
		void registerTypes(const char* uri) override;
	};
}   // namespace IzTree

#endif   // IZTREE_IZTREEVIEWPLUGIN_H
