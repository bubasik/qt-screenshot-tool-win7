#ifndef SCREENSHOTTOOL_H
#define SCREENSHOTTOOL_H

#include <QMainWindow>
#include <QComboBox>
#include <QLabel>
#include <QPixmap>
#include <QTimer>

class ScreenshotTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScreenshotTool(QWidget *parent = nullptr);
    ~ScreenshotTool() override;

private slots:
    void onNewScreenshot();
    void onSave();
    void onCopy();
    void onThemeChanged(int index);

private:
    void setupUI();
    void applyTheme(const QString &theme);
    QPixmap captureScreen();

    QLabel *previewLabel;
    QComboBox *themeComboBox;
    QPixmap currentScreenshot;
};

#endif // SCREENSHOTTOOL_H
