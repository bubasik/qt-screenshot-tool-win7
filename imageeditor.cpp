#include "imageeditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QColorDialog>
#include <QSpinBox>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QLineEdit>
#include <QTextEdit>
#include <QStyleOption>
#include <QStyle>
#include <QApplication>
#include <cmath>

ImageEditor::ImageEditor(QWidget *parent)
    : QWidget(parent)
    , currentTool(EditTool::Select)
    , isDrawing(false)
    , currentColor(Qt::red)
    , currentThickness(3)
    , currentBlurRadius(10)
    , handleSize(10)
    , isDraggingHandle(false)
    , dragHandleIndex(-1)
    , isTextEditing(false)
{
    setupUI();
    setupConnections();
}

ImageEditor::~ImageEditor()
{
    // Clean up resources if needed
}

void ImageEditor::setImage(const QPixmap &image)
{
    originalImage = image;
    currentImage = image;
    previewImage = image;
    update();
}

QPixmap ImageEditor::getImage() const
{
    return currentImage;
}

void ImageEditor::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Create toolbar
    toolbar = new QWidget(this);
    QHBoxLayout *toolbarLayout = new QHBoxLayout(toolbar);
    
    // Tool buttons
    QToolButton *cropButton = new QToolButton(this);
    cropButton->setText("Crop");
    cropButton->setCheckable(true);
    connect(cropButton, &QToolButton::clicked, [this]() { onToolSelected(EditTool::Crop); });
    
    QToolButton *blurButton = new QToolButton(this);
    blurButton->setText("Blur");
    blurButton->setCheckable(true);
    connect(blurButton, &QToolButton::clicked, [this]() { onToolSelected(EditTool::Blur); });
    
    QToolButton *arrowButton = new QToolButton(this);
    arrowButton->setText("Arrow");
    arrowButton->setCheckable(true);
    connect(arrowButton, &QToolButton::clicked, [this]() { onToolSelected(EditTool::Arrow); });
    
    QToolButton *textButton = new QToolButton(this);
    textButton->setText("Text");
    textButton->setCheckable(true);
    connect(textButton, &QToolButton::clicked, [this]() { onToolSelected(EditTool::Text); });
    
    // Color button
    QPushButton *colorButton = new QPushButton("Color", this);
    connect(colorButton, &QPushButton::clicked, this, &ImageEditor::onColorChanged);
    
    // Thickness control
    QLabel *thicknessLabel = new QLabel("Thickness:", this);
    QSpinBox *thicknessSpinBox = new QSpinBox(this);
    thicknessSpinBox->setRange(1, 20);
    thicknessSpinBox->setValue(currentThickness);
    connect(thicknessSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &ImageEditor::onThicknessChanged);
    
    // Blur radius control
    QLabel *blurLabel = new QLabel("Blur Radius:", this);
    QSpinBox *blurSpinBox = new QSpinBox(this);
    blurSpinBox->setRange(1, 50);
    blurSpinBox->setValue(currentBlurRadius);
    connect(blurSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &ImageEditor::onBlurRadiusChanged);
    
    // Text input
    textLineEdit = new QLineEdit(this);
    textLineEdit->setPlaceholderText("Enter text...");
    textLineEdit->hide();
    connect(textLineEdit, &QLineEdit::editingFinished, this, &ImageEditor::onTextEditingFinished);
    
    // Add widgets to toolbar
    toolbarLayout->addWidget(cropButton);
    toolbarLayout->addWidget(blurButton);
    toolbarLayout->addWidget(arrowButton);
    toolbarLayout->addWidget(textButton);
    toolbarLayout->addWidget(colorButton);
    toolbarLayout->addWidget(thicknessLabel);
    toolbarLayout->addWidget(thicknessSpinBox);
    toolbarLayout->addWidget(blurLabel);
    toolbarLayout->addWidget(blurSpinBox);
    toolbarLayout->addWidget(textLineEdit);
    toolbarLayout->addStretch();
    
    mainLayout->addWidget(toolbar);
    
    // Create graphics view for text editing
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameShape(QFrame::NoFrame);
    view->setVisible(false);
    
    mainLayout->addWidget(view);
    
    setLayout(mainLayout);
}

