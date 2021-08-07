#ifndef HISTORYLINEEDIT_H
#define HISTORYLINEEDIT_H

#include <QLabel>
#include <QStringList>
#include <QWheelEvent>

class HistoryLabel : public QLabel
{
    Q_OBJECT

public:
    explicit HistoryLabel(const QString& message, QWidget* parent = nullptr);
    ~HistoryLabel();

    void wheelEvent(QWheelEvent *event) override;

    void logToFile(const QString& filename);

private:
    void makeConnections();
    void showMessage();

    // List of messages and index of current message.
    int m_currentMessageIndex;
    QStringList m_messages;

signals:
    void messageChanged(bool toPrevious);

public slots:
    void addMessage(const QString& message);
    void onMessageChanged(bool toPrevious);
};

#endif // HISTORYLINEEDIT_H
