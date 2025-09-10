#ifndef JSONPARSER_H
#define JSONPARSER_H

#include "category.h"
#include <QAbstractListModel>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QMimeDatabase>
#include <QSharedPointer>
#include <QString>

class JsonParser : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)

    static QPair<bool, QString> imageToBase64(const QString& filePath);

public:
    enum CategoryRoles {
        NameRole = Qt::UserRole + 1,
        WordsRole,
        ImagesRole
    };

    explicit JsonParser(QObject* parent = nullptr);
    ~JsonParser() override = default;

    // Свойство filePath
    QString filePath() const;
    void setFilePath(const QString& filePath);

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Методы доступа к коллекции
    QSharedPointer<Category> categoryAt(int index) const;
    int categoryCount() const;
    void clearCategories();
    Q_INVOKABLE bool saveToFile(const QString& filePath) const;
    Q_INVOKABLE void setImage(int index, const QString& filePath);

signals:
    void filePathChanged();
    void categoriesChanged();
    void errorOccured(const QString& error) const;

private slots:
    void onFileChanged(const QString& path);

private:
    void parseFile();
    void parseJsonData(const QByteArray& jsonData);

    QString m_filePath;
    QList<QSharedPointer<Category>> m_categories;
};

#endif // JSONPARSER_H
