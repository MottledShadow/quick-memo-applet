#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

#include "memostore.h"

#include <QWidget>

class QCheckBox;
class QKeySequenceEdit;
class QLabel;
class QPushButton;
class QTableWidget;

class DashboardWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWindow(MemoStore *store, QWidget *parent = nullptr);

public slots:
    void refresh();
    void setStatusMessage(const QString &message);

signals:
    void showMemoRequested(MemoType type);
    void hideMemoRequested(MemoType type);
    void alwaysOnTopChanged(MemoType type, bool enabled);
    void hotkeyChangeRequested(const QKeySequence &sequence);
    void autostartChanged(bool enabled);
    void exitRequested();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void setupUi();
    QWidget *createMemoControls(MemoType type);
    QTableWidget *createRecordsTable(QWidget *parent) const;
    void refreshMemoControls(MemoType type);
    void refreshRecords();
    void fillRecordsTable(QTableWidget *table, const QVector<MemoItem> &records);

    MemoStore *store;
    QPushButton *questionVisibilityButton;
    QPushButton *todoVisibilityButton;
    QCheckBox *questionTopCheck;
    QCheckBox *todoTopCheck;
    QCheckBox *autostartCheck;
    QKeySequenceEdit *hotkeyEdit;
    QTableWidget *questionRecordsTable;
    QTableWidget *todoRecordsTable;
    QLabel *statusLabel;
};

#endif // DASHBOARDWINDOW_H
