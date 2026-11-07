#ifndef ENTIDAD_H
#define ENTIDAD_H
#include <QRectF>

class entidad{
protected:
    float x, y;
    float vx, vy;
    short vida;

public:
    entidad(float posicionX, float posicionY, short hp);
    virtual ~entidad();
    virtual void mover(float dt);
    void recibirDanio(short cant);
    void setX(float nuevaX);
    virtual QRectF GETHitbox() const = 0;
    float GETX() const;
    float GETY() const;
    void setY(float nuevaY);
    void setVy(float nuevaVy);
    float GETVy() const;
};

#endif // ENTIDAD_H
