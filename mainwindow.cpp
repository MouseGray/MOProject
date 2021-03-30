#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // ===================================[ Таблица ввода данных ]=================================== //
    // Модель
    m_sourceTableModel = new QStandardItemModel();
    m_sourceTableModel->setRowCount(1);
    m_sourceTableModel->setColumnCount(1);
    // Таблица
    ui->sourceTable->setModel(m_sourceTableModel);
    ui->sourceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->sourceTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->sourceTable->setItemDelegate(new InputControlDelegate());
    // ===================================[ Таблица ввода базиса ]=================================== //
    // Модель
    m_basisTableModel = new QStandardItemModel();
    m_basisTableModel->setRowCount(1);
    m_basisTableModel->setColumnCount(1);
    // Таблица
    ui->basisTable->setModel(m_basisTableModel);
    ui->basisTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->basisTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->basisTable->setItemDelegate(new InputControlDelegate());
    ui->basisTable->setEnabled(false);
    // ===============================[ Таблица вывода данных (верх) ]=============================== //
    // Модель
    m_viewTopTableModel = new QStandardItemModel();
    m_viewTopTableModel->setColumnCount(16);
    m_viewTopTableModel->setRowCount(16);
    // Таблица
    ui->viewTopTable->setModel(m_viewTopTableModel);
    ui->viewTopTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->viewTopTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // ===============================[ Таблица вывода данных (низ) ]=============================== //
    // Модель
    m_viewBottomTableModel = new QStandardItemModel();
    m_viewBottomTableModel->setColumnCount(16);
    m_viewBottomTableModel->setRowCount(16);
    // Таблица
    ui->viewBottomTable->setModel(m_viewBottomTableModel);
    ui->viewBottomTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->viewBottomTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // ===================================[ Список вывода шагов ]=================================== //
    // Модель
    m_viewListModel = new QStringListModel();
    // Список
    ui->viewList->setModel(m_viewListModel);
    ui->viewList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(on_menuOpen_clicked()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(on_menuSave_clicked()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(on_menuExit_clicked()));

    connect(ui->menuAbout, SIGNAL(aboutToShow()), this, SLOT(on_menuAbout_clicked()));

    ui->variableCount->setValue(5);
    ui->restrictCount->setValue(3);

    SetMethod(0);

    m_fileSystem = new FileSystem(this);
    m_fileSystem->setReceiver(this);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(hideLoadLabel()));
    hideLoadLabel();
}

MainWindow::~MainWindow()
{
    delete m_sourceTableModel;
    delete ui;
}

void MainWindow::setDataToEngine()
{
    m_simplexEngine.createTask(GetRows() - 1, GetColumns() - 1);

    for(int c = 0; c < GetColumns() - 1; c++){
        m_simplexEngine.setFCoef(c, m_sourceTableModel->data(m_sourceTableModel->index(0, c)).toString().toStdString());
    }
    for(int r = 1; r < GetRows(); r++){
        for(int c = 0; c < GetColumns() - 1; c++){
            m_simplexEngine.setCell(r - 1, c, m_sourceTableModel->data(m_sourceTableModel->index(r, c)).toString().toStdString());
        }
        m_simplexEngine.setB(r - 1, m_sourceTableModel->data(m_sourceTableModel->index(r, GetColumns() - 1)).toString().toStdString());
   }

    for(int c = 0; c < GetColumns() - 1; c++){
        m_simplexEngine.setBasis(c, m_basisTableModel->data(m_basisTableModel->index(0, c)).toString().toStdString());
    }

    m_simplexEngine.setMethod(m_method);
}

