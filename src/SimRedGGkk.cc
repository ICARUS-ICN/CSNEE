#include "RNG.h"

#if (defined modeMM1) && !(defined modeMM)
#define modeMM
#endif

#include "Uniform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef modeMM
#include "Constant.h"
#include "Erlang.h"
#include "LogNorm.h"
#include "NegExp.h"
#include "Pareto.h"
#include "Pareto0.h"
#include "Weibull.h"
#endif

#include "Enlace.hh"
#include "Trafico.hh"

#include "DpdentSmplStat.hh"

#include "GetOpt.h"
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <stdio.h>
using namespace std;

ofstream fallos;

const char *PossibleDistributions =
    "\nPosibles distribuciones y sus parametros:\n"
    "\tErlang: \tE\t<media> \t<varianza>\n"
    "\tLogNormal:\tL\t<media> \t<varianza>\n"
    "\tExponencial:\tM\t<media>\n"
    "\tUniforme:\tU\t<ínfimo>\t<superior>\n"
    "\tPareto: \tP\t<m>\t<alfa>\n"
    "\tPareto0:\tO\t<m>\t<alfa>\t(si X~P(m,alfa) => X-m~O(m,alfa)\n"
    "\tWeibull:\tW\t<alfa>  \t<beta>\n"
    "\tDeterminista:\tD\t<valor>";

void usage(char *program)
{
  cerr << "uso: " << program << " [s<semilla>q<calidad>t<tolerancia relativa>"
                                "T<tiempo transitorio>n<circuitos reservados>"
                                "m<numero minimo de muestras>aAbf]"
                                " <fichero de configuración>\n"
       << "a => calcula promedio agregado" << endl
       << "A => calcula promedio agregado siendo éste el único con requisitos de calidad" << endl
       << "b => utiliza mejor (más libre) ruta alternativa (2ª o posterior)" << endl
       << "f => vuelca tiempos de los rechazos en fichero TiemposBloqueos.txt" << endl
       << PossibleDistributions << endl
       << "\nFormato fichero:\n"
          " 1ª línea: nº de cicuitos separados por espacios de los enlaces 0, 1, 2...\n"
          " 2ª línea: nº de tráficos ofrecidos\n"
          " 3º por cada tráfico:\n"
          "   1ª línea especifica tiempo entre llegadas:\n"
          "      [<número de usuarios> ]<letra distribución> <par.1>[ <par.2>]\n"
          "   2ª línea especifica tiempo de servicio demandado:\n"
          "      <letra distribución> <par.1>[ <par.2>]\n"
          "   3ª línea especifica las rutas y opcionalmente promedio potencialmente compartido\n"
          "      (por defecto orden del tráfico empezando en 'a'):\n"
          "        <ruta>{ <ruta>} [letra del promedio]\n"
          "        con <ruta> = <enlace>{,<enlace>}\n"
          " 4º optativa especificación de reintentos:\n"
          "   1ª línea especifica tiempo entre reintentos:\n"
          "      <letra distribución> <par.1>[ <par.2>]\n"
          "   2ª línea especifica la probabilidad de persistencia"
       << endl;
  exit(0);
}

void bad_file(const char *message = nullptr)
{
  if (message)
    cerr << message << '\n';
  cerr << "Error en formato de fichero"
       << PossibleDistributions << endl;
  exit(3);
}

char *EnterLine(ifstream &in, int modo)
{
  static char line[65536];
  do
  {
    in.getline(line, 65536, '\n');
  } while (!in.eof() && (*line == '#' || !strlen(line)));
  if (in.eof())
  {
    if (modo == 1)
      bad_file("File ends before time");
    else
      return NULL;
  }
  return line;
}

void InitRandVar(ifstream &in,
#if !(defined modeM1) && !(defined modeMM)
                 Random **RV,
#else
                 double *RV,
#endif
                 RNG *rng, int modo, unsigned *users)
{
  char *line = EnterLine(in, modo);
  if (!line)
    return;

  char *aux;
  unsigned number = strtol(line, &aux, 0);
  if (line == aux)
    number = 0;

#if (defined modeM1) || (defined modeMM) || (defined modeG1)
  if (number)
    bad_file(strcat(line, ": finite users only available in GG and GD simulation flavors."));
#endif

  for (; !isalpha(*aux) && *aux; aux++)
    ;
  if (!aux)
  {
    if (modo == 1)
      bad_file(line);
    else
#if !(defined modeM1) && !(defined modeMM)
      *RV = NULL;
#else
      *RV = 0;
#endif
  }

  double p1, p2, p3;
  int pn = sscanf(aux + 1, "%lf %lf %lf", &p1, &p2, &p3);

  switch (*aux)
  {
  case 'M': // Exponential
    if (pn != 1)
      bad_file(line);
#if (defined modeM1) || (defined modeMM)
    *RV = p1;
    break;
#else
    *RV = new NegativeExpntl(p1, rng);
    break;
  case 'D': // Constant
    if (pn != 1)
      bad_file(line);
    *RV = new Constant(p1, rng);
    break;

  case 'E': // Erlang
    if (pn != 2)
      bad_file(line);
    *RV = new Erlang(p1, p2, rng);
    break;
  case 'L': // LogNormal
    if (pn != 2)
      bad_file(line);
    *RV = new LogNormal(p1, p2, rng);
    break;
  case 'U': // Uniform
    if (pn != 2)
      bad_file(line);
    *RV = new Uniform(p1, p2, rng);
    break;
  case 'P': // Pareto
    if (pn != 2)
      bad_file();
    *RV = new Pareto(p1, p2, rng);
    break;
  case 'O': // Pareto desplazada al 0: X-m con X~P(m,alfa)
    if (pn != 2)
      bad_file();
    *RV = new Pareto0(p1, p2, rng);
    break;
  case 'W': // Weibull
    if (pn != 2)
      bad_file(line);
    *RV = new Weibull(p1, p2, rng);
    break;
#endif
  default:
    bad_file(line);
  }
  *users = number;
}

