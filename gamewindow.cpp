#include "gamewindow.h"
#include <QPainter>
#include <QKeyEvent>
#include <QTimerEvent>
#include "obstacle.h"
#include <QDebug>
#include <QRect>
#include <QPixmap>  // Für QPixmap zum Laden des Bildes

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent),
    viereckX(100), viereckY(500), viereckB(50), viereckH(50), isJumping(false), geschwindigkeitY(3), onGround(true), geschwindigkeitX(0), gamePaused(false),
    plattform{300, 350, 200, 20}
{
    setFixedSize(1024, 512);        // Setzt die Fenstergröße
    startTimer(15);                  // wie schnell das Spiel ist

    // Initiales Hindernis
    obstacles.append(Obstacle(500, 512, 50, 50));
}

GameWindow::~GameWindow() {}

void GameWindow::paintEvent(QPaintEvent *event)
{
    QPainter Farbe(this);


    // Hintergrundbild laden und zeichnen
    QPixmap background(":/graphics/Background/Background_v2-0.png"); // Hier wird das Bild aus Ressourcen geladen
    Farbe.drawPixmap(0, 0, width(), height(), background); // Das Bild wird auf die ganze Fenstergröße skaliert


    // Zeichne den Spieler (grünes Rechteck)
    Farbe.setBrush(Qt::green);
    Farbe.drawRect(viereckX, viereckY, viereckB, viereckH);

    // Zeichne Hindernisse (rote Rechtecke)
    Farbe.setBrush(Qt::red);
    for (const Obstacle &obstacle : obstacles) {
        Farbe.drawRect(obstacle.getRect());
    }

    if (gamePaused) {
        // Setzt eine transparente Farbe für das Overlay
        Farbe.setBrush(QColor(0, 0, 0, 100));  // Transparente schwarze Farbe für das Overlay
        Farbe.drawRect(0, 0, width(), height()); // Über das gesamte Fenster

        // Setzt die Textfarbe auf weiß und zeigt die Pause-Nachricht an
        Farbe.setPen(Qt::white);
        Farbe.setFont(QFont("Arial", 20));
        Farbe.drawText(width() / 2 - 100, height() / 2, "Spiel Pausiert! Drücke 'R' zum Fortsetzen.");
    }
    Farbe.setBrush(Qt::blue);
    Farbe.drawRect(plattform);
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    if (gamePaused) {
        // Wenn das Spiel pausiert ist, drücke 'R', um fortzufahren
        if (event->key() == Qt::Key_R) {
            gamePaused = false;

            // Nach dem Fortsetzen, zurücksetzen der Schwerkraft und der Boden-Erkennung
            geschwindigkeitY = 3;
            onGround = true;

            update(); // Das Spiel wieder aktualisieren
        }
        return;
    }

    if (event->key() == Qt::Key_Space && !isJumping && onGround) {
        isJumping = true;
        geschwindigkeitY = -30;
        onGround = false;
    }

    if (event->key() == Qt::Key_A) {
        geschwindigkeitX = -5;
    }

    if (event->key() == Qt::Key_D) {
        geschwindigkeitX = 5;
    }
}

void GameWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && geschwindigkeitY < 0) {
        geschwindigkeitY = 0;
    }

    if (event->key() == Qt::Key_A || event->key() == Qt::Key_D) {
        geschwindigkeitX = 0;
    }
}

