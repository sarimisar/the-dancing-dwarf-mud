#include "resourcemanager.h"
#include "formloadresources.h"

#include <QThread>
#include <QObject>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

ResourceManager &ResourceManager::instance()
{
    static ResourceManager instance;
    return instance;
}

QImage applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent = 0)
{
    if (src.isNull())
        return QImage();

    if (!effect)
        return src;

    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(QPixmap::fromImage(src));
    item.setGraphicsEffect(effect);
    scene.addItem(&item);
    QImage res(src.size()+QSize(extent*2, extent*2), QImage::Format_ARGB32);
    res.fill(Qt::transparent);
    QPainter ptr(&res);
    scene.render(&ptr, QRectF(), QRectF( -extent, -extent, src.width()+extent*2, src.height()+extent*2 ) );

    return res;
}

void ResourceManager::loadResources(FormLoadResources *dialog)
{
    QThread *thread = new QThread();

    QObject::connect(thread, &QThread::started, dialog, [&]() {
        qDebug() << "Load resources start... ";

        int totalResources = 4;
        int currentResources = 0;

        emit dialog->status(0);

        // -----------------------
        // White circle
        // -----------------------
        {
            m_whiteCircle = QImage(80, 80, QImage::Format_ARGB32);
            m_whiteCircle.fill(Qt::transparent);

            {
                QPainter painter(&m_whiteCircle);
                painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
                painter.setRenderHints(QPainter::Antialiasing, true);
                painter.setBrush(Qt::white);
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(10, 10, 60, 60);
            }

            QGraphicsDropShadowEffect *e = new QGraphicsDropShadowEffect;
            e->setColor(Qt::black);
            e->setOffset(0, 0);
            e->setBlurRadius(10);
            m_whiteCircle = applyEffectToImage(m_whiteCircle, e, 0);
        }

        emit dialog->status(totalResources == (++currentResources) ? 100 : static_cast<double>(currentResources)/totalResources * 100);

        // -----------------------
        // Red circle
        // -----------------------
        {
            m_redCircle = QImage(80, 80, QImage::Format_ARGB32);
            m_redCircle.fill(Qt::transparent);

            {
                QPainter painter(&m_redCircle);
                painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
                painter.setRenderHints(QPainter::Antialiasing, true);
                painter.setBrush(QColor(250, 100, 100));
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(10, 10, 60, 60);
            }

            QGraphicsDropShadowEffect *e = new QGraphicsDropShadowEffect;
            e->setColor(Qt::black);
            e->setOffset(0, 0);
            e->setBlurRadius(10);
            m_redCircle = applyEffectToImage(m_redCircle, e, 0);
        }

        emit dialog->status(totalResources == (++currentResources) ? 100 : static_cast<double>(currentResources)/totalResources * 100);

        // -----------------------
        // Blue circle
        // -----------------------
        {
            m_blueCircle = QImage(80, 80, QImage::Format_ARGB32);
            m_blueCircle.fill(Qt::transparent);

            {
                QPainter painter(&m_blueCircle);
                painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
                painter.setRenderHints(QPainter::Antialiasing, true);
                painter.setBrush(QColor(100, 100, 255));
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(10, 10, 60, 60);
            }

            QGraphicsDropShadowEffect *e = new QGraphicsDropShadowEffect;
            e->setColor(Qt::black);
            e->setOffset(0, 0);
            e->setBlurRadius(10);
            m_blueCircle = applyEffectToImage(m_blueCircle, e, 0);
        }

        emit dialog->status(totalResources == (++currentResources) ? 100 : static_cast<double>(currentResources)/totalResources * 100);

        // -----------------------
        // Orange circle
        // -----------------------
        {
            m_orangeCircle = QImage(80, 80, QImage::Format_ARGB32);
            m_orangeCircle.fill(Qt::transparent);

            {
                QPainter painter(&m_orangeCircle);
                painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
                painter.setRenderHints(QPainter::Antialiasing, true);
                painter.setBrush(QColor(250, 180, 100));
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(10, 10, 60, 60);
            }

            QGraphicsDropShadowEffect *e = new QGraphicsDropShadowEffect;
            e->setColor(Qt::black);
            e->setOffset(0, 0);
            e->setBlurRadius(10);
            m_orangeCircle = applyEffectToImage(m_orangeCircle, e, 0);
        }

        emit dialog->status(totalResources == (++currentResources) ? 100 : static_cast<double>(currentResources)/totalResources * 100);

        emit dialog->complete();

        qDebug() << "Load resources done";
    });

    thread->start();
}

QImage ResourceManager::whiteCircle() const
{
    return m_whiteCircle;
}

QImage ResourceManager::redCircle() const
{
    return m_redCircle;
}

QImage ResourceManager::blueCircle() const
{
    return m_blueCircle;
}

QImage ResourceManager::orangeCircle() const
{
    return m_orangeCircle;
}

ResourceManager::ResourceManager()
{

}
