#include "screenshottool.h"
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

ScreenshotTool::ScreenshotTool(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    applyTheme(Themes::Light); // Светлая по умолчанию

    // Симуляция скриншота для демо (на реальном приложении заменить на захват)
    QTimer::singleShot(500, this, &ScreenshotTool::onNewScreenshot);
}

ScreenshotTool::~ScreenshotTool()
{
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
    layout->addWidget(previewLabel);
    
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Панель инструментов
    QToolBar *toolBar = new QToolBar("Инструменты", this);
    toolBar->setMovable(false);
    addToolBar(toolBar);

    // Кнопки
    QPushButton *btnNew = new QPushButton("📸 Новый", this);
    btnNew->setObjectName("primary");
    connect(btnNew, &QPushButton::clicked, this, &ScreenshotTool::onNewScreenshot);
    toolBar->addWidget(btnNew);

    toolBar->addSeparator();

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
    toolBar->addWidget(new QLabel("Тема: ", this));
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

QPixmap ScreenshotTool::captureScreen()
{
    // Для демонстрации — захватываем весь экран
    // На Windows 7 работает корректно через QScreen
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) return QPixmap();
    
    return screen->grabWindow(0); // 0 = весь экран
}

void ScreenshotTool::onNewScreenshot()
{
    currentScreenshot = captureScreen();
    
    if (!currentScreenshot.isNull()) {
        // Масштабируем для предпросмотра
        QPixmap preview = currentScreenshot.scaled(
            previewLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );
        previewLabel->setPixmap(preview);
        previewLabel->setText("");
        statusBar()->showMessage(QString("Скриншот сделан: %1x%2")
            .arg(currentScreenshot.width())
            .arg(currentScreenshot.height()));
    } else {
        previewLabel->setText("Ошибка захвата экрана");
        statusBar()->showMessage("Не удалось сделать скриншот", 5000);
    }
}

void ScreenshotTool::onSave()
{
    if (currentScreenshot.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Нет скриншота для сохранения");
        return;
    }

    QString path = QFileDialog::getSaveFileName(
        this,
        "Сохранить скриншот",
        "screenshot.png",
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