void MainWindow::setDataFromEngineToTable(QStandardItemModel *model, int index, bool color, bool vertical)
{
    MatrixX* matrix = m_simplexEngine.getStorage().getMatrix(index);
    model->setColumnCount(matrix->columns() + 1);
    model->setRowCount(matrix->rows() + 1);

    rgb** colorMatrix = nullptr;
    if(color) colorMatrix = matrix->colorMatrix();

    bool check = ui->checkBox->isChecked();
    for(int r = 0; r < matrix->rows(); r++){
        for(int c = 0; c < matrix->columns(); c++){
            model->setData(model->index(r, c), check ? QString::number(matrix->cell(r, c).toDouble()) : matrix->cell(r, c).toString().data());
            if(color) model->item(r, c)->setBackground(QColor(colorMatrix[r][c].r(), colorMatrix[r][c].g(), colorMatrix[r][c].b()));
            else model->item(r, c)->setBackground(Qt::white);
        }
        model->setData(model->index(r, matrix->columns()), check ? QString::number(matrix->b(r).toDouble()) : matrix->b(r).toString().data());
        model->item(r, matrix->columns())->setBackground(Qt::white);
    }
    for(int c = 0; c < matrix->columns(); c++){
        model->setData(model->index(matrix->rows(), c), check ? QString::number(matrix->fRow(c).toDouble()) : matrix->fRow(c).toString().data());
        model->item(matrix->rows(), c)->setBackground(Qt::white);
    }
    model->setData(model->index(matrix->rows(), matrix->columns()), check ? QString::number(matrix->f().toDouble()) : matrix->f().toString().data());
    model->item(matrix->rows(), matrix->columns())->setBackground(Qt::white);

    QStringList hHeaders;
    for(int c = 0; c < matrix->columns(); c++){
        hHeaders << matrix->cHeader(c).symbol + QString::number(matrix->cHeader(c).index);
    }
    hHeaders << " ";
    model->setHorizontalHeaderLabels(hHeaders);

    QStringList vHeaders;
    for(int c = 0; c < matrix->rows(); c++){
        if(vertical) vHeaders << matrix->vHeader(c).symbol + QString::number(matrix->vHeader(c).index);
        else vHeaders << " ";
    }
    if(vertical) vHeaders << "f(x)";
    else vHeaders << " ";

    model->setVerticalHeaderLabels(vHeaders);

    delete colorMatrix;
}

void MainWindow::setDataFromEngineToTop(int index, bool color, bool vertical)
{
    setDataFromEngineToTable(m_viewTopTableModel, index, color, vertical);
}

void MainWindow::setDataFromEngineToBottom(int index, bool color, bool vertical)
{
    if(index == -1){
        ui->viewBottomTable->setHidden(true);
        return;
    }
    ui->viewBottomTable->setHidden(false);
    setDataFromEngineToTable(m_viewBottomTableModel, index, color, vertical);
}

void MainWindow::getDataFromEngine()
{
    QStringList data;
    for(int i = 0; i < m_simplexEngine.getStorage().size(); i++){
             if(m_simplexEngine.getStorage().getMatrix(i)->getState() == SourceMatrix)          data << QString::number(i) + QString::fromLocal8Bit(") Исходная матрица");
        else if(m_simplexEngine.getStorage().getMatrix(i)->getState() == AfterGauss)            data << QString::number(i) + QString::fromLocal8Bit(") Преобразование Гаусса");
        else if(m_simplexEngine.getStorage().getMatrix(i)->getState() == InArtificialBasis)     data << QString::number(i) + QString::fromLocal8Bit(") Метод искусственного базиса");
        else if(m_simplexEngine.getStorage().getMatrix(i)->getState() == AfterArtificialBasis)  data << QString::number(i) + QString::fromLocal8Bit(") Метод искусственного базиса");
        else if(m_simplexEngine.getStorage().getMatrix(i)->getState() == InSimplexMethod)       data << QString::number(i) + QString::fromLocal8Bit(") Симплекс метод");
        else if(m_simplexEngine.getStorage().getMatrix(i)->getState() == AfterSimplexMethod)    data << QString::number(i) + QString::fromLocal8Bit(") Симплекс метод");
    }
    m_viewListModel->setStringList(data);
    setDataFromEngineToTop(m_simplexEngine.getStorage().size() - 1, false, true);
    setDataFromEngineToBottom(-1, false, true);
}

void MainWindow::showLoadLabel()
{
    ui->loadLabel->setVisible(true);
    m_timer.start(4000);
}

void MainWindow::hideLoadLabel()
{
    ui->loadLabel->setVisible(false);
}


void MainWindow::on_variableCount_valueChanged(int arg1)
{
    setColumns(0, arg1 + 1);
}

void MainWindow::on_restrictCount_valueChanged(int arg1)
{
    setRows(0, arg1 + 1);
}

void MainWindow::on_radioButton_clicked()
{
    ui->basisTable->setEnabled(false);
    m_method = 0;
}

