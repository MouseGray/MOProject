#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QObject>
#include <QFile>
#include <QFileDialog>

#include "IFileSystem.h"

#define once(var, exp)               if((var)) { exp; (var) = false; }

enum class FileSystemError {
    OK,
    FileClosed,
    FileCantBeOpened,
    IncorrectData,
    ExceedingLimit
};

class FileSystem : public QObject
{
    Q_OBJECT
public:
    explicit FileSystem(QWidget *parent);
    void setReceiver(IFileSystem* receiver);
    FileSystemError openLoadFile();
    FileSystemError readFile();
    void            closeFile();

    FileSystemError openSaveFile();
    FileSystemError saveFile();
signals:

private:
    FileSystemError getMatrix(QString& data, bool* isFirst);
    FileSystemError getBasis (QString& data, bool* isFirst);

    IFileSystem* m_receiver = nullptr;

    QWidget* m_parent = nullptr;
    QFile m_file;

    const QRegExp regExp = QRegExp("[-]{0,1}[0-9]{1,5}([./][0-9]{1,5})?");

    static constexpr int minSize = 2;
    static constexpr int maxSize = 17;
};

#endif // FILESYSTEM_H
