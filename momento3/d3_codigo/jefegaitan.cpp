#include "jefegaitan.h"
#include <cmath>
#include <chrono>

constexpr float JefeGaitan::ZONAS_X[3];

JefeGaitan::JefeGaitan(int dificultad)
    : enemigo(ZONAS_X[1], ALTURA_VUELO, true, TipoEnemigoArma::conservador, 100),
    rng(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())) {
    configurarDificultad(dificultad);
    fase2 = false;
    cambioDeFasePendiente = false;
    cooldownAtaque = 2.0f;
    tiempoFlotar = 0.0f;
    yAncla = ALTURA_VUELO;
    zonaActual = 1;
    ataquesDesdeTeleport = 0;
    destello = 0.0f;
    tiempoDolor = 0.0f;
    tiempoGrito = 0.0f;
    faseLaser = 0;
    tiempoLaser = 0.0f;
    anguloLaser = 1.57f;
    laserGolpeo = false;
    acumuladoLaser = 0.0f;
    xJugador = yJugador = vxJugador = xAnterior = 0.0f;
    jugadorEnAire = false;
    primeraLectura = true;
    for (int i = 0; i < 3; i++) {
        tiempoEnZona[i] = 0.0f;
        efectividad[i] = 0.5f;
        usos[i] = 0;
    }
    ultimoAtaque = -1;
    repeticiones = 0;
}

void JefeGaitan::configurarDificultad(int dificultad) {
    if (dificultad == 0) {
        tasaAprendizaje = 0.15f;
        exploracion = 0.40f;
        usaPrediccion = false;
        cadenciaBase = 3.4f;
        giroLaser = 0.3f;
        duracionLaser = 1.4f;
        factorDanio = 0.6f;
        cantidadPendulos = 2;
        cantidadLluvia = 5;
        vidaMaxima = 400;
    } else {
        tasaAprendizaje = 0.30f;
        exploracion = 0.20f;
        usaPrediccion = true;
        cadenciaBase = 2.4f;
        giroLaser = 0.6f;
        duracionLaser = 1.8f;
        factorDanio = 0.9f;
        cantidadPendulos = 3;
        cantidadLluvia = 6;
        vidaMaxima = 600;
    }
    vida = vidaMaxima;
}

float JefeGaitan::aleatorio() {
    return std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);
}

int JefeGaitan::zonaDe(float xMundo) const {
    if (xMundo < (ZONAS_X[0] + ZONAS_X[1]) / 2.0f) return 0;
    if (xMundo < (ZONAS_X[1] + ZONAS_X[2]) / 2.0f) return 1;
    return 2;
}

// ---------------------------------------------------------------- 1. PERCEPCIÓN
void JefeGaitan::percibir(const jugador &j, float dt) {
    QRectF h = j.GETHitbox();
    float xActual = static_cast<float>(h.center().x());
    if (primeraLectura) { xAnterior = xActual; primeraLectura = false; }
    float vxMedida = (xActual - xAnterior) / dt;
    vxJugador = vxJugador * 0.8f + vxMedida * 0.2f;
    xAnterior = xActual;
    xJugador = xActual;
    yJugador = static_cast<float>(h.center().y());
    jugadorEnAire = j.GETEnElAire();
    for (int i = 0; i < 3; i++) tiempoEnZona[i] *= 0.995f;
    tiempoEnZona[zonaDe(xJugador)] += dt;
}

float JefeGaitan::puntoObjetivoX(float tiempoVuelo) const {
    if (!usaPrediccion) return xJugador;
    return xJugador + vxJugador * tiempoVuelo;
}

// ---------------------------------------------------------------- 2. RAZONAMIENTO
AtaqueGaitan JefeGaitan::razonar() {
    int elegido;
    if (aleatorio() < exploracion) {
        elegido = static_cast<int>(aleatorio() * 3.0f) % 3;     // explorar: probar cualquiera
    } else {
        elegido = 0;                                            // explotar: el más efectivo
        for (int i = 1; i < 3; i++) {
            if (efectividad[i] > efectividad[elegido]) elegido = i;
        }
    }
    if (elegido == ultimoAtaque && repeticiones >= 2) {
        elegido = (elegido + 1 + static_cast<int>(aleatorio() * 2.0f)) % 3;
    }
    repeticiones = (elegido == ultimoAtaque) ? repeticiones + 1 : 1;
    ultimoAtaque = elegido;
    usos[elegido]++;
    return static_cast<AtaqueGaitan>(elegido);
}