void ImageEditor::setupConnections()
{
    // Connections are handled in setupUI
}

void ImageEditor::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    
    // Draw the current image
    if (!currentImage.isNull()) {
        // Calculate position to center the image
        int x = (width() - currentImage.width()) / 2;
        int y = (height() - currentImage.height()) / 2;
        
        painter.drawPixmap(x, y, currentImage);
        
        // Draw editing overlays based on current tool
        if (currentTool == EditTool::Crop && isDrawing) {
            drawCropHandles(painter);
        } else if (currentTool == EditTool::Blur && isDrawing) {
            drawBlurOverlay(painter);
        } else if (currentTool == EditTool::Arrow && isDrawing) {
            drawArrow(painter);
        } else if (currentTool == EditTool::Text && isDrawing) {
            drawText(painter);
        }
    }
}

void ImageEditor::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }
    
    startPoint = event->pos();
    endPoint = event->pos();
    isDrawing = true;
    
    // Check if clicking on a crop handle
    if (currentTool == EditTool::Crop) {
        // Calculate handle positions based on current activeCropRect
        if (activeCropRect.isValid()) {
            topLeftHandle = QRect(activeCropRect.topLeft() - QPoint(handleSize/2, handleSize/2), 
                                  QSize(handleSize, handleSize));
            topRightHandle = QRect(activeCropRect.topRight() - QPoint(handleSize/2, handleSize/2), 
                                   QSize(handleSize, handleSize));
            bottomLeftHandle = QRect(activeCropRect.bottomLeft() - QPoint(handleSize/2, handleSize/2), 
                                     QSize(handleSize, handleSize));
            bottomRightHandle = QRect(activeCropRect.bottomRight() - QPoint(handleSize/2, handleSize/2), 
                                      QSize(handleSize, handleSize));
        }
        
        if (topLeftHandle.contains(startPoint)) {
            isDraggingHandle = true;
            dragHandleIndex = 0; // top-left
        } else if (topRightHandle.contains(startPoint)) {
            isDraggingHandle = true;
            dragHandleIndex = 1; // top-right
        } else if (bottomLeftHandle.contains(startPoint)) {
            isDraggingHandle = true;
            dragHandleIndex = 2; // bottom-left
        } else if (bottomRightHandle.contains(startPoint)) {
            isDraggingHandle = true;
            dragHandleIndex = 3; // bottom-right
        } else {
            // Start new crop selection
            isDraggingHandle = false;
            activeCropRect = QRect(startPoint, QSize());
        }
    }
    
    update();
}

void ImageEditor::mouseMoveEvent(QMouseEvent *event)
{
    if (!isDrawing) {
        return;
    }
    
    endPoint = event->pos();
    
    if (currentTool == EditTool::Crop && isDraggingHandle) {
        // Adjust crop rectangle based on which handle is being dragged
        switch (dragHandleIndex) {
            case 0: // top-left
                activeCropRect.setTopLeft(endPoint);
                break;
            case 1: // top-right
                activeCropRect.setTopRight(endPoint);
                break;
            case 2: // bottom-left
                activeCropRect.setBottomLeft(endPoint);
                break;
            case 3: // bottom-right
                activeCropRect.setBottomRight(endPoint);
                break;
        }
        
        // Ensure the rectangle remains valid (width and height > 0)
        if (activeCropRect.width() < 1) {
            if (dragHandleIndex == 0 || dragHandleIndex == 2) { // left handles
                activeCropRect.setLeft(activeCropRect.right() - 1);
            } else { // right handles
                activeCropRect.setRight(activeCropRect.left() + 1);
            }
        }
        
        if (activeCropRect.height() < 1) {
            if (dragHandleIndex == 0 || dragHandleIndex == 1) { // top handles
                activeCropRect.setTop(activeCropRect.bottom() - 1);
            } else { // bottom handles
                activeCropRect.setBottom(activeCropRect.top() + 1);
            }
        }
    } else if (currentTool == EditTool::Crop && !isDraggingHandle) {
        // Update the crop rectangle
        activeCropRect = getNormalizedRect(startPoint, endPoint);
    }
    
    update();
}

