#include <utility>

#include "TreeViewController.h"

#include <QDebug>
#include <QTimer>

#include "IzTree/SQLDataProvider.h"
#include "IzTree/Tree.h"
#include "IzTree/Node.h"
#include "IzTree/TreeModel.h"
#include "IzTree/TreeProxyModel.h"

#include "ConnectorsModel.h"
#include "TreeVisibilityFilter.h"

IzTree::TreeViewController::TreeViewController(QQuickItem* parent)
	: QQuickItem(parent)
	, m_tree(new Tree(this))
	, m_model(new TreeProxyModel(m_tree, this))
	, m_visibilityFilter(new TreeVisibilityFilter(m_model, this, this))
	, m_visibilityCheckTimer(new QTimer(this))
	, m_connectors(new ConnectorsModel(m_tree, this))
{
	// internal connects
	connect(this, &TreeViewController::dataGenerated, this, [this]() {
		m_isWorking = false;
		emit isWorkingChanged();
	});

	connect(m_tree, &Tree::dimensionsCalculated, this, [this]() {
		emit treeDimensionsChanged();
	});

	connect(m_model, &TreeProxyModel::filteringStarted, this, [this]() {
		m_isFilteringData = true;
		// TODO: przywrócic poprzedni indeks po filtrowaniu jeżeli aktualnie istnieje
		setCurrentIndex(-1);
		filteringStateChanged();
	});

	connect(m_model, &TreeProxyModel::filteringEnded, this, [this]() {
		m_isFilteringData = false;
		filteringStateChanged();
	});

	connect(m_tree, &Tree::dataParsed, this, [this]() {
		emit dataGenerated();
		emit modelSizeChanged();
	});

	connect(m_tree, &Tree::dataCleared, this, [this]() {
		m_nodeItemConnections.clear();
		m_connectors->resetModel();
		emit dataCleared();
		emit modelSizeChanged();
	});

	connect(m_model, &TreeProxyModel::invalidRegExpObject, this, [this]() {
		emit this->invalidRegExpSet();
	});

	// reflow timer setup
	m_visibilityCheckTimer->setInterval(50);
	m_visibilityCheckTimer->setSingleShot(true);

	// reflow timer setup
	connect(m_visibilityCheckTimer, &QTimer::timeout, this, [this]() {
		recalculateVisibleNodes();
		emit viewChanged();
	});
}

void IzTree::TreeViewController::clearNodes()
{
	m_tree->clearNodes();
}

IzTree::Node* IzTree::TreeViewController::nodeByID(int nodeID) const
{
	return m_tree->nodeByID(nodeID);
}

IzTree::Node* IzTree::TreeViewController::nodeByIndex(int visualProxyIndex) const
{
	return m_tree->nodeByIndex(mapVisibilityFilterToTreeModel(visualProxyIndex, 0).row());
}

int IzTree::TreeViewController::indexFromID(int nodeID) const
{
	return m_tree->indexFromID(nodeID);
}

QVariant IzTree::TreeViewController::getTreeData(int visualProxyIndex, const QString& role) const
{
	return m_model->getData(mapVisibilityFilterToTreeProxyModel(visualProxyIndex, 0).row(), role);
}

int IzTree::TreeViewController::rootNodeID() const
{
	return m_tree->rootNodeID();
}

QList<QObject*> IzTree::TreeViewController::nodesOnDepth(int depth) const
{
	return m_tree->nodesOnDepth(depth);
}

QList<QObject*> IzTree::TreeViewController::nodeChildren(int nodeID) const
{
	return m_tree->nodeChildren(nodeID);
}

QList<QQuickItem*> IzTree::TreeViewController::itemModes() const
{
	return m_nodeItemConnections.values();
}

int IzTree::TreeViewController::childrenCountWithAdditionalData(int nodeID, const QString& field, const QVariant& value) const
{
	return m_tree->childrenCountWithAdditionalData(nodeID, field, value);
}

bool IzTree::TreeViewController::hasChild(int parentID, int childID) const
{
	return m_tree->hasChild(parentID, childID);
}

bool IzTree::TreeViewController::isParent(int nodeID, int childID) const
{
	return m_tree->isParent(nodeID, childID);
}

void IzTree::TreeViewController::loadDataFromSQL(const QString& sqlQuery,
												 const QString& nodeIDColumn,
												 const QString& parentIDColumn,
												 const QString& nodeNameColumn,
												 const QString& nodeDescriptionColumn,
												 const QStringList& additionalDataColumns)
{
	if (m_isWorking) {
		qWarning() << "IzTreeView controller is currently loading data.";
		return;
	}
	m_isWorking = true;
	emit isWorkingChanged();
	auto dataProvider = new SQLDataProvider(this, nodeIDColumn, parentIDColumn, nodeNameColumn, nodeDescriptionColumn, additionalDataColumns);
	dataProvider->setQuery(sqlQuery);
	connect(dataProvider, &SQLDataProvider::loadStarted, this, &TreeViewController::loadStarted);
	connect(dataProvider, &SQLDataProvider::loadEnded, this, &TreeViewController::onLoadEnded);
	connect(dataProvider, &SQLDataProvider::loadEnded, dataProvider, &SQLDataProvider::deleteLater);
	dataProvider->loadData();
}

void IzTree::TreeViewController::setGlobalFilter(const QString& filterValue)
{
	if (!filterValue.isEmpty()) {
		m_model->enableGlobalFilter(filterValue);
	} else {
		m_model->disableGlobalFilter();
	}
}

int IzTree::TreeViewController::currentIndex() const
{
	return m_currentIndex;
}

