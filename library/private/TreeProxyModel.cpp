#include "IzTree/TreeProxyModel.h"

#include <functional>

#include <QDebug>
#include <QtConcurrent>

#include "IzTree/Node.h"
#include "IzTree/Tree.h"
#include "IzTree/TreeModel.h"

IzTree::TreeProxyModel::TreeProxyModel(Tree* tree, QObject* parent)
	: QSortFilterProxyModel(parent)
	, m_treeModel(new TreeModel(tree, this))
{
	m_filterWatcher = new QFutureWatcher<QSet<int>>(this);
	m_regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

	connect(m_filterWatcher, &QFutureWatcher<QSet<int>>::finished, this, &TreeProxyModel::onDataFiltered);

	connect(this, &TreeProxyModel::globalFilterChanged, this, [this]() {
		if (m_regExpFilteringEnabled) {
			m_regExp.setPattern(m_globalFilter);

			if (m_regExp.isValid()) {
				this->onFilterChangeRequestedRegExp();
			} else {
				emit this->invalidRegExpObject();
			}
		} else {
			this->onFilterChangeRequested();
		}
	});

	connect(this, &TreeProxyModel::refilterData, this, [this]() {
		if (m_globalFilter.isEmpty()) {
			m_treeModel->tree()->calculateDimensions();
		} else {
			if (m_regExpFilteringEnabled) {
				onFilterChangeRequestedRegExp();
			} else {
				onFilterChangeRequested();
			}
		}
	});

	connect(m_treeModel, &TreeModel::modelReset, this, [this]() {
		if (!m_globalFilter.isEmpty()) {
			emit this->refilterData();
		}
	});

	connect(this, &TreeProxyModel::filteringEnded, this, [this]() {
		m_treeModel->tree()->calculateDimensions();
	});

	setSourceModel(m_treeModel);
	qRegisterMetaType<IzTree::TreeModel*>("IzTree::TreeProxyModel*");
}

int IzTree::TreeProxyModel::sourceRow(int proxyRow) const
{
	QModelIndex tmpIndex = index(proxyRow, 0);
	if (tmpIndex.isValid()) {
		return mapToSource(tmpIndex).row();
	}

	qWarning() << "Got invalid proxy model row:" << proxyRow;
	return -1;
}

int IzTree::TreeProxyModel::proxyRow(int sourceRow) const
{
	QModelIndex tmpIndex = static_cast<TreeModel*>(sourceModel())->index(sourceRow, 0);
	if (tmpIndex.isValid()) {
		return mapFromSource(tmpIndex).row();
	}

	qWarning() << "Got invalid source model row:" << sourceRow;
	return -1;
}

QVariant IzTree::TreeProxyModel::getData(int proxyRow, const QString& role) const
{
	return static_cast<TreeModel*>(sourceModel())->getData(sourceRow(proxyRow), role);
}

bool IzTree::TreeProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
	Q_UNUSED(sourceParent)
	if (m_dataFiltered) {
		return m_filteredIndexes.contains(sourceRow);
	}

	return true;
}

IzTree::TreeModel* IzTree::TreeProxyModel::treeModel() const
{
	return m_treeModel;
}

bool IzTree::TreeProxyModel::regExpFilteringEnabled() const
{
	return m_regExpFilteringEnabled;
}

void IzTree::TreeProxyModel::setRegExpFilteringEnabled(bool regExpFilteringEnabled)
{
	m_regExpFilteringEnabled = regExpFilteringEnabled;
}

void IzTree::TreeProxyModel::onDataFiltered()
{
	m_filteredIndexes = m_filterWatcher->result();
	emit invalidateFilter();
	emit filteringEnded();
}

