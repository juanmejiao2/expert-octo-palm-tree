#ifndef BALA_H
#define BALA_H
#include "entidad.h"

class bala : public entidad{
private:
    float velocidadX;
    bool delJugador;

public:
    bala(float x, float y, float vx, bool esDelJugador);
    void mover(float dt) override;
    bool GETDelJugador() const;
};

#endif // BALA_H
