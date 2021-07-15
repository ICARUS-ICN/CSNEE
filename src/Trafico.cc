#if (defined modeMM1) && !(defined modeMM)
#define modeMM
#endif
#include "Trafico.hh"
#include "Enlace.hh"
#include "RNG.h"
#include "Uniform.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef modeMM
#include "Comunicacion.hh"
#include "Constant.h"
#include "Erlang.h"
#include "LogNorm.h"
#include "NegExp.h"
#include "Weibull.h"
#include <queue>
using namespace std;

double Trafico::reloj = 0;
#else
PTree *Trafico::root = NULL;
#endif

bool Trafico::best = false;

#ifndef modeM1
Reintentos *Trafico::reintentos;
#endif

#if !(defined modeM1) && !(defined modeMM)
priority_queue<Comunicacion> Trafico::eventos;
#else
RNG *Trafico::rng;
#ifdef modeMM
PTree *root = NULL;
#else
double Trafico::next_arrive;
NegativeExpntl *Trafico::Llegada;
#endif
#endif //!(defined modeM1) && !(defined modeMM)

double Trafico::Transitorio;
unsigned int Trafico::flag_trans = 0;
unsigned int Trafico::flag_first1 = 0;
Trafico *Trafico::primero;

#if (defined modeG1) || (defined modeM1)
queue<Salida> Trafico::salidas;
double Trafico::Demanda = -1.0;
#endif

#ifdef modeMM
void Trafico::inicio(void)
{
	new PTree(this, tipo_llegada);
	depart = new PTree(this, tipo_salida);
	conns = 0;
	if (reintentos)
	{
		nodo_reintentos = new PTree(this, tipo_reintento);
		num_rein = 0;
	}
}

PTree::PTree(PTree *son, PTree *newson)
{
	up = son->up;
	if (!up)
		Trafico::root = this;
	else if (up->left == son)
		up->left = this;
	else
		up->right = this;
	son->up = this;
	left = son;
	right = newson;
	newson->up = this;
	amount = son->amount + newson->amount;
	source = NULL;
}

PTree::PTree(Trafico *trafico, Tipo tipot)
{
	source = trafico;
	amount = trafico->llegadas;
	tipo = tipot;
	left = right = NULL;
	if (Trafico::root == NULL)
	{
		Trafico::root = this;
		up = NULL;
	}
	else
	{
		PTree *aux = Trafico::root;
		while (!aux->source)
		{
			if (aux->left->amount > amount &&
				aux->right->amount > amount)
				aux = (aux->left->amount < aux->right->amount) ? aux->left : aux->right;
			else
			{
				if (aux->left->amount > amount)
					aux = aux->right;
				else if (aux->right->amount > amount)
					aux = aux->left;
				break;
			}
		}
		new PTree(aux, this);
	}
}

double PTree::initiate(void)
{
	if (source != NULL)
	{
		if (tipo == tipo_llegada)
			return amount;
		if ((tipo == tipo_salida) || (tipo == tipo_reintento))
			return (amount = 0);
	}
	return (amount = left->initiate() + right->initiate());
}

void PTree::changed(double amountt)
{
	amount = amountt;
	for (PTree *aux = up; aux; aux = aux->up)
		aux->amount = aux->left->amount + aux->right->amount;
}

void PTree::next(double ratio)
{
	ratio *= amount;
	PTree *aux = this;
	while (!aux->source)
		if (aux->left->amount > ratio)
			aux = aux->left;
		else
		{
			ratio -= aux->left->amount;
			aux = aux->right;
		}

	if (aux->tipo == tipo_llegada)
		aux->source->llegada();
	else
	{
		if (aux->tipo == tipo_reintento)
			aux->source->reintento();
		else
			aux->source->salida(ratio);
	}
}
#endif //modeMM

#if !(defined modeM1) && !(defined modeMM)
void Comunicacion::ejecuta()
{
	switch (tipo)
	{
	case tipo_llegada:
		trafico->llegada(tiempo_ejecucion);
		return;
	case tipo_salida:
#ifdef modeGG
		ruta()->libera();
#endif
		trafico->salida(tiempo_ejecucion);
		return;
	case tipo_reintento:
		trafico->reintento(tiempo_ejecucion);
		return;
	default:
		cerr << "Error en tipo de evento" << endl;
		exit(1);
	}
}
#endif //!(defined modeM1) && !(defined modeMM)

