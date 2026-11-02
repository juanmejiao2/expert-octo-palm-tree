#ifndef ARMA_H
#define ARMA_H

class arma{
private:
    short tipo;
    short dano;
    float VelociadInicial;
    int CadenciaDisparos;

public:
    arma(short TipoArma, short DanoBase, float Velocidad, int Cadencia);
    short GETDano() const;
    float GETVelocidadInicial() const;

};

#endif // ARMA_H