void InitRandVar1(ifstream &in,
#if (defined modeGG)
                  Random **RV,
#else
                  double *RV,
#endif
                  RNG *rng)
{
  char *line = EnterLine(in, 1);

  for (; !isalpha(*line) && *line; line++)
    ;
  if (!line)
    bad_file(line);
#if (defined modeGG)
  *RV = NULL;
#else
  *RV = 0;
#endif

  double p1, p2, p3;
  int pn = sscanf(line + 1, "%lf %lf %lf", &p1, &p2, &p3);

  switch (*line)
  {
  case 'D': // Constant
    if (pn != 1)
      bad_file(line);
#if !(defined modeMM) && !(defined modeGG)
    *RV = p1;
    break;
#else
#ifdef modeMM
    bad_file(line);
#else
    *RV = new Constant(p1, rng);
#endif
    break;
  case 'M': // Exponential
    if (pn != 1)
      bad_file(line);
#ifdef modeMM
    *RV = p1;
    break;
#else
    *RV = new NegativeExpntl(p1, rng);
    break;

  case 'E': // Erlang
    if (pn != 2)
      bad_file(line);
    *RV = new Erlang(p1, p2, rng);
    break;
  case 'L': // LogNormal
    if (pn != 2)
      bad_file(line);
    *RV = new LogNormal(p1, p2, rng);
    break;
  case 'U': // Uniform
    if (pn != 2)
      bad_file(line);
    *RV = new Uniform(p1, p2, rng);
    break;
  case 'P': // Pareto
    if (pn != 2)
      bad_file();
    *RV = new Pareto(p1, p2, rng);
    break;
  case 'O': // Pareto desplazada al 0: X-m con X~P(m,alfa)
    if (pn != 2)
      bad_file();
    *RV = new Pareto0(p1, p2, rng);
    break;
  case 'W': // Weibull
    if (pn != 2)
      bad_file(line);
    *RV = new Weibull(p1, p2, rng);
    break;
#endif
#endif
  default:
    bad_file(line);
  }
}

unsigned uso_letra_promedio = 0;

Trafico *InputTrafico(ifstream &in, RNG *rng, Enlaces &enlaces, float quality, float r_tolerance, unsigned long min_samples, DependentSampleStatistic *promedios[], unsigned numTraficos, unsigned este)
{
#if !(defined modeM1) && !(defined modeMM)
  Random *Llega;
#else
  double Llega;
#endif
  unsigned users;
  InitRandVar(in, &Llega, rng, 1, &users);

#if !(defined modeGG)
  double demanda;
#else
  Random *demanda;
#endif
  InitRandVar1(in, &demanda, rng);

  char *line = EnterLine(in, 1);
  char *aux;
  // estimacion por arriba del numero de rutas
  unsigned num_rutas = 1;
  aux = line;
  while (*aux)
    if (isspace(*aux++))
      num_rutas++;
  Ruta **rutas = new Ruta *[num_rutas];

  num_rutas = 0;
  aux = strtok(line, " 	");
  while (aux && isdigit(*aux))
  {
    rutas[num_rutas++] = new Ruta(aux, enlaces);
    aux = strtok(NULL, " 	");
  }

  DependentSampleStatistic *bloqueo;
  if (aux) // viene letra indicando cuál de los promedios usar
  {
    uso_letra_promedio = 1;
    unsigned i = *aux - 'a';
    if (i >= numTraficos)
    {
      cerr << "Error: No se pueden definir más promedios (" << *aux << "→" << i << ") que tráficos (" << numTraficos << ")" << endl;
      exit(2);
    }
    if (promedios[i] == NULL)
      promedios[i] = new DependentSampleStatistic(quality, r_tolerance, min_samples);
    bloqueo = promedios[i];
  }
  else
  {
    if (promedios[este] == NULL)
      promedios[este] = new DependentSampleStatistic(quality, r_tolerance, min_samples);
    bloqueo = promedios[este];
  }

  return new Trafico(rutas, num_rutas, Llega, demanda, quality,
                     r_tolerance, min_samples, bloqueo, users);
}

