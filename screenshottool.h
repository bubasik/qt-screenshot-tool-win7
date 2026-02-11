#ifndef SCREENSHOTTOOL_H
#define SCREENSHOTTOOL_H

#include <QMainWindow>
#include <QComboBox>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QShortcut>

class RegionSelector;
class QPushButton;

class ScreenshotTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScreenshotTool(QWidget *parent = nullptr);
    ~ScreenshotTool() override;

private slots:
    void onFullScreenshot();
    void onRegionScreenshot();
    void onRegionSelected(const QPixmap &pixmap);
    void onRegionCancelled();
    void onSave();
    void onCopy();
    void onThemeChanged(int index);

private:
    void setupUI();
    void setupShortcuts();
    void applyTheme(const QString &theme);
    void setPreviewPixmap(const QPixmap &pixmap);
    QPixmap captureFullScreen();

    QLabel *previewLabel;
    QComboBox *themeComboBox;
    QPushButton *regionButton;
    QPushButton *fullButton;
    QPixmap currentScreenshot;
    RegionSelector *regionSelector;
    QList<QShortcut*> shortcuts;
};

#endif // SCREENSHOTTOOL_H