void ImageEditor::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !isDrawing) {
        return;
    }
    
    isDrawing = false;
    isDraggingHandle = false;
    
    if (currentTool == EditTool::Crop) {
        applyCrop();
    } else if (currentTool == EditTool::Blur) {
        applyBlur();
    } else if (currentTool == EditTool::Arrow) {
        applyArrow();
    } else if (currentTool == EditTool::Text) {
        applyText();
    }
    
    update();
}

void ImageEditor::drawCropHandles(QPainter &painter)
{
    if (activeCropRect.isValid()) {
        // Draw semi-transparent overlay
        painter.setBrush(QColor(0, 0, 0, 128));
        painter.setPen(Qt::NoPen);
        
        // Top
        painter.drawRect(0, 0, width(), activeCropRect.top());
        // Bottom
        painter.drawRect(0, activeCropRect.bottom(), width(), height() - activeCropRect.bottom());
        // Left
        painter.drawRect(0, activeCropRect.top(), activeCropRect.left(), activeCropRect.height());
        // Right
        painter.drawRect(activeCropRect.right(), activeCropRect.top(), 
                         width() - activeCropRect.right(), activeCropRect.height());
        
        // Draw crop rectangle border
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(Qt::white, 2));
        painter.drawRect(activeCropRect.adjusted(-1, -1, 1, 1));
        
        // Draw handles at corners
        painter.setBrush(Qt::white);
        painter.setPen(QPen(Qt::black, 1));
        
        topLeftHandle = QRect(activeCropRect.topLeft() - QPoint(handleSize/2, handleSize/2), 
                              QSize(handleSize, handleSize));
        topRightHandle = QRect(activeCropRect.topRight() - QPoint(handleSize/2, handleSize/2), 
                               QSize(handleSize, handleSize));
        bottomLeftHandle = QRect(activeCropRect.bottomLeft() - QPoint(handleSize/2, handleSize/2), 
                                 QSize(handleSize, handleSize));
        bottomRightHandle = QRect(activeCropRect.bottomRight() - QPoint(handleSize/2, handleSize/2), 
                                  QSize(handleSize, handleSize));
        
        painter.drawRect(topLeftHandle);
        painter.drawRect(topRightHandle);
        painter.drawRect(bottomLeftHandle);
        painter.drawRect(bottomRightHandle);
    }
}

