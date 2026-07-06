#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H

#include "memostore.h"

#include <QPoint>
#include <QWidget>

class QEvent;
class QHideEvent;
class QLineEdit;
class QParallelAnimationGroup;
class QFrame;
class QLabel;
class QPropertyAnimation;
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
    void startShowAnimation();
    void startHideAnimation();
    void stopInputAnimation();

    QFrame *inputPanel;
    QLineEdit *input;
    QPushButton *typeButton;
    QLabel *enterHint;
    QLabel *escHint;
    QParallelAnimationGroup *inputAnimation;
    QPropertyAnimation *opacityAnimation;
    QPropertyAnimation *positionAnimation;
    QPoint visiblePosition;
    MemoType activeType;
    bool captureOpen;
    bool hidingWithAnimation;
};

#endif // INPUTWINDOW_H