int main(int argc, char *argv[])
{
#ifdef PURIFY
  AP_Report(0);
#endif
  GetOpt getopt(argc, argv, "s:q:t:T:n:m:aAbf");
  int option_char;

  int seed = 0, agregados = 0, reserved = 0, best = 0, flag_fallos = 0;
  unsigned long min_samples = 0;
  float quality = 0.9, r_tolerance = 0.1;
  double transitorio = -1;

  char *aux;

  while ((option_char = getopt()) != EOF)
    switch (option_char)
    {
    case 's':
      seed = atoi(getopt.optarg);
      break;
    case 'q':
      quality = atof(getopt.optarg);
      break;
    case 't':
      r_tolerance = atof(getopt.optarg);
      break;
    case 'T':
      transitorio = atof(getopt.optarg);
      break;
    case 'n':
      Enlace::init_reserved(reserved = atoi(getopt.optarg));
      break;
    case 'b':
      Trafico::best_route();
      best = 1;
      break;
    case 'm':
      min_samples = strtoul(getopt.optarg, &aux, 0);
      break;
    case 'a':
      agregados = 1;
      break;
    case 'A':
      agregados = 2;
      break;
    case 'f':
      flag_fallos = 1;
      break;
    default:
      usage(argv[0]);
    }

#ifdef PURIFY
  AP_Report(1);
#endif

  if (getopt.optind >= argc)
    usage(argv[0]);

  if (agregados)
  {
    DependentSampleStatistic::start_aggregated_mean(quality, r_tolerance, min_samples);
    if (agregados > 1)
      quality = 0;
  }
  ifstream in(argv[getopt.optind]);
  if (!in)
  {
    cerr << "Couldnot open input file " << argv[getopt.optind] << endl;
    exit(2);
  }

  char *line = EnterLine(in, 1);
  Enlaces enlaces(line);
  // delete line;

  line = EnterLine(in, 1);
  unsigned numTraficos = strtol(line, &aux, 0);
  if (line == aux || !numTraficos)
    bad_file(line);
  // delete line;

  RNG rng(seed);
  Trafico *traficos[numTraficos];
  DependentSampleStatistic *promedios[numTraficos];
  for (unsigned i = 0; i < numTraficos; i++)
    promedios[i] = NULL;
#if (defined modeM1) || (defined modeMM)
  Trafico::InicEventos(transitorio, &rng);
#else
  Trafico::InicEventos(numTraficos, transitorio);
#endif

  for (unsigned i = 0; i < numTraficos; i++)
    traficos[i] = InputTrafico(in, &rng, enlaces, quality, r_tolerance, min_samples, promedios, numTraficos, i);

#if !(defined modeM1)
#if !(defined modeMM)
  Random *retardo_intentos = NULL;
#else
  double retardo_intentos = 0;
#endif
  unsigned nul;
  InitRandVar(in, &retardo_intentos, &rng, 0, &nul);

  if (retardo_intentos)
  {
    if (uso_letra_promedio)
    {
      cerr << "Reintentos con selección de promedio actualmente no soportado" << endl;
      exit(2);
    }
    line = EnterLine(in, 1);
    double persistencia = strtod(line, &aux);
    if (line == aux || persistencia < 0 || persistencia > 1)
      bad_file(line);
    Trafico::InicReintentos(retardo_intentos, persistencia, &rng, min_samples);
    // delete line;
  }
#endif
  if (flag_fallos)
  {
    fallos.open("TiemposBloqueos.txt");
    if (!fallos)
    {
      cerr << "Couldnot open input file " << argv[getopt.optind] << endl;
      exit(2);
    }
  }

  Trafico::ejecuta();

  if (flag_fallos)
    fallos.close();

  char fich_salida[strlen(argv[getopt.optind]) + 50];
  strcpy(fich_salida, argv[getopt.optind]);
  if (reserved)
  {
    char aux[30];
    sprintf(aux, ".%dk", reserved);
    strcat(fich_salida, aux);
  }
  if (best)
    strcat(fich_salida, ".best");
  strcat(fich_salida, ".out");
  ofstream out;
  out.open(fich_salida);
  if (!out)
  {
    cerr << "Couldnot open input file " << argv[getopt.optind] << endl;
    exit(2);
  }
  out << "Semilla: " << seed
      << "\nTolerancia relativa: " << r_tolerance << "\n\n";

  if (uso_letra_promedio)
  {
    char a = 'a';
    for (unsigned i = 0; agregados <= 1 && i < numTraficos; i++, a++)
      if (promedios[i] != NULL)
      {
        out << "Trafico/s promediado/s en " << a << ":\n";
        out << "Probabilidad de bloqueo estimada: " << *promedios[i] << endl;
      }
  }
  else
    for (unsigned i = 0; agregados <= 1 && i < numTraficos; i++)
    {
      out << "Trafico " << i << ":\n";
      out << *traficos[i] << '\n';
    }

  if (agregados)
    DependentSampleStatistic::show_aggregated_mean(out);
  out.close();

  exit(0);
}