#ifndef modeM1
Reintentos::Reintentos(
#ifndef modeMM
	Random *ret,
#else
	double ret,
#endif
	double persist, RNG *rng, unsigned long min_sam)
	: probabilidad(0, 1, rng)
{
#ifndef modeMM
	Retardo = ret;
#else
	Retardo = 1 / ret;
#endif
	persistencia = persist;
	for (Trafico *aux = Trafico::primero; aux; aux = aux->siguiente)
	{
		aux->p_bl_primer = new DependentSampleStatistic(aux->calidad, aux->tolerancia, min_sam);
		aux->p_bl_reint = new DependentSampleStatistic(aux->calidad, aux->tolerancia, min_sam);
	}
}

double Reintentos::operator()()
{
	if (probabilidad() < persistencia)
#ifndef modeMM
		return (*Retardo)();
#else
		return Retardo;
#endif
	else
		return 0.0;
}

void Trafico::InicReintentos(
#ifndef modeMM
	Random *retardo,
#else
	double retardo,
#endif
	double persist, RNG *rng, unsigned long min_sam)
{
	reintentos = new Reintentos(retardo, persist, rng, min_sam);
}
#endif //ndef modeM1

#ifdef modeMM
double Reintentos::valor(void)
{
	return Retardo;
}
#endif

void Trafico::InicEventos(
#if (defined modeM1) || (defined modeMM)
	double transitorio, RNG *rngg)
#else
	unsigned numTraficos, double transitorio)
#endif
{
#if (defined modeM1) || (defined modeMM)
	rng = rngg;
#endif

	if (transitorio > 0)
	{
		Transitorio = transitorio;
		flag_trans = 1;
	}
	if (transitorio < 0)
	{
		cerr << "Transitorio " << transitorio << endl;
		flag_trans = 1;
		flag_first1 = 1;
	}
}

Trafico::Trafico(Ruta **rutas_inic, unsigned num_rutas2,
#if (defined modeM1) || (defined modeMM)
				 double llegada,
#else
				 Random *llegada,
#endif
#ifndef modeGG
				 double demanda,
#else
				 Random *demanda,
#endif
				 float calidadd, float tol_rel, unsigned long min_sam, DependentSampleStatistic *bloqueo, unsigned users)
{
	p_bloqueo = bloqueo;
#if (defined modeG1) || (defined modeM1) || (defined modeMM1)
	if (Demanda != -1.0)
	{
		if (demanda != Demanda)
		{
			cerr << "Simulator for mode G1 only" << endl;
			exit(1);
		}
	}
	else
	{
		Demanda = demanda;
	}
#else
#ifdef modeMM
	mu = 1 / demanda;
#endif
	Demanda = demanda;
#endif //(defined modeG1) || (defined modeM1) || (defined modeMM1)
	rutas = rutas_inic;
	num_rutas = num_rutas2;

#ifndef modeM1
	p_bl_primer = NULL;
	p_bl_reint = NULL;
#endif

#if (defined modeM1) || (defined modeMM)
	llegadas = 1 / llegada;
#else
	Llegada = llegada;
	cerr << "Usuarios: " << users << endl;
	if (users)
	{
		finite_users = true;
		for (unsigned i = 0; i < users; i++)
			eventos.push(Comunicacion((*Llegada)(), tipo_llegada, this));
	}
	else
	{
		finite_users = false;
		eventos.push(Comunicacion((*Llegada)(), tipo_llegada, this));
	}
#endif //(defined modeM1) || (defined modeMM)

	siguiente = primero;
	primero = this;
	tolerancia = tol_rel;
	calidad = calidadd;
}

ostream &operator<<(ostream &out, Trafico &traf)
{
	out << "Probabilidad de bloqueo estimada: " << *traf.p_bloqueo << endl;

#if !(defined modeM1)
	if (traf.p_bl_primer)
		out << "Probabilidad de los primeros intentos: " << (*traf.p_bl_primer) << endl;
	if (traf.p_bl_reint)
		out << "Probabilidad de los reintentos: " << (*traf.p_bl_reint) << endl;
#endif

	return out;
}

Ruta *Trafico::reserva(void)
{
	if (best)
	{
		Ruta **p = rutas;
		if ((*p)->reserva(0))
			return *p;
		unsigned max = 0;
		Ruta **b = NULL;
		for (Ruta **l = rutas + num_rutas; p != l; p++)
			if (unsigned m = (*p)->mejora(max))
			{
				max = m;
				b = p;
			}
		if (b && (*b)->reserva(1))
			return *b;
		return NULL;
	}

	unsigned overload = 0;
	for (Ruta **p = rutas, **l = rutas + num_rutas; p != l; p++, overload = 1)
		if ((*p)->reserva(overload))
			return *p;
	return NULL;
}

