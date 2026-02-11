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
#include <QShortcut>  // ← Добавлено

ScreenshotTool::ScreenshotTool(QWidget *parent)
    : QMainWindow(parent),
      regionSelector(nullptr),
      isPreviewFullScreen(false)
{
    setupUI();
    setupShortcuts();  // ← Инициализация горячих клавиш
    applyTheme(Themes::Light);

    // Демо-скриншот через 500 мс
    QTimer::singleShot(500, this, &ScreenshotTool::onFullScreenshot);
}

ScreenshotTool::~ScreenshotTool()
{
    // Очистка горячих клавиш
    qDeleteAll(shortcuts);
    shortcuts.clear();
    
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
    previewLabel->setTextFormat(Qt::RichText);
    previewLabel->setText("<div style='color: #666; font-size: 14px; padding: 20px;'>"
                          "📸 Сделайте скриншот<br><br>"
                          "<span style='font-size: 12px; color: #999;'>"
                          "Горячие клавиши:<br>"
                          "Ctrl+Shift+S — весь экран<br>"
                          "Ctrl+Shift+A — выделить область<br>"
                          "Ctrl+S — сохранить<br>"
                          "Ctrl+C — копировать<br>"
                          "F11 — полноэкранный предпросмотр"
                          "</span>"
                          "</div>");
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
    fullButton->setToolTip("Ctrl+Shift+S");
    connect(fullButton, &QPushButton::clicked, this, &ScreenshotTool::onFullScreenshot);
    toolBar->addWidget(fullButton);

    regionButton = new QPushButton("✏️ Выделить область", this);
    regionButton->setToolTip("Ctrl+Shift+A");
    connect(regionButton, &QPushButton::clicked, this, &ScreenshotTool::onRegionScreenshot);
    toolBar->addWidget(regionButton);

    toolBar->addSeparator();

    // Кнопки действий
    QPushButton *btnSave = new QPushButton("💾 Сохранить", this);
    btnSave->setToolTip("Ctrl+S");
    connect(btnSave, &QPushButton::clicked, this, &ScreenshotTool::onSave);
    toolBar->addWidget(btnSave);

    QPushButton *btnCopy = new QPushButton("📋 Копировать", this);
    btnCopy->setToolTip("Ctrl+C");
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

    // Статусбар с подсказками по горячим клавишам
    statusBar()->showMessage("Готово • Горячие клавиши: Ctrl+Shift+S/A, Ctrl+S/C, F11");
}

void ScreenshotTool::setupShortcuts()
{
    // Ctrl+Shift+S — весь экран
    QShortcut *shortcutFull = new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S), this);
    connect(shortcutFull, &QShortcut::activated, this, &ScreenshotTool::onFullScreenshot);
    shortcutFull->setToolTip("Скриншот всего экрана");
    shortcuts.append(shortcutFull);

    // Ctrl+Shift+A — выделение области
    QShortcut *shortcutRegion = new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A), this);
    connect(shortcutRegion, &QShortcut::activated, this, &ScreenshotTool::onRegionScreenshot);
    shortcutRegion->setToolTip("Выделить область");
    shortcuts.append(shortcutRegion);

    // Ctrl+S — сохранить
    QShortcut *shortcutSave = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this);
    connect(shortcutSave, &QShortcut::activated, this, &ScreenshotTool::onSave);
    shortcutSave->setToolTip("Сохранить скриншот");
    shortcuts.append(shortcutSave);

    // Ctrl+C — копировать
    QShortcut *shortcutCopy = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_C), this);
    connect(shortcutCopy, &QShortcut::activated, this, &ScreenshotTool::onCopy);
    shortcutCopy->setToolTip("Копировать в буфер");
    shortcuts.append(shortcutCopy);

    // F11 — полноэкранный предпросмотр
    QShortcut *shortcutFullscreen = new QShortcut(QKeySequence(Qt::Key_F11), this);
    connect(shortcutFullscreen, &QShortcut::activated, this, &ScreenshotTool::toggleFullScreenPreview);
    shortcutFullscreen->setToolTip("Полноэкранный режим");
    shortcuts.append(shortcutFullscreen);
}

