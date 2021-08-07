#include "uielement.h"

#include <QPainter>
#include <QDebug>

UIElement::UIElement(const QString& message, QGraphicsRectItem* parent)
    : QGraphicsRectItem (parent)
{
    m_defaultText = message;

    setText(message);
    setSize(QSize(0,0));
    setShape(Shape::RECTANGLE);
    setColor(Element::BORDER, QColor("#bbb"));
    setColor(Element::FILL,   QColor("#111"));
    setColor(Element::TEXT,   QColor("#fff"));
    setHidden(true);
}

UIElement::~UIElement()
{

}

QRectF UIElement::boundingRect() const
{
    return rect();
}

void UIElement::setRole(const Role &elementType)
{
    m_element = elementType;
}

void UIElement::setPosition(const QPoint &position)
{
    qDebug() << text() << ": " << position;

    m_position = position;
    m_bR = QRectF(m_position.x(), m_position.y(), m_size.width(), m_size.height());
    // setPos(position);
}

void UIElement::setSize(const QSize &size)
{
    m_size = size;
}

void UIElement::setHidden(bool isHidden)
{
    m_isHidden = isHidden;
}

void UIElement::setShape(const Shape& shape)
{
    int maxValue = qMax(m_size.width(), m_size.height());

    m_shape = shape;
    switch (m_shape)
    {
        case Shape::SQUARE:
        m_path.addRect(QRect(rect().x(), rect().y(), maxValue, maxValue));
        break;

        case Shape::RECTANGLE:
        m_path.addRect(QRect(rect().x(), rect().y(), m_size.width(), m_size.height()));
        break;

        case Shape::ROUNDED_RECTANGLE:
        m_path.addRoundedRect(QRect(rect().x(), rect().y(), m_size.width(), m_size.height()), 12, 12);
        break;

        case Shape::ELLIPSE:
        m_path.addEllipse(QRect(rect().x(), rect().y(), m_size.width(), m_size.height()));
        break;
    }
}

void UIElement::setBorderWidth(int width)
{
    if (width >= 0 && width <= 5)
        m_borderWidth = width;
    else
        qDebug() << "Width should be in range[0;5].";
}

void UIElement::setColor(UIElement::Element instrument, const QColor &color)
{
    switch (instrument)
    {
        case UIElement::Element::BORDER:
        m_borderColor = color;
        break;

        case UIElement::Element::FILL:
        m_fillColor = color;
        break;

        case UIElement::Element::TEXT:
        m_textColor = color;
    }
}

void UIElement::setFont(const QFont &font)
{
    m_font = font;
}

void UIElement::setText(const QString &text)
{
    if (m_currentText != text)
        m_currentText = text;
}

const UIElement::Role& UIElement::role() const
{
    return m_element;
}

const QPoint &UIElement::position() const
{
    return m_position;
}

const QPainterPath &UIElement::shape()
{
    return m_path;
}

const QColor &UIElement::color(UIElement::Element instrument) const
{
    switch (instrument)
    {
    case UIElement::Element::BORDER:
        return m_borderColor;
        break;

    case UIElement::Element::FILL:
        return m_fillColor;
        break;

    case UIElement::Element::TEXT:
        return m_textColor;
        break;
    }
}

const QString &UIElement::defaultText() const
{
    return m_defaultText;
}

bool UIElement::isHidden()
{
    return m_isHidden;
}

const QSize &UIElement::size() const
{
    return m_size;
}

const QString &UIElement::text() const
{
    return m_currentText;
}

const QFont &UIElement::font() const
{
    return m_font;
}

const int &UIElement::borderWidth() const
{
    return m_borderWidth;
}

void UIElement::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Do nothing, if the UIElement has parent.
    if (parentItem())
    {
        qDebug() << "There is a parent.";
        return;
    }

    // If UIElement is hidden, no need to do anything.
    if (isHidden())
        return;

    // To test: different color of graphics items depending on current mouse events.
    // 1. Active move mouse events, change active state and drawing instruments
    // 2. Tinker with StyleOption class instance
    // 3. Try fill and load qss.

    // Prepare drawing instruments.
    painter->setRenderHint(QPainter::Antialiasing);
    QPen borderPen(QBrush(m_borderColor), m_borderWidth);
    QPen   textPen(m_textColor);

    // Draw UIElement shape and put some text inside.
    // Since the path is generated at origin position, we need to move the painter to where it should be drawn.
    // In our case this is the top left button if this UIElement bounding rectangle. We'll use translate method to get this transform.

    if (!m_path.isEmpty())
    {
        painter->setPen(borderPen);
            painter->drawPath(m_path);
            painter->fillPath(m_path, m_fillColor);

        painter->setPen(textPen);
        painter->setFont(m_font);
            painter->drawText(m_path.boundingRect(), m_defaultText, QTextOption(Qt::AlignCenter));
    }    
}