#ifdef modeMM
void Trafico::llegada(void)
{
	Ruta *r = reserva();
	if (r)
	{
		conns++;
#ifdef modeMM1
		depart->changed(conns);
#else
		depart->changed(conns * mu);
#endif
		if (!flag_trans)
		{
			p_bloqueo->no();
			if (p_bl_primer)
				(*p_bl_primer).no();
		}
		else if (!flag_first1)
			flag_trans--;
	}
	else
	{
		if (flag_first1)
			flag_trans = flag_first1 = 0;
		if (!flag_trans)
		{
			p_bloqueo->yes();
			if (p_bl_primer)
				(*p_bl_primer).yes();
		}
		else if (!flag_first1)
			flag_trans--;
		double t;
		if (reintentos && (t = (*reintentos)()) > 0.0)
		{
			num_rein++;
			nodo_reintentos->changed(num_rein * t);
		}
	}
}

void Trafico::reintento(void)
{
	Ruta *r = reserva();
	if (r)
	{
		num_rein--;
		double t;
		t = reintentos->valor();
		nodo_reintentos->changed(num_rein * t);
		conns++;
#ifdef modeMM1
		depart->changed(conns);
#else
		depart->changed(conns * mu);
#endif
		if (!flag_trans)
		{
			p_bloqueo->no();
			if (p_bl_reint)
				(*p_bl_reint).no();
		}
	}
	else
	{
		if (!flag_trans)
		{
			p_bloqueo->yes();
			if (p_bl_reint)
				(*p_bl_reint).yes();
		}
		double t;
		if (reintentos && (t = (*reintentos)()) <= 0.0)
		{
			num_rein--;
			t = reintentos->valor();
			nodo_reintentos->changed(num_rein * t);
		}
	}
}
#else //modeMM
void Trafico::llegada(double ejecucion)
{
	reloj = ejecucion;
	if (flag_trans && !flag_first1 && reloj > Transitorio)
		flag_trans = 0;
	Ruta *r = reserva();
#ifdef modeM1
	if (r)
	{
		salidas.push(Salida(reloj + Demanda, r));
		if (!flag_trans)
			p_bloqueo->no();
	}
	else
	{
		if (flag_first1)
			flag_trans = flag_first1 = 0;
		if (!flag_trans)
			p_bloqueo->yes();
	}
#else //modeM1
	if (r)
	{
#ifdef modeGG
		eventos.push(Comunicacion(reloj + (*Demanda)(), tipo_salida, this, r));
#else
		if (salidas.empty())
			eventos.push(Comunicacion(reloj + Demanda, tipo_salida, this));
		salidas.push(Salida(reloj + Demanda, r));
#endif
		if (!flag_trans)
		{
			p_bloqueo->no();
			if (p_bl_primer)
				(*p_bl_primer).no();
		}
	}
	else
	{
		if (flag_first1)
			flag_trans = flag_first1 = 0;
		if (!flag_trans)
		{
			extern ofstream fallos;
			fallos << reloj << endl;
			p_bloqueo->yes();
			if (p_bl_primer)
				(*p_bl_primer).yes();
		}
		double t;
		if (reintentos && (t = (*reintentos)()) > 0.0)
			eventos.push(Comunicacion(reloj + t, tipo_reintento, this));
		else if (finite_users)
			eventos.push(Comunicacion(reloj + (*Llegada)(), tipo_llegada, this));
	}
	if (!finite_users)
		eventos.push(Comunicacion(reloj + (*Llegada)(), tipo_llegada, this));
#endif //modeM1
}
#endif //modeMM

#ifdef modeMM
void Trafico::salida(double ratio)
{
#ifdef modeMM1
	unsigned num_conn = unsigned(floor(ratio));
#else
	unsigned num_conn = unsigned(floor(ratio * Demanda));
#endif
	num_conn++;
	// COMPROBAR QUE NO PUEDE OCURRIR num_conn > conns ??
	for (Ruta **r = rutas; num_conn; r++)
		(*r)->libera(&num_conn);
	conns--;
#ifdef modeMM1
	depart->changed(conns);
#else
	depart->changed(conns * mu);
#endif
}

