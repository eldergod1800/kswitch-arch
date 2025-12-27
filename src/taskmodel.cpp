#include "taskmodel.h"

// TaskItem implementation

TaskItem::TaskItem(const QString &title, TaskItem *parent)
    : m_title(title)
    , m_completed(false)
    , m_priority(Priority::None)
    , m_parent(parent)
{
}

TaskItem::~TaskItem()
{
    qDeleteAll(m_children);
}

void TaskItem::setCompleted(bool completed)
{
    m_completed = completed;
    if (completed) {
        updateChildrenCompletionState();
    }
}

void TaskItem::updateChildrenCompletionState()
{
    for (TaskItem *child : m_children) {
        child->setCompleted(true);
    }
}

void TaskItem::appendChild(TaskItem *child)
{
    child->m_parent = this;
    m_children.append(child);
}

void TaskItem::insertChild(int position, TaskItem *child)
{
    if (position < 0 || position > m_children.size())
        return;

    child->m_parent = this;
    m_children.insert(position, child);
}

void TaskItem::removeChild(int position)
{
    if (position < 0 || position >= m_children.size())
        return;

    delete m_children.takeAt(position);
}

TaskItem *TaskItem::child(int row) const
{
    if (row < 0 || row >= m_children.size())
        return nullptr;
    return m_children.at(row);
}

int TaskItem::childCount() const
{
    return m_children.count();
}

int TaskItem::row() const
{
    if (m_parent)
        return m_parent->m_children.indexOf(const_cast<TaskItem*>(this));
    return 0;
}

QJsonObject TaskItem::toJson() const
{
    QJsonObject obj;
    obj["title"] = m_title;
    obj["description"] = m_description;
    obj["completed"] = m_completed;
    obj["dueDate"] = m_dueDate.toString(Qt::ISODate);
    obj["priority"] = static_cast<int>(m_priority);
    obj["category"] = m_category;

    QJsonArray tagsArray;
    for (const QString &tag : m_tags) {
        tagsArray.append(tag);
    }
    obj["tags"] = tagsArray;

    QJsonArray childrenArray;
    for (TaskItem *child : m_children) {
        childrenArray.append(child->toJson());
    }
    obj["children"] = childrenArray;

    return obj;
}

TaskItem* TaskItem::fromJson(const QJsonObject &json, TaskItem *parent)
{
    TaskItem *item = new TaskItem(json["title"].toString(), parent);
    item->setDescription(json["description"].toString());
    item->setCompleted(json["completed"].toBool());
    item->setDueDate(QDateTime::fromString(json["dueDate"].toString(), Qt::ISODate));
    item->setPriority(static_cast<Priority>(json["priority"].toInt()));
    item->setCategory(json["category"].toString());

    QStringList tags;
    QJsonArray tagsArray = json["tags"].toArray();
    for (const QJsonValue &tag : tagsArray) {
        tags.append(tag.toString());
    }
    item->setTags(tags);

    QJsonArray childrenArray = json["children"].toArray();
    for (const QJsonValue &childValue : childrenArray) {
        TaskItem *child = TaskItem::fromJson(childValue.toObject(), item);
        item->appendChild(child);
    }

    return item;
}

bool TaskItem::matchesSearch(const QString &searchText) const
{
    if (searchText.isEmpty())
        return true;

    QString search = searchText.toLower();

    if (m_title.toLower().contains(search))
        return true;
    if (m_description.toLower().contains(search))
        return true;
    if (m_category.toLower().contains(search))
        return true;

    for (const QString &tag : m_tags) {
        if (tag.toLower().contains(search))
            return true;
    }

    return false;
}

// TaskModel implementation

TaskModel::TaskModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_rootItem(new TaskItem())
{
}

TaskModel::~TaskModel()
{
    delete m_rootItem;
}

QModelIndex TaskModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TaskItem *parentItem = getItem(parent);
    TaskItem *childItem = parentItem->child(row);

    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex TaskModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TaskItem *childItem = getItem(index);
    TaskItem *parentItem = childItem->parent();

    if (parentItem == m_rootItem || !parentItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TaskModel::rowCount(const QModelIndex &parent) const
{
    TaskItem *parentItem = getItem(parent);
    return parentItem->childCount();
}

int TaskModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant TaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TaskItem *item = getItem(index);

    switch (role) {
    case TitleRole:
        return item->title();
    case DescriptionRole:
        return item->description();
    case CompletedRole:
        return item->completed();
    case DueDateRole:
        return item->dueDate();
    case PriorityRole:
        return static_cast<int>(item->priority());
    case TagsRole:
        return item->tags();
    case CategoryRole:
        return item->category();
    case HasChildrenRole:
        return item->childCount() > 0;
    case ChildCountRole:
        return item->childCount();
    default:
        return QVariant();
    }
}

