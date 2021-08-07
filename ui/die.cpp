#include "die.h"

#include <QFileInfo>
#include <QDebug>

Die::Die()
{
    loadSpritelist(6, QSize(100, 100), "D:/monopoly/die/spritelist.png");
}

Die::~Die()
{
    clear();
}

void Die::drop()
{
    m_stopped = false;
    m_stopping = false;
    m_interval = 55;

    connect(&m_dropTimer, SIGNAL(timeout()), this, SLOT(onDieDropped()));
    m_dropTimer.start(m_interval);
}

void Die::stop()
{
    m_stopping = true;
}

const QImage &Die::side() const
{
    return m_currentSide;
}

int Die::value() const
{
    if (m_stopped)
        return m_sideIdx;
    else
    {
        qDebug() << "Timer is till active";
        return -1;
    }
}

void Die::clear()
{
    if (m_spritelist)
        m_spritelist->clear();

    delete m_spritelist;
    m_spritelist = nullptr;
}

void Die::loadSpritelist(int frames, const QSize& framesize, const QString &filename)
{
    QFileInfo fi (filename);
    if (fi.exists() && fi.suffix() == "png")
    {
        QImage spritelist (filename);
        prepareSpritelist (frames, framesize, spritelist);
    }
    else
    {
        qDebug() << QString("There is no image with the name %1.").arg(filename);
    }
}

void Die::prepareSpritelist(int frames, const QSize& framesize, const QImage &spritelist)
{
    m_spritelist = new QList<QImage>();

    // Cut the spritelist based on frames count and size.
    for (int i = 0; i < frames; ++i)
    {
        QRect  imageRect = QRect(QPoint(0, framesize.height() * i), QPoint(framesize.width(), framesize.width() * (i + 1)));
        QImage frame = spritelist.copy(imageRect);

        m_spritelist->append(frame);
    }
}

void Die::onDieDropped()
{
    // if the list is empty, do nothing
    // if there is no current image, take first image from the list
    // otherwise, take next image from the list in a circular order
    if (!m_spritelist || m_spritelist->isEmpty())
    {
        qDebug() << "There is no frames data.";
        return;
    }

    if (m_currentSide.isNull())
    {
        qDebug() << "Setting first image of the spritelist as current.";
        m_sideIdx = 0;
    }
    else
    {
        qDebug() << "Setting next image of the spritelist in a circular way.";
        if (m_sideIdx == m_spritelist->count())
            m_sideIdx = 0;

        ++m_sideIdx;
    }

    m_currentSide = m_spritelist->at(m_sideIdx);


    // If user clicked the stop button, interval between timer ticks should slowly increase until some value and then stop at all
    if (m_stopping)
    {
        m_interval *= 1.5f;
        if (m_interval > 1000)
        {
            // if interval gets to some value, find the value connected to image and set it as current value
            m_stopped = true;

            m_dropTimer.stop();
            disconnect(&m_dropTimer, SIGNAL(timeout()), this, SLOT(onDieDropped()));
        }
    }
}