void ScreenshotTool::applyTheme(const QString &theme)
{
    qApp->setStyleSheet(theme);
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
    statusBar()->showMessage(QString("Тема: %1 • Горячие клавиши активны").arg(themeComboBox->currentText()));
}

QPixmap ScreenshotTool::captureFullScreen()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) return QPixmap();
    
    return screen->grabWindow(0);
}

void ScreenshotTool::onFullScreenshot()
{
    currentScreenshot = captureFullScreen();
    
    if (!currentScreenshot.isNull()) {
        setPreviewPixmap(currentScreenshot);
        statusBar()->showMessage(QString("Скриншот всего экрана: %1x%2 • Ctrl+S — сохранить, Ctrl+C — копировать")
            .arg(currentScreenshot.width())
            .arg(currentScreenshot.height()));
    } else {
        previewLabel->setText("<div style='color: #f44336; padding: 20px;'>Ошибка захвата экрана</div>");
        statusBar()->showMessage("Не удалось сделать скриншот", 5000);
    }
}

void ScreenshotTool::onRegionScreenshot()
{
    if (!regionSelector) {
        regionSelector = new RegionSelector();
        connect(regionSelector, &RegionSelector::selectionFinished,
                this, &ScreenshotTool::onRegionSelected);
        connect(regionSelector, &RegionSelector::selectionCancelled,
                this, &ScreenshotTool::onRegionCancelled);
    }
    
    statusBar()->showMessage("Выделите область мышью • Esc — отмена");
    regionSelector->startSelection();
}

void ScreenshotTool::onRegionSelected(const QPixmap &pixmap)
{
    currentScreenshot = pixmap;
    setPreviewPixmap(currentScreenshot);
    statusBar()->showMessage(QString("Выделенная область: %1x%2 • Ctrl+S — сохранить")
        .arg(currentScreenshot.width())
        .arg(currentScreenshot.height()));
}

void ScreenshotTool::onRegionCancelled()
{
    statusBar()->showMessage("Выделение отменено • Попробуйте снова: Ctrl+Shift+A");
}

void ScreenshotTool::setPreviewPixmap(const QPixmap &pixmap)
{
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
            statusBar()->showMessage(QString("Сохранено: %1 • Размер: %2 КБ")
                .arg(path)
                .arg(QFile(path).size() / 1024), 3000);
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
    statusBar()->showMessage("Скриншот скопирован в буфер обмена • Ctrl+V для вставки", 3000);
}

void ScreenshotTool::toggleFullScreenPreview()
{
    if (currentScreenshot.isNull()) return;
    
    if (isPreviewFullScreen) {
        // Возврат в оконный режим
        showNormal();
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
        setCentralWidget(previewLabel->parentWidget());
        show();
        isPreviewFullScreen = false;
        statusBar()->show();
        statusBar()->showMessage("Полноэкранный режим отключён • F11 для переключения");
    } else {
        // Переход в полноэкранный режим
        statusBar()->hide();
        setCentralWidget(nullptr);
        setCentralWidget(previewLabel);
        showFullScreen();
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        previewLabel->setPixmap(currentScreenshot.scaled(
            previewLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        ));
        isPreviewFullScreen = true;
        // Подсказка в углу экрана
        QTimer::singleShot(3000, this, [this]() {
            previewLabel->setText("<div style='background: rgba(0,0,0,0.5); color: white; padding: 10px; position: absolute; bottom: 20px; left: 20px; border-radius: 4px;'>"
                                  "F11 — выйти из полноэкранного режима<br>"
                                  "Ctrl+S — сохранить | Ctrl+C — копировать"
                                  "</div>");
        });
    }
}

void ScreenshotTool::keyPressEvent(QKeyEvent *event)
{
    // Обработка F11 вручную (на случай если горячая клавиша не сработала)
    if (event->key() == Qt::Key_F11) {
        toggleFullScreenPreview();
        return;
    }
    // Передаём остальные события базовому классу
    QMainWindow::keyPressEvent(event);
}
