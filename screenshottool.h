#ifndef SCREENSHOTTOOL_H
#define SCREENSHOTTOOL_H

#include <QMainWindow>
#include <QComboBox>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QShortcut>  // ← Добавлено для горячих клавиш

class RegionSelector;
class QPushButton;

class ScreenshotTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScreenshotTool(QWidget *parent = nullptr);
    ~ScreenshotTool() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;  // Для F11 (полноэкранный режим)

private slots:
    void onFullScreenshot();
    void onRegionScreenshot();
    void onRegionSelected(const QPixmap &pixmap);
    void onRegionCancelled();
    void onSave();
    void onCopy();
    void onThemeChanged(int index);
    void toggleFullScreenPreview();  // Для F11

private:
    void setupUI();
    void setupShortcuts();  // ← Новая функция для горячих клавиш
    void applyTheme(const QString &theme);
    void setPreviewPixmap(const QPixmap &pixmap);
    QPixmap captureFullScreen();

    QLabel *previewLabel;
    QComboBox *themeComboBox;
    QPushButton *regionButton;
    QPushButton *fullButton;
    QPixmap currentScreenshot;
    RegionSelector *regionSelector;
    bool isPreviewFullScreen;
    QList<QShortcut*> shortcuts;  // Для очистки при удалении
};

#endif // SCREENSHOTTOOL_H
