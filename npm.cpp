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
int ADYACENTE = -2;

// Información de la instancia a resolver.
int cantidad_locales, limite_contagio;
// n = cantidad de locales
// M = valor del límite de contagio
vector<pair<int, int>> locales;

//=================================FUERZA BRUTA=================================================

// i : es el local que estamos evaluando actualmente
// contagio : es la cantidad de contagio acumulada hasta este momento
//beneficio : es la cantidad de beneficio acumulado hasta este momento
//ultimo_seleccionado : es el indice del ultimo local seleccionado, se utiliza para saber si hay un elemento adyacente seteando su valor en la variable global ADYACENTE
int NPM_fb(int i, int contagio, int beneficio ,int ultimo_seleccionado){ //es llamada como NPM(cantidad_locales-1, 0, -1, false);
	// Caso base.
	// si ya seleccioné todos los locales	
	if (i == -1)  
		return contagio <= limite_contagio && ultimo_seleccionado != ADYACENTE? beneficio : INFTY;

	// Recursión.
	// no agrego un local al conjunto de seleccionados
	int noAgregoLocal = NPM_fb(i - 1, contagio,beneficio,ultimo_seleccionado); //no suma contagio ni cambia el ultimo seleccionado
	// agrego un local al conjunto de seleccionados
	// si no es el primer elemento a agregar y no hubo adyacentes hasta ahora, pregunto si i+1 == ultimo_seleccionado
	if (i != cantidad_locales-1 && ultimo_seleccionado != ADYACENTE) 
		ultimo_seleccionado = i+1 == ultimo_seleccionado? ADYACENTE : i;
	int agregoLocal = NPM_fb(i - 1, contagio + locales[i].second, beneficio+locales[i].first, ultimo_seleccionado);
	return max(noAgregoLocal, agregoLocal);
}

/*
NPM(L,S,c,b):	   //S := el conjunto de locales seleccionados, comienza en vacío
				   //L := conjunto de locales comienza {l1,..,ln}
				   //c := contagio obtenido, comienza en 0
				   //b := beneficio obtenido, comienza en 0
	si L esta vacio :
		retorno c<=M y no existe li en S tal que li+1 o li-1 esté en S ? b : -INFTY
	
	sino :
		retorno max (NPM({l1,..,ln-1}, S, c,b), NPM({l1,..,ln-1}, S u {ln}, c + ln.contagio,b + ln.beneficio))
	 
*/

// i : es el local que estamos evaluando actualmente
// contagio : es la cantidad de contagio acumulada hasta este momento
//beneficio : es la cantidad de beneficio acumulado hasta este momento
//ultimo_seleccionado : es el indice del ultimo local seleccionado, se utiliza para saber si hay un elemento adyacente seteando su valor en la variable global ADYACENTE
int NPM_fb2(int i, int suma_contagio, int suma_beneficio, vector<bool> seleccionados){
	//Caso base.
	if ( i == cantidad_locales ) {
		int j = 0;
		while (j < cantidad_locales - 1 && !(seleccionados[j] && seleccionados[j+1])) 
			j++;
		return suma_contagio <= limite_contagio && j == cantidad_locales - 1 ? suma_beneficio : INFTY;
	}
	// Recursión.
	int noAgregoLocal = NPM_fb2(i + 1, suma_contagio, suma_beneficio ,seleccionados);
	seleccionados[i] = true;
	int agregoLocal = NPM_fb2(i + 1, suma_contagio + locales[i].second, suma_beneficio + locales[i].first , seleccionados) ;
	
	return max(noAgregoLocal, agregoLocal);

}

