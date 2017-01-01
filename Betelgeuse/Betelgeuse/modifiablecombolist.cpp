#include "modifiablecombolist.h"
#include "ui_modifiablecombolist.h"

#include <QDebug>
#include <functional>

using namespace std;

ModifiableComboList::ModifiableComboList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModifiableComboList)
{
    ui->setupUi(this);

    connect(ui->button_newItem, SIGNAL(clicked(bool)), this, SLOT(newButtonClicked()));
    connect(ui->button_copyItem, SIGNAL(clicked(bool)), this, SLOT(copyButtonClicked()));
    connect(ui->button_deleteItem, SIGNAL(clicked(bool)), this, SLOT(deleteButtonClicked()));

    connect(ui->edit_name, SIGNAL(textEdited(QString)), this, SLOT(textChanged(QString)));
    connect(ui->edit_name, &QLineEdit::editingFinished,
    [this]()
    {
        if(newName == "" || oldName == "") return;
        qDebug() << "Name changed to " << newName;
        buffered = false;
        if(!nameChange(oldName, newName))
        {
            ui->combo_choices->setItemText(ui->combo_choices->currentIndex(), oldName);
            ui->edit_name->setText(oldName);
        }
        oldName = newName = "";
    });

    connect(ui->combo_choices, SIGNAL(currentIndexChanged(QString)), ui->edit_name, SLOT(setText(QString)));
    connect(ui->combo_choices, SIGNAL(currentIndexChanged(QString)), this, SIGNAL(selectionChange(QString)));

    ui->edit_name->setReadOnly(true);
    ui->button_copyItem->setEnabled(false);
}

ModifiableComboList::~ModifiableComboList()
{
    delete ui;
}

void ModifiableComboList::setItem(QString item, int number)
{
    if(number < 0) number = ui->combo_choices->currentIndex();
    ui->combo_choices->setItemText(number, item);
}

void ModifiableComboList::setItems(QStringList items)
{
    ui->combo_choices->clear();
    for(QString s : items)
    {
        ui->combo_choices->insertItem(0, getUniqueItem(s));
    }

    ui->edit_name->setReadOnly(items.size() == 0);
    ui->button_copyItem->setEnabled(items.size() > 0);
}

void ModifiableComboList::newButtonClicked()
{
    QString inserted = getUniqueItem(defaultItem);
    if(!newItem(inserted)) return;

    qDebug() << "Inserting new item to combo";
    ui->combo_choices->insertItem(0, inserted);
    ui->combo_choices->setCurrentIndex(0);

    ui->edit_name->setReadOnly(false);
    ui->button_copyItem->setEnabled(true);
}

void ModifiableComboList::copyButtonClicked()
{
    qDebug() << "Copying combo item";
    QString current = ui->combo_choices->currentText();
    QString copy = getUniqueItem(current + "(Copy)");

    if(!copyItem(current, copy)) return;

    ui->combo_choices->insertItem(0, copy);
    ui->combo_choices->setCurrentIndex(0);
}

void ModifiableComboList::deleteButtonClicked()
{
    QString current = ui->combo_choices->currentText();
    if(!deleteItem(current)) return;
    qDebug() << "Deleting combo item";

    ui->combo_choices->removeItem(ui->combo_choices->currentIndex());

    ui->edit_name->setReadOnly(ui->combo_choices->count() == 0);
    ui->button_copyItem->setEnabled(ui->combo_choices->count() > 0);
}

void ModifiableComboList::textChanged(QString text)
{
    if(!buffered)
    {
        oldName = ui->combo_choices->currentText();
        buffered = true;
    }
    qDebug() << "Text of current combo item changed from " << oldName << " to " << text;
    setItem(text, ui->combo_choices->currentIndex());

    newName = text;
}

QString ModifiableComboList::currentIndex()
{
    return ui->combo_choices->currentText();
}

QString ModifiableComboList::getUniqueItem(QString s)
{
    QString unique = s;
    int i = 1;
    while(ui->combo_choices->findText(unique) != -1) unique = s + "(" + QString::number(i++) + ")";

    return unique;
}

