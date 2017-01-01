#include "generalscripteditor.h"
#include "ui_generalscripteditor.h"

#include <QFile>
#include <QDebug>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <functional>
#include "yesnocanceldialog.h"

using namespace std;

generalScriptEditor::generalScriptEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::generalScriptEditor)
{
    ui->setupUi(this);

    connect(ui->action_new, SIGNAL(triggered(bool)), this, SLOT(newFile()));
    connect(ui->action_open, SIGNAL(triggered(bool)), this, SLOT(openFile()));
    connect(ui->action_save, SIGNAL(triggered(bool)), this, SLOT(save()));
    connect(ui->action_saveAs, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
    connect(ui->button_executeScript, SIGNAL(clicked(bool)), this, SLOT(execute()));
    connect(ui->text_editor, &QPlainTextEdit::textChanged,
    [this]()
    {
        if(saved)
        {
            ui->label_filename->setText(filePath + " (Unsaved)");
            saved = false;
        }
    });

    ui->label_filename->setText("");
    saved = true;
}

generalScriptEditor::~generalScriptEditor()
{
    delete ui;
}

void generalScriptEditor::save()
{
    if(filePath != "")
    {
        qDebug() << "Saving to " << filePath;
        QFile fout(filePath);
        fout.open(QIODevice::ReadWrite | QIODevice::Truncate);
        fout.write(ui->text_editor->toPlainText().toLatin1());
        fout.close();

        saved = true;
        ui->label_filename->setText(filePath);
    }
    else
        saveAs();
}

void generalScriptEditor::saveAs()
{
    filePath = QFileDialog::getSaveFileName(this, "Save As...", ".");
    if(filePath != "") save();
}

void generalScriptEditor::newFile()
{
    if(!saved)
    {
        YesNoCancelDialog dialog("Save before clearing?");
        switch(dialog.exec())
        {
            case -1: break;
            case 1: save(); break;
            case 0: default: return;
        }
    }
    filePath = "";
    ui->label_filename->setText("");
    ui->text_editor->setPlainText("");

    saved = true;
}

void generalScriptEditor::openFile()
{
    newFile();
    filePath = QFileDialog::getOpenFileName(this, "Open Script", ".", "Sql Script (*.sql);;Any (*)");
    if(filePath == "")
        return;

    QFile fin(filePath);
    if(fin.exists() && fin.open(QIODevice::ReadOnly))
    {
        ui->text_editor->setPlainText(QString::fromLatin1(fin.readAll()));
        ui->label_filename->setText(filePath);
        fin.close();
    }
    else
    {
        ui->label_filename->setText("Unable to open " + filePath);
        filePath = "";
    }
}

void generalScriptEditor::execute()
{
    emit runScript(ui->text_editor->toPlainText().replace('\n', ' '));
}
