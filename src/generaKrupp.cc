#include "GetOpt.h"
#include <ctype.h>
#include <fstream>
#include <iostream>

using namespace std;

void usage(char *program)
{
  cerr << "uso: " << program
       << " [N<numero de nodos>n<numero de circuitos>a<trafico por par de nodos>]"
       << endl;
  exit(0);
}

int main(int argc, char *argv[])
{
  GetOpt getopt(argc, argv, "N:n:a:");
  int option_char;

  unsigned N = 0;
  unsigned n = 0;
  double A = 0;

  while ((option_char = getopt()) != EOF)
    switch (option_char)
    {
    case 'N':
      N = atoi(getopt.optarg);
      break;
    case 'n':
      n = atoi(getopt.optarg);
      break;
    case 'a':
      A = atof(getopt.optarg);
      break;
    default:
      usage(argv[0]);
    }
  if (getopt.optind != argc || !N || !n || A == 0)
    usage(argv[0]);

  double lambda = 1 / A;

  unsigned a[N][N];

  unsigned c = 0;
  for (unsigned i = 0; i < N; i++)
    for (unsigned j = 0; j < N; j++)
      if (j < i)
        a[i][j] = a[j][i];
      else if (j > i)
        a[i][j] = c++;

  cout << n;
  for (unsigned i = 1; i < c; i++)
    cout << ' ' << n;

  cout << '\n'
       << '\n'
       << c << '\n';

  for (unsigned i = 0; i < N; i++)
    for (unsigned j = i + 1; j < N; j++)
    {
      cout << "\n\n"
           << "M\t" << lambda << '\n'
           << "M\t" << '1' << '\n';
      cout << a[i][j];
      for (unsigned k = (j < N - 1) ? j + 1 : 0; k != j; k = (k < N - 1) ? k + 1 : 0)
        if (k != i)
          cout << ' ' << a[i][k] << ',' << a[k][j];
    }
  cout << endl;
}