void MainWindow::on_radioButton_2_clicked()
{
    ui->basisTable->setEnabled(true);
    m_method = 1;
}

void MainWindow::on_radioButton_3_clicked()
{
    ui->basisTable->setEnabled(true);
    m_method = 2;
}

void MainWindow::on_menuExit_clicked()
{
    this->close();
}

void MainWindow::on_menuAbout_clicked()
{
    QMessageBox::information(this, QString::fromLocal8Bit("Справка"),
                             QString::fromLocal8Bit("Программа MOProject\n"
                             "\n"
                             "Данная программа позволяет решать\n"
                             "задачи симплекс методом по заданному\n"
                             "базису или по найденному с помощью\n"
                             "метода искусственного базиса.\n"
                             "\n"
                             "Также поддерживает решение графическим\n"
                             "методом задач, у которых разность\n"
                             "кол-ва переменных и возможных базисных\n"
                             "переменных равна 2."
                             "\n"
                             "Задачи вводятся в каноническом виде.\n"
                             "\n"
                             "На этапе редактирования шагов\n"
                             "поддерживается изменение хода\n"
                             "симплекс метода.\n"
                             "\n"
                             "----------------------\n"
                             "Проект выполнен в рамках лабораторной\n"
                             "работы по курсу \"Методы оптимизации и \n"
                             "исследование операций\"."));
}

void MainWindow::nullify()
{
    for(int r = 0; r < GetRows(); r++){
        for(int c = 0; c < GetColumns(); c++){
            m_sourceTableModel->setData(m_sourceTableModel->index(r, c), "0");
            m_sourceTableModel->item(r, c)->setEditable(true);
        }
    }
    m_sourceTableModel->setData(m_sourceTableModel->index(0, GetColumns() - 1), "min");
    m_sourceTableModel->item(0, GetColumns() - 1)->setEditable(false);

    for(int c = 0; c < GetColumns(); c++){
        m_basisTableModel->setData(m_basisTableModel->index(0, c), "0");
    }
}

void MainWindow::setView(int index)
{
    m_currentIndex = index;
    MatrixX* matrix = m_simplexEngine.getStorage().getMatrix(index);
    MatrixX* matrix2 = (index + 1) < m_simplexEngine.getStorage().size() ? m_simplexEngine.getStorage().getMatrix(index + 1) : nullptr;

    switch(matrix->getState()){
    case SourceMatrix: case AfterClear: case AfterGauss:
        setDataFromEngineToTop(index, false, false);
        break;
    case InArtificialBasis: case InSimplexMethod:
        setDataFromEngineToTop(index, true, true);
        break;
    case AfterArtificialBasis: case AfterSimplexMethod:
        setDataFromEngineToTop(index, false, true);
        break;
    }

    if(matrix2 == nullptr){
        setDataFromEngineToBottom(-1, true, true);
        ui->viewHeader->setText(QString::fromLocal8Bit("Результат"));
        return;
    }

    switch(matrix2->getState()){
    case SourceMatrix: case AfterClear:
        setDataFromEngineToBottom(index + 1, false, false);
        break;
    case AfterGauss:
        ui->viewHeader->setText(QString::fromLocal8Bit("Преобразование Гаусса"));
        setDataFromEngineToBottom(index + 1, false, false);
        break;
    case InArtificialBasis:
        if(matrix->getState() != InArtificialBasis) ui->viewHeader->setText(QString::fromLocal8Bit("Переход к методу искуственного базиса"));
        else ui->viewHeader->setText(QString::fromLocal8Bit("Шаг метода искуственного базиса"));
        setDataFromEngineToBottom(index + 1, true, true);
        break;
    case InSimplexMethod:
        if(matrix->getState() != InSimplexMethod) ui->viewHeader->setText(QString::fromLocal8Bit("Переход к симплекс методу"));
        else ui->viewHeader->setText(QString::fromLocal8Bit("Шаг симплекс метода"));
        setDataFromEngineToBottom(index + 1, true, true);
        break;
    case AfterArtificialBasis:
        ui->viewHeader->setText(QString::fromLocal8Bit("Шаг метода искуственного базиса"));
        setDataFromEngineToBottom(index + 1, false, true);
        break;
    case AfterSimplexMethod:
        ui->viewHeader->setText(QString::fromLocal8Bit("Шаг симплекс метода"));
        setDataFromEngineToBottom(index + 1, false, true);
        break;
    }
}

