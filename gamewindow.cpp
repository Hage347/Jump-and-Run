#include "gamewindow.h"
#include <QPainter>
#include <QKeyEvent>
#include <QTimerEvent>
#include <QRandomGenerator>
#include "obstacle.h"
#include <QDebug>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent),
    viereckX(100), viereckY(500), viereckB(50), viereckH(50), isJumping(false), geschwindigkeitY(3), onGround(true), geschwindigkeitX(0),
    platform(1024, 300, 100, 20)  // Initialisiert die Plattform mit einer Breite von 100 und Höhe 20 an zufälliger Position
{
    setFixedSize(1024, 512);  // Setzt die Fenstergröße
    startTimer(9);  // wie schnell das Spiel ist

    // Initiales Hindernis
    obstacles.append(Obstacle(500, 512, 50, 50));  // Erstes Hindernis rechts außerhalb des Bildschirms

    // Initiale Plattform generieren
    generateNewPlatform();
}


GameWindow::~GameWindow() {}

void GameWindow::paintEvent(QPaintEvent *event)
{
    QPainter Farbe(this); // QPainter ist von Qt selbst

    // Zeichne das Viereck (Spieler) und die jeweilige Farbe
    Farbe.setBrush(Qt::green);
    Farbe.drawRect(viereckX, viereckY, viereckB, viereckH);

    // Zeichne die Hindernisse und die jeweilige Farbe
    Farbe.setBrush(Qt::red);
    for (const Obstacle &obstacle : obstacles) {
        Farbe.drawRect(obstacle.getRect());
    }

    // Zeichne die Plattform
    Farbe.setBrush(Qt::blue);  // Plattformen haben eine blaue Farbe
    Farbe.drawRect(platform.getRect());
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !isJumping && onGround) {
        // Nur springen, wenn das Viereck auf dem Boden ist oder auf der Plattform
        isJumping = true;
        geschwindigkeitY = -30;  // Anfangsgeschwindigkeit des Sprungs
        onGround = false;  // Spieler ist jetzt in der Luft
    }
    if (event->key() == Qt::Key_A) {
        geschwindigkeitX = -5; // Bewege nach links
    }
    if (event->key() == Qt::Key_D) {
        geschwindigkeitX = 5; // Bewege nach rechts
    }
}


void GameWindow::keyReleaseEvent(QKeyEvent *event)
{
    // Leertaste loslassen: Verhindern, dass das Viereck weiter nach oben schießt
    if (event->key() == Qt::Key_Space && geschwindigkeitY < 0) {
        geschwindigkeitY = 0;  // Stoppe das Springen, wenn die Taste losgelassen wird
    }
    if (event->key() == Qt::Key_A || event->key() == Qt::Key_D) {
        geschwindigkeitX = 0; // Stoppe Bewegung, wenn Taste losgelassen wird
    }
}

void GameWindow::timerEvent(QTimerEvent *event)
{
    // Schwerkraft anwenden (geschwindigkeitY wird in jedem Frame erhöht)
    if (!onGround) {
        geschwindigkeitY += 1;  // Schwerkraft anwenden (angepasst)
    }

    // Position des Vierecks aktualisieren
    viereckY += geschwindigkeitY;
    viereckX += geschwindigkeitX;

    // Begrenzung, damit das Rechteck nicht aus dem Fenster läuft
    if (viereckX < 0) viereckX = 0;
    if (viereckX > width() - viereckB) viereckX = width() - viereckB;

    // Wenn das Viereck den Boden erreicht hat
    if (viereckY >= height() - 50) {
        viereckY = height() - 50;  // Das Viereck darf nicht unter den Boden gehen
        onGround = true;  // Es steht jetzt auf dem Boden
        geschwindigkeitY = 0;  // Stoppe die Bewegung in Y-Richtung
        isJumping = false;  // Jetzt kann wieder gesprungen werden
    }

    // Bewege das Hindernis
    for (Obstacle &obstacle : obstacles) {
        obstacle.move();  // Bewege das Hindernis
        obstacle.reset(width(), height());  // Setze es neu, wenn es den Bildschirm verlässt
    }

    // Bewege die Plattform
    platform.move();  // Bewege die Plattform nach links
    if (platform.getRect().right() < 0) {
        // Wenn die Plattform den linken Rand erreicht hat, generiere eine neue Plattform
        generateNewPlatform();
    }

    QRect playerRect(viereckX, viereckY, viereckB, viereckH);

    // Überprüfen, ob der Spieler mit der Plattform kollidiert
    if (playerRect.intersects(platform.getRect())) {
        // Wenn der Spieler von oben auf die Plattform trifft, stoppen wir die Y-Bewegung
        if (viereckY + viereckH <= platform.getRect().top() + geschwindigkeitY) {
            onGround = true; // Spieler steht auf der Plattform
            geschwindigkeitY = 0;  // Stoppe die Bewegung nach unten
            viereckY = platform.getRect().top() - viereckH;  // Positioniere den Spieler genau auf der Plattform
        }
    } else {
        // Wenn der Spieler nicht mehr auf der Plattform ist, fällt er wieder
        onGround = false;
    }

    // Wenn der Spieler auf dem Boden oder einer Plattform ist, kann er springen
    if (onGround && !isJumping) {
        geschwindigkeitY = 0;  // Stoppe die Bewegung in Y-Richtung, wenn der Spieler auf dem Boden ist
    }

    update();  // Das Fenster neu zeichnen
}

void GameWindow::generateNewPlatform()
{
    // Generiere eine neue Plattform an einer zufälligen Position
    int randX = width();  // Neue Plattform startet am rechten Bildschirmrand
    int randY = QRandomGenerator::global()->bounded(150, 300); // Zufällige Höhe zwischen 100 und 300 (maximale Höhe ist 300)
    platform = Obstacle(randX, randY, 200, 20);  // Plattform mit 100px Breite und 20px Höhe
}