int NPM_fb3(int i, vector<pair<int, int>> locales_seleccionados){
	//Caso base.
	if ( i == cantidad_locales ) {
		int j = 0;
		int suma_beneficio = 0;
		int suma_contagio = 0;
		while (j < cantidad_locales - 1 && !(locales_seleccionados[j].first !=0 && locales_seleccionados[j+1].first != 0)){ 
			suma_beneficio += locales_seleccionados[j].first;
			suma_contagio += locales_seleccionados[j].second;
			j++;
		}
		return suma_contagio <= limite_contagio && j == cantidad_locales - 1 ? suma_beneficio : INFTY;
	}
	else {
		// Recursión.
		int noAgregoLocal = NPM_fb3(i + 1, locales_seleccionados);
		locales_seleccionados[i] = locales[i];
		int agregoLocal = NPM_fb3(i + 1, locales_seleccionados)  ;
		return max(noAgregoLocal, agregoLocal);
	}
}
//=================================BACKTRACKING=================================================
bool poda_factibilidad = true; // define si la poda por factibilidad esta habilitada.
bool poda_optimalidad = true; // define si la poda por optimalidad esta habilitada.
int mayor_beneficio = INFTY; // Mejor solucion hasta el momento.
// i : es el local que estamos evaluando actualmente
// contagio : es la cantidad de contagio acumulada hasta este momento
//beneficio : es la cantidad de beneficio acumulado hasta este momento
//ultimo_seleccionado : es el indice del ultimo local seleccionado, se utiliza para saber si hay un elemento adyacente seteando su valor en la variable global ADYACENTE
int NPM_bt(int i, int contagio, int beneficio, int ultimo_seleccionado)
{
	// Caso base.
    if ( i == -1 )  {		
		// actualizo el mayor beneficio si corresponde
    	if (contagio <= limite_contagio && ultimo_seleccionado != ADYACENTE) 
			mayor_beneficio = max(mayor_beneficio, beneficio);
        return contagio <= limite_contagio && ultimo_seleccionado != ADYACENTE ? beneficio : INFTY;
	}

    // Poda por factibilidad.
    if (poda_factibilidad && (contagio > limite_contagio || ultimo_seleccionado == ADYACENTE)) 
		return INFTY;

    // Poda por optimalidad.
    if (poda_optimalidad){
		// Recorremos todos los locales que nos faltan y sumamos sus beneficios
		int suma_faltantes = beneficio;
		for (int j = i; j >=0; j--) suma_faltantes += locales[j].first;
		//si la suma total no supera el mayor beneficio obtenido hasta ahora, podamos la rama
		if(suma_faltantes <= mayor_beneficio) 
			return INFTY;
	} 

    int noAgregoLocal = NPM_bt(i - 1, contagio,beneficio, ultimo_seleccionado);//no suma contagio ni cambia el ultimo seleccionado
	//agrego un local al conjunto de seleccionados
	//si no es el primer elemento a agregar y no hubo adyacentes hasta ahora, pregunto si i+1 == ultimo_seleccionado
	if(i != cantidad_locales-1 && ultimo_seleccionado != ADYACENTE) 
		ultimo_seleccionado = i + 1 == ultimo_seleccionado? ADYACENTE : i;
	int agregoLocal = NPM_bt(i - 1, contagio + locales[i].second,beneficio+locales[i].first, ultimo_seleccionado);
	return max(noAgregoLocal, agregoLocal);
}
int NPM_bt2(int i, int suma_contagio, vector<int> beneficio_seleccionados)
{
	// Caso base.
    if (i == cantidad_locales) {
		//chequeamos que no haya adyacentes
		int j = 0;
		int suma_beneficio = 0;
		while (j < cantidad_locales - 1 && !(beneficio_seleccionados[j] != 0 && beneficio_seleccionados[j+1] != 0)){
			suma_beneficio += beneficio_seleccionados[j];
			j++;
		}
		//actualizamos la maxima solución global si es solución
		if (suma_contagio <= limite_contagio && j == cantidad_locales - 1) 
			mayor_beneficio = max(mayor_beneficio, suma_beneficio);
        return suma_contagio <= limite_contagio && j == cantidad_locales - 1 ? suma_beneficio : INFTY;
    }

    // Poda por factibilidad.
    if (poda_factibilidad && suma_contagio > limite_contagio ) return INFTY;

    // Poda por optimalidad.
    if (poda_optimalidad) {
		// Recorremos todos los locales que nos faltan y sumamos sus beneficios
		int suma_faltantes = 0;
		for (int j = 0 ; j < cantidad_locales ; j+=2){
			if(j < i)
				suma_faltantes += beneficio_seleccionados[j];
			else 
				suma_faltantes += locales[j].first;
		}
		if(suma_faltantes <= mayor_beneficio)
			return INFTY;
	} 
	

    // Recursión.
	int noAgregoLocal = NPM_bt2(i + 1, suma_contagio, beneficio_seleccionados);
	beneficio_seleccionados[i] = locales[i].first;
	int agregoLocal = NPM_bt2(i + 1, suma_contagio + locales[i].second, beneficio_seleccionados);
	DBG(agregoLocal);
	DBG(noAgregoLocal);
	return max(noAgregoLocal, agregoLocal);
	
}