int MainWindow::basisSize()
{
    int counter = 0;
    for(int i = 0; i < m_simplexEngine.getBasis().size(); i++){
        if(m_simplexEngine.getBasis()[i] != 0) counter++;
    }
    return counter;
}

void MainWindow::on_startButton_clicked()
{
    setDataToEngine();

    switch (m_method) {
    case 1:
        if(basisSize() != GetRows() - 1) {
            QMessageBox::warning(this, RUS("Ошибка"), RUS("Некорректный базис для данной задачи!"));
            return;
        }
        break;
    case 2:
        if(basisSize() != GetRows() - 1) {
            QMessageBox::warning(this, RUS("Ошибка"), RUS("Некорректный базис для данной задачи!"));
            return;
        }
        if(GetColumns() - basisSize() - 1 != 2){
            QMessageBox::warning(this, RUS("Ошибка"), RUS("Данную задачу невозможно решить данным методом!"));
            return;
        }
        break;
    }

    ui->stackedWidget->setCurrentIndex(1);
    ui->toSteps->setEnabled(true);

    int result = m_simplexEngine.start();

    ui->viewHeader->setText(QString::fromLocal8Bit("Результат"));
    /*  codes:
     *  0 - результат найден
     *  1 - матрица по базису вырождена
     *  2 - базис содержит искусственные переменные
     *  3 - решения нет
     */
    switch(result){
        case 0:
            ui->viewLabel->setText("OK!");
            ui->resultText->setText(m_simplexEngine.getResultText(m_simplexEngine.getStorage().size() - 1, !ui->checkBox->isChecked()));
        break;
        case 1:
            ui->viewLabel->setText(QString::fromLocal8Bit("Минор по заданному базису вырожден!"));
            ui->resultText->setText(QString::fromLocal8Bit("Решения по данному базису нет!"));
        break;
        case 2:
            ui->viewLabel->setText(QString::fromLocal8Bit("Базис содержит искусственные переменные!"));
            ui->resultText->setText(QString::fromLocal8Bit("Решения нет!\nБазис содержит искусственные переменные!"));
        break;
        case 3:
            ui->viewLabel->setText(QString::fromLocal8Bit("Решения нет!"));
            ui->resultText->setText(QString::fromLocal8Bit("Решения нет!"));
        break;
    }

    getDataFromEngine();
}

void MainWindow::on_viewList_doubleClicked(const QModelIndex &index)
{
    setView(index.row());
}

void MainWindow::on_viewTopTable_doubleClicked(const QModelIndex &index)
{
    if(m_viewTopTableModel->item(index.row(), index.column())->background().color() != QColor(GREEN)) return;
    m_simplexEngine.changePath(m_currentIndex, index.row(), index.column());

    getDataFromEngine();

    setView(m_currentIndex);
}

