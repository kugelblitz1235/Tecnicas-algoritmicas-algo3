#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <set>
#include <map>

using namespace std;

int INFTY = 10e6; // Valor para indicar que no hubo solución.

// Información de la instancia a resolver.
int n, W;
vector<int> S;

// i: posicion del elemento a considerar en este nodo.
// w: suma de los elementos seleccionados hasta este nodo.
// k: cantidad de elementos seleccionados hasta este nodo.
int FB(int i, int w, int k)
{
    // Caso base.
    if (i == n) return w == W ? k : INFTY;

    // Recursión.
    return min(FB(i+1, w, k), FB(i+1, w+S[i], k+1));
}

// i: posicion del elemento a considerar en este nodo.
// k: cantidad de elementos seleccionados hasta este nodo.
// w: suma de los elementos seleccionados hasta este nodo.
bool poda_factibilidad = true; // define si la poda por factibilidad esta habilitada.
bool poda_optimalidad = true; // define si la poda por optimalidad esta habilitada.
int K = INFTY; // Mejor solucion hasta el momento.
int BT(int i, int w, int k)
{
	// Caso base.
    if (i == n)
    {
    	if (w == W) K = min(K, k);
        if (i == n) return w == W ? k : INFTY;
    }

    // Poda por factibilidad.
    if (poda_factibilidad && w > W) return INFTY;

    // Poda por optimalidad.
    if (poda_optimalidad && k >= K) return INFTY;

    // Recursión.
    return min(BT(i+1, w, k), BT(i+1, w+S[i], k+1));
}

vector<vector<int>> M; // Memoria de PD.
const int UNDEFINED = -1;
// PD(i, w): minimo cardinal de un subconjunto de {Si, ... , Sn} que sume W−w.
int PD(int i, int w)
{
	if (w > W) return INFTY;
	if (i == n && w != W) return INFTY;
	if (i == n && w == W) return 0;
	if (M[i][w] == UNDEFINED) M[i][w] = min(PD(i+1, w), 1+PD(i+1, w+S[i]));
	return M[i][w];
}

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
    cin >> n >> W;
    S.assign(n, 0);
    for (int i = 0; i < n; ++i) cin >> S[i];

    // Ejecutamos el algoritmo y obtenemos su tiempo de ejecución.
	int optimum;
	optimum = INFTY;
	auto start = chrono::steady_clock::now();
	if (algoritmo == "FB")
	{
		optimum = FB(0, 0, 0);
	}
	else if (algoritmo == "BT")
	{
		K = INFTY;
		poda_optimalidad = poda_factibilidad = true;
		optimum = BT(0, 0, 0);
	}
	else if (algoritmo == "BT-F")
	{
		K = INFTY;
		poda_optimalidad = false;
		poda_factibilidad = true;
		optimum = BT(0, 0, 0);
	}
	else if (algoritmo == "BT-O")
	{
		K = INFTY;
		poda_optimalidad = true;
		poda_factibilidad = false;
		optimum = BT(0, 0, 0);
	}
	else if (algoritmo == "DP")
	{
		// Precomputamos la solucion para los estados.
		M = vector<vector<int>>(n+1, vector<int>(W+1, UNDEFINED));
		for (int i = 0; i < n+1; ++i)
			for (int j = 0; j < W+1; ++j)
				PD(i, j);

		// Obtenemos la solucion optima.
		optimum = PD(0, 0);
	}
	auto end = chrono::steady_clock::now();
	double total_time = chrono::duration<double, milli>(end - start).count();

	// Imprimimos el tiempo de ejecución por stderr.
	clog << total_time << endl;

    // Imprimimos el resultado por stdout.
    cout << (optimum == INFTY ? -1 : optimum) << endl;
    return 0;
}