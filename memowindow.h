#ifndef MEMOWINDOW_H
#define MEMOWINDOW_H

#include "memostore.h"

#include <QPoint>
#include <QVector>
#include <QWidget>

class QBoxLayout;
class QLabel;
class QPushButton;

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
    void applyTheme(ThemeMode mode);

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
    void rebuildList();
    void updateWindowFlags(bool keepVisible);
    void emitStateChanged();

    MemoType type;
    QVector<MemoItem> currentRecords;
    QWidget *titleBar;
    QLabel *titleLabel;
    QPushButton *topButton;
    QBoxLayout *listLayout;
    bool alwaysOnTop;
    bool dragging;
    QPoint dragOffset;
};

#endif // MEMOWINDOW_H
