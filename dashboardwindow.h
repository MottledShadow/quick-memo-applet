#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

#include "memostore.h"

#include <QKeySequence>
#include <QWidget>

class QCheckBox;
class QComboBox;
class QEvent;
class QBoxLayout;
class QFrame;
class QKeySequenceEdit;
class QLabel;
class QPushButton;
class QScrollArea;

class DashboardWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWindow(MemoStore *store, QWidget *parent = nullptr);

public slots:
    void refresh();
    void setStatusMessage(const QString &message);
    void applyTheme(ThemeMode mode, FontSizeMode fontSize, DensityMode density);

signals:
    void showMemoRequested(MemoType type);
    void hideMemoRequested(MemoType type);
    void alwaysOnTopChanged(MemoType type, bool enabled);
    void hotkeyChangeRequested(const QKeySequence &sequence);
    void themeChangeRequested(ThemeMode mode);
    void languageChangeRequested(AppLanguage language);
    void fontSizeChangeRequested(FontSizeMode mode);
    void densityChangeRequested(DensityMode mode);
    void memoStartupDisplayChangeRequested(MemoStartupDisplayMode mode);
    void defaultInputTypeChangeRequested(DefaultInputTypeMode mode);
    void recordClickActionChangeRequested(RecordClickAction action);
    void recordSortOrderChangeRequested(RecordSortOrder order);
    void exportJsonRequested();
    void importJsonRequested();
    void inputAutoHideChanged(bool enabled);
    void recordClickRequested(const QString &id, MemoType type);
    void autostartChanged(bool enabled);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void setupUi();
    QWidget *createRecordsColumn(MemoType type, QWidget *parent);
    QWidget *createRecordCard(const MemoItem &memo, QWidget *parent);
    QWidget *createMemoControls(MemoType type, QWidget *parent);
    QFrame *createSettingsGroup(QLabel **titleLabel,
                                QWidget *parent,
                                QBoxLayout **contentLayout,
                                bool expandedByDefault = false) const;
    void retranslateUi();
    void rebuildComboLabels();
    void updateSettingsGroupSummaries();
    void refreshMemoControls(MemoType type);
    void refreshRecords();
    void refreshRecordColumn(MemoType type, const QVector<MemoItem> &records);
    void clearLayout(QBoxLayout *layout) const;
    void configureScrollArea(QScrollArea *scrollArea) const;

    MemoStore *store;
    QPushButton *questionVisibilityButton;
    QPushButton *todoVisibilityButton;
    QCheckBox *questionTopCheck;
    QCheckBox *todoTopCheck;
    QCheckBox *autostartCheck;
    QCheckBox *hideInputAfterSaveCheck;
    QComboBox *languageCombo;
    QComboBox *themeCombo;
    QComboBox *fontSizeCombo;
    QComboBox *densityCombo;
    QComboBox *memoStartupDisplayCombo;
    QComboBox *defaultInputTypeCombo;
    QComboBox *recordClickActionCombo;
    QComboBox *recordSortOrderCombo;
    QKeySequenceEdit *hotkeyEdit;
    QPushButton *applyHotkeyButton;
    QPushButton *exportJsonButton;
    QPushButton *importJsonButton;
    QFrame *hotkeyGroupFrame;
    QFrame *personalizationGroupFrame;
    QFrame *memoDisplayGroupFrame;
    QFrame *inputGroupFrame;
    QFrame *recordGroupFrame;
    QFrame *dataGroupFrame;
    QFrame *systemGroupFrame;
    QLabel *recordsTitleLabel;
    QLabel *memoSectionTitleLabel;
    QLabel *settingsSectionTitleLabel;
    QLabel *questionColumnTitleLabel;
    QLabel *todoColumnTitleLabel;
    QLabel *questionCountLabel;
    QLabel *todoCountLabel;
    QLabel *questionControlTitleLabel;
    QLabel *todoControlTitleLabel;
    QLabel *hotkeyGroupTitleLabel;
    QLabel *hotkeyFieldLabel;
    QLabel *personalizationGroupTitleLabel;
    QLabel *languageFieldLabel;
    QLabel *themeFieldLabel;
    QLabel *fontSizeFieldLabel;
    QLabel *densityFieldLabel;
    QLabel *memoDisplayGroupTitleLabel;
    QLabel *memoStartupDisplayFieldLabel;
    QLabel *inputGroupTitleLabel;
    QLabel *defaultInputTypeFieldLabel;
    QLabel *recordGroupTitleLabel;
    QLabel *recordClickActionFieldLabel;
    QLabel *recordSortOrderFieldLabel;
    QLabel *dataGroupTitleLabel;
    QLabel *systemGroupTitleLabel;
    QBoxLayout *questionRecordsLayout;
    QBoxLayout *todoRecordsLayout;
    QLabel *statusLabel;
    QFrame *recordsPanel;
    QFrame *sidePanel;
};

#endif // DASHBOARDWINDOW_H
