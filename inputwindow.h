#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H

#include "memostore.h"

#include <QWidget>

class QEvent;
class QHideEvent;
class QLineEdit;
class QFrame;
class QLabel;
class QPushButton;

class InputWindow : public QWidget
{
    Q_OBJECT

public:
    explicit InputWindow(QWidget *parent = nullptr);

    MemoType currentType() const;
    bool isOpenForCapture() const;

public slots:
    void setCurrentType(MemoType type);
    void toggleCurrentType();
    void showAndFocus();
    void applyTheme(ThemeMode mode);

signals:
    void memoSubmitted(MemoType type, const QString &text);
    void currentTypeChanged(MemoType type);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupUi();
    void updateTypeButton();

    QFrame *inputPanel;
    QLineEdit *input;
    QPushButton *typeButton;
    QLabel *enterHint;
    QLabel *escHint;
    MemoType activeType;
    bool captureOpen;
};

#endif // INPUTWINDOW_H
