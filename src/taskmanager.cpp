#include "taskmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

// TaskFilterProxyModel implementation

TaskFilterProxyModel::TaskFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setRecursiveFilteringEnabled(true);
}

void TaskFilterProxyModel::setSearchText(const QString &text)
{
    if (m_searchText != text) {
        m_searchText = text;
        invalidateFilter();
        emit searchTextChanged();
    }
}

bool TaskFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (m_searchText.isEmpty())
        return true;

    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    return checkItemAndChildren(index);
}

bool TaskFilterProxyModel::checkItemAndChildren(const QModelIndex &index) const
{
    TaskModel *model = qobject_cast<TaskModel*>(sourceModel());
    if (!model)
        return false;

    TaskItem *item = model->getItem(index);
    if (!item)
        return false;

    if (item->matchesSearch(m_searchText))
        return true;

    // Check children
    for (int i = 0; i < item->childCount(); ++i) {
        if (checkItemAndChildren(model->index(i, 0, index)))
            return true;
    }

    return false;
}

// TaskManager implementation

TaskManager::TaskManager(QObject *parent)
    : QObject(parent)
    , m_taskModel(new TaskModel(this))
    , m_filterModel(new TaskFilterProxyModel(this))
{
    m_filterModel->setSourceModel(m_taskModel);

    // Set up data directory
    QString dataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dataDir(dataLocation);
    if (!dataDir.exists()) {
        dataDir.mkpath(".");
    }

    m_dataFilePath = dataLocation + "/tasks.json";

    // Load existing data
    load();

    // Connect to save on changes
    connect(m_taskModel, &TaskModel::dataChanged, this, [this]() {
        save();
        updateCategoriesAndTags();
    });
}

TaskManager::~TaskManager()
{
    save();
}

void TaskManager::save()
{
    QJsonArray jsonArray = m_taskModel->toJson();
    QJsonDocument doc(jsonArray);

    QFile file(m_dataFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << m_dataFilePath;
        return;
    }

    file.write(doc.toJson());
    file.close();

    qDebug() << "Saved tasks to" << m_dataFilePath;
}

void TaskManager::load()
{
    QFile file(m_dataFilePath);
    if (!file.exists()) {
        qDebug() << "No existing data file found at" << m_dataFilePath;
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file for reading:" << m_dataFilePath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        qWarning() << "Invalid JSON data";
        return;
    }

    m_taskModel->fromJson(doc.array());
    updateCategoriesAndTags();

    qDebug() << "Loaded tasks from" << m_dataFilePath;
}

void TaskManager::addCategory(const QString &category)
{
    if (!m_categories.contains(category)) {
        m_categories.append(category);
        m_categories.sort();
        emit categoriesChanged();
    }
}

void TaskManager::addTag(const QString &tag)
{
    if (!m_tags.contains(tag)) {
        m_tags.append(tag);
        m_tags.sort();
        emit tagsChanged();
    }
}

void TaskManager::removeCategory(const QString &category)
{
    if (m_categories.removeOne(category)) {
        emit categoriesChanged();
    }
}

void TaskManager::removeTag(const QString &tag)
{
    if (m_tags.removeOne(tag)) {
        emit tagsChanged();
    }
}

void TaskManager::updateCategoriesAndTags()
{
    m_categories.clear();
    m_tags.clear();

    TaskItem *root = m_taskModel->getItem(QModelIndex());
    for (int i = 0; i < root->childCount(); ++i) {
        collectCategoriesAndTags(root->child(i));
    }

    m_categories.removeDuplicates();
    m_tags.removeDuplicates();
    m_categories.sort();
    m_tags.sort();

    emit categoriesChanged();
    emit tagsChanged();
}

void TaskManager::collectCategoriesAndTags(TaskItem *item)
{
    if (!item)
        return;

    if (!item->category().isEmpty() && !m_categories.contains(item->category())) {
        m_categories.append(item->category());
    }

    for (const QString &tag : item->tags()) {
        if (!tag.isEmpty() && !m_tags.contains(tag)) {
            m_tags.append(tag);
        }
    }

    for (int i = 0; i < item->childCount(); ++i) {
        collectCategoriesAndTags(item->child(i));
    }
}
