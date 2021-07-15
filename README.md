CSNEE
======

A Simulator of Circuit Switched Networks

![build](https://github.com/ICARUS-ICN/CSNEE/actions/workflows/main.yml/badge.svg)

## Overview

This is a simulation tool comprising several simulation flavors, such as Monte Carlo (only changing states are simulated) or unit service times, in order to optimize the simulation time. 

## USAGE:
	CSNEE [OPTIONS|FLAGS] ARGS

### FLAGS: aAbf
	- a		Compute aggregated average
	- A		Just compute aggregated average abied to confidence interval requirement
	- b		Use best alternative route (second one or latter)
	- f		Write blocking times to file TiemposBloqueos.txt

### OPTIONS:

 	- s<seed>
	- q<quality>
	- t<relative tolerance>
	- T<transitory interval>
	- n<reserved circuits>
	- m<minimum sample size>

### ARGS:

	<Configuration file>	Simulation description
## Legal
Copyright ⓒ 1997–2021 Andrés Suárez González <asuarez@det.uvigo.es>.

This simulator is licensed under the GNU General Public License, version 2 (GPL-2.0). For information see LICENSE

