#include "IzTree/DataProvider.h"

#include <QDebug>

#include "TreeData.h"

IzTree::DataProvider::DataProvider(QObject* parent)
	: QObject(parent)
{
	connect(this, &DataProvider::loadStarted, this, [this]() {
		m_isWorking = true;
	});

	connect(this, &DataProvider::loadEnded, this, [this]() {
		m_isWorking = false;
	});

	qRegisterMetaType<std::shared_ptr<TreeData>>("std::shared_ptr<TreeData>");
}

bool IzTree::DataProvider::isWorking() const
{
	return m_isWorking.load();
}
