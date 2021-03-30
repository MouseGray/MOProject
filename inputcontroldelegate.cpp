#include "inputcontroldelegate.h"

InputControlDelegate::InputControlDelegate()
{

}


QWidget *InputControlDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    QLineEdit *editor = new QLineEdit(parent);
    editor->setValidator(new QRegExpValidator(QRegExp("[-]{0,1}[0-9]{1,5}([./][0-9]{1,5})?"), editor));
    return editor;
}

void InputControlDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QLineEdit *line = static_cast<QLineEdit*>(editor);
    line->setText(value);
}

void InputControlDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *line = static_cast<QLineEdit*>(editor);
    model->setData(index, line->text());
}
