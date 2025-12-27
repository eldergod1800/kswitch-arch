#ifndef TASKMODEL_H
#define TASKMODEL_H

#include <QAbstractItemModel>
#include <QDateTime>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>

enum class Priority {
    None = 0,
    Low = 1,
    Medium = 2,
    High = 3
};

class TaskItem {
public:
    TaskItem(const QString &title = QString(), TaskItem *parent = nullptr);
    ~TaskItem();

    // Getters
    QString title() const { return m_title; }
    QString description() const { return m_description; }
    bool completed() const { return m_completed; }
    QDateTime dueDate() const { return m_dueDate; }
    Priority priority() const { return m_priority; }
    QStringList tags() const { return m_tags; }
    QString category() const { return m_category; }
    TaskItem *parent() const { return m_parent; }
    QList<TaskItem*> children() const { return m_children; }

    // Setters
    void setTitle(const QString &title) { m_title = title; }
    void setDescription(const QString &desc) { m_description = desc; }
    void setCompleted(bool completed);
    void setDueDate(const QDateTime &date) { m_dueDate = date; }
    void setPriority(Priority priority) { m_priority = priority; }
    void setTags(const QStringList &tags) { m_tags = tags; }
    void setCategory(const QString &category) { m_category = category; }

    // Child management
    void appendChild(TaskItem *child);
    void insertChild(int position, TaskItem *child);
    void removeChild(int position);
    TaskItem *child(int row) const;
    int childCount() const;
    int row() const;

    // Serialization
    QJsonObject toJson() const;
    static TaskItem* fromJson(const QJsonObject &json, TaskItem *parent = nullptr);

    // Search
    bool matchesSearch(const QString &searchText) const;

private:
    QString m_title;
    QString m_description;
    bool m_completed;
    QDateTime m_dueDate;
    Priority m_priority;
    QStringList m_tags;
    QString m_category;

    TaskItem *m_parent;
    QList<TaskItem*> m_children;

    void updateChildrenCompletionState();
};

class TaskModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        DescriptionRole,
        CompletedRole,
        DueDateRole,
        PriorityRole,
        TagsRole,
        CategoryRole,
        HasChildrenRole,
        ChildCountRole
    };

    explicit TaskModel(QObject *parent = nullptr);
    ~TaskModel();

    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Task management
    Q_INVOKABLE void addTask(const QString &title, const QModelIndex &parent = QModelIndex());
    Q_INVOKABLE void removeTask(const QModelIndex &index);
    Q_INVOKABLE void updateTask(const QModelIndex &index, const QVariantMap &data);
    Q_INVOKABLE QVariantMap getTaskData(const QModelIndex &index) const;

    // Serialization
    QJsonArray toJson() const;
    void fromJson(const QJsonArray &json);

    // Utility
    TaskItem* getItem(const QModelIndex &index) const;

signals:
    void dataChanged();

private:
    TaskItem *m_rootItem;
};

#endif // TASKMODEL_H
