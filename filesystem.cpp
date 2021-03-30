#include "filesystem.h"

FileSystem::FileSystem(QWidget *parent) : QObject(parent), m_parent(parent) { }

void FileSystem::setReceiver(IFileSystem *receiver)
{
    m_receiver = receiver;
}

void FileSystem::closeFile()
{
    m_file.close();
}

FileSystemError FileSystem::openSaveFile()
{
    QString filename = QFileDialog::getSaveFileName(m_parent, "Save", "", "*.smp");
    if(filename.isEmpty()) return FileSystemError::FileClosed;
    m_file.setFileName(filename);
    if(!m_file.open(QIODevice::WriteOnly)) return FileSystemError::FileCantBeOpened;
    return FileSystemError::OK;
}

FileSystemError FileSystem::saveFile()
{
    m_file.write("#matrix = ");
    for(int r = 0; r < m_receiver->GetRows(); r++){
        for(int c = 0; c < m_receiver->GetColumns(); c++){
            if(r == 0 && c == m_receiver->GetColumns() - 1) continue;
            m_file.write(m_receiver->GetData(r, c, 0).toUtf8());
            m_file.write(" ");
        }
        m_file.write("\n");
    }
    m_file.write("\n");
    m_file.write("#basis = ");
    for(int c = 0; c < m_receiver->GetColumns(); c++){
        m_file.write(m_receiver->GetData(0, c, 1).toUtf8());
        m_file.write(" ");
    }
    m_file.write("\n");
    m_file.write("#method = ");
    m_file.write(QString::number(m_receiver->GetMethod()).toUtf8());
    return FileSystemError::OK;
}

FileSystemError FileSystem::openLoadFile()
{
    QString filename = QFileDialog::getOpenFileName(m_parent, "Open", "", "*.smp");
    if(filename.isEmpty()) return FileSystemError::FileClosed;
    m_file.setFileName(filename);
    if(!m_file.open(QIODevice::ReadOnly)) return FileSystemError::FileCantBeOpened;
    return FileSystemError::OK;
}

FileSystemError FileSystem::readFile()
{
    bool isFirst = true;
    QString data = m_file.readAll();
    QStringList parts = data.split("#", Qt::SplitBehaviorFlags::SkipEmptyParts);
    for(int i = 0; i < parts.size(); i++){
        QStringList key_value = parts[i].split("=", Qt::SplitBehaviorFlags::SkipEmptyParts);
        if(key_value.size() != 2) return FileSystemError::IncorrectData;
        key_value[0] = key_value[0].trimmed();
        key_value[1] = key_value[1].trimmed();
        if(key_value[0] == "matrix"){
            FileSystemError res = getMatrix(key_value[1], &isFirst);
            if(res != FileSystemError::OK) return res;
        }
        else if(key_value[0] == "basis"){
            FileSystemError res = getBasis(key_value[1], &isFirst);
            if(res != FileSystemError::OK) return res;
        }
        else if(key_value[0] == "method"){
            bool ok;
            int method = key_value[1].toInt(&ok);
            if(!ok) return FileSystemError::IncorrectData;
            m_receiver->SetMethod(method);
        }
        else return FileSystemError::IncorrectData;
    }
    return FileSystemError::OK;
}

FileSystemError FileSystem::getMatrix(QString &data, bool* isFirst)
{
    QStringList rows = data.split("\n", Qt::SplitBehaviorFlags::SkipEmptyParts);
    if(rows.size() < minSize || rows.size() > maxSize) return FileSystemError::ExceedingLimit;

    m_receiver->SetRows(0, rows.size());

    for(int r = 0; r < rows.size(); r++){
        QStringList cells = rows[r].split(" ", Qt::SplitBehaviorFlags::SkipEmptyParts);
        if(cells.size() < minSize || cells.size() > maxSize) return FileSystemError::ExceedingLimit;

        once(*isFirst, m_receiver->SetColumns(0, cells.size() + 1))
        else if(m_receiver->GetColumns(0) != cells.size()) return FileSystemError::IncorrectData;
        for(int c = 0; c < cells.size(); c++){
            cells[c] = cells[c].trimmed();
            if(!regExp.exactMatch(cells[c])) return FileSystemError::IncorrectData;
            m_receiver->SetData(0, r, c, cells[c]);
        }
    }
    return FileSystemError::OK;
}

FileSystemError FileSystem::getBasis(QString &data, bool* isFirst)
{
    QStringList cells = data.split(" ", Qt::SplitBehaviorFlags::SkipEmptyParts);
    if(cells.size() < minSize || cells.size() > maxSize) return FileSystemError::ExceedingLimit;

    once(*isFirst, m_receiver->SetColumns(1, cells.size() + 1))
    else{
        if(m_receiver->GetColumns(1) - 1 != cells.size()) return FileSystemError::IncorrectData;
        for(int c = 0; c < cells.size(); c++){
            cells[c] = cells[c].trimmed();
            if(!regExp.exactMatch(cells[c])) return FileSystemError::IncorrectData;
            m_receiver->SetData(1, 0, c, cells[c]);
        }
    }
    return FileSystemError::OK;
}
