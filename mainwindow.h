#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QStringListModel>
#include <QTimer>

#include <engine.h>
#include <chart.h>

#include "filesystem.h"
#include "inputcontroldelegate.h"

#define RUS(text) QString::fromLocal8Bit(text)

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public IFileSystem
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setDataToEngine();

    void setDataFromEngineToTable(QStandardItemModel* model, int index, bool color, bool vertical);
    void setDataFromEngineToTop(int index, bool color, bool vertical);
    void setDataFromEngineToBottom(int index, bool color, bool vertical);

    void getDataFromEngine();

    void showLoadLabel();

private slots:
    void hideLoadLabel();

    void on_variableCount_valueChanged(int arg1);
    void on_restrictCount_valueChanged(int arg1);
    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_menuExit_clicked();
    void on_menuAbout_clicked();
    void on_startButton_clicked();
    void on_viewList_doubleClicked(const QModelIndex &index);
    void on_viewTopTable_doubleClicked(const QModelIndex &index);
    void on_resultButton_clicked();
    void on_zoom_valueChanged(int arg1);
    void on_nullifyButton_clicked();
    void on_radioButton_3_clicked();
    void on_sourceButton_clicked();
    void on_toSteps_clicked();
    void on_toStepsButton_clicked();
    void on_pushButton_5_clicked();

    void on_actionOpen_triggered();
    void on_actionSave_triggered();

private:
    void nullify();

    void setColumns(int tableID, int columns);
    void setRows(int tableID, int rows);

    void setView(int index);

    int basisSize();

    Ui::MainWindow *ui;

    QStandardItemModel* m_sourceTableModel = nullptr;
    QStandardItemModel* m_basisTableModel = nullptr;

    QStandardItemModel* m_viewTopTableModel = nullptr;
    QStandardItemModel* m_viewBottomTableModel = nullptr;

    QStringListModel* m_viewListModel = nullptr;

    Engine m_simplexEngine;
    FileSystem* m_fileSystem = nullptr;
    int m_method = 0;

    int m_currentIndex = 0;

    QTimer m_timer;
    // IFileSystem interface
public:
    void SetRows(int tableID, int rows) override;
    void SetColumns(int tableID, int columns) override;
    void SetData(int tableID, int row, int column, QString value) override;
    int GetRows(int tableID = 0) override;
    int GetColumns(int tableID = 0) override;
    void SetMethod(int value) override;
    QString GetData(int row, int column, int tableID) override;
    int GetMethod() override;
};
#endif // MAINWINDOW_H