// ---------------------------------------------------------------- 3. ACCIÓN
void JefeGaitan::actuar(AtaqueGaitan ataque, QList<Proyectil*> &proyectiles) {
    int extra = fase2 ? 1 : 0;
        if (ataque != AtaqueLaser)
        tiempoGrito = 0.45f;
    if (ataque == AtaqueLaser) {
        faseLaser = 1;
        tiempoLaser = 0.9f;
        laserGolpeo = false;
        acumuladoLaser = 0.0f;
        QPointF ojos = GETOjos();
        anguloLaser = std::atan2(yJugador - static_cast<float>(ojos.y()), puntoObjetivoX(0.4f) - static_cast<float>(ojos.x()));
    } else if (ataque == AtaquePendulo) {
        int n = cantidadPendulos + extra;
        float centro = puntoObjetivoX(1.5f);
        for (int i = 0; i < n; i++) {
            float pivoteX = centro + (i - (n - 1) / 2.0f) * 170.0f;
            if (pivoteX < 280.0f) pivoteX = 280.0f;
            if (pivoteX > 1000.0f) pivoteX = 1000.0f;
            float angulo = (i % 2 == 0) ? 1.1f : -1.1f;
            Proyectil *p = new Proyectil(pivoteX, -2920.0f, 0.0f, 55.0f, false, TipoMovimientoProyectil::Pendulo);
            p->configurarPendulo(pivoteX, -2920.0f, 300.0f, angulo);
            p->setAtaqueOrigen(AtaquePendulo);
            proyectiles.append(p);
        }
    } else {
        int n = cantidadLluvia + 2 * extra;
        int zonaFavorita = 0;
        for (int i = 1; i < 3; i++) if (tiempoEnZona[i] > tiempoEnZona[zonaFavorita]) zonaFavorita = i;
        float x0 = x, y0 = y;
        for (int i = 0; i < n; i++) {
            float objetivo = (i % 2 == 0) ? puntoObjetivoX(1.0f) : ZONAS_X[zonaFavorita];
            objetivo += (aleatorio() - 0.5f) * 240.0f;
            float vy0 = -300.0f - aleatorio() * 120.0f;

            float a = 0.5f * 980.0f, b = vy0, c = y0 - PISO_ARENA;
            float t = (-b + std::sqrt(b * b - 4.0f * a * c)) / (2.0f * a);
            Proyectil *p = new Proyectil(x0, y0, (objetivo - x0) / t, vy0, false, TipoMovimientoProyectil::Parabolico);
            p->setAtaqueOrigen(AtaqueLluvia);
            proyectiles.append(p);
        }
    }
}

// ---------------------------------------------------------------- 4. APRENDIZAJE
void JefeGaitan::aprender(int ataque, bool acerto) {
    if (ataque < 0 || ataque > 2) return;
    float resultado = acerto ? 1.0f : 0.0f;
    efectividad[ataque] += tasaAprendizaje * (resultado - efectividad[ataque]);
}

void JefeGaitan::actualizar(float dt, const jugador &j, QList<Proyectil*> &proyectiles) {
    if (GETEstado() == EstadoEnemigo::destruido) return;
    percibir(j, dt);
    mover(dt);
    if (destello > 0.0f) destello -= dt;
    if (tiempoDolor > 0.0f) tiempoDolor -= dt;
    if (tiempoGrito > 0.0f) tiempoGrito -= dt;

    if (faseLaser != 0) {
        QPointF ojos = GETOjos();
        float objetivo = std::atan2(yJugador - static_cast<float>(ojos.y()), puntoObjetivoX(0.4f) - static_cast<float>(ojos.x()));
        float diferencia = std::remainder(objetivo - anguloLaser, 6.2831853f);
        float paso = (faseLaser == 2) ? giroLaser * dt : 0.0f;
        if (diferencia > paso) diferencia = paso;
        if (diferencia < -paso) diferencia = -paso;
        anguloLaser += diferencia;
        tiempoLaser -= dt;
        if (tiempoLaser <= 0.0f) {
            if (faseLaser == 1) {
                faseLaser = 2;
                tiempoLaser = duracionLaser + (fase2 ? 0.5f : 0.0f);
            } else {
                faseLaser = 0;
                aprender(AtaqueLaser, laserGolpeo);
            }
        }
        return;
    }

    cooldownAtaque -= dt;
    if (cooldownAtaque <= 0.0f) {
        actuar(razonar(), proyectiles);
        ataquesDesdeTeleport++;
        cooldownAtaque = cadenciaBase * (fase2 ? 0.8f : 1.0f);
        if (ataquesDesdeTeleport >= 2 && faseLaser == 0) teletransportar();
    }
}

