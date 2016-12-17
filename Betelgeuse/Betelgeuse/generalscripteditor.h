#ifndef GENERALSCRIPTEDITOR_H
#define GENERALSCRIPTEDITOR_H

#include <QMainWindow>

namespace Ui {
class generalScriptEditor;
}

class generalScriptEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit generalScriptEditor(QWidget *parent = 0);
    ~generalScriptEditor();

private:
    Ui::generalScriptEditor *ui;
    QString filePath;
    bool saved = false;

private slots:
    void save();
    void saveAs();
    void newFile();
    void openFile();
    void execute();

signals:
    void runScript(QString);
};

#endif // GENERALSCRIPTEDITOR_H
