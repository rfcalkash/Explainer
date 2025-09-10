#include "category.h"

QString Category::name() const
{
    return m_name;
}

QStringList Category::words() const
{
    return m_words;
}

void Category::setName(const QString& name)
{
    if (name == m_name) {
        return;
    }
    m_name = name;
    emit nameChanged();
}

void Category::setWords(const QStringList& words)
{
    if (words == m_words) {
        return;
    }
    m_words = words;
    emit wordsChanged();
}

QJsonObject Category::toJsonObject() const
{
    QJsonObject obj;
    obj["name"] = m_name;
    obj["image"] = m_image;
    QJsonArray wordsArray;
    for (const QString& word : m_words) {
        wordsArray.append(word);
    }
    obj["words"] = wordsArray;

    return obj;
}

void Category::fromJsonObject(const QJsonObject& jsonObject)
{
    m_name = jsonObject["name"].toString();
    m_image = jsonObject["image"].toString();

    QJsonArray wordsArray = jsonObject["words"].toArray();
    m_words.clear();
    for (const QJsonValue& value : std::as_const(wordsArray)) {
        m_words.append(value.toString());
    }
}

Category* Category::fromJson(const QJsonObject& jsonObject)
{
    auto category = new Category();
    category->fromJsonObject(jsonObject);
    return category;
}

QString Category::image() const
{
    return m_image;
}

void Category::setImage(const QString& newImage)
{
    if (m_image == newImage)
        return;
    m_image = newImage;
    emit imageChanged();
}
