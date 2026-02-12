#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QRect>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QLineEdit>
#include <QTextEdit>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsBlurEffect>
#include <QColorDialog>
#include <QSpinBox>
#include <QSlider>

enum class EditTool {
    Select,
    Crop,
    Blur,
    Arrow,
    Text
};

class ImageEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ImageEditor(QWidget *parent = nullptr);
    ~ImageEditor() override;

    void setImage(const QPixmap &image);
    QPixmap getImage() const;

signals:
    void imageEdited(const QPixmap &editedImage);

private slots:
    void onToolSelected(EditTool tool);
    void onColorChanged();
    void onThicknessChanged(int thickness);
    void onBlurRadiusChanged(int radius);
    void onTextAdded(const QString &text);
    void onTextEditingFinished();

private:
    void setupUI();
    void setupConnections();
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void drawCropHandles(QPainter &painter);
    void drawBlurOverlay(QPainter &painter);
    void drawArrow(QPainter &painter);
    void drawText(QPainter &painter);
    void applyCrop();
    void applyBlur();
    void applyArrow();
    void applyText();
    QRect getNormalizedRect(const QPoint &p1, const QPoint &p2) const;
    void updatePreview();
    void blurImage(QImage &image, int radius);

    QPixmap originalImage;
    QPixmap currentImage;
    QPixmap previewImage;
    
    EditTool currentTool;
    QPoint startPoint;
    QPoint endPoint;
    bool isDrawing;
    
    // Editing properties
    QColor currentColor;
    int currentThickness;
    int currentBlurRadius;
    
    // UI elements
    QWidget *toolbar;
    QLineEdit *textLineEdit;
    QColorDialog *colorDialog;
    
    // Crop handles
    QRect topLeftHandle;
    QRect topRightHandle;
    QRect bottomLeftHandle;
    QRect bottomRightHandle;
    int handleSize;
    QRect activeCropRect;
    bool isDraggingHandle;
    int dragHandleIndex; // 0=top-left, 1=top-right, 2=bottom-left, 3=bottom-right
    
    // Text editing
    QGraphicsScene *scene;
    QGraphicsView *view;
    QGraphicsTextItem *textItem;
    bool isTextEditing;
};

#endif // IMAGEEDITOR_H