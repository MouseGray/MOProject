#ifndef IFILESYSTEM_H
#define IFILESYSTEM_H

#include <QString>

class IFileSystem {
public:
    virtual void SetRows    (int tableID, int rows)                           = 0;
    virtual void SetColumns (int tableID, int columns)                        = 0;
    virtual void SetData    (int tableID, int row, int column, QString value) = 0;

    virtual void SetMethod  (int value) = 0;

    virtual int     GetRows     (int tableID = 0) = 0;
    virtual int     GetColumns  (int tableID = 0) = 0;
    virtual QString GetData     (int row, int column, int tableID = 0) = 0;

    virtual int GetMethod  () = 0;
};

#endif // IFILESYSTEM_H
