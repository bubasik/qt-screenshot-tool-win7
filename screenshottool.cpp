#include "screenshottool.h"
#include "regionselector.h"
#include "themes.h"
#include <QToolBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QClipboard>
#include <QScreen>
#include <QApplication>
#include <QStatusBar>
#include <QMessageBox>
#include <QDateTime>

ScreenshotTool::ScreenshotTool(QWidget *parent)
    : QMainWindow(parent),
      regionSelector(nullptr)
{
    setupUI();
    applyTheme(Themes::Light); // Светлая по умолчанию

    // Демо-скриншот через 500 мс
    QTimer::singleShot(500, this, &ScreenshotTool::onFullScreenshot);
}

ScreenshotTool::~ScreenshotTool()
{
    delete regionSelector;
}

void ScreenshotTool::setupUI()
{
    // Центральный виджет с предпросмотром
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    
    previewLabel = new QLabel("Сделайте скриншот", this);
    previewLabel->setObjectName("preview");
    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setMinimumSize(400, 300);
    previewLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(previewLabel);
    
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Панель инструментов
    QToolBar *toolBar = new QToolBar("Инструменты", this);
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(24, 24));
    addToolBar(toolBar);

    // Кнопки скриншотов
    fullButton = new QPushButton("🖼️ Весь экран", this);
    fullButton->setObjectName("primary");
    connect(fullButton, &QPushButton::clicked, this, &ScreenshotTool::onFullScreenshot);
    toolBar->addWidget(fullButton);

    regionButton = new QPushButton("✏️ Выделить область", this);
    connect(regionButton, &QPushButton::clicked, this, &ScreenshotTool::onRegionScreenshot);
    toolBar->addWidget(regionButton);

    toolBar->addSeparator();

    // Кнопки действий
    QPushButton *btnSave = new QPushButton("💾 Сохранить", this);
    connect(btnSave, &QPushButton::clicked, this, &ScreenshotTool::onSave);
    toolBar->addWidget(btnSave);

    QPushButton *btnCopy = new QPushButton("📋 Копировать", this);
    connect(btnCopy, &QPushButton::clicked, this, &ScreenshotTool::onCopy);
    toolBar->addWidget(btnCopy);

    toolBar->addSeparator();

    // Выбор темы
    themeComboBox = new QComboBox(this);
    themeComboBox->addItem("🎨 Светлая");
    themeComboBox->addItem("🌙 Тёмная");
    themeComboBox->addItem("💎 Бирюзовая");
    themeComboBox->addItem("💻 Матрица");
    connect(themeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ScreenshotTool::onThemeChanged);
    toolBar->addWidget(new QLabel(" Тема: ", this));
    toolBar->addWidget(themeComboBox);

    // Статусбар
    statusBar()->showMessage("Готово к захвату экрана");
}

void ScreenshotTool::applyTheme(const QString &theme)
{
    qApp->setStyleSheet(theme);
    // Обновляем стили дочерних элементов
    previewLabel->style()->unpolish(previewLabel);
    previewLabel->style()->polish(previewLabel);
    previewLabel->update();
    
    fullButton->style()->unpolish(fullButton);
    fullButton->style()->polish(fullButton);
    fullButton->update();
    
    regionButton->style()->unpolish(regionButton);
    regionButton->style()->polish(regionButton);
    regionButton->update();
}

void ScreenshotTool::onThemeChanged(int index)
{
    switch (index) {
        case 0: applyTheme(Themes::Light); break;
        case 1: applyTheme(Themes::Dark); break;
        case 2: applyTheme(Themes::Teal); break;
        case 3: applyTheme(Themes::Matrix); break;
    }
    statusBar()->showMessage(QString("Тема: %1").arg(themeComboBox->currentText()));
}

QPixmap ScreenshotTool::captureFullScreen()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) return QPixmap();
    
    return screen->grabWindow(0); // 0 = весь экран
}

void ScreenshotTool::onFullScreenshot()
{
    currentScreenshot = captureFullScreen();
    
    if (!currentScreenshot.isNull()) {
        setPreviewPixmap(currentScreenshot);
        statusBar()->showMessage(QString("Скриншот всего экрана: %1x%2")
            .arg(currentScreenshot.width())
            .arg(currentScreenshot.height()));
    } else {
        previewLabel->setText("Ошибка захвата экрана");
        statusBar()->showMessage("Не удалось сделать скриншот", 5000);
    }
}

void ScreenshotTool::onRegionScreenshot()
{
    // Создаём селектор при первом использовании
    if (!regionSelector) {
        regionSelector = new RegionSelector();
        connect(regionSelector, &RegionSelector::selectionFinished,
                this, &ScreenshotTool::onRegionSelected);
        connect(regionSelector, &RegionSelector::selectionCancelled,
                this, &ScreenshotTool::onRegionCancelled);
    }
    
    statusBar()->showMessage("Выделите область мышью. ESC — отмена, ПКМ — отмена");
    regionSelector->startSelection();
}

void ScreenshotTool::onRegionSelected(const QPixmap &pixmap)
{
    currentScreenshot = pixmap;
    setPreviewPixmap(currentScreenshot);
    statusBar()->showMessage(QString("Выделенная область: %1x%2")
        .arg(currentScreenshot.width())
        .arg(currentScreenshot.height()));
}

void ScreenshotTool::onRegionCancelled()
{
    statusBar()->showMessage("Выделение отменено");
}

void ScreenshotTool::setPreviewPixmap(const QPixmap &pixmap)
{
    // Масштабируем для предпросмотра с сохранением пропорций
    QPixmap preview = pixmap.scaled(
        previewLabel->size() * 0.9,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );
    previewLabel->setPixmap(preview);
    previewLabel->setText("");
}

void ScreenshotTool::onSave()
{
    if (currentScreenshot.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Нет скриншота для сохранения");
        return;
    }

    QString defaultName = QString("screenshot_%1.png")
        .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    
    QString path = QFileDialog::getSaveFileName(
        this,
        "Сохранить скриншот",
        defaultName,
        "PNG Изображение (*.png);;JPEG Изображение (*.jpg)"
    );

    if (!path.isEmpty()) {
        if (currentScreenshot.save(path)) {
            statusBar()->showMessage(QString("Сохранено: %1").arg(path), 3000);
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось сохранить файл");
        }
    }
}

void ScreenshotTool::onCopy()
{
    if (currentScreenshot.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Нет скриншота для копирования");
        return;
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setPixmap(currentScreenshot);
    statusBar()->showMessage("Скриншот скопирован в буфер обмена", 3000);
}
