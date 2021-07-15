CSNEE
======

A Simulator of Circuit Switched Networks

## Overview

This is a simulation tool comprising several simulation flavors, such as Monte Carlo (only changing states are simulated) or unit service times, in order to optimize the simulation time. 

## USAGE:
	CSNEE [OPTIONS|FLAGS] <configuration file>

### FLAGS: aAbf
* a => compute aggregated average
* A => only computed aggregated average abied to confidence interval requirement
* b => use best alternative route (second one or latter)
* f => write blocking times to file TiemposBloqueos.txt

### OPTIONS: 
 	s<seed>
	q<quality>
	t<relative tolerance>
	T<transitory interval>
	n<reserved circuits>
	m<minimum sample size>

## Legal
Copyright ⓒ 1997–2021 Andrés Suárez González <asuarez@det.uvigo.es>.

This simulator is licensed under the GNU General Public License, version 2 (GPL-2.0). For information see LICENSE

