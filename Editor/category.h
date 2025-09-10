#ifndef CATEGORY_H
#define CATEGORY_H

#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QStringList>

class Category : public QObject {
    Q_DISABLE_COPY_MOVE(Category)
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QStringList words READ words WRITE setWords NOTIFY wordsChanged)
    Q_PROPERTY(QString image READ image WRITE setImage NOTIFY imageChanged FINAL)

    QString m_name;
    QStringList m_words;

public:
    Category() = default;
    ~Category() = default;

    // Геттеры
    QString name() const;
    QStringList words() const;

    // Сеттеры
    void setName(const QString& name);
    void setWords(const QStringList& words);

    // Методы сериализации/десериализации
    QJsonObject toJsonObject() const;
    void fromJsonObject(const QJsonObject& jsonObject);

    // Статический метод для создания из JSON
    static Category* fromJson(const QJsonObject& jsonObject);

    QString image() const;
    void setImage(const QString& newImage);

signals:
    void nameChanged();
    void wordsChanged();
    void imageChanged();

private:
    QString m_image;
};

#endif // CATEGORY_H
