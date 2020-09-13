#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <set>
#include <map>
#define DBG(x) cerr << #x << " = " << (x) <<"\n"

using namespace std;

int INFTY = 10e-6; // Valor para indicar que no hubo solución.

// Información de la instancia a resolver.
int cantidad_locales, limite_contagio;
// n = cantidad de locales
// M = valor del límite de contagio
vector<pair<int, int>> locales;

// i : la posicion del local dentro de la lista de locales.
// suma_contagio : suma del valor de contagio hasta este local.
// suma_beneficio : beneficio sumado hasta ahora.
// seleccionados : locales seleccionados hasta ahora.
int FB(int i, int suma_contagio, int suma_beneficio, vector<bool> seleccionados){
	//Caso base.
	if ( i == cantidad_locales ) {
		int j = 0;
		while (j < cantidad_locales - 1 && !(seleccionados[j] && seleccionados[j+1])) 
			j++;
		return suma_contagio <= limite_contagio && j == cantidad_locales - 1 ? suma_beneficio : INFTY;
	}
	else {
		// Recursión.
		vector<bool> seleccionarActual = seleccionados;
		seleccionarActual[i] = true;
		return max(FB(i + 1, suma_contagio, suma_beneficio, seleccionados), FB(i + 1, suma_contagio + locales[i].second, suma_beneficio + locales[i].first, seleccionarActual));
	}
}

// i: la posicion del local dentro de la lista de locales.
// suma_contagio : suma del valor de contagio hasta este local.
// suma_beneficio : beneficio sumado hasta ahora.
bool poda_factibilidad = true; // define si la poda por factibilidad esta habilitada.
bool poda_optimalidad = true; // define si la poda por optimalidad esta habilitada.
int mayor_beneficio = INFTY; // Mejor solucion hasta el momento.
int BT(int i, int suma_contagio, int suma_beneficio, vector<bool> seleccionados, bool adyacente)
{
	// Caso base.
    if (i == cantidad_locales)
    {
    	if (suma_contagio <= limite_contagio && !adyacente) mayor_beneficio = max(mayor_beneficio, suma_beneficio);
        if (i == cantidad_locales) return suma_contagio <= limite_contagio && !adyacente ? suma_beneficio : INFTY;
    }

    // Poda por factibilidad.
    if (poda_factibilidad && suma_contagio > limite_contagio && adyacente) return INFTY;

    // Poda por optimalidad.
    if (poda_optimalidad){
		// Recorremos todos los locales que nos faltan y sumamos sus beneficios
		int suma_faltantes = suma_beneficio;
		for (int j = i ; j < cantidad_locales ; j+=2)
			suma_faltantes += locales[j].first;
		if(suma_faltantes <= mayor_beneficio)
			return INFTY;
	} 

    // Recursión.
	// Utilizamos la variable adyacente para chequear si seleccionamos locales que están adyacentes
	// En caso de hacerlo no creamos un nuevo vector que representa la solución parcial y le pasamos el mismo
	// esperamos que esto reduzca la complejidad espacial y en el caso de la optimalidad resulta más beneficioso
	if(!adyacente){
		vector<bool> seleccionarActual = seleccionados;
		seleccionarActual[i] = true;		
		return max(BT(i + 1, suma_contagio, suma_beneficio, seleccionados, adyacente),
				BT(i + 1, suma_contagio + locales[i].second, suma_beneficio + locales[i].first, seleccionarActual, i!=0? seleccionarActual[i-1]: adyacente));
	}
	else {
		return max(BT(i + 1, suma_contagio, suma_beneficio, seleccionados, adyacente),
					BT(i + 1, suma_contagio + locales[i].second, suma_beneficio + locales[i].first, seleccionados, adyacente));
	}
}

vector<vector<int>> M; // Memoria de PD.
const int UNDEFINED = -1;
// PD(i, w): minimo cardinal de un subconjunto de {Si, ... , Sn} que sume W−w.
/*int PD(int i, int w)
{
	if (w > M) return INFTY;
	if (i == n && w != W) return INFTY;
	if (i == n && w == W) return 0;
	if (M[i][w] == UNDEFINED) M[i][w] = min(PD(i+1, w), 1+PD(i+1, w+locales[i]));
	return M[i][w];
}
*/
// Recibe por parámetro qué algoritmos utilizar para la ejecución separados por espacios.
// Imprime por clog la información de ejecución de los algoritmos.
// Imprime por cout el resultado de algun algoritmo ejecutado.
int main(int argc, char** argv)
{
	// Leemos el parametro que indica el algoritmo a ejecutar.
	map<string, string> algoritmos_implementados = {
		{"FB", "Fuerza Bruta"}, {"BT", "Backtracking con podas"}, {"BT-F", "Backtracking con poda por factibilidad"}, 
		{"BT-O", "Backtracking con poda por optimalidad"}, {"DP", "Programacion dinámica"}
	};

	// Verificar que el algoritmo pedido exista.
	if (argc < 2 || algoritmos_implementados.find(argv[1]) == algoritmos_implementados.end())
	{
		cerr << "Algoritmo no encontrado: " << argv[1] << endl;
		cerr << "Los algoritmos existentes son: " << endl;
		for (auto& alg_desc: algoritmos_implementados) cerr << "\t- " << alg_desc.first << ": " << alg_desc.second << endl;
		return 0;
	}
	string algoritmo = argv[1];

    // Leemos el input.
    cin >> cantidad_locales >> limite_contagio;
    locales.assign(cantidad_locales, make_pair(0,0));
    for (int i = 0; i < cantidad_locales; ++i) cin >> locales[i].first >> locales[i].second;

    // Ejecutamos el algoritmo y obtenemos su tiempo de ejecución.
	int optimum;
	optimum = INFTY;
	auto start = chrono::steady_clock::now();
	if (algoritmo == "FB")
	{
		optimum = FB(0, 0, 0, vector<bool>(cantidad_locales, false));
	}
	else if (algoritmo == "BT")
	{
		mayor_beneficio = INFTY;
		poda_optimalidad = poda_factibilidad = true;
		optimum = BT(0, 0, 0, vector<bool>(cantidad_locales, false), false);
	}
	else if (algoritmo == "BT-F")
	{
		mayor_beneficio = INFTY;
		poda_optimalidad = false;
		poda_factibilidad = true;
		optimum = BT(0, 0, 0, vector<bool>(cantidad_locales, false), false);
	}
	else if (algoritmo == "BT-O")
	{
		mayor_beneficio = INFTY;
		poda_optimalidad = true;
		poda_factibilidad = false;
		optimum = BT(0, 0, 0, vector<bool>(cantidad_locales, false), false);
	}
	/*else if (algoritmo == "DP")
	{
		// Precomputamos la solucion para los estados.
		M = vector<vector<int>>(n+1, vector<int>(W+1, UNDEFINED));
		for (int i = 0; i < n+1; ++i)
			for (int j = 0; j < W+1; ++j)
				PD(i, j);

		// Obtenemos la solucion optima.
		optimum = PD(0, 0);
	}*/
	auto end = chrono::steady_clock::now();
	double total_time = chrono::duration<double, milli>(end - start).count();

	// Imprimimos el tiempo de ejecución por stderr.
	clog << total_time << endl;

    // Imprimimos el resultado por stdout.
    cout << (optimum == INFTY ? -1 : optimum) << endl;
    return 0;
}/*
./npm "FB"
5 40
50 10 
25 10 
10 20 
20 30 
15 20*/