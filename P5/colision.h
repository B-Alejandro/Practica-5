#ifndef COLISION_H
#define COLISION_H

#include "particula.h"
#include "vector.h"
#include <vector>

/**
 * @brief Clase base abstracta para manejar colisiones entre partículas
 */
class Colision {
protected:
    int colisionesTotales;
    double energiaPerdida;

public:
    Colision();
    virtual ~Colision() = default;

    // Método virtual puro - debe ser implementado por clases derivadas
    virtual void resolverColision(Particula& p1, Particula& p2) = 0;

    // Métodos auxiliares comunes
    void separarParticulas(Particula& p1, Particula& p2);
    void detectarYResolverColisiones(std::vector<Particula*>& particulas);

    // Cálculos de conservación
    double calcularEnergiaCineticaTotal(const std::vector<Particula*>& particulas) const;
    Vector calcularMomentoTotal(const std::vector<Particula*>& particulas) const;

    // Getters y estadísticas
    int getColisionesTotales() const;
    double getEnergiaPerdida() const;
    void resetEstadisticas();
    virtual void mostrarEstadisticas() const;

protected:
    static constexpr double EPSILON = 1e-10;
};

/**
 * @brief Colisión perfectamente elástica (e = 1.0)
 * Conserva momento lineal y energía cinética
 */
class ColisionElastica : public Colision {
public:
    ColisionElastica();

    void resolverColision(Particula& p1, Particula& p2) override;
    void mostrarEstadisticas() const override;
};

/**
 * @brief Colisión inelástica con coeficiente de restitución 0 < e < 1
 * Conserva momento lineal pero pierde energía cinética
 */
class ColisionInelastica : public Colision {
private:
    double coeficienteRestitucion;

public:
    explicit ColisionInelastica(double coefRestitucion = 0.8);

    void resolverColision(Particula& p1, Particula& p2) override;
    void mostrarEstadisticas() const override;

    // Getters y setters específicos
    double getCoeficienteRestitucion() const;
    void setCoeficienteRestitucion(double coef);
};

/**
 * @brief Colisión completamente inelástica (e = 0)
 * Las partículas se fusionan en una sola
 */
class ColisionCompletamenteInelastica : public Colision {
private:
    int siguienteId;

public:
    explicit ColisionCompletamenteInelastica(int idInicial = 100);

    void resolverColision(Particula& p1, Particula& p2) override;
    Particula* fusionarParticulas(Particula& p1, Particula& p2);
    void mostrarEstadisticas() const override;

    void setSiguienteId(int id);

private:
    Vector calcularCentroMasa(const Particula& p1, const Particula& p2) const;
    double calcularNuevoRadio(const Particula& p1, const Particula& p2) const;
};

#endif // COLISION_H
