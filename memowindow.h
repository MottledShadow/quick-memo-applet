#ifndef MEMOWINDOW_H
#define MEMOWINDOW_H

#include "memostore.h"

#include <QPoint>
#include <QVector>
#include <QWidget>

class QBoxLayout;
class QFrame;
class QLabel;
class QPushButton;
class QSizeGrip;

class MemoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MemoWindow(MemoType type, QWidget *parent = nullptr);

    MemoType memoType() const;
    MemoWindowState currentState() const;

public slots:
    void setRecords(const QVector<MemoItem> &records);
    void restoreState(const MemoWindowState &state);
    void setAlwaysOnTop(bool enabled);
    void setLanguage(AppLanguage language);
    void applyTheme(ThemeMode mode, FontSizeMode fontSize, DensityMode density);

signals:
    void memoClicked(const QString &id);
    void stateChanged(MemoType type, const MemoWindowState &state);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void setupUi();
    void retranslateUi();
    void rebuildList();
    void startDeleteAnimation(QWidget *recordWidget, const QString &id);
    void updateWindowFlags(bool keepVisible);
    void updateResizeGripGeometry();
    void emitStateChanged();

    MemoType type;
    QVector<MemoItem> currentRecords;
    QVector<MemoItem> pendingRecords;
    QFrame *panel;
    QWidget *titleBar;
    QLabel *titleLabel;
    QLabel *titleCountLabel;
    QPushButton *topButton;
    QPushButton *hideButton;
    QSizeGrip *resizeGrip;
    QBoxLayout *listLayout;
    QString deletingMemoId;
    AppLanguage appLanguage;
    bool alwaysOnTop;
    bool dragging;
    bool deleteAnimationActive;
    bool rebuildPending;
    QPoint dragOffset;
};

#endif // MEMOWINDOW_H
