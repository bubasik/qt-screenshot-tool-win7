#ifndef REGIONSELECTOR_H
#define REGIONSELECTOR_H

#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QRect>

class RegionSelector : public QWidget
{
    Q_OBJECT

public:
    explicit RegionSelector(QWidget *parent = nullptr);
    ~RegionSelector() override;

    void startSelection();
    QPixmap capturedPixmap() const { return capturedImage; }

signals:
    void selectionFinished(const QPixmap &pixmap);
    void selectionCancelled();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QRect selectionRect;
    QPoint startPos;
    QPoint currentPos;
    bool isSelecting;
    QPixmap fullScreenPixmap;
    QPixmap capturedImage;

    void drawSelectionArea(QPainter &painter);
    QRect normalizedRect() const;
};

#endif // REGIONSELECTOR_H
