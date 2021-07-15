#ifndef _Trafico_h
#define _Trafico_h 1

#include <iostream>
using namespace std;

#include "Comunicacion.hh"
#include "Enlace.hh"
#include "RNG.h"

#include "Uniform.h"
#ifndef modeMM
#include "NegExp.h"
#include <queue>
using namespace std;
#ifndef modeGG
#include "Salida.hh"
#endif
#endif

// #include "Promedio.hh"

#include "DpdentSmplStat.hh"

#ifdef modeMM
class Trafico;
#endif

#if !(defined modeM1)
class Reintentos
{
  Uniform probabilidad;
  double persistencia;
#ifndef modeMM
  Random *Retardo;
#else
  double Retardo;
#endif
public:
#ifndef modeMM
  Reintentos(Random *retardo, double persist, RNG *rng, unsigned long min_sam);
#else
  Reintentos(double retardo, double persist, RNG *rng, unsigned long min_sam);
  double valor(void);
#endif
  double operator()();
};
#endif

#ifdef modeMM
class PTree
{
  PTree *up, *left, *right;
  double amount;
  Trafico *source;
  Tipo tipo;

public:
  PTree(Trafico *trafico, Tipo tipot);
  PTree(PTree *son, PTree *newson);
  double initiate(void);
  void changed(double amountt);
  void next(double ratio);
};
#endif

class Trafico
{
#ifndef modeMM
  static double reloj;
#endif
  static bool best;
#ifndef modeM1
  static Reintentos *reintentos;
  //  Promedio *p_bl_primer;
  //  Promedio *p_bl_reint;
  DependentSampleStatistic *p_bl_primer;
  DependentSampleStatistic *p_bl_reint;
#endif

#if defined modeM1 || defined modeMM
  double llegadas;
  static void run();
  static RNG *rng;
#ifdef modeMM
  static PTree *root;
  PTree *depart;
  PTree *nodo_reintentos;
#else
  long d_prob_acum;
  static NegativeExpntl *Llegada;
  static double next_arrive;
#endif
#else
  static priority_queue<Comunicacion> eventos;
  Random *Llegada;
  bool finite_users;
#endif

  //  Promedio p_bloqueo;
  DependentSampleStatistic *p_bloqueo;
  static double Transitorio;
  static unsigned int flag_trans;
  static unsigned int flag_first1;
  float calidad;
  float tolerancia;

  static Trafico *primero;
  Trafico *siguiente;
  Ruta **rutas;
  unsigned num_rutas;
#if (defined modeG1) || (defined modeM1)
  static double Demanda;
  static queue<Salida> salidas;
  unsigned max_conns;
#else
#ifdef modeMM1
  static double Demanda = -1.0;
#else
#ifdef modeGG
  Random *Demanda;
#else
  double Demanda;
#endif
#endif
#ifdef modeMM
  double mu;
  unsigned num_rein;
  unsigned conns;
#else
#ifndef modeGG
  queue<Salida> salidas;
#endif
#endif
#endif

public:
#if (defined modeM1) || (defined modeMM)
  static void InicEventos(double transitorio, RNG *rngg);
#if (defined modeMM)
  static void InicReintentos(double retardo, double persist, RNG *rng, unsigned long min_sam);
#endif
#else
  static void InicEventos(unsigned numTraficos, double transitorio);
  static void InicReintentos(Random *retardo, double persist, RNG *rng, unsigned long min_sam);
#endif

#ifdef modeMM
  void inicio(void);
#endif

  Trafico(Ruta **rutas_inic, unsigned num_rutas,
#if !(defined modeM1) && !(defined modeMM)
          Random *llegada,
#else
          double llegada,
#endif
#ifdef modeGG
          Random *demanda,
#else
          double demanda,
#endif
          float calidad, float tol_rel, unsigned long min_sam,
          DependentSampleStatistic *bloqueo, unsigned users = 0);

  Ruta *reserva(void);

  static void best_route(void) { best = true; };

#ifdef modeMM
  void llegada(void);
  void reintento(void);
  friend class Reintentos;
  friend class PTree;
  void salida(double);
#else
  void llegada(double ejecucion);
#endif

#if !(defined modeM1) && !(defined modeMM)
#ifdef modeG1
  static
#endif
      void
      salida(double ejecucion);
  void reintento(double ejecucion);
  friend class Reintentos;
#endif
  static void ejecuta();
  friend ostream &operator<<(ostream &out, Trafico &traf);
};
#endif