//reducimos la complejidad temporal
int NPM_bt3(int i, pair<int, int> suma_locales, vector<bool> seleccionados, bool adyacente)
{
	// Caso base.
    if (i == cantidad_locales)
    {
    	if (suma_locales.second <= limite_contagio && !adyacente) 
			mayor_beneficio = max(mayor_beneficio, suma_locales.first);
        return suma_locales.second <= limite_contagio && !adyacente ? suma_locales.first : INFTY;
	}

    // Poda por factibilidad.
    if (poda_factibilidad && suma_locales.second > limite_contagio && adyacente) return INFTY;

    // Poda por optimalidad.
    if (poda_optimalidad){
		// Recorremos todos los locales que nos faltan y sumamos sus beneficios
		int suma_faltantes = suma_locales.first;
		for (int j = i ; j < cantidad_locales ; j+=2)
			suma_faltantes += locales[j].first;
		if(suma_faltantes <= mayor_beneficio)
			return INFTY;
	} 

    // Recursión.
	int noAgregoLocal = NPM_bt3(i + 1, suma_locales, seleccionados, adyacente);
	seleccionados[i] = true;
	int agregoLocal = NPM_bt3(i + 1, make_pair(suma_locales.first + locales[i].first , suma_locales.second + locales[i].second), seleccionados, (i != 0 && !adyacente) ? seleccionados[i-1] : adyacente);		
	return max(noAgregoLocal, agregoLocal);
}

