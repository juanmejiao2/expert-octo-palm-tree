#include "camionesmad.h"
#include <algorithm>
#include <chrono>
#include <cmath>

CamionEsmad::CamionEsmad(int dificultad): enemigo(X_CAMION_BASE, Y_CAMION_BASE, true, TipoEnemigoArma::lanzagranadas, 100), rng(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())) {
    configurarDificultad(dificultad);
    fase2 = false;
    cambioDeFasePendiente = false;
    derrotaRegistrada = false;
    cooldownAtaque = 1.8f;
    cooldownSierras = 0.0f;
    sierrasPendientes = 0;
    timerSiguienteSierra = 0.0f;
    tiempoMotor = 0.0f;
    destelloImpacto = 0.0f;
    xJugador = yJugador = vxJugador = xAnterior = 0.0f;
    jugadorEnAire = false;
    primeraLectura = true;
    tiempoEnMitadIzq = 0.0f;
    tiempoEnMitadDer = 0.0f;
    tiempoFogonazo = 0.0f;
    timerCicloHumo = 0.0f;

    for (int i = 0; i < 3; ++i) {
        efectividad[i] = 0.5f;
        fallosConsecutivos[i] = 0;
        usos[i] = 0;
    }
    ultimoAtaque = -1;
    repeticiones = 0;
}

void CamionEsmad::configurarDificultad(int dificultad) {
    if (dificultad == 0) {
        tasaAprendizaje = 0.18f;
        exploracion = 0.30f;
        usaPrediccion = false;
        cadenciaBase = 2.5f;
        factorDanio = 0.55f;
        cantidadSierras = 2;
        cantidadGranadas = 1;
        balasRafaga = 3;
        vidaMaxima = 450;
    } else {
        tasaAprendizaje = 0.35f;
        exploracion = 0.12f;
        usaPrediccion = true;
        cadenciaBase = 2.65f;
        factorDanio = 0.55f;
        cantidadSierras = 2;
        cantidadGranadas = 2;
        balasRafaga = 3;
        vidaMaxima = 700;
    }
    vida = vidaMaxima;
}

float CamionEsmad::aleatorio() {
    return std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);
}

void CamionEsmad::percibir(const jugador &j, float dt) {
    QRectF h = j.GETHitbox();
    float xActual = static_cast<float>(h.center().x());

    if (primeraLectura) {
        xAnterior = xActual;
        primeraLectura = false;
    }
    float vxMedida = (xActual - xAnterior) / std::max(dt, 0.001f);
    vxJugador = vxJugador * 0.75f + vxMedida * 0.25f;
    xAnterior = xActual;

    xJugador = xActual;
    yJugador = static_cast<float>(h.center().y());
    jugadorEnAire = j.GETEnElAire();

    float mitadArena = (ARENA_MIN_X + ARENA_MAX_X) * 0.5f;
    tiempoEnMitadIzq *= 0.995f;
    tiempoEnMitadDer *= 0.995f;
    if (xJugador < mitadArena) {
        tiempoEnMitadIzq += dt;
    } else {
        tiempoEnMitadDer += dt;
    }
}

float CamionEsmad::puntoObjetivoX(float tiempoVuelo) const {
    if (!usaPrediccion) return xJugador;
    float predicho = xJugador + vxJugador * tiempoVuelo;
    return std::clamp(predicho, ARENA_MIN_X + 60.0f, X_CAMION_BASE - 60.0f);
}

AtaqueCamion CamionEsmad::razonar() {
    int elegido = 0;
    float puntuacion[3];

    for (int i = 0; i < 3; ++i) {
        float penalizacionFallos = fallosConsecutivos[i] * 0.22f;
        puntuacion[i] = efectividad[i] - penalizacionFallos;
    }

    if (!jugadorEnAire && std::abs(yJugador - (PISO_ARENA - 45.0f)) < 40.0f) {
        puntuacion[AtaqueSierras] += 0.12f;
    }

    if (aleatorio() < exploracion) {
        elegido = static_cast<int>(aleatorio() * 3.0f) % 3;
    } else {
        elegido = 0;
        for (int i = 1; i < 3; ++i) {
            if (puntuacion[i] > puntuacion[elegido]) {
                elegido = i;
            }
        }
    }

    if (elegido == ultimoAtaque && repeticiones >= 2) {
        elegido = (elegido + 1 + static_cast<int>(aleatorio() * 2.0f)) % 3;
    }

    repeticiones = (elegido == ultimoAtaque) ? (repeticiones + 1) : 1;
    ultimoAtaque = elegido;
    usos[elegido]++;
    return static_cast<AtaqueCamion>(elegido);
}