void IzTree::TreeProxyModel::onFilterChangeRequested()
{
	if (m_filterWatcher->isRunning()) {
		m_filterWatcher->cancel();
	} else {
		emit filteringStarted();
	}

	QSet<int> sequence;
	for (auto i{ 0 }; i < sourceModel()->rowCount(); ++i) {
		sequence.insert(i);
	}

	QFuture<QSet<int>> filterFuture = QtConcurrent::run([filter = m_globalFilter, this]()->QSet<int> {
		QHash<int, Node*> tmp;
		QSet<int> res;
		for (int i = 0; i < sourceModel()->rowCount(); ++i) {
			auto n = m_treeModel->tree()->nodeByIndex(i);
			n->setIsFiltered(true);
			// clang-format off
			if (
					// spełnienie filtru
					n->nodeDescription().contains(filter, Qt::CaseInsensitive) || n->nodeName().contains(filter, Qt::CaseInsensitive)

					// lub root node
					|| n->isRootNode()
				)
			{
				res.insert(i);
				n->setIsFiltered(false);
				tmp.insert(n->id(), n);
			}
			// clang-format on
		}

		QHashIterator<int, Node*> it(tmp);
		while (it.hasNext()) {
			it.next();

			// add parent nodes
			auto p = it.value();
			while (p->hasParent()) {
				auto tmp = static_cast<TreeModel*>(sourceModel())->tree()->nodeByID(p->id());
				res.insert(static_cast<TreeModel*>(sourceModel())->tree()->indexFromID(tmp->id()));
				tmp->setIsFiltered(false);
				p = p->parentNode();
			}
		}

		return res;
	});

	m_filterWatcher->setFuture(filterFuture);
}

void IzTree::TreeProxyModel::onFilterChangeRequestedRegExp()
{
	if (m_filterWatcher->isRunning()) {
		m_filterWatcher->cancel();
	} else {
		emit filteringStarted();
	}

	QSet<int> sequence;
	for (auto i{ 0 }; i < sourceModel()->rowCount(); ++i) {
		sequence.insert(i);
	}

	QFuture<QSet<int>> filterFuture = QtConcurrent::run([filter = m_globalFilter, this]()->QSet<int> {
		QHash<int, Node*> tmp;
		QSet<int> res;

		for (int i = 0; i < sourceModel()->rowCount(); ++i) {
			auto n = m_treeModel->tree()->nodeByIndex(i);
			n->setIsFiltered(true);
			if (n->nodeDescription().contains(m_regExp) || n->nodeName().contains(m_regExp) || n->isRootNode()) {
				res.insert(i);
				n->setIsFiltered(false);
				tmp.insert(i, n);
			}
		}

		QHashIterator<int, Node*> it(tmp);
		while (it.hasNext()) {
			it.next();
			const Node* p = it.value();
			while (p->hasParent()) {
				auto tmp = static_cast<TreeModel*>(sourceModel())->tree()->nodeByID(p->id());
				res.insert(static_cast<TreeModel*>(sourceModel())->tree()->indexFromID(tmp->id()));
				tmp->setIsFiltered(false);
				p = p->parentNode();
			}
		}

		return res;
	});
	m_filterWatcher->setFuture(filterFuture);
}

bool IzTree::TreeProxyModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	return m_treeModel->setData(sourceModel()->index(sourceRow(index.row()), 0), value, role);
}

void IzTree::TreeProxyModel::enableGlobalFilter(const QString& filterValue)
{
	if (m_globalFilter != filterValue) {
		m_globalFilter = filterValue;
		m_dataFiltered = true;
		emit globalFilterChanged(m_globalFilter);
	}
}

void IzTree::TreeProxyModel::disableGlobalFilter()
{
	m_globalFilter.clear();
	m_dataFiltered = false;

	for (int i = 0; i < sourceModel()->rowCount(); ++i) {
		auto n = m_treeModel->tree()->nodeByIndex(i);
		n->setIsFiltered(false);
	}

	m_treeModel->tree()->calculateDimensions();
	emit invalidateFilter();
}

const QSet<int>& IzTree::TreeProxyModel::filteredIndexes() const
{
	return m_filteredIndexes;
}
