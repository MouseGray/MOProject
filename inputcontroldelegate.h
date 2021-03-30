#ifndef INPUTCONTROLDELEGATE_H
#define INPUTCONTROLDELEGATE_H

#include <QLineEdit>
#include <QItemDelegate>

class InputControlDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    InputControlDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
};

#endif // INPUTCONTROLDELEGATE_H
