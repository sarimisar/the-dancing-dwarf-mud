#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QImage>

class FormLoadResources;

class ResourceManager
{
public:
    static ResourceManager& instance();

    void loadResources(FormLoadResources* dialog);

    QImage whiteCircle() const;
    QImage redCircle() const;
    QImage blueCircle() const;
    QImage orangeCircle() const;

private:
    ResourceManager();

public:
    ResourceManager(ResourceManager const&) = delete;
    void operator=(ResourceManager const&)  = delete;

private:
    QImage m_whiteCircle;
    QImage m_redCircle;
    QImage m_blueCircle;
    QImage m_orangeCircle;
};

#endif // RESOURCEMANAGER_H
