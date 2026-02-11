#ifndef THEMES_H
#define THEMES_H

#include <QString>

namespace Themes {

// Светлая тема (по умолчанию)
const QString Light = R"(
    QMainWindow {
        background-color: #F8F9FA;
        color: #212529;
    }
    QToolBar {
        background-color: #FFFFFF;
        border-bottom: 1px solid #DEE2E6;
        spacing: 8px;
        padding: 4px;
    }
    QPushButton {
        background-color: #E9ECEF;
        color: #212529;
        border: 1px solid #CED4DA;
        border-radius: 4px;
        padding: 6px 12px;
        min-width: 80px;
    }
    QPushButton:hover {
        background-color: #DEE2E6;
    }
    QPushButton:pressed {
        background-color: #CED4DA;
    }
    QPushButton#primary {
        background-color: #0D6EFD;
        color: white;
        border: 1px solid #0A58CA;
    }
    QPushButton#primary:hover {
        background-color: #0B5ED7;
    }
    QComboBox {
        background-color: white;
        border: 1px solid #CED4DA;
        border-radius: 4px;
        padding: 4px;
        min-width: 150px;
    }
    QLabel#preview {
        background-color: #E9ECEF;
        border: 2px dashed #ADB5BD;
        color: #6C757D;
    }
    QStatusBar {
        background-color: #FFFFFF;
        border-top: 1px solid #DEE2E6;
        color: #6C757D;
    }
)";

// Тёмная тема
const QString Dark = R"(
    QMainWindow {
        background-color: #2D2D30;
        color: #E0E0E0;
    }
    QToolBar {
        background-color: #1E1E1E;
        border-bottom: 1px solid #3E3E42;
        spacing: 8px;
        padding: 4px;
    }
    QPushButton {
        background-color: #3E3E42;
        color: #E0E0E0;
        border: 1px solid #515156;
        border-radius: 4px;
        padding: 6px 12px;
        min-width: 80px;
    }
    QPushButton:hover {
        background-color: #4A4A4F;
    }
    QPushButton:pressed {
        background-color: #515156;
    }
    QPushButton#primary {
        background-color: #007ACC;
        color: white;
        border: 1px solid #005A9E;
    }
    QPushButton#primary:hover {
        background-color: #0066B3;
    }
    QComboBox {
        background-color: #3E3E42;
        color: #E0E0E0;
        border: 1px solid #515156;
        border-radius: 4px;
        padding: 4px;
        min-width: 150px;
    }
    QComboBox QAbstractItemView {
        background-color: #2D2D30;
        color: #E0E0E0;
        selection-background-color: #007ACC;
    }
    QLabel#preview {
        background-color: #1E1E1E;
        border: 2px dashed #515156;
        color: #A0A0A0;
    }
    QStatusBar {
        background-color: #1E1E1E;
        border-top: 1px solid #3E3E42;
        color: #A0A0A0;
    }
)";

// Бирюзовая тема (teal/aqua)
const QString Teal = R"(
    QMainWindow {
        background-color: #E6F7F7;
        color: #004D4D;
    }
    QToolBar {
        background-color: #B2DFDB;
        border-bottom: 1px solid #80CBC4;
        spacing: 8px;
        padding: 4px;
    }
    QPushButton {
        background-color: #80CBC4;
        color: #004D4D;
        border: 1px solid #4DB6AC;
        border-radius: 4px;
        padding: 6px 12px;
        min-width: 80px;
    }
    QPushButton:hover {
        background-color: #4DB6AC;
        color: white;
    }
    QPushButton:pressed {
        background-color: #26A69A;
    }
    QPushButton#primary {
        background-color: #00897B;
        color: white;
        border: 1px solid #00695C;
    }
    QPushButton#primary:hover {
        background-color: #00796B;
    }
    QComboBox {
        background-color: #B2DFDB;
        color: #004D4D;
        border: 1px solid #4DB6AC;
        border-radius: 4px;
        padding: 4px;
        min-width: 150px;
    }
    QLabel#preview {
        background-color: #E0F2F1;
        border: 2px dashed #80CBC4;
        color: #00695C;
    }
    QStatusBar {
        background-color: #B2DFDB;
        border-top: 1px solid #80CBC4;
        color: #004D4D;
    }
)";

// Тема "Матрица" (зелёная на чёрном)
const QString Matrix = R"(
    QMainWindow {
        background-color: #000000;
        color: #00FF41; /* Неоново-зелёный */
        font-family: "Courier New", monospace;
    }
    QToolBar {
        background-color: #0A0A0A;
        border-bottom: 1px solid #003300;
        spacing: 8px;
        padding: 4px;
    }
    QPushButton {
        background-color: #001A00;
        color: #00FF41;
        border: 1px solid #004D00;
        border-radius: 4px;
        padding: 6px 12px;
        min-width: 80px;
        font-family: "Courier New", monospace;
        font-weight: bold;
    }
    QPushButton:hover {
        background-color: #003300;
        border: 1px solid #00FF41;
    }
    QPushButton:pressed {
        background-color: #004D00;
    }
    QPushButton#primary {
        background-color: #003300;
        color: #00FF41;
        border: 2px solid #00FF41;
        font-weight: bold;
    }
    QPushButton#primary:hover {
        background-color: #004D00;
        box-shadow: 0 0 8px #00FF41;
    }
    QComboBox {
        background-color: #001A00;
        color: #00FF41;
        border: 1px solid #004D00;
        border-radius: 4px;
        padding: 4px;
        min-width: 150px;
        font-family: "Courier New", monospace;
    }
    QComboBox QAbstractItemView {
        background-color: #000A00;
        color: #00FF41;
        selection-background-color: #003300;
        border: 1px solid #004D00;
    }
    QLabel#preview {
        background-color: #000A00;
        border: 2px dashed #004D00;
        color: #00AA00;
        font-family: "Courier New", monospace;
    }
    QStatusBar {
        background-color: #000A00;
        border-top: 1px solid #003300;
        color: #00AA00;
        font-family: "Courier New", monospace;
    }
    /* Эффект "падающих символов" для статусбара (упрощённо) */
    QStatusBar::item {
        animation: matrix-rain 10s linear infinite;
    }
    @keyframes matrix-rain {
        0% { opacity: 0.3; }
        50% { opacity: 1.0; }
        100% { opacity: 0.3; }
    }
)";

} // namespace Themes

#endif // THEMES_H
