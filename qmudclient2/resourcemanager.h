#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QImage>


class FormLoadResources;
class QSGTexture;
class QQuickWindow;

class ResourceManager
{
public:
    static ResourceManager& instance();

    void loadResources(FormLoadResources* dialog);

    QImage whiteCircle() const;
    QImage redCircle() const;
    QImage blueCircle() const;
    QImage orangeCircle() const;
    QImage blueRectangle() const;

private:
    ResourceManager();

public:
    ResourceManager(ResourceManager const&) = delete;
    void operator=(ResourceManager const&)  = delete;

    struct LoadedMapData
    {
        LoadedMapData() :
            textureLevel0(Q_NULLPTR),
            textureLevel1(Q_NULLPTR)
        {}

        QSGTexture *textureLevel0;
        QSGTexture *textureLevel1;
        QPoint id;
    };

    const LoadedMapData &map(QPoint id, QQuickWindow* wnd);

private:
    QImage m_whiteCircle;
    QImage m_redCircle;
    QImage m_blueCircle;
    QImage m_orangeCircle;
    QImage m_blueRectangle;

    QVector<LoadedMapData> m_vLoadedMaps;
};

#endif // RESOURCEMANAGER_H