void ImageEditor::drawBlurOverlay(QPainter &painter)
{
    if (activeCropRect.isValid()) {
        // Create a blurred version of the selected area
        QPixmap blurredSection = currentImage.copy(activeCropRect).scaled(
            activeCropRect.size() * 0.1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        blurredSection = blurredSection.scaled(
            activeCropRect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        
        // Draw the blurred section
        painter.drawPixmap(activeCropRect, blurredSection);
        
        // Draw border around the blurred area
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(Qt::white, 2));
        painter.drawRect(activeCropRect.adjusted(-1, -1, 1, 1));
    }
}

void ImageEditor::drawArrow(QPainter &painter)
{
    Q_UNUSED(painter)
    // Arrow will be drawn in the temporary preview
    // Actual arrow will be applied when mouse is released
}

void ImageEditor::drawText(QPainter &painter)
{
    Q_UNUSED(painter)
    // Text will be handled by the graphics view
    // Show text input field
    textLineEdit->show();
    textLineEdit->setFocus();
}

void ImageEditor::applyCrop()
{
    if (activeCropRect.isValid() && !originalImage.isNull()) {
        // Convert coordinates from widget space to image space
        // Calculate the scaling factor between widget and image
        qreal scaleX = static_cast<qreal>(originalImage.width()) / width();
        qreal scaleY = static_cast<qreal>(originalImage.height()) / height();
        
        // Adjust crop rectangle to image coordinates
        QRect imageCropRect(
            static_cast<int>(activeCropRect.x() * scaleX),
            static_cast<int>(activeCropRect.y() * scaleY),
            static_cast<int>(activeCropRect.width() * scaleX),
            static_cast<int>(activeCropRect.height() * scaleY)
        );
        
        // Ensure the crop rectangle stays within image bounds
        imageCropRect = imageCropRect.intersected(originalImage.rect());
        
        if (imageCropRect.isValid() && !imageCropRect.isEmpty()) {
            // Crop the image to the selected rectangle
            QPixmap cropped = originalImage.copy(imageCropRect);
            currentImage = cropped;
            originalImage = cropped; // Update original for future operations
            
            // Reset crop rectangle
            activeCropRect = QRect();
            
            emit imageEdited(currentImage);
        }
    }
}

void ImageEditor::applyBlur()
{
    if (activeCropRect.isValid() && !currentImage.isNull()) {
        // Create a copy of the current image to modify
        QPixmap result = currentImage;
        
        // Convert coordinates from widget space to image space
        qreal scaleX = static_cast<qreal>(currentImage.width()) / width();
        qreal scaleY = static_cast<qreal>(currentImage.height()) / height();
        
        // Adjust blur rectangle to image coordinates
        QRect imageBlurRect(
            static_cast<int>(activeCropRect.x() * scaleX),
            static_cast<int>(activeCropRect.y() * scaleY),
            static_cast<int>(activeCropRect.width() * scaleX),
            static_cast<int>(activeCropRect.height() * scaleY)
        );
        
        // Ensure the blur rectangle stays within image bounds
        imageBlurRect = imageBlurRect.intersected(currentImage.rect());
        
        if (imageBlurRect.isValid() && !imageBlurRect.isEmpty()) {
            // Create a copy of the area to blur
            QPixmap sourceSection = currentImage.copy(imageBlurRect);
            
            // Apply blur effect using QImage manipulation
            QImage blurredImage = sourceSection.toImage();
            blurImage(blurredImage, currentBlurRadius);
            
            // Draw the blurred section back onto the result image
            QPainter painter(&result);
            painter.drawImage(imageBlurRect, blurredImage);
            
            currentImage = result;
        }
        
        // Reset blur rectangle
        activeCropRect = QRect();
        
        emit imageEdited(currentImage);
    }
}

// Helper function to apply Gaussian blur to an image
void ImageEditor::blurImage(QImage &image, int radius)
{
    if (radius <= 0) return;
    
    // A simple box blur implementation - for a more sophisticated Gaussian blur,
    // we could implement a more complex algorithm
    QImage blurred = image;
    
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            int r = 0, g = 0, b = 0, a = 0;
            int count = 0;
            
            // Sample pixels in a neighborhood defined by the radius
            for (int dy = -radius; dy <= radius; ++dy) {
                for (int dx = -radius; dx <= radius; ++dx) {
                    int nx = x + dx;
                    int ny = y + dy;
                    
                    // Check if the neighbor pixel is within bounds
                    if (nx >= 0 && nx < image.width() && ny >= 0 && ny < image.height()) {
                        QRgb pixel = image.pixel(nx, ny);
                        r += qRed(pixel);
                        g += qGreen(pixel);
                        b += qBlue(pixel);
                        a += qAlpha(pixel);
                        count++;
                    }
                }
            }
            
            // Calculate average values
            if (count > 0) {
                r /= count;
                g /= count;
                b /= count;
                a /= count;
                
                blurred.setPixel(x, y, qRgba(r, g, b, a));
            }
        }
    }
    
    image = blurred;
}