void CamionEsmad::actuar(AtaqueCamion ataque, QList<Proyectil*> &proyectiles) {
    int extra = fase2 ? 1 : 0;
    QPointF canon = GETCanonPos();
    float origenX = static_cast<float>(canon.x());
    float origenY = static_cast<float>(canon.y());

    if (ataque == AtaqueSierras) {
        sierrasPendientes = cantidadSierras + extra;
        timerSiguienteSierra = 0.0f;
    } else if (ataque == AtaqueGranadasCamion) {
        tiempoFogonazo = 0.12f;
        int n = cantidadGranadas + extra;
        float mitadArena = (ARENA_MIN_X + ARENA_MAX_X) * 0.5f;
        float zonaPreferida = (tiempoEnMitadIzq >= tiempoEnMitadDer) ? (ARENA_MIN_X + 260.0f): (mitadArena + 120.0f);

        for (int i = 0; i < n; ++i) {
            float objetivoX = (i == 0) ? puntoObjetivoX(0.9f) : (zonaPreferida + (aleatorio() - 0.5f) * 260.0f);
            objetivoX = std::clamp(objetivoX, ARENA_MIN_X + 60.0f, X_CAMION_BASE - 80.0f);

            float vy0 = -340.0f - aleatorio() * 110.0f;
            float a = 0.5f * 980.0f;
            float b = vy0;
            float c = origenY - PISO_ARENA;
            float discriminante = std::max(0.0f, b * b - 4.0f * a * c);
            float tVuelo = std::max(0.25f, (-b + std::sqrt(discriminante)) / (2.0f * a));
            float vx0 = (objetivoX - origenX) / tVuelo;

            Proyectil *granada = new Proyectil(origenX, origenY, vx0, vy0, false, TipoMovimientoProyectil::Parabolico);
            granada->setAtaqueOrigen(AtaqueGranadasCamion);
            proyectiles.append(granada);
        }
    } else {
        tiempoFogonazo = 0.12f;
        int n = balasRafaga + extra;
        for (int i = 0; i < n; ++i) {
            float objetivoX = puntoObjetivoX(0.35f + i * 0.08f);
            float objetivoY = yJugador + (i - n * 0.5f) * 24.0f;
            float dx = objetivoX - origenX;
            float dy = objetivoY - origenY;
            float dist = std::sqrt(dx * dx + dy * dy);
            float vel = fase2 ? 430.0f : 360.0f;

            float vxBala = (dist > 0.0f) ? (dx / dist) * vel : -vel;
            float vyBala = (dist > 0.0f) ? (dy / dist) * vel : 0.0f;

            Proyectil *bala = new Proyectil(origenX, origenY - i * 6.0f, vxBala, vyBala, false, TipoMovimientoProyectil::Rectilineo);
            bala->setAtaqueOrigen(AtaqueRafagaContencion);
            proyectiles.append(bala);
        }
    }
}

void CamionEsmad::aprender(int ataque, bool acerto) {
    if (ataque < 0 || ataque > 2) return;
    float resultado = acerto ? 1.0f : 0.0f;
    efectividad[ataque] += tasaAprendizaje * (resultado - efectividad[ataque]);

    if (acerto) {
        fallosConsecutivos[ataque] = 0;
    } else {
        fallosConsecutivos[ataque]++;
    }
}