void Trafico::ejecuta()
{
	double lambda = 0;
	for (Trafico *p = primero; p != NULL; p = p->siguiente)
	{
		lambda += p->llegadas;
		p->inicio();
	}
	if (!flag_first1)
		flag_trans = int(Transitorio * lambda);

	root->initiate();
	const double inv_max_rng = 1.0 / rng->sup();
	while (DependentSampleStatistic::number_averages())
	{
		double ratio = rng->asLong() * inv_max_rng;
		root->next(ratio);
	}
}
#else //modeMM

#ifndef modeM1
void Trafico::salida(double ejecucion)
{
	reloj = ejecucion;
#ifndef modeGG
	Salida &s = salidas.front();
	s.ruta()->libera();
	salidas.pop();
	if (!salidas.empty())
	{
		Salida &siguiente = salidas.front();
		eventos.push(Comunicacion(siguiente.ejecucion(), tipo_salida
#ifndef modeG1
								  ,
								  this
#endif
								  ));
	}
#endif

#ifndef modeG1
	if (finite_users)
		eventos.push(Comunicacion(reloj + (*Llegada)(), tipo_llegada, this));
#endif
}

void Trafico::reintento(double ejecucion)
{
	reloj = ejecucion;
	if (flag_trans && !flag_first1 && reloj > Transitorio)
		flag_trans = 0;
	Ruta *r = reserva();
	if (r)
	{
#ifdef modeGG
		eventos.push(Comunicacion(reloj + (*Demanda)(), tipo_salida, this, r));
#else
		if (salidas.empty())
			eventos.push(Comunicacion(reloj + Demanda, tipo_salida, this));
		salidas.push(Salida(reloj + Demanda, r));
#endif
		if (!flag_trans)
		{
			p_bloqueo->no();
			if (p_bl_reint)
				(*p_bl_reint).no();
		}
	}
	else
	{
		if (!flag_trans)
		{
			p_bloqueo->yes();
			if (p_bl_reint)
				(*p_bl_reint).yes();
		}
		double t;
		if ((t = (*reintentos)()) > 0.0)
			eventos.push(Comunicacion(reloj + t, tipo_reintento, this));
		else if (finite_users)
			eventos.push(Comunicacion(reloj + (*Llegada)(), tipo_llegada, this));
	}
}

void Trafico::ejecuta()
{
	while (DependentSampleStatistic::number_averages())
	{
		// revisar si aquí podemos usar const y &c
		Comunicacion c = eventos.top();
		c.ejecuta();
		eventos.pop();
	}
}

#else  //modeM1

struct Prob_Lleg
{
	unsigned d_prob_acum;
	Trafico *traf;
};

static Prob_Lleg *dist_lleg;
static unsigned numTrafs = 0;

void Trafico::run()
{
	Salida &s = salidas.front();
	if (salidas.empty() || s.ejecucion() > next_arrive)
	{
		unsigned long random = rng->asLong();
		Trafico *t;

		unsigned min = 0, max = numTrafs;
		unsigned aux = (min + max) >> 1;
		while (min != aux)
		{
			if (dist_lleg[aux].d_prob_acum < random)
				min = aux;
			else
				max = aux;
			aux = (min + max) >> 1;
		}
		t = (min != max && dist_lleg[min].d_prob_acum > random) ? dist_lleg[min].traf : dist_lleg[max].traf;

		t->llegada(next_arrive);
		next_arrive += (*Llegada)();
	}
	else
	{
		Salida &s = salidas.front();
		s.ruta()->libera();
		salidas.pop();
	}
}

void Trafico::ejecuta()
{
	double llegadas_total = 0;
	for (Trafico *t = primero; t != NULL; t = t->siguiente, numTrafs++)
		llegadas_total += t->llegadas;
	Llegada = new NegativeExpntl(1 / llegadas_total, rng);

	dist_lleg = new Prob_Lleg[numTrafs];
	double acum = 0;

	numTrafs = 0;
	const unsigned long max_rng = rng->sup() - 1;
	for (Trafico *t = Trafico::primero; t != NULL;
		 t = t->siguiente, numTrafs++)
	{
		acum += t->llegadas;
		dist_lleg[numTrafs].d_prob_acum =
			t->d_prob_acum = (unsigned long)(acum * max_rng / llegadas_total);
		dist_lleg[numTrafs].traf = t;
	}
	next_arrive = (*Llegada)();
	while (DependentSampleStatistic::number_averages())
	{
		run();
	}
}
#endif //modeM1
#endif //modeMM
