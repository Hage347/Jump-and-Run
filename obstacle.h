#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QRect>

//QRect ist etwas von Qt selbst

class Obstacle
{
public:
    Obstacle(int x, int y, int width, int height);
    void move();  // Bewegt das Hindernis
    void reset(int screenWidth, int screenHeight);  // Setzt das Hindernis neu, wenn es den Bildschirm verlässt
    QRect getRect() const;  // Gibt das Hindernis als QRect zurück
    void movePlatform();  // Bewegt die Plattform von rechts nach links

private:
    QRect rect;  // Das Hindernis als Rechteck
    int speed;   // Geschwindigkeit des Hindernisses
};

#endif // OBSTACLE_H
