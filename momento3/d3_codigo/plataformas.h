#include "entidad.h"

class plataformas : public entidad {
private:
    float ancho;
    float alto;

public:
    plataformas(float x, float y, float w, float h);
    QRectF GETHitbox() const override;
};