void GameWindow::timerEvent(QTimerEvent *event)
{
    if (gamePaused) {
        return; // Wenn das Spiel pausiert ist, keine Updates durchführen
    }

    if (!onGround) {
        geschwindigkeitY += 1; // Schwerkraft anwenden (angepasst)
    }

    viereckY += geschwindigkeitY;
    viereckX += geschwindigkeitX;

    // Grenzwerte für den Spieler
    if (viereckX < 0) viereckX = 0;
    if (viereckX > width() - viereckB) viereckX = width() - viereckB;

    QRect playerRect(viereckX, viereckY, viereckB, viereckH);

    // Überprüfen, ob der Spieler den Boden berührt
    if (viereckY >= height() - 50) {
        viereckY = height() - 50;       // Das Viereck darf nicht unter den Boden gehen
        onGround = true;                // Spieler steht auf dem Boden
        geschwindigkeitY = 0;           // Stoppe die Bewegung in Y-Richtung
        isJumping = false;              // Jetzt kann wieder gesprungen werden
    }

    // Überprüfen, ob der Spieler auf der Plattform steht
    bool playerOnPlatform =
        playerRect.bottom() >= plattform.y() &&
        playerRect.bottom() <= plattform.y() + 5 &&
        playerRect.right() >= plattform.left() &&
        playerRect.left() <= plattform.right();

    if (playerOnPlatform) {
        viereckY = plattform.y() - viereckH; // Spieler landet auf der Plattform
        onGround = true;                     // Spieler steht auf der Plattform
        geschwindigkeitY = 0;
        isJumping = false;
    }
    // Spieler verlässt die Plattform -> soll wieder fallen
    else if (onGround && viereckY < height() - 50 && !playerOnPlatform) {
        onGround = false;
    }

    // Hindernisse bewegen
    for (Obstacle &obstacle : obstacles) {
        obstacle.move();                         // Bewege das Hindernis
        obstacle.reset(width(), height());       // Setze es neu, wenn es den Bildschirm verlässt
    }

    // Überprüfe Kollisionen mit Hindernissen
    for (const Obstacle &obstacle : obstacles) {
        if (checkCollisionPixelBased(playerRect, obstacle)) {
            qDebug() << "💥 Kollision erkannt! Spieler bei:" << viereckX << viereckY
                     << "| Hindernis bei:" << obstacle.getRect();
            gamePaused = true;  // Spiel pausieren
            handleCollision(playerRect, obstacle); // Kollision behandeln
        }
    }

    update();
}

bool GameWindow::checkCollisionPixelBased(const QRect &playerRect, const Obstacle &obstacle)
{
    QRect obstacleRect = obstacle.getRect();

    // Erst eine grobe Rechteck-Kollisionserkennung
    if (!playerRect.intersects(obstacleRect)) {
        return false;  // Keine Kollision, also keine weitere Prüfung nötig
    }

    // Berechnung der überlappenden Region
    int startX = std::max(playerRect.left(), obstacleRect.left());
    int startY = std::max(playerRect.top(), obstacleRect.top());
    int endX = std::min(playerRect.right(), obstacleRect.right());
    int endY = std::min(playerRect.bottom(), obstacleRect.bottom());

    // Überprüfung nur in der überlappenden Region
    for (int x = startX; x <= endX; ++x) {
        for (int y = startY; y <= endY; ++y) {
            // Spieler Rechteck und Hindernis Rechteck im Bereich des Überlappings überprüfen
            if (playerRect.contains(x, y) && obstacleRect.contains(x, y)) {
                qDebug() << "🎯 Pixel-Kollision erkannt bei" << x << y;
                return true;  // Kollision erkannt
            }
        }
    }

    return false;  // Keine Kollision
}

void GameWindow::handleCollision(const QRect &playerRect, const Obstacle &obstacle)
{
    // Beispielhafte Korrektur: Spieler nach der Kollision an die Kante des Hindernisses setzen
    QRect obstacleRect = obstacle.getRect();

    // Wenn der Spieler das Hindernis von unten trifft, setze den Spieler auf die Oberkante des Hindernisses
    if (playerRect.bottom() > obstacleRect.top() && playerRect.top() < obstacleRect.top()) {
        viereckY = obstacleRect.top() - viereckH;
        geschwindigkeitY = 0;
        onGround = true;
    }

    // Wenn der Spieler das Hindernis von der Seite trifft, setze die horizontale Position zurück
    if (playerRect.right() > obstacleRect.left() && playerRect.left() < obstacleRect.right()) {
        if (geschwindigkeitX > 0) {  // Rechts treffen
            viereckX = obstacleRect.left() - viereckB;
        } else if (geschwindigkeitX < 0) {  // Links treffen
            viereckX = obstacleRect.right();
        }
    }
}
