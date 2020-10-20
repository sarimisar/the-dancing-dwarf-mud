#include "characterlistitemmodel.h"

#include <QQmlEngine>

CharacterListItemModel::CharacterListItemModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

void CharacterListItemModel::declareQML()
{
    qmlRegisterType<CharacterListItemModel>("QMudEnums", 1, 0, "CharacterRoles");
}

void CharacterListItemModel::clear()
{
    if (m_vCharacters.isEmpty())
        return;

    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    m_vCharacters.clear();
    endRemoveRows();
}

void CharacterListItemModel::addCharacter(const CharacterInfo &ch)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_vCharacters.push_back(ch);
    endInsertRows();
}

int CharacterListItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_vCharacters.count();
}

QVariant CharacterListItemModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_vCharacters.count())
        return QVariant();

    const CharacterInfo &ch = m_vCharacters[index.row()];

    if (role == NameRole)
        return ch.name();
    else if (role == LevelRole)
        return ch.level();
    else if (role == ClassRole)
        return ch.chClass();
    else if (role == RaceRole)
        return ch.race();
    else
        return QVariant();
}

QHash<int, QByteArray> CharacterListItemModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[LevelRole] = "level";
    roles[ClassRole] = "chClass";
    roles[RaceRole] = "race";
    return roles;
}

CharacterInfo::CharacterInfo(QString name, int level, QString chClass, QString race) :
    m_strName(name),
    m_iLevel(level),
    m_strClass(chClass),
    m_strRace(race)
{

}

QString CharacterInfo::name() const
{
    return m_strName;
}

int CharacterInfo::level() const
{
    return m_iLevel;
}

QString CharacterInfo::chClass() const
{
    return m_strClass;
}

QString CharacterInfo::race() const
{
    return m_strRace;
}
