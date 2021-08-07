#ifndef NODEEDITOR_H
#define NODEEDITOR_H

#include <QDialog>

#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>
#include <QGridLayout>

#include "node.h"
#include "helper/description.h"

class NodeEditor : public QDialog
{
    Q_OBJECT

public:
    NodeEditor(Node* node, QWidget *parent = nullptr);
    ~NodeEditor();

    const QString& name();
    const QString& description();
    const QString& imagePath();
    int activatedTokenIndex(const Description::ObjectType& type);

    void makeSelector(const Description::ObjectType& type, QList<Description*>* description);

private:
    void gatherData(Node* node);
    void fillOTCombobox();
    void fillATCombobox();

    void makeUI();
    void clearUI();

    void ok();
    void cancel();

    QString m_name;
    QString m_description;
    QString m_imagePath;

    QStringList m_OTData;
    QStringList m_ATData;
    int m_OTSelected = -1;
    int m_ATSelected = -1;

    QLabel *nameLabel;
    QLabel *descriptionLabel;
    QLabel *imageLabel;
    QLabel *ATSelectorLabel;
    QLabel *OTSelectorLabel;

    QLineEdit   *nameEditor;
    QTextEdit   *descriptionEditor;
    QPushButton *imageButton;
    QComboBox   *OTSelectorCombobox;
    QComboBox   *ATSelectorCombobox;

    QGroupBox    *selectorsGroupBox;
    QRadioButton *actionTokensRB;
    QRadioButton *ownershipTokensRB;

    QPushButton *pb_ok;
    QPushButton *pb_cancel;
    QGridLayout *m_layout;

public slots:    
    void onNameChanged(const QString& text);
    void onDescriptionChanged();
    void onImageChanged();
    void onTokenRBClicked();
    void onTokenActivated(int index);
    void onOk();

};

#endif // NODEEDITOR_H
