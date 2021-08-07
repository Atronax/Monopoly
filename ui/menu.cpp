#include "menu.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QDebug>

#include "ui/uielementfactory.h"

Menu::Menu()
{

}

Menu::~Menu()
{
    deleteButtons();
}

QRectF Menu::boundingRect() const
{
    return rect();
}

void Menu::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Drawing instruments
    painter->setRenderHint(QPainter::Antialiasing, true);

    QPen border1Pen = QPen(QColor("#000"), 2);
    QPen border2Pen = QPen(QColor("#eee"), 1);
    QPen   textPen = QPen(Qt::white);
    QColor backgroundColor = QColor("#111");
    backgroundColor.setAlpha(233);

    // Draw shapes
    painter->setPen(border1Pen);
        painter->drawRect(boundingRect());
    painter->setPen(border2Pen);
        painter->drawRect(boundingRect().adjusted(4, 4, -4, -4));
        painter->fillRect(boundingRect().adjusted(4, 4, -4, -4), backgroundColor);

    painter->setPen(textPen);
    for (int i = 0; i < m_buttons->count(); ++i)
    {
        UIElement* button = m_buttons->at(i);        
        if (!button->isHidden())
        {
            if (!button->shape().isEmpty())
            {
                painter->setPen(QPen(button->color(UIElement::Element::BORDER), button->borderWidth()));
                    painter->drawPath(button->shape());
                    painter->fillPath(button->shape(), button->color(UIElement::Element::FILL));

                painter->setPen(button->color(UIElement::Element::TEXT));
                painter->setFont(button->font());
                    painter->drawText(button->shape().boundingRect(), button->text(), QTextOption(Qt::AlignCenter));
            }
        }
    }
}

UIElement *Menu::buttonAtPosition(const QPoint &pixelPosition)
{
    UIElement* pretender = nullptr;

    for (int i = 0; i < m_buttons->count(); ++i)
    {
        UIElement* button = m_buttons->at(i);

        // qDebug() << "BR: " << button->rect();
        // qDebug() << "PP: " << pixelPosition;

        if (button && !button->isHidden() && button->rect().contains(pixelPosition))
        {
            pretender = button;
            break;
        }
    }

    if (pretender == nullptr)
        qDebug() << QString("There is no button at position (%1,%2).").arg(pixelPosition.x()).arg(pixelPosition.y());

    return pretender;
}

void Menu::showButtons()
{
    if (!scene())
    {
        qDebug() << "There is no active scene yet";
        return;
    }

    qDebug() << QString("There are %1 buttons in menu.").arg(m_buttons->count());

    for (int i = 0; i < m_buttons->count(); ++i)
    {
        UIElement* button = m_buttons->at(i);

        if (button->isHidden())
        {
            button->setHidden(false);
            scene()->addItem(button);
        }
    }
}

void Menu::hideButtons()
{
    if (!scene())
    {
        qDebug() << "There is no active scene yet";
        return;
    }

    for (int i = 0; i < m_buttons->count(); ++i)
    {
        UIElement* button = m_buttons->at(i);

        if (!button->isHidden())
        {
            button->setHidden(true);
            scene()->removeItem(button);
        }
    }
}

void Menu::defaultButtons()
{
    qDebug() << "unselectButtons triggered";
    for (int i = 0; i < m_buttons->count(); ++i)
    {
        UIElement* button = m_buttons->at(i);

        button->setColor(UIElement::Element::FILL,   QColor("#111"));
        button->setFont(QFont("Comic Sans", 10));
        button->setText(button->defaultText());
    }
}

bool Menu::isHidden() const
{
    return m_isHidden;
}

void Menu::setHidden(bool isHidden)
{
    m_isHidden = isHidden;
    if (m_isHidden)
        hideButtons();
    else
        showButtons();
}

void Menu::setColor(const QColor &color)
{
    for (int i = 0; i < m_buttons->count(); ++i)
    {
        UIElement* button = m_buttons->at(i);
        button->setColor(UIElement::Element::TEXT, color);
    }
}

void Menu::setState(const Menu::State &state)
{
    m_state = state;
    switch (m_state)
    {
        case State::MENU_HOVER:
        if (!m_menuHovered)
            m_menuHovered = true;

        qDebug() << "menu hovered";

        // menu enter event
        if (m_buttonHovered)
        {
            // do something with menu and buttons
            defaultButtons();
            m_buttonHovered = false;
        }
        break;

        case State::BUTTON_HOVER:
        if (!m_buttonHovered)
            m_buttonHovered = true;

        qDebug() << "button hovered";

        // button enter event
        if (m_menuHovered)
        {
            // do something with menu and buttons
            m_menuHovered = false;
        }
        break;

        case State::DEFAULT:
        break;

    }
}

void Menu::createButtons()
{
    m_buttons = new QList<UIElement*>();

    int ws = 13, b = 46, s = 42, m = 10;

    m_buttons->append(UIElementFactory::create(UIElement::Role::BUTTON, "New Game", QSize(233, 33), QPoint(rect().x() + ws, rect().y() + b + (s + m)*0), this));
    m_buttons->append(UIElementFactory::create(UIElement::Role::BUTTON, "Save",     QSize(233, 33), QPoint(rect().x() + ws, rect().y() + b + (s + m)*1), this));
    m_buttons->append(UIElementFactory::create(UIElement::Role::BUTTON, "Load",     QSize(233, 33), QPoint(rect().x() + ws, rect().y() + b + (s + m)*2), this));
    m_buttons->append(UIElementFactory::create(UIElement::Role::BUTTON, "Options",  QSize(233, 33), QPoint(rect().x() + ws, rect().y() + b + (s + m)*3), this));
    m_buttons->append(UIElementFactory::create(UIElement::Role::BUTTON, "Quit",     QSize(233, 33), QPoint(rect().x() + ws, rect().y() + b + (s + m)*4), this));

    /*
    createButton ("New",      b + (s + m)*0);
    createButton ("Save",     b + (s + m)*1);
    createButton ("Load",     b + (s + m)*2);
    createButton ("Options",  b + (s + m)*3);
    createButton ("Quit",     b + (s + m)*4);
    */
}

void Menu::deleteButtons()
{
    for (int i = 0; i < m_buttons->count(); ++i)
    {
        UIElement* button = m_buttons->at(i);
        m_buttons->removeOne(button);

        delete button;
        button = nullptr;
    }

    delete m_buttons;
    m_buttons = nullptr;
}
