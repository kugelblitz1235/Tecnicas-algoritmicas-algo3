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

int INFTY = -10e6; // Valor para indicar que no hubo solución.
int ADYACENTE = -2;

// Información de la instancia a resolver.
int cantidad_locales, limite_contagio;
// n = cantidad de locales
// M = valor del límite de contagio
vector<pair<int, int>> locales;
/*
NPM(i, c):	   		//i := indice del local a seleccionar
				   	//L := conjunto de locales comienza {l1,..,ln}
				   	//c := contagio obtenido, comienza en 0
				   	//M := limite de contagio
	si i >= n :
		retorno c<=M ? 0 : -INFTY
	
	sino :
		retorno max (NPM(i+1, S, c,b), NPM(i+2, c + ln.contagio) + li.beneficio)
	 
*/

//=================================FUERZA BRUTA=================================================
// i : es el local que estamos evaluando actualmente
// contagio : es el valor de contagio acumulado hasta el momento
int NPM_fb(int i, int contagio){ //es llamada como NPM(0, 0);
	// Caso base.
	if (i >= cantidad_locales)
		return contagio <= limite_contagio? 0 : INFTY;
		
	// Recursión:
	return max(NPM_fb(i + 1, contagio), NPM_fb(i + 2, contagio + locales[i].second) + locales[i].first);
}
//=================================BACKTRACKING=================================================
bool poda_factibilidad = true; // define si la poda por factibilidad esta habilitada.
bool poda_optimalidad = true; // define si la poda por optimalidad esta habilitada.
int mayor_beneficio = INFTY; // Mayor beneficio obtenido hasta el momento
// i : es el local que estamos evaluando actualmente
// contagio : es la cantidad de contagio acumulada hasta este momento
// beneficio : es la cantidad de beneficio acumulado hasta este momento, lo vamos a utilizar para la poda de optimalidad
int NPM_bt(int i, int contagio, int beneficio)
{
	// Caso base.
	// en la misma guarda realizamos la poda por factibilidad
    if (i >= cantidad_locales || (poda_factibilidad && contagio > limite_contagio))  {		
		if (contagio <= limite_contagio) // actualizo el mayor beneficio si es un resultado valido
			mayor_beneficio = max(mayor_beneficio, beneficio);
        return contagio <= limite_contagio ? beneficio : INFTY;
	}
    // Poda por optimalidad.
    if (poda_optimalidad){
		// Recorremos todos los locales que nos faltan y sumamos sus beneficios
		int suma_faltantes = beneficio;
		for (int j = i; j < cantidad_locales && suma_faltantes <= mayor_beneficio; j++){
			suma_faltantes += locales[j].first;
		}
		//DBG(mayor_beneficio);
		//DBG(suma_faltantes); 
		//si la suma total no supera el mayor beneficio obtenido hasta ahora, podamos la rama
		if(suma_faltantes <= mayor_beneficio) 
			return INFTY;
	} 
	//Recursion
	return max(NPM_bt(i + 1, contagio,beneficio), NPM_bt(i + 2, contagio + locales[i].second,beneficio+locales[i].first));
}


//=================================PROGRAMACION DINAMICA=================================================
vector<vector<int>> memo; // Memoria de PD de dimensión (cantidad_locales)x(suma_contagio + 1)
const int UNDEFINED = -1; //inicializamos la estructura con -1
// i : es el local que estamos evaluando actualmente
// contagio : es el valor de contagio acumulado hasta el momento
int NPM_pd(int i, int contagio)
{
	// Caso base.
	if (i >= cantidad_locales || contagio > limite_contagio)
		return contagio <= limite_contagio? 0 : INFTY;
	
	if(memo[i][contagio] == UNDEFINED){
		// Recursión:
		memo[i][contagio] = max(NPM_pd(i + 1, contagio), NPM_pd(i + 2, contagio + locales[i].second) + locales[i].first);
	}
	return memo[i][contagio];
}

// Recibe por parámetro qué algoritmos utilizar para la ejecución separados por espacios.
// Imprime por clog la información de ejecución de los algoritmos.
// Imprime por cout el resultado de algun algoritmo ejecutado.
int main(int argc, char** argv)
{
	// Leemos el parametro que indica el algoritmo a ejecutar.
	map<string, string> algoritmos_implementados = {
		{"FB", "Fuerza Bruta"}, {"BT", "Backtracking con podas"}, {"BT-F", "Backtracking con poda por factibilidad"}, 
		{"BT-O", "Backtracking con poda por optimalidad"},{"DP", "Programacion dinámica"}
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
	int beneficio_optimo = INFTY;
	auto start = chrono::steady_clock::now();
	if (algoritmo == "FB")
	{   
		//O(2^n)
		beneficio_optimo = NPM_fb(0,0);
	}
	else if (algoritmo == "BT")
	{
		//O(n 2^n)
		mayor_beneficio = INFTY;
		poda_optimalidad = true;
		poda_factibilidad = true;
		beneficio_optimo = NPM_bt(0, 0,0);
	}
	else if (algoritmo == "BT-F")
	{	
		
		mayor_beneficio = INFTY;
		poda_optimalidad = false;
		poda_factibilidad = true;
		beneficio_optimo = NPM_bt(0, 0,0);
	}
	else if (algoritmo == "BT-O")
	{
		mayor_beneficio = INFTY;
		poda_optimalidad = true;
		poda_factibilidad = false;
		beneficio_optimo = NPM_bt(0, 0,0);
	}
	else if (algoritmo == "DP")
	{
		// inicializamos la estructura de memoizacion
		memo = vector<vector<int>>(cantidad_locales, vector<int>(limite_contagio + 1, UNDEFINED));
		//precomputamos los valores 
		for(int i = cantidad_locales-1; i >= 0; i--)
			for(int j = limite_contagio ; j >=0  ; j--)
				NPM_pd(i, j);
		//obtenemos el beneficio optimo O(n*M)
		beneficio_optimo = NPM_pd(0, 0);
	}
	auto end = chrono::steady_clock::now();
	double total_time = chrono::duration<double, milli>(end - start).count();

	// Imprimimos el tiempo de ejecución por stderr.
	clog << total_time << endl;

    // Imprimimos el resultado por stdout.
    cout << (beneficio_optimo == INFTY ? -1 : beneficio_optimo) << endl;
    return 0;
}
