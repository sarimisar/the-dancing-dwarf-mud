#ifndef CHARACTERLISTITEMMODEL_H
#define CHARACTERLISTITEMMODEL_H

#include <QAbstractListModel>

class CharacterInfo
{
public:
    CharacterInfo(QString name, int level, QString chClass, QString race);

    QString name() const;
    int level() const;
    QString chClass() const;
    QString race() const;

private:
    QString m_strName;
    int m_iLevel;
    QString m_strClass;
    QString m_strRace;
};

class CharacterListItemModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum CharacterRoles {
        NameRole = Qt::UserRole + 1,
        LevelRole,
        ClassRole,
        RaceRole
    };
    Q_ENUMS(CharacterRoles)

    CharacterListItemModel(QObject *parent = Q_NULLPTR);

    static void declareQML();

    void clear();
    void addCharacter(const CharacterInfo &ch);

    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QList<CharacterInfo> m_vCharacters;
};

#endif // CHARACTERLISTITEMMODEL_H