void IzTree::TreeViewController::setCurrentIndex(int currentIndex)
{
	if (m_currentIndex != currentIndex) {
		m_currentIndex = currentIndex;
		if (m_currentIndex != -1) {
			m_currentNodeID = m_tree->nodeByIndex(mapVisibilityFilterToTreeModel(m_currentIndex, 0).row())->id();
		} else {
			m_currentNodeID = -1;
		}
		emit currentIndexChanged();
	}
}

int IzTree::TreeViewController::treeDepth() const
{
	return m_tree->treeDepth();
}

int IzTree::TreeViewController::treeLevel() const
{
	return m_tree->treeLevel();
}

int IzTree::TreeViewController::modelSize() const
{
	return m_model->sourceModel()->rowCount();
}

bool IzTree::TreeViewController::hasData() const
{
	return m_tree->hasNodes();
}

bool IzTree::TreeViewController::hasSelection() const
{
	return m_currentIndex != -1;
}

void IzTree::TreeViewController::changeNodeVisibility(int visualProxyIndex, bool isVisible)
{
	m_model->setData(m_model->index(mapVisibilityFilterToTreeProxyModel(visualProxyIndex, 0).row(), 0), isVisible, static_cast<int>(IzTree::TreeModel::TreeRoleNames::ChildrenHidden));
}

void IzTree::TreeViewController::visualNodeCreated(int nodeID, QQuickItem* item)
{
	m_nodeItemConnections.insert(nodeID, item);
}

void IzTree::TreeViewController::visualNodeDestroyed(int nodeID)
{
	m_nodeItemConnections.remove(nodeID);
}

QQuickItem* IzTree::TreeViewController::visualNodeFromID(int nodeID) const
{
	return m_nodeItemConnections.value(nodeID, nullptr);
}

bool IzTree::TreeViewController::regExpFilteringModeSet()
{
	return m_model->regExpFilteringEnabled();
}

void IzTree::TreeViewController::setRegExpFilteringMode(bool regExpFilteringEnabled)
{
	m_model->setRegExpFilteringEnabled(regExpFilteringEnabled);
	emit filteringModeChanged();
}

void IzTree::TreeViewController::onLoadEnded(bool result, std::shared_ptr<TreeData> data)
{
	if (result) {
		m_tree->setRawData(std::move(data));
	} else {
		qCritical() << "Load data failed.";
	}
	emit loadEnded(result);
}

void IzTree::TreeViewController::recalculateVisibleNodes()
{
	m_visibilityFilter->setVisualDimensions(m_flickable->property("contentY").toInt(), m_flickable->property("height").toInt(), m_nodeHeight, m_rowSpacing);
}

int IzTree::TreeViewController::rowSpacing() const
{
	return m_rowSpacing;
}

void IzTree::TreeViewController::setRowSpacing(int rowSpacing)
{
	if (m_rowSpacing != rowSpacing) {
		m_rowSpacing = rowSpacing;
		emit rowSpacingChanged();
	}
}

IzTree::Node* IzTree::TreeViewController::currentNode() const
{
	if (m_currentIndex == -1) {
		return {};
	}
	return m_tree->nodeByIndex(mapVisibilityFilterToTreeModel(m_currentIndex, 0).row());
}

QModelIndex IzTree::TreeViewController::mapVisibilityFilterToTreeModel(int visualProxyRow, int visualProxyColumn) const
{
	return m_model->mapToSource(m_visibilityFilter->mapToSource(m_visibilityFilter->index(visualProxyRow, visualProxyColumn)));
}

QModelIndex IzTree::TreeViewController::mapVisibilityFilterToTreeProxyModel(int visualProxyRow, int visualProxyColumn) const
{
	return m_visibilityFilter->mapToSource(m_visibilityFilter->index(visualProxyRow, visualProxyColumn));
}

int IzTree::TreeViewController::currentNodeID() const
{
	return m_currentNodeID;
}

int IzTree::TreeViewController::columnSpacing() const
{
	return m_columnSpacing;
}

void IzTree::TreeViewController::setColumnSpacing(int columnSpacing)
{
	if (m_columnSpacing != columnSpacing) {
		m_columnSpacing = columnSpacing;
		emit columnSpacingChanged();
	}
}

int IzTree::TreeViewController::nodeHeight() const
{
	return m_nodeHeight;
}

void IzTree::TreeViewController::setNodeHeight(int nodeHeight)
{
	if (m_nodeHeight != nodeHeight) {
		m_nodeHeight = nodeHeight;
		emit nodeHeightChanged();
	}
}

int IzTree::TreeViewController::nodeWidth() const
{
	return m_nodeWidth;
}

void IzTree::TreeViewController::setNodeWidth(int nodeWidth)
{
	if (m_nodeWidth != nodeWidth) {
		m_nodeWidth = nodeWidth;
		emit nodeWidthChanged();
	}
}

void IzTree::TreeViewController::setFlickable(QQuickItem* flickable)
{
	if (m_flickable != flickable) {
		m_flickable             = flickable;
		auto contentYChangedIDX = m_flickable->metaObject()->indexOfMethod(QMetaObject::normalizedSignature("contentYChanged()"));
		auto widthChangedIDX    = m_flickable->metaObject()->indexOfMethod(QMetaObject::normalizedSignature("widthChanged()"));
		auto startIDX           = m_visibilityCheckTimer->metaObject()->indexOfMethod(QMetaObject::normalizedSignature("start()"));
		connect(m_flickable, m_flickable->metaObject()->method(contentYChangedIDX), m_visibilityCheckTimer, m_visibilityCheckTimer->metaObject()->method(startIDX));
		connect(m_flickable, m_flickable->metaObject()->method(widthChangedIDX), m_visibilityCheckTimer, m_visibilityCheckTimer->metaObject()->method(startIDX));
		emit flickableChanged();
	}
}

QQuickItem* IzTree::TreeViewController::flickable() const
{
	return m_flickable;
}
