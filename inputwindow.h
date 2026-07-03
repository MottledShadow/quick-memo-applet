#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H

#include "memostore.h"

#include <QWidget>

class QLineEdit;
class QPushButton;

class InputWindow : public QWidget
{
    Q_OBJECT

public:
    explicit InputWindow(QWidget *parent = nullptr);

    MemoType currentType() const;

public slots:
    void setCurrentType(MemoType type);
    void toggleCurrentType();
    void showAndFocus();

signals:
    void memoSubmitted(MemoType type, const QString &text);
    void currentTypeChanged(MemoType type);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupUi();
    void updateTypeButton();

    QLineEdit *input;
    QPushButton *typeButton;
    MemoType activeType;
};

#endif // INPUTWINDOW_H
