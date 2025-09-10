#include "jsonparser.h"

QPair<bool, QString> JsonParser::imageToBase64(const QString& filePath)
{
    // Проверяем существование файла
    if (!QFile::exists(filePath)) {
        return qMakePair(false, QString("File does not exist: %1").arg(filePath));
    }

    // Открываем файл для чтения
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return qMakePair(false, QString("Cannot open file: %1. Error: %2").arg(filePath, file.errorString()));
    }

    // Читаем содержимое файла
    QByteArray imageData = file.readAll();
    file.close();

    // Проверяем, что файл не пустой
    if (imageData.isEmpty()) {
        return qMakePair(false, QString("File is empty: %1").arg(filePath));
    }

    // Определяем MIME тип файла
    QMimeDatabase mimeDb;
    QFileInfo fileInfo(filePath);
    QString mimeType = mimeDb.mimeTypeForFile(fileInfo).name();

    // Проверяем, что это изображение
    if (!mimeType.startsWith("image/")) {
        return qMakePair(false, QString("File is not an image: %1. MIME type: %2").arg(filePath, mimeType));
    }

    // Конвертируем в base64
    QString base64String = imageData.toBase64();

    // Формируем data URL для QML Image source
    QString dataUrl = QString("data:%1;base64,%2").arg(mimeType, base64String);

    return qMakePair(true, dataUrl);
}

JsonParser::JsonParser(QObject* parent)
    : QAbstractListModel(parent)
{
}

QString JsonParser::filePath() const
{
    return m_filePath;
}

void JsonParser::setFilePath(const QString& filePath)
{
    const auto adjustedPath = filePath.startsWith("file://", Qt::CaseInsensitive) ? QUrl(filePath).toLocalFile() : filePath;
    if (m_filePath != adjustedPath) {
        m_filePath = adjustedPath;
        emit filePathChanged();
        parseFile();
    }
}

int JsonParser::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_categories.size();
}

bool JsonParser::saveToFile(const QString& filePath) const
{
    emit errorOccured("");
    QJsonArray dataArray;

    // Собираем все категории в JSON массив
    for (const auto& category : m_categories) {
        if (category) {
            dataArray.append(category->toJsonObject());
        }
    }

    // Создаем корневой объект в нужном формате
    QJsonObject rootObject;
    rootObject["data"] = dataArray;

    // Создаем JSON документ
    QJsonDocument document(rootObject);

    // Сохраняем в файл
    QFile file(filePath.startsWith("file://", Qt::CaseInsensitive) ? QUrl(filePath).toLocalFile() : filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        emit errorOccured("Не удалось открыть файл для записи: " + filePath);
        return false;
    }

    qint64 bytesWritten = file.write(document.toJson(QJsonDocument::Indented));
    if (bytesWritten == -1) {
        emit errorOccured("Ошибка записи в файл: " + filePath);
        return false;
    }

    return true;
}

void JsonParser::setImage(int index, const QString& filePath)
{
    if (index < 0 || index >= m_categories.size() || m_categories.at(index).isNull()) {
        return;
    }
    emit errorOccured("");
    const auto& result = imageToBase64(filePath);
    if (!result.first) {
        emit errorOccured(result.second);
        return;
    }
    m_categories.at(index)->setImage(result.second);
    // Создаем QModelIndex для измененного элемента
    QModelIndex modelIndex = createIndex(index, 0);

    // Испускаем сигнал dataChanged
    emit dataChanged(modelIndex, modelIndex);
}

QVariant JsonParser::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_categories.size()) {
        return QVariant();
    }

    const auto& category = m_categories.at(index.row());

    switch (role) {
    case NameRole:
    case Qt::DisplayRole:
        return category->name();
    case WordsRole:
        return category->words();
    case ImagesRole:
        return category->image();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> JsonParser::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[WordsRole] = "words";
    roles[ImagesRole] = "image";
    return roles;
}

QSharedPointer<Category> JsonParser::categoryAt(int index) const
{
    if (index >= 0 && index < m_categories.size()) {
        return m_categories.at(index);
    }
    return nullptr;
}

int JsonParser::categoryCount() const
{
    return m_categories.size();
}

void JsonParser::clearCategories()
{
    beginResetModel();
    m_categories.clear();
    endResetModel();
    emit categoriesChanged();
}

void JsonParser::onFileChanged(const QString& path)
{
    Q_UNUSED(path)
    parseFile();
}

void JsonParser::parseFile()
{
    emit errorOccured("");
    if (m_filePath.isEmpty()) {
        clearCategories();
        return;
    }

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit errorOccured("Не удалось открыть файл: " + m_filePath);
        clearCategories();
        return;
    }

    QByteArray jsonData = file.readAll();
    parseJsonData(jsonData);
}

void JsonParser::parseJsonData(const QByteArray& jsonData)
{
    emit errorOccured("");
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(jsonData, &error);

    if (error.error != QJsonParseError::NoError) {
        emit errorOccured("Ошибка парсинга JSON: " + error.errorString());
        clearCategories();
        return;
    }

    if (!document.isObject()) {
        emit errorOccured("JSON не является объектом");
        clearCategories();
        return;
    }

    QJsonObject rootObject = document.object();
    QJsonArray dataArray = rootObject["data"].toArray();

    beginResetModel();
    m_categories.clear();

    for (const QJsonValue& value : dataArray) {
        if (value.isObject()) {
            QJsonObject categoryObject = value.toObject();
            auto category = QSharedPointer<Category>(Category::fromJson(categoryObject));
            m_categories.append(category);
        }
    }

    endResetModel();
    emit categoriesChanged();
}
