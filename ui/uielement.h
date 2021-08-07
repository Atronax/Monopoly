#ifndef UIELEMENT_H
#define UIELEMENT_H

#include <QGraphicsRectItem>

#include <QFont>
#include <QPen>

class UIElement : public QGraphicsRectItem
{
public:
    enum class Shape  {SQUARE, RECTANGLE, ROUNDED_RECTANGLE, ELLIPSE};
    enum class Element{BORDER, FILL, TEXT};
    enum class Role   {BUTTON, TEXT};

    UIElement(const QString& message, QGraphicsRectItem* parent = nullptr);
    ~UIElement();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;

    void setHidden(bool isHidden);
    bool isHidden();

    void setRole(const Role& role);
    void setPosition(const QPoint& position);
    void setSize (const QSize& size);
    void setShape(const Shape& shape);
    void setColor(Element instrument, const QColor& color);
    void setText(const QString& text);
    void setFont (const QFont& font);
    void setBorderWidth(int width);

    const Role& role() const;
    const QPoint& position() const;
    const QSize& size() const;
    const QPainterPath& shape();
    const QColor& color(Element instrument) const;
    const QString& defaultText() const;
    const QString& text() const;
    const QFont& font() const;
    const int& borderWidth() const;



private:
    bool m_isHidden;

    Shape m_shape;
    Role m_element;
    QPainterPath m_path;

    QRectF m_bR;
    QPoint m_position;
    QSize  m_size;
    int    m_borderWidth;
    QColor m_borderColor, m_fillColor, m_textColor;

    QFont   m_font;    
    QString m_defaultText, m_currentText;
};

#endif // UIELEMENT_H