void MainWindow::on_resultButton_clicked()
{
    if(m_method == 2 && m_simplexEngine.getStorage().size() > 1){
        MatrixX* matrix = m_simplexEngine.getStorage().getMatrix(2);
        ui->chart->clearRestrict();
        for(int r = 0; r < matrix->rows(); r++){
            ui->chart->addRestrict(matrix->cell(r, 0), matrix->cell(r, 1), matrix->b(r));
        }
        ui->chart->setFunction(matrix->fRow(0).toDouble(), matrix->fRow(1).toDouble());
        if(ui->viewLabel->text() != "OK!") ui->chart->removeRedPoint();
        else {
            std::vector<num> res = m_simplexEngine.getStorage().matrix()->getResult();
            ui->chart->setRedPoint(QPointF(res[matrix->cHeader(0).index].toDouble(), res[matrix->cHeader(1).index].toDouble()));
        }
        ui->chart->setBasis(matrix->cHeader(0).symbol + QString::number(matrix->cHeader(0).index), matrix->cHeader(1).symbol + QString::number(matrix->cHeader(1).index));

        ui->chart->start();

        ui->chart->show();
        ui->zoom->show();
        ui->zoomLabel->show();
    }
    else {
        ui->chart->hide();
        ui->zoom->hide();
        ui->zoomLabel->hide();
    }
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_zoom_valueChanged(int arg1)
{
    ui->chart->setZoom_px(arg1);
}

void MainWindow::on_nullifyButton_clicked()
{
    nullify();
}

void MainWindow::on_sourceButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_toSteps_clicked()
{
   ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_toStepsButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->variableCount->setValue(5);
    ui->restrictCount->setValue(3);
}

void MainWindow::on_actionOpen_triggered()
{
    switch(m_fileSystem->openLoadFile()){
    case FileSystemError::OK:
        switch(m_fileSystem->readFile()){
        case FileSystemError::OK:
            showLoadLabel();
            break;
        case FileSystemError::ExceedingLimit:
            QMessageBox::warning(this, RUS("Ошибка"), RUS("Размер матрицы не может быть\nменьше 2 и больше 17."));
            break;
        case FileSystemError::IncorrectData:
            QMessageBox::warning(this, RUS("Ошибка"), RUS("Файл содержит некорректные данные и не\nможет быть интерпретирован."));
            break;
        default:
            break;
        }
        m_fileSystem->closeFile();
        break;
    case FileSystemError::FileCantBeOpened:
        QMessageBox::warning(this, RUS("Ошибка"), RUS("Ошибка при открытии файла."));
        break;
    default:
        return;
    }
}

void MainWindow::on_actionSave_triggered()
{
    switch(m_fileSystem->openSaveFile()){
    case FileSystemError::OK:
        m_fileSystem->saveFile();
        m_fileSystem->closeFile();
        break;
    case FileSystemError::FileCantBeOpened:
        QMessageBox::warning(this, RUS("Ошибка"), RUS("Ошибка при открытии файла."));
        break;
    default:
        return;
    }
}

void MainWindow::SetRows(int tableID, int rows)
{
    Q_UNUSED(tableID);

    ui->restrictCount->setValue(rows - 1);
}

void MainWindow::SetColumns(int tableID, int columns)
{
    Q_UNUSED(tableID);

    ui->variableCount->setValue(columns - 1);
}

void MainWindow::setRows(int tableID, int rows)
{
    Q_UNUSED(tableID);

    m_sourceTableModel->setRowCount(rows);

    QStringList basis;
    basis << " ";
    m_basisTableModel->setVerticalHeaderLabels(basis);
    QStringList vHeaders;
    vHeaders << "f(x)";
    for(int i = 1; i < rows; i++) vHeaders << QString::number(i) + ")";
    m_sourceTableModel->setVerticalHeaderLabels(vHeaders);

    nullify();
}

void MainWindow::setColumns(int tableID, int columns)
{
    Q_UNUSED(tableID);

    m_sourceTableModel->setColumnCount(columns);
    m_basisTableModel->setColumnCount(columns - 1);

    QStringList hHeaders;
    for(int i = 0; i < columns - 1; i++) hHeaders << "x" + QString::number(i);
    m_basisTableModel->setHorizontalHeaderLabels(hHeaders);
    hHeaders << " ";
    m_sourceTableModel->setHorizontalHeaderLabels(hHeaders);

    nullify();
}

void MainWindow::SetData(int tableID, int row, int column, QString value)
{
    tableID == 0 ? m_sourceTableModel->setData(m_sourceTableModel->index(row, column), value) :
                   m_basisTableModel ->setData(m_basisTableModel ->index(row, column), value);
}

int MainWindow::GetRows(int tableID)
{
    Q_UNUSED(tableID);
    return m_sourceTableModel->rowCount();
}

int MainWindow::GetColumns(int tableID)
{
    Q_UNUSED(tableID);
    return m_sourceTableModel->columnCount();
}

void MainWindow::SetMethod(int value)
{
    switch(value){
        case 0:
            ui->radioButton->setChecked(true);
            ui->basisTable->setEnabled(false);
        break;
        case 1:
            ui->radioButton_2->setChecked(true);
            ui->basisTable->setEnabled(true);
        break;
        case 2:
            ui->radioButton_3->setChecked(true);
            ui->basisTable->setEnabled(true);
        break;
    }
    m_method = value;
}


QString MainWindow::GetData(int row, int column, int tableID)
{
    return tableID == 0 ? m_sourceTableModel->data(m_sourceTableModel->index(row, column)).toString() :
                          m_basisTableModel ->data(m_basisTableModel ->index(row, column)).toString();
}

int MainWindow::GetMethod()
{
    return m_method;
}
