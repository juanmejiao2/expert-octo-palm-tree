#ifndef ENTIDAD_H
#define ENTIDAD_H

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
    void setX(float nuevaX) { x = nuevaX; }
float GETX() const{
    return x;
    }

float GETY() const{
    return y;
    }

};

#endif // ENTIDAD_H