void ImageEditor::applyArrow()
{
    if (!currentImage.isNull()) {
        // Create a copy of the current image to modify
        QPixmap result = currentImage;
        QPainter painter(&result);
        
        // Set pen for arrow
        QPen pen(currentColor, currentThickness);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(pen);
        
        // Draw the arrow line
        painter.drawLine(startPoint, endPoint);
        
        // Draw arrowhead
        double angle = std::atan2(static_cast<double>(endPoint.y() - startPoint.y()), 
                                  static_cast<double>(endPoint.x() - startPoint.x()));
        
        // Calculate arrowhead points
        double headLength = 10 + currentThickness; // Make arrowhead size proportional to thickness
        QPointF arrowP1 = endPoint - QPointF(headLength * std::cos(angle - M_PI / 6), 
                                            headLength * std::sin(angle - M_PI / 6));
        QPointF arrowP2 = endPoint - QPointF(headLength * std::cos(angle + M_PI / 6), 
                                            headLength * std::sin(angle + M_PI / 6));
        
        painter.drawLine(endPoint, arrowP1);
        painter.drawLine(endPoint, arrowP2);
        
        currentImage = result;
        
        emit imageEdited(currentImage);
    }
}

void ImageEditor::applyText()
{
    if (!currentImage.isNull() && !textLineEdit->text().isEmpty()) {
        // Create a copy of the current image to modify
        QPixmap result = currentImage;
        QPainter painter(&result);
        
        // Set text properties
        painter.setPen(QPen(currentColor, 1)); // Text color
        painter.setFont(QFont("Arial", 16 + currentThickness)); // Size based on thickness setting
        
        // Draw the text at the start point
        painter.drawText(startPoint, textLineEdit->text());
        
        currentImage = result;
        
        // Hide text input and clear it
        textLineEdit->hide();
        textLineEdit->clear();
        
        emit imageEdited(currentImage);
    } else {
        // If no text was entered, just hide the input field
        textLineEdit->hide();
    }
}

QRect ImageEditor::getNormalizedRect(const QPoint &p1, const QPoint &p2) const
{
    int x1 = qMin(p1.x(), p2.x());
    int y1 = qMin(p1.y(), p2.y());
    int x2 = qMax(p1.x(), p2.x());
    int y2 = qMax(p1.y(), p2.y());
    return QRect(x1, y1, x2 - x1, y2 - y1);
}

void ImageEditor::onToolSelected(EditTool tool)
{
    currentTool = tool;
    
    // Update button states
    for (auto child : toolbar->findChildren<QToolButton*>()) {
        if (child->text() == "Crop") {
            child->setChecked(tool == EditTool::Crop);
        } else if (child->text() == "Blur") {
            child->setChecked(tool == EditTool::Blur);
        } else if (child->text() == "Arrow") {
            child->setChecked(tool == EditTool::Arrow);
        } else if (child->text() == "Text") {
            child->setChecked(tool == EditTool::Text);
        }
    }
    
    // Hide text input if not in text mode
    if (tool != EditTool::Text) {
        textLineEdit->hide();
    }
    
    update();
}

void ImageEditor::onColorChanged()
{
    QColor newColor = QColorDialog::getColor(currentColor, this, "Select Color");
    if (newColor.isValid()) {
        currentColor = newColor;
    }
}

void ImageEditor::onThicknessChanged(int thickness)
{
    currentThickness = thickness;
}

void ImageEditor::onBlurRadiusChanged(int radius)
{
    currentBlurRadius = radius;
}

void ImageEditor::onTextAdded(const QString &text)
{
    Q_UNUSED(text)
    // Handle text addition
}

void ImageEditor::onTextEditingFinished()
{
    if (!textLineEdit->text().isEmpty()) {
        // Add text to the image
        if (!currentImage.isNull()) {
            QPixmap result = currentImage;
            QPainter painter(&result);
            
            // Set text properties
            painter.setPen(QPen(currentColor, currentThickness));
            painter.setFont(QFont("Arial", 16));
            
            // Draw the text at the start point
            painter.drawText(startPoint, textLineEdit->text());
            
            currentImage = result;
            emit imageEdited(currentImage);
        }
        
        textLineEdit->hide();
        textLineEdit->clear();
    }
}