//=================================PROGRAMACION DINAMICA=================================================
vector<vector<int>> memo; // Memoria de PD de dimensión (cantidad_locales)x(suma_contagio + 1)
const int UNDEFINED = -1; //inicializamos la estructura con -1
// i : es el local que estamos evaluando actualmente
// contagio : es la cantidad de contagio acumulada hasta este momento
//beneficio : es la cantidad de beneficio acumulado hasta este momento
//ultimo_seleccionado : es el indice del ultimo local seleccionado, se utiliza para saber si hay un elemento adyacente seteando su valor en la variable global ADYACENTE
int NPM_pd(int i, int contagio, int beneficio, int ultimo_seleccionado)
{
	// Caso base
    if (i == -1 || ultimo_seleccionado == ADYACENTE || contagio > limite_contagio)  
		return contagio <= limite_contagio && ultimo_seleccionado != ADYACENTE ? beneficio : INFTY;
	
	if(memo[i][contagio] == UNDEFINED){
		//si la instacia no fue calculada, la calculo
		int noAgregoLocal = NPM_pd(i - 1, contagio, beneficio, ultimo_seleccionado);//no suma contagio ni cambia el ultimo seleccionado
		//agrego un local al conjunto de seleccionados
		//si no es el primer elemento a agregar y no hubo adyacentes hasta ahora, pregunto si i+1 == ultimo_seleccionado
		if(i != cantidad_locales - 1 && ultimo_seleccionado != ADYACENTE) 
			ultimo_seleccionado = i + 1 == ultimo_seleccionado? ADYACENTE : i;
		int agregoLocal = NPM_pd(i - 1, contagio + locales[i].second, beneficio + locales[i].first, ultimo_seleccionado);
		memo[i][contagio] = max(noAgregoLocal, agregoLocal);
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
		{"BT-O", "Backtracking con poda por optimalidad"},{"BT2", "Backtracking alternativo con podas"} ,{"DP", "Programacion dinámica"},
		{"DP-C", "Programacion dinámica con valores precoputados"}
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
	{//solucion optima indicando con un entero el ultimo elemento seleccionado para saber si hay adyacencia O(2^n)
		beneficio_optimo = NPM_fb(cantidad_locales-1, 0,0,-1);
	}
	if (algoritmo == "FB2")
	{//solucion con complejidad espacial media, guardando solo cuales elementos fueron seleccionados
		beneficio_optimo = NPM_fb2(0,0,0,vector<bool>(cantidad_locales,false));
	}
	if (algoritmo == "FB3")
	{//solucion con mayor complejidad espacial, guardando toda la informacion de cada local seleccionado
		beneficio_optimo = NPM_fb3(0, vector<pair<int, int>>(cantidad_locales,make_pair(0,0)));
	}
	else if (algoritmo == "BT")
	{
		//solucion con menos complejidad espacial, utilizando un entero para indicar la adyacencia O(n 2^n)
		mayor_beneficio = INFTY;
		poda_optimalidad = true;
		poda_factibilidad = true;
		beneficio_optimo = NPM_bt(cantidad_locales-1, 0,0,-1);
	}
	else if (algoritmo == "BT2")
	{	
		//solucion acumulando los beneficios para chequear adyacencia O(n^2 2^n)
		mayor_beneficio = INFTY;
		poda_optimalidad = poda_factibilidad = true;
		beneficio_optimo = NPM_bt2(0, 0, vector<int>(cantidad_locales, 0));
	}
	else if (algoritmo == "BT3")
	{
		//solucion con un conjunto que indica las posiciones y una variable booleana que indica adyacencia O(n 2^n)
		mayor_beneficio = INFTY;
		poda_optimalidad = poda_factibilidad = true;
		beneficio_optimo = NPM_bt3(0, make_pair(0,0), vector<bool>(cantidad_locales, false),false);
	}
	else if (algoritmo == "BT-F")
	{	
		//O(2^n)
		mayor_beneficio = INFTY;
		poda_optimalidad = false;
		poda_factibilidad = true;
		beneficio_optimo = NPM_bt(cantidad_locales-1, 0,0,-1);
	}
	else if (algoritmo == "BT-O")
	{
		mayor_beneficio = INFTY;
		poda_optimalidad = true;
		poda_factibilidad = false;
		beneficio_optimo = NPM_bt(cantidad_locales - 1, 0,0,-1);
	}
	else if (algoritmo == "DP-C")
	{
		// inicializamos la estructura de memoizacion
		memo = vector<vector<int>>(cantidad_locales, vector<int>(limite_contagio + 1, UNDEFINED));
		//precomputamos los valores 
		for(int i = cantidad_locales-1; i >= 0; i--)
			for(int j = 0 ; j < limite_contagio+1 ; j++)
				NPM_pd(i, j, 0, -1);
		//obtenemos el beneficio optimo O(n*M)
		beneficio_optimo = NPM_pd(cantidad_locales - 1, 0, 0, -1);
	}
	else if (algoritmo == "DP")
	{
		// inicializamos la estructura de memoizacion
		memo = vector<vector<int>>(cantidad_locales, vector<int>(limite_contagio + 1, UNDEFINED));
		//obtenemos el beneficio optimo sin precomputar O(2^n)
		beneficio_optimo = NPM_pd(cantidad_locales - 1, 0, 0, -1);
	}
	auto end = chrono::steady_clock::now();
	double total_time = chrono::duration<double, milli>(end - start).count();

	// Imprimimos el tiempo de ejecución por stderr.
	clog << total_time << endl;

    // Imprimimos el resultado por stdout.
    cout << (beneficio_optimo == INFTY ? -1 : beneficio_optimo) << endl;
    return 0;
}
