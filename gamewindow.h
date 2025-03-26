#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QKeyEvent>
#include <QTimerEvent>
#include <QList>
#include "obstacle.h"  // Füge die Hindernis-Klasse hinzu

class GameWindow : public QWidget
{
    Q_OBJECT

public:
    GameWindow(QWidget *parent = nullptr); // repräsentiert einen Nullzeiger, die normale "0" kann zu Problemen führen
    ~GameWindow();// Destruktor

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

private:
    void generateNewPlatform();  // Funktion, um eine neue Plattform zu generieren

    int viereckX;              // X-Position des Vierecks
    int viereckY;              // Y-Position des Vierecks
    int viereckH;              // Höhe des Vierecks
    int viereckB;              // Breite des Vierecks
    bool isJumping;            // Überprüft, ob das Viereck springt
    int geschwindigkeitY;      // Geschwindigkeit in Y-Richtung (für Schwerkraft)
    int geschwindigkeitX;      // Geschwindigkeit in X-Richtung (seitliche Bewegung)
    bool onGround;             // Überprüft, ob das Viereck auf dem Boden ist
    QList<Obstacle> obstacles; // Liste der Hindernisse, jetzt Obstacle-Objekte
    Obstacle platform;         // Eine einzelne Plattform
};

#endif // GAMEWINDOW_H
