#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "declarations.h"

#include "processor/processor.h"

#include <QMainWindow>
#include <QStandardItemModel>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    // import-export.cpp
    void    importProject();
    void    exportProject();

    void    reset();

    void    addCore();
    void    editCore(int index);
    void    removeCore(int core);

    void    runProcessor();

private slots:
    void    leftSupportToggled(bool checked);
    void    rightSupportToggled(bool checked);

    void    pointCalc(int core, double pos);

private: // Methods
    // import-export.cpp
    QJsonDocument   serialize() const;

    QJsonArray      serializeCores() const;
    QJsonObject     serializeCore(int row) const;
    QJsonArray      serializeLoads() const;
    QJsonObject     serializeSupports() const;

    void    deserialize(const QJsonObject& project);
    bool    deserializeCores(const QJsonArray& cores);
    Core    deserializeCore(const QJsonObject& json) const;
    bool    deserializeLoads(const QJsonArray& loads);
    bool    deserializeSupports(const QJsonObject& supports);
    void    projectImportError();

    void    projectIOError(const QString &title, const QString& error);
    // ------------------

    void    addCoresTableItem(int row, int column, QVariant data);

    void    showCalcResults(const QVector<QVector<Stress> > &result);

private:
    QStandardItemModel* coresModel;
    QStandardItemModel* loadsModel;

    Processor   processor{this};

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
