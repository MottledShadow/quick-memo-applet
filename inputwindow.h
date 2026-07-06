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
class QTimer;

class InputWindow : public QWidget
{
    Q_OBJECT

public:
    explicit InputWindow(QWidget *parent = nullptr);

    MemoType currentType() const;
    bool isOpenForCapture() const;

public slots:
    void setCurrentType(MemoType type);
    void setHideAfterSave(bool enabled);
    void setLanguage(AppLanguage language);
    void toggleCurrentType();
    void showAndFocus();
    void applyTheme(ThemeMode mode, FontSizeMode fontSize, DensityMode density);

signals:
    void memoSubmitted(MemoType type, const QString &text);
    void currentTypeChanged(MemoType type);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupUi();
    void retranslateUi();
    void updateTypeButton();
    void showFeedback(const QString &message, const QString &kind, int durationMs, bool hideAfterDelay);
    void clearFeedback();
    void startShowAnimation();
    void startHideAnimation();
    void stopInputAnimation();

    QFrame *inputPanel;
    QLineEdit *input;
    QPushButton *typeButton;
    QLabel *enterHint;
    QLabel *escHint;
    QLabel *feedbackLabel;
    QTimer *feedbackTimer;
    QTimer *autoHideTimer;
    QParallelAnimationGroup *inputAnimation;
    QPropertyAnimation *opacityAnimation;
    QPropertyAnimation *positionAnimation;
    QPoint visiblePosition;
    MemoType activeType;
    AppLanguage appLanguage;
    bool hideAfterSave;
    bool captureOpen;
    bool hidingWithAnimation;
};

#endif // INPUTWINDOW_H