void CamionEsmad::actualizar(float dt, const jugador &j, QList<Proyectil*> &proyectiles) {
    if (GETEstado() == EstadoEnemigo::destruido) return;

    percibir(j, dt);
    mover(dt);

    if (destelloImpacto > 0.0f) {
        destelloImpacto = std::max(0.0f, destelloImpacto - dt);
    }

    if (tiempoFogonazo > 0.0f) {
        tiempoFogonazo = std::max(0.0f, tiempoFogonazo - dt);
    }

    float periodoHumo = fase2 ? 3.0f : 4.5f;
    timerCicloHumo += dt;

    if (timerCicloHumo >= periodoHumo) {
        timerCicloHumo = std::fmod(timerCicloHumo, periodoHumo);
    }

    if (cooldownSierras > 0.0f) {
        cooldownSierras = std::max(0.0f, cooldownSierras - dt);
    }

    cooldownAtaque -= dt;
    if (cooldownAtaque <= 0.0f) {
        AtaqueCamion elegido = razonar();
        if (elegido == AtaqueSierras && cooldownSierras > 0.0f) {
            elegido = (ultimoAtaque == AtaqueGranadasCamion) ? AtaqueRafagaContencion : AtaqueGranadasCamion;
        }

        actuar(elegido, proyectiles);

        if (elegido == AtaqueSierras) {
            cooldownSierras = fase2 ? 4.2f : 5.5f;
        }

        float multFase = fase2 ? 0.72f : 1.0f;
        cooldownAtaque = cadenciaBase * multFase;
    }
    if (sierrasPendientes > 0) {
        timerSiguienteSierra -= dt;
        if (timerSiguienteSierra <= 0.0f) {
            QPointF canon = GETCanonPos();
            float origenX = static_cast<float>(canon.x());
            float ySuelo = PISO_ARENA - 36.0f;
            float vxSierra = fase2 ? -370.0f : -300.0f;

            Proyectil *sierra = new Proyectil(origenX, ySuelo, vxSierra, 0.0f, false, TipoMovimientoProyectil::Rectilineo);
            sierra->setAtaqueOrigen(AtaqueSierras);
            proyectiles.append(sierra);

            sierrasPendientes--;
            timerSiguienteSierra = fase2 ? 1.15f : 1.40f;
        }
    }
}

void CamionEsmad::mover(float dt) {
    if (GETEstado() == EstadoEnemigo::destruido) return;
    tiempoMotor += dt;

    float oscilacionX = std::sin(tiempoMotor * (fase2 ? 2.4f : 1.3f)) * (fase2 ? 28.0f : 15.0f);
    float nuevaX = std::clamp(X_CAMION_BASE + oscilacionX, 8350.0f, 9065.0f);
    x = nuevaX;
    y = Y_CAMION_BASE;
}

QRectF CamionEsmad::GETHitbox() const {
    return QRectF(x - 110.0f, y - 135.0f, 220.0f, 135.0f);
}

bool CamionEsmad::recibirDisparo(int danio) {
    recibirdano(danio);
    destelloImpacto = 0.12f;

    if (!fase2 && vida > 0 && vida <= vidaMaxima / 2) {
        fase2 = true;
        cambioDeFasePendiente = true;
    }

    if (GETEstado() == EstadoEnemigo::destruido) {
        derrotaRegistrada = true;
        return true;
    }
    return false;
}

bool CamionEsmad::consumirCambioDeFase() {
    bool habia = cambioDeFasePendiente;
    cambioDeFasePendiente = false;
    return habia;
}

bool CamionEsmad::enFase2() const {
    return fase2;
}

bool CamionEsmad::enPeligro() const {
    return derrotaRegistrada || GETEstado() == EstadoEnemigo::destruido;
}

short CamionEsmad::GETVidaMaxima() const {
    return vidaMaxima;
}

float CamionEsmad::GETDestello() const {
    return destelloImpacto;
}

int CamionEsmad::GETUltimoAtaque() const {
    return ultimoAtaque;
}

int CamionEsmad::escalarDanio(int danioBase) const {
    int calculado = static_cast<int>(std::lround(danioBase * factorDanio));
    return std::max(1, calculado);
}

QPointF CamionEsmad::GETCanonPos() const {
    return QPointF(x - 136.0f, y - 122.0f);
}

float CamionEsmad::GETTiempoFogonazo() const {
    return tiempoFogonazo;
}

bool CamionEsmad::estaSacandoHumo() const {
    float duracionHumo = fase2 ? 1.25f : 1.0f;
    return timerCicloHumo <= duracionHumo;
}

float CamionEsmad::GETProgresoHumo() const {
    float duracionHumo = fase2 ? 1.25f : 1.0f;
    if (duracionHumo <= 0.0f) return 0.0f;
    return std::clamp(timerCicloHumo / duracionHumo, 0.0f, 1.0f);
}