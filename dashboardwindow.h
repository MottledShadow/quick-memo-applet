#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

#include "memostore.h"

#include <QWidget>

class QCheckBox;
class QComboBox;
class QBoxLayout;
class QFrame;
class QKeySequenceEdit;
class QLabel;
class QPushButton;

class DashboardWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWindow(MemoStore *store, QWidget *parent = nullptr);

public slots:
    void refresh();
    void setStatusMessage(const QString &message);
    void applyTheme(ThemeMode mode);

signals:
    void showMemoRequested(MemoType type);
    void hideMemoRequested(MemoType type);
    void alwaysOnTopChanged(MemoType type, bool enabled);
    void hotkeyChangeRequested(const QKeySequence &sequence);
    void themeChangeRequested(ThemeMode mode);
    void autostartChanged(bool enabled);
    void exitRequested();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void setupUi();
    QWidget *createRecordsColumn(MemoType type, QWidget *parent);
    QWidget *createRecordCard(const MemoItem &memo, QWidget *parent) const;
    QWidget *createEmptyState(MemoType type, QWidget *parent) const;
    QWidget *createMemoControls(MemoType type, QWidget *parent);
    void refreshMemoControls(MemoType type);
    void refreshRecords();
    void refreshRecordColumn(MemoType type, const QVector<MemoItem> &records);
    void clearLayout(QBoxLayout *layout) const;

    MemoStore *store;
    QPushButton *questionVisibilityButton;
    QPushButton *todoVisibilityButton;
    QCheckBox *questionTopCheck;
    QCheckBox *todoTopCheck;
    QCheckBox *autostartCheck;
    QComboBox *themeCombo;
    QKeySequenceEdit *hotkeyEdit;
    QLabel *questionCountLabel;
    QLabel *todoCountLabel;
    QBoxLayout *questionRecordsLayout;
    QBoxLayout *todoRecordsLayout;
    QLabel *statusLabel;
    QFrame *recordsPanel;
    QFrame *sidePanel;
};

#endif // DASHBOARDWINDOW_H
