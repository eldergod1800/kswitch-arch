#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QObject>
#include <QString>
#include <QSortFilterProxyModel>
#include "taskmodel.h"

class TaskFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)

public:
    explicit TaskFilterProxyModel(QObject *parent = nullptr);

    QString searchText() const { return m_searchText; }
    void setSearchText(const QString &text);

signals:
    void searchTextChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString m_searchText;
    bool checkItemAndChildren(const QModelIndex &index) const;
};

class TaskManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TaskModel* taskModel READ taskModel CONSTANT)
    Q_PROPERTY(TaskFilterProxyModel* filteredModel READ filteredModel CONSTANT)
    Q_PROPERTY(QStringList allCategories READ allCategories NOTIFY categoriesChanged)
    Q_PROPERTY(QStringList allTags READ allTags NOTIFY tagsChanged)

public:
    explicit TaskManager(QObject *parent = nullptr);
    ~TaskManager();

    TaskModel* taskModel() const { return m_taskModel; }
    TaskFilterProxyModel* filteredModel() const { return m_filterModel; }

    QStringList allCategories() const { return m_categories; }
    QStringList allTags() const { return m_tags; }

    Q_INVOKABLE void save();
    Q_INVOKABLE void load();
    Q_INVOKABLE void addCategory(const QString &category);
    Q_INVOKABLE void addTag(const QString &tag);
    Q_INVOKABLE void removeCategory(const QString &category);
    Q_INVOKABLE void removeTag(const QString &tag);

signals:
    void categoriesChanged();
    void tagsChanged();

private:
    TaskModel *m_taskModel;
    TaskFilterProxyModel *m_filterModel;
    QString m_dataFilePath;
    QStringList m_categories;
    QStringList m_tags;

    void updateCategoriesAndTags();
    void collectCategoriesAndTags(TaskItem *item);
};

#endif // TASKMANAGER_H
