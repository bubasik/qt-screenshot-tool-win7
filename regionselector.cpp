#include "regionselector.h"
#include <QPainter>
#include <QScreen>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>

RegionSelector::RegionSelector(QWidget *parent)
    : QWidget(parent),
      isSelecting(false)
{
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setCursor(Qt::CrossCursor);

    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        fullScreenPixmap = screen->grabWindow(0);
        resize(fullScreenPixmap.size());
    }
}

RegionSelector::~RegionSelector()
{
}

void RegionSelector::startSelection()
{
    showFullScreen();
    startPos = QPoint();
    currentPos = QPoint();
    isSelecting = false;
    update();
}

void RegionSelector::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPixmap(0, 0, fullScreenPixmap);

    if (isSelecting) {
        painter.fillRect(rect(), QColor(0, 0, 0, 120));
        
        QRect r = normalizedRect();
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.fillRect(r, Qt::transparent);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

        QPen pen(QColor(0, 162, 232), 2);
        pen.setDashPattern({5, 5});
        painter.setPen(pen);
        painter.drawRect(r.adjusted(0, 0, -1, -1));

        QString sizeInfo = QString("%1 x %2").arg(r.width()).arg(r.height());
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(r.topLeft() + QPoint(10, 20), sizeInfo);
    }
}

void RegionSelector::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        startPos = event->pos();
        currentPos = startPos;
        isSelecting = true;
        update();
    } else if (event->button() == Qt::RightButton) {
        close();
        emit selectionCancelled();
    }
}

void RegionSelector::mouseMoveEvent(QMouseEvent *event)
{
    if (isSelecting && event->buttons() & Qt::LeftButton) {
        currentPos = event->pos();
        update();
    }
}

void RegionSelector::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isSelecting) {
        QRect finalRect = normalizedRect();
        if (finalRect.width() >= 10 && finalRect.height() >= 10) {
            capturedImage = fullScreenPixmap.copy(finalRect);
            emit selectionFinished(capturedImage);
        } else {
            emit selectionCancelled();
        }
        close();
    }
}

void RegionSelector::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
        emit selectionCancelled();
        event->accept();
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (isSelecting) {
            QRect finalRect = normalizedRect();
            if (finalRect.width() >= 10 && finalRect.height() >= 10) {
                capturedImage = fullScreenPixmap.copy(finalRect);
                emit selectionFinished(capturedImage);
            } else {
                emit selectionCancelled();
            }
            close();
        }
        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
}

QRect RegionSelector::normalizedRect() const
{
    int x1 = qMin(startPos.x(), currentPos.x());
    int y1 = qMin(startPos.y(), currentPos.y());
    int x2 = qMax(startPos.x(), currentPos.x());
    int y2 = qMax(startPos.y(), currentPos.y());
    return QRect(x1, y1, x2 - x1, y2 - y1);
}