void JefeGaitan::teletransportar() {
    int nueva = (zonaActual + 1 + static_cast<int>(aleatorio() * 2.0f)) % 3;
    if (nueva == zonaDe(xJugador)) nueva = (nueva + 1) % 3;
    zonaActual = nueva;
    x = ZONAS_X[zonaActual];
    ataquesDesdeTeleport = 0;
    destello = 0.3f;
}

void JefeGaitan::mover(float dt) {
    tiempoFlotar += dt;
    y = yAncla + std::sin(tiempoFlotar * 2.0f) * 12.0f;
}

QRectF JefeGaitan::GETHitbox() const {
    return QRectF(x - 60.0f, y - 55.0f, 120.0f, 115.0f);
}

bool JefeGaitan::recibirDisparo(int danio) {
    recibirdano(danio);
        tiempoDolor = 0.36f;
    if (!fase2 && vida <= vidaMaxima / 2) {
        fase2 = true;
        cambioDeFasePendiente = true;
    }
    if (GETEstado() == EstadoEnemigo::destruido) {
        faseLaser = 0;
        return true;
    }
    return false;
}

bool JefeGaitan::consumirCambioDeFase() {
    bool habia = cambioDeFasePendiente;
    cambioDeFasePendiente = false;
    return habia;
}

bool JefeGaitan::enFase2() const { return fase2; }
short JefeGaitan::GETVidaMaxima() const { return vidaMaxima; }
float JefeGaitan::GETDestello() const { return destello; }
int JefeGaitan::escalarDanio(int danioBase) const { return static_cast<int>(std::lround(danioBase * factorDanio)); }
int JefeGaitan::GETFaseLaser() const { return faseLaser; }

bool JefeGaitan::miraIzquierda() const { return xJugador < x; }
float JefeGaitan::GETDolor() const { return tiempoDolor; }
float JefeGaitan::GETGrito() const { return tiempoGrito; }

QPointF JefeGaitan::GETOjos() const {
    return QPointF(x + (miraIzquierda() ? -24.0f : 24.0f), y - 12.0f);
}

QPointF JefeGaitan::GETFinLaser() const {
    QPointF o = GETOjos();
    return QPointF(o.x() + std::cos(anguloLaser) * 1600.0f, o.y() + std::sin(anguloLaser) * 1600.0f);
}

// ¿El segmento ojos->fin del rayo cruza el rectángulo? (recorte de Liang-Barsky, sin raíces ni trigonometría)
static bool segmentoTocaRect(float x0, float y0, float x1, float y1, const QRectF &r) {
    float dx = x1 - x0, dy = y1 - y0;
    float p[4] = {-dx, dx, -dy, dy};
    float q[4] = {x0 - static_cast<float>(r.left()), static_cast<float>(r.right()) - x0,
                  y0 - static_cast<float>(r.top()), static_cast<float>(r.bottom()) - y0};
    float t0 = 0.0f, t1 = 1.0f;
    for (int i = 0; i < 4; i++) {
        if (p[i] == 0.0f) {
            if (q[i] < 0.0f) return false;
        } else {
            float t = q[i] / p[i];
            if (p[i] < 0.0f) { if (t > t1) return false; if (t > t0) t0 = t; }
            else             { if (t < t0) return false; if (t < t1) t1 = t; }
        }
    }
    return true;
}

int JefeGaitan::aplicarLaser(const QRectF &hitboxJugador, float dt) {
    if (faseLaser != 2) return 0;
    QPointF o = GETOjos(), f = GETFinLaser();
    if (!segmentoTocaRect(static_cast<float>(o.x()), static_cast<float>(o.y()), static_cast<float>(f.x()), static_cast<float>(f.y()), hitboxJugador)) {
        return 0;
    }
    laserGolpeo = true;
    acumuladoLaser += dt;
    if (acumuladoLaser >= 0.3f) {
        acumuladoLaser -= 0.3f;
        return escalarDanio(6);
    }
    return 0;
}