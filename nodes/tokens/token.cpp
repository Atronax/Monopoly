#include "token.h"

#include <QFile>
#include <QDebug>

Token::Token()
    : QGraphicsRectItem()
{

}

Token::Token(const QString& name, const QString& description, const QString& imagePath)
    : QGraphicsRectItem ()
{
    setName(name);
    setDescription(description);
    setImage(imagePath);
}

Token::Token(const Token &other)
    : QGraphicsRectItem()
{    
    setName(other.name());
    setDescription(other.description());
    setImage(other.imagePath());
}

Token::~Token()
{

}

QRectF Token::boundingRect() const
{
    return rect();
}

void Token::setName(const QString &n)
{
    m_name = n;
}

void Token::setDescription(const QString &d)
{
    m_description = d;
}

void Token::setImage(const QString &path)
{
    m_imagePath = path;
    qDebug() << "here" << m_imagePath;

    m_image = QImage(path);
    qDebug() << m_image.format();

    qDebug() << m_image.size();
}

const QString &Token::name() const
{
    return m_name;
}

const QImage &Token::image() const
{
    return m_image;
}

const QString &Token::imagePath() const
{
    qDebug() << QString("Image path for token %1 is %2").arg(m_name).arg(m_imagePath);

    return m_imagePath;
}

const QString &Token::description() const
{
    return m_description;
}

QString Token::toString()
{
    return QString("Name: %1. Description: %2. Image path: %3.").arg(name()).arg(description()).arg(imagePath());
}

void Token::operator=(const Token &rhs)
{
    setName(rhs.name());
    setDescription(rhs.description());
    setImage(rhs.imagePath());
}

void Token::activate()
{

}

QDataStream& operator<<(QDataStream &out, const Token &t)
{
    out << t.name() << t.description() << t.imagePath();

    return out;
}

QDataStream& operator>>(QDataStream &in, Token &t)
{
    QString name;
    QString description;
    QString imagePath;

    in >> name >> description >> imagePath;

    t.setName(name);
    t.setDescription(description);
    t.setImage(imagePath);

    return in;
}