bool TaskModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    TaskItem *item = getItem(index);

    switch (role) {
    case TitleRole:
        item->setTitle(value.toString());
        break;
    case DescriptionRole:
        item->setDescription(value.toString());
        break;
    case CompletedRole:
        item->setCompleted(value.toBool());
        break;
    case DueDateRole:
        item->setDueDate(value.toDateTime());
        break;
    case PriorityRole:
        item->setPriority(static_cast<Priority>(value.toInt()));
        break;
    case TagsRole:
        item->setTags(value.toStringList());
        break;
    case CategoryRole:
        item->setCategory(value.toString());
        break;
    default:
        return false;
    }

    emit dataChanged(index, index, {role});
    return true;
}

Qt::ItemFlags TaskModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QHash<int, QByteArray> TaskModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[DescriptionRole] = "description";
    roles[CompletedRole] = "completed";
    roles[DueDateRole] = "dueDate";
    roles[PriorityRole] = "priority";
    roles[TagsRole] = "tags";
    roles[CategoryRole] = "category";
    roles[HasChildrenRole] = "hasChildren";
    roles[ChildCountRole] = "childCount";
    return roles;
}

void TaskModel::addTask(const QString &title, const QModelIndex &parent)
{
    TaskItem *parentItem = getItem(parent);
    int row = parentItem->childCount();

    beginInsertRows(parent, row, row);
    TaskItem *newTask = new TaskItem(title);
    parentItem->appendChild(newTask);
    endInsertRows();

    emit dataChanged();
}

void TaskModel::removeTask(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    TaskItem *item = getItem(index);
    TaskItem *parentItem = item->parent();

    if (!parentItem)
        return;

    int row = item->row();
    QModelIndex parentIndex = index.parent();

    beginRemoveRows(parentIndex, row, row);
    parentItem->removeChild(row);
    endRemoveRows();

    emit dataChanged();
}

void TaskModel::updateTask(const QModelIndex &index, const QVariantMap &data)
{
    if (!index.isValid())
        return;

    TaskItem *item = getItem(index);

    if (data.contains("title"))
        item->setTitle(data["title"].toString());
    if (data.contains("description"))
        item->setDescription(data["description"].toString());
    if (data.contains("completed"))
        item->setCompleted(data["completed"].toBool());
    if (data.contains("dueDate"))
        item->setDueDate(data["dueDate"].toDateTime());
    if (data.contains("priority"))
        item->setPriority(static_cast<Priority>(data["priority"].toInt()));
    if (data.contains("tags"))
        item->setTags(data["tags"].toStringList());
    if (data.contains("category"))
        item->setCategory(data["category"].toString());

    emit dataChanged(index, index);
    emit dataChanged();
}

QVariantMap TaskModel::getTaskData(const QModelIndex &index) const
{
    QVariantMap data;
    if (!index.isValid())
        return data;

    TaskItem *item = getItem(index);

    data["title"] = item->title();
    data["description"] = item->description();
    data["completed"] = item->completed();
    data["dueDate"] = item->dueDate();
    data["priority"] = static_cast<int>(item->priority());
    data["tags"] = item->tags();
    data["category"] = item->category();

    return data;
}

QJsonArray TaskModel::toJson() const
{
    QJsonArray array;
    for (int i = 0; i < m_rootItem->childCount(); ++i) {
        array.append(m_rootItem->child(i)->toJson());
    }
    return array;
}

void TaskModel::fromJson(const QJsonArray &json)
{
    beginResetModel();
    delete m_rootItem;
    m_rootItem = new TaskItem();

    for (const QJsonValue &value : json) {
        TaskItem *item = TaskItem::fromJson(value.toObject());
        m_rootItem->appendChild(item);
    }

    endResetModel();
}

TaskItem* TaskModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TaskItem *item = static_cast<TaskItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return m_rootItem;
}
