#ifndef DIE_H
#define DIE_H

#include <QTimer>

#include "uielement.h"

class Die : public QObject
{
    Q_OBJECT

public:
    Die();   
    ~Die();

    void drop();
    void stop();

    const QImage&  side() const;
    int           value() const;


private:
    void clear();

    void loadSpritelist    (int frames, const QSize& framesize, const QString& filename);
    void prepareSpritelist (int frames, const QSize& framesize, const QImage&  spritelist);

    // use this class
    // * to store the available sides as images
    // * to take the current image to draw it on the view panel
    // * to get the received pseudo-random side index for later use
    QList<QImage> *m_spritelist;
    QImage m_currentSide;

    bool m_stopped;
    int  m_sideIdx;

    // move view relative stuff to some other class
    // UIElement* m_dropDieButton;

    // when user turns on some button: the timer starts and spritelist begins to change the images rapidly (var.1)
    // when user turns of the button : the timer slows down () and eventually stops
    // the got image is compared then with the one in the list and corresponding integer value is returned and set as a current value
    QTimer m_dropTimer;
    bool   m_stopping;
    int    m_interval; // interval between timer ticks in milliseconds

signals:

public slots:
    void onDieDropped();

};

#endif // DIE_H
