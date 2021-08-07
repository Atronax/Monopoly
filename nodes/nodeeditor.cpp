#include "nodeeditor.h"

#include <QFileDialog>
#include <QDebug>

NodeEditor::NodeEditor(Node* node, QWidget *parent)
    : QDialog (parent)
{
    gatherData(node);
    makeUI();   
}

NodeEditor::~NodeEditor()
{
    clearUI();
}

const QString &NodeEditor::name()
{
    return m_name;
}

const QString &NodeEditor::description()
{
    return m_description;
}

const QString &NodeEditor::imagePath()
{
    return m_imagePath;
}

int NodeEditor::activatedTokenIndex(const Description::ObjectType& type)
{
    if (type == Description::ObjectType::ACTION_TOKEN)
        return m_ATSelected;

    if (type == Description::ObjectType::OWNERSHIP_TOKEN)
        return m_OTSelected;

    return -1;
}

void NodeEditor::makeSelector(const Description::ObjectType& type, QList<Description*>* description)
{
    // Take relevant information from OT description blocks.
    switch(type)
    {
        case Description::ObjectType::ACTION_TOKEN:
        {
            for (int i = 0; i < description->count(); ++i)
            {
                int index = description->at(i)->index();
                QString name = description->at(i)->name();

                m_ATData.append(QString("%1. %2").arg(index).arg(name));
            }

            fillATCombobox();
        }
        break;

        case Description::ObjectType::OWNERSHIP_TOKEN:
        {
            for (int i = 0; i < description->count(); ++i)
            {
                int index = description->at(i)->index();
                QString name = description->at(i)->name();

                m_OTData.append(QString("%1. %2").arg(index).arg(name));
            }

            fillOTCombobox();
        }
        break;

        case Description::ObjectType::CARD  :
        case Description::ObjectType::EMPTY :
        break;
    }
}

void NodeEditor::gatherData(Node *node)
{
    Token *token = node->token();

    m_name = token->name();
    m_description = token->description();
    m_imagePath = token->imagePath();
}

void NodeEditor::fillOTCombobox()
{
    // If there is no data in the list, do nothing.
    if (m_OTData.isEmpty())
    {
        qDebug() << "There is no data in the list of ownership tokens.";
        return;
    }

    // Else:
    // Remove old items and add new to combobox, attach connections.
    OTSelectorCombobox->clear();
    OTSelectorCombobox->addItems(m_OTData);
    OTSelectorCombobox->addItem("Nothing");
    OTSelectorCombobox->setCurrentIndex(OTSelectorCombobox->count() - 1);

}

void NodeEditor::fillATCombobox()
{
    if (m_ATData.isEmpty())
    {
        qDebug() << "There is no data in the list of action tokens.";
        return;
    }

    ATSelectorCombobox->clear();
    ATSelectorCombobox->addItems(m_ATData);
    ATSelectorCombobox->addItem("Nothing");
    ATSelectorCombobox->setCurrentIndex(ATSelectorCombobox->count() - 1);
}

