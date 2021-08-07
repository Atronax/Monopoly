#include "historylabel.h"

#include <QMouseEvent>
#include <QMessageBox>
#include <QDateTime>
#include <QFile>
#include <QDebug>

HistoryLabel::HistoryLabel(const QString& message, QWidget *parent)
    : QLabel(message, parent),
      m_currentMessageIndex(0)
{
    makeConnections();
}

HistoryLabel::~HistoryLabel()
{

}

void HistoryLabel::wheelEvent(QWheelEvent *ev)
{
    // Widget should show previous message, it the wheel is scrolled up, and next message, if the wheel is scrolled down.
    // We'll find delta and send relevant signal to update the current message.

    int movement = ev->angleDelta().ry();
    if (movement > 0)
        emit messageChanged(true);
    else
        emit messageChanged(false);

    ev->accept();
}

void HistoryLabel::logToFile(const QString &filename)
{
    QFile file (filename);

    bool fileIsAvailable = true;
    if (file.exists())
    {
        QMessageBox mb (QMessageBox::Warning,
                        QString("File with the name %1 already exists.").arg(filename),
                        QString("Do you really want to use this file?"),
                        QMessageBox::Ok | QMessageBox::Cancel);

        if (QMessageBox::Ok != mb.exec())
            fileIsAvailable = false;
    }

    if (fileIsAvailable)
    {
        if (file.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            QTextStream stream(&file);

            stream << "************************** \n\n";
            stream << QDateTime::currentDateTime().toString() + "\n\n";
            for (int i = 0; i < m_messages.count(); ++i)
                stream << i << ". " <<  m_messages.at(i) + (i < m_messages.count() - 1 ? "\n" : "");
            stream << "\n\n";
            stream << "**************************";

            file.close();
        }
    }
}

void HistoryLabel::makeConnections()
{
    connect (this, SIGNAL(messageChanged(bool)), this, SLOT(onMessageChanged(bool)));
}

void HistoryLabel::showMessage()
{
    if (!m_messages.isEmpty())
        setText(QString("%1. %2").arg(m_currentMessageIndex).arg(m_messages.at(m_currentMessageIndex)));
}

void HistoryLabel::addMessage(const QString &message)
{
    m_messages.append(message);
    m_currentMessageIndex = m_messages.count() - 1;

    showMessage();
}

void HistoryLabel::onMessageChanged(bool toPrevious)
{
    if (toPrevious)
    {
        --m_currentMessageIndex;
        if (m_currentMessageIndex < 0)
            m_currentMessageIndex = 0;
    }
    else
    {
        ++m_currentMessageIndex;
        if (m_currentMessageIndex > m_messages.count() - 1)
            m_currentMessageIndex = m_messages.count() - 1;
    }

    showMessage();
}