void NodeEditor::makeUI()
{
    nameLabel = new QLabel ("Name:");
    nameEditor = new QLineEdit (m_name);

    descriptionLabel = new QLabel ("Description:");
    descriptionEditor = new QTextEdit (m_description);

    imageLabel = new QLabel ("Image path:");
    imageButton = new QPushButton (m_imagePath);

    // Also make some group using radiobutton or whatsoever to choose only one type of token for a single node.
    ATSelectorLabel = new QLabel("Action token selector:");
    ATSelectorCombobox = new QComboBox();
    ATSelectorCombobox->setObjectName("ATSelector");

    OTSelectorLabel = new QLabel("Ownership token selector:");
    OTSelectorCombobox = new QComboBox();        
    OTSelectorCombobox->setObjectName("OTSelector");

    selectorsGroupBox = new QGroupBox ("Choose token");

    actionTokensRB = new QRadioButton("Action token");
    actionTokensRB->setObjectName("ATRadiobutton");

    ownershipTokensRB = new QRadioButton("Ownership token");
    ownershipTokensRB->setObjectName("OTRadiobutton");

    QVBoxLayout* selectorsLayout = new QVBoxLayout(this);
    selectorsLayout->addWidget(actionTokensRB);
    selectorsLayout->addWidget(ownershipTokensRB);
    selectorsGroupBox->setLayout(selectorsLayout);

    pb_ok = new QPushButton ("Ok");   
    pb_cancel = new QPushButton ("Cancel");

    connect (nameEditor, SIGNAL(textChanged(const QString&)), this, SLOT(onNameChanged(const QString&)));
    connect (descriptionEditor, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()));
    connect (actionTokensRB, SIGNAL(clicked()), this, SLOT(onTokenRBClicked()));
    connect (ownershipTokensRB, SIGNAL(clicked()), this, SLOT(onTokenRBClicked()));
    connect (ATSelectorCombobox, SIGNAL(activated(int)), this, SLOT(onTokenActivated(int)));
    connect (OTSelectorCombobox, SIGNAL(activated(int)), this, SLOT(onTokenActivated(int)));
    connect (imageButton, SIGNAL(clicked()), this, SLOT(onImageChanged()));
    connect (pb_ok, SIGNAL(clicked()), this, SLOT(onOk()));
    connect (pb_cancel, SIGNAL(clicked()), this, SLOT(reject()));

    m_layout = new QGridLayout (this);
    m_layout->addWidget(nameLabel,             0, 0, 1, 1);
    m_layout->addWidget(nameEditor,            0, 1, 1, 1);
    m_layout->addWidget(descriptionLabel,      1, 0, 2, 1);
    m_layout->addWidget(descriptionEditor,     1, 1, 2, 1);
    m_layout->addWidget(imageLabel,            3, 0, 1, 1);
    m_layout->addWidget(imageButton,           3, 1, 1, 1);
    m_layout->addWidget(selectorsGroupBox,     4, 0, 1, 2);
    // when one of the radiobuttons trigger the click, 5th line of layout changes to AT or OT selector
    /*
    m_layout->addWidget(ATSelectorLabel,       4, 0, 1, 1);
    m_layout->addWidget(ATSelectorCombobox,    4, 1, 1, 1);
    m_layout->addWidget(OTSelectorLabel,       5, 0, 1, 1);
    m_layout->addWidget(OTSelectorCombobox,    5, 1, 1, 1);
    */
    m_layout->addWidget(pb_cancel,             6, 0, 1, 1);
    m_layout->addWidget(pb_ok,                 6, 1, 1, 1);
    setLayout(m_layout);

    pb_cancel->setFocus();

}

void NodeEditor::clearUI()
{
    nameLabel->deleteLater();
    nameEditor->deleteLater();
    descriptionLabel->deleteLater();
    descriptionEditor->deleteLater();
    OTSelectorLabel->deleteLater();
    OTSelectorCombobox->deleteLater();
    imageLabel->deleteLater();
    imageButton->deleteLater();
    pb_ok->deleteLater();
    pb_cancel->deleteLater();

    m_layout->deleteLater();
}

void NodeEditor::onNameChanged(const QString& text)
{
    // check for validity
    if (text.length() >= 1)
        m_name = text;
    else
        qDebug() << "Make sure the text has length of 1 character or more.";
}

void NodeEditor::onDescriptionChanged()
{
    // the same goes here
    QString text = static_cast<QTextEdit*>(sender())->toPlainText();
    if (text.length() >= 1)
        m_description = text;
}

void NodeEditor::onImageChanged()
{
    QString imagePath = QFileDialog::getOpenFileName(nullptr, "Choose image", QString(), "*.png *.jpg");

    m_imagePath = imagePath;
    imageButton->setText(m_imagePath);
}

void NodeEditor::onTokenRBClicked()
{
    QRadioButton* rb = dynamic_cast<QRadioButton*>(sender());
    QString name = rb->objectName();

    qDebug() << "in onTokenRBToggled:: " << name;

    if (name == "ATRadiobutton")
    {
        m_layout->addWidget(ATSelectorLabel,    5, 0, 1, 1);
        m_layout->addWidget(ATSelectorCombobox, 5, 1, 1, 1);
        ATSelectorLabel->show();
        ATSelectorCombobox->show();

        m_layout->removeWidget(OTSelectorLabel);
        m_layout->removeWidget(OTSelectorCombobox);
        OTSelectorLabel->hide();
        OTSelectorCombobox->hide();
    }

    if (name == "OTRadiobutton")
    {
        m_layout->addWidget(OTSelectorLabel,    5, 0, 1, 1);
        m_layout->addWidget(OTSelectorCombobox, 5, 1, 1, 1);
        OTSelectorLabel->show();
        OTSelectorCombobox->show();

        m_layout->removeWidget(ATSelectorLabel);
        m_layout->removeWidget(ATSelectorCombobox);
        ATSelectorLabel->hide();
        ATSelectorCombobox->hide();
    }
}

void NodeEditor::onTokenActivated(int index)
{
    QComboBox* cb = dynamic_cast<QComboBox*>(sender());
    QString name = cb->objectName();

    if (name == "ATSelector")
        m_ATSelected = index;

    if (name == "OTSelector")
        m_OTSelected = index;
}

void NodeEditor::onOk()
{
    if (!m_name.isEmpty() && !m_description.isEmpty())
        accept();
    else
        qDebug() << "Fill all the neccessary information";
}

