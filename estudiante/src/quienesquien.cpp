#include "../include/quienesquien.h"
#include <sstream>
#include <iostream>
#include <iterator>
#include <math.h>
#include <algorithm>

QuienEsQuien::QuienEsQuien(){
}

QuienEsQuien::QuienEsQuien(const QuienEsQuien &quienEsQuien){
}

QuienEsQuien& QuienEsQuien::operator= (const QuienEsQuien &quienEsQuien){
     return *this;
}

QuienEsQuien::~QuienEsQuien(){
     this->limpiar() ;
}

void QuienEsQuien::limpiar(){
     this->personajes.clear();
     this->atributos.clear();
     this->personajes_images.clear();
     for(vector<vector<bool>>::iterator it_tablero = this->tablero.begin();
         it_tablero != this->tablero.end();
         it_tablero++){
          (*it_tablero).clear();
     }
     this->tablero.clear();
     this->arbol.clear() ;
}

template <typename T>
std::ostream& operator<<  (std::ostream& out, const std::vector<T>& v) {
     if ( !v.empty() ) {
          out <<  '[' ;
          std::copy (v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
          out <<  "\b\b]";
     }

     return out;
}

void QuienEsQuien::mostrar_estructuras_leidas(){
     cout <<  "personajes: " <<  (this->personajes) <<  endl;
     cout <<  "atributos: " <<  (this->atributos) <<  endl;
     cout <<  "tablero:    " <<  endl;
     // Escribe la cabecera del tablero
     for(vector<string>::iterator it_atributos = this->atributos.begin() ;
         it_atributos != this->atributos.end() ;
         it_atributos++){
          cout <<  *it_atributos << "\t";
     }
     cout << endl;

     int indice_personaje = 0;
     for(vector<vector<bool>>::iterator it_tablero_atributos = tablero.begin();
         it_tablero_atributos!= tablero.end();
         it_tablero_atributos++){
          string personaje = this->personajes[indice_personaje];
          int indice_atributo = 0;
          for(vector<bool>::iterator it_atributos_personaje = (*it_tablero_atributos).begin();
              it_atributos_personaje != (*it_tablero_atributos).end() ;
              it_atributos_personaje++){
               cout <<  *it_atributos_personaje<< "\t";
               indice_atributo++;
          }
          cout <<  personaje <<  endl;
          indice_personaje++;
     }
}

/**
  * @brief Devuelve una copia de la cadena original sin las subcadenas no deseadas.
  *
  * @param cadena_original Cadena de la que se eliminan las subcadenas no deseadas.
  * @param cadena_a_eliminar Subcadena que se busca y se elimina.
  *
  * @return Copia de la cadena original sin las subcadenas no deseadas.
  */


string limpiar_string(string cadena_original,string cadena_a_eliminar){
     string linea(cadena_original);
     while(linea.find_first_of(cadena_a_eliminar) != std::string::npos){
          linea.erase(linea.find_first_of(cadena_a_eliminar),cadena_a_eliminar.length());
     }

     return linea;
}

istream& operator >>  (istream& is, QuienEsQuien &quienEsQuien) {
     quienEsQuien.limpiar();
     if(is.good()){ //Lee la linea con los nombres de las columnas
          string linea;
          getline(is, linea, '\n');
          linea = limpiar_string(linea,"\r");
          bool salir=false;
          while(!salir && linea.find('\t') != string::npos ){
               string atributo = linea.substr(0,linea.find('\t'));
               quienEsQuien.atributos.push_back(atributo);
               linea = linea.erase(0,linea.find('\t')+1);
               if (linea.substr(0,linea.find('\t'))=="Nombre personaje")
                   salir=true;
          }
     }
     //LEE los atributos para cada personaje
     while( is.good()) {
          string linea;
          getline(is, linea, '\n');
          linea = limpiar_string(linea,"\r");

          //Si la linea contiene algo extrae el personaje.
          //Si no la ignora.
          if(linea != ""){;
               vector<bool> atributos_personaje;
               int indice_atributo=0;
               bool salir=false;
               while(!salir && linea.find('\t') != string::npos){
                   string valor = linea.substr(0,linea.find('\t'));
                   if (valor == "0" || valor == "1"){
                    bool valor_atributo = valor == "1";
                    atributos_personaje.push_back(valor_atributo);
                    linea = linea.erase(0,linea.find('\t')+1) ;
                    indice_atributo++;
                   }
                   else{
                       salir=true;
                   }
               }
               string nombre_personaje;
               string imagen_personaje;
               if (linea.find('\t') == string::npos){
                nombre_personaje = linea;
                imagen_personaje=""; //no tiene imagen
               }
               else{
                nombre_personaje=linea.substr(0,linea.find('\t'));
                linea = linea.erase(0,linea.find('\t')+1) ;
                imagen_personaje=linea;

               }
               quienEsQuien.personajes.push_back(nombre_personaje);
               quienEsQuien.tablero.push_back(atributos_personaje);
               quienEsQuien.personajes_images.push_back(imagen_personaje);
          }
     }

     quienEsQuien.mostrar_estructuras_leidas();
     quienEsQuien.arbol = quienEsQuien.crear_arbol();
     quienEsQuien.tg=nullptr; //puntero al tablero grafico
     quienEsQuien.con=nullptr; //puntero a la consola
     if (quienEsQuien.modo_graph){
        int nper = quienEsQuien.personajes.size();
        int nrows,ncols=3;
        if (nper<8)
           nrows = nper/ncols;
        else{
            ncols=4;
            nrows = nper/ncols;
        }
        //se crea un tablero grafico
        quienEsQuien.tg = new TableroGrafico(400,400,nrows,ncols);
        //ponemos en el tablero grafico las imágenes
        for (int i=0;i<nper;i++){

            quienEsQuien.tg->putImagen(i/ncols,i%ncols,quienEsQuien.personajes_images[i].c_str());
        }
        //creamos la consola
        quienEsQuien.con=new Consola();
     }
     return is;
}

ostream& operator << (ostream& os, const QuienEsQuien &quienEsQuien){
     //Escribimos la cabecera, que contiene los atributos y al final una columna para el nombre

     cout<<"NUmero de Atributos "<<quienEsQuien.atributos.size()<<endl;
     for(vector<string>::const_iterator it_atributos = quienEsQuien.atributos.begin();
         it_atributos != quienEsQuien.atributos.end();
         it_atributos++){
          os << *it_atributos << "\t";
     }

     os << "Nombre personaje" << endl;
     
     //Rellenamos con ceros y unos cada linea y al final ponemos el nombre del personaje.
     for(int indice_personaje=0;indice_personaje<quienEsQuien.personajes.size();indice_personaje++){
          for(int indice_atributo=0;indice_atributo<quienEsQuien.atributos.size();indice_atributo++){
               os << quienEsQuien.tablero[indice_personaje][indice_atributo] << "\t";
          }
          os << quienEsQuien.personajes[indice_personaje] << endl;
     }
     
     return os;
}

/**
* @brief Convierte un numero a un vector de bool que corresponde
*        con su representadon en binario con un numero de digitos
*              f i j o.
*
* @param n Numero a convertir en binario.
* @param digitos Numero de digitos de la representaddn binaria.
*
* @return Vector de booleanos con la representadon en binario de @e n
*      con el numero de elementos especificado por @e digitos.
*/
vector<bool> convertir_a_vector_bool(int n, int digitos) {
     vector<bool> ret;
     while(n) {
          if (n&1)
               ret.push_back(true);
          else
               ret.push_back(false);

          n>>=1;
     }

     while(ret.size()<digitos)
          ret.push_back(false);

     reverse(ret.begin(),ret.end());
     return ret;
}

/**
* @brief Esta es una propuesta de cabecera de la funcion para crear el arbol.
*/

bintree<Pregunta> QuienEsQuien::crear_arbol(vector<string> atributos,
                                    int indice_atributo,
                                    vector<string> personajes,
                                    vector<bool> personajes_restantes,
                                    vector<vector<bool>> tablero){

     int personajes_restantes_count = 0;
     int personajes_last_index = -1;
     for(int i = 0; i < personajes_restantes.size(); i++){
          if (personajes_restantes[i]){
               personajes_restantes_count++;
               personajes_last_index = i;
          }
     }
     
     if (personajes_restantes_count == 0){
         return bintree<Pregunta>();
     }
     
     if (personajes_restantes_count == 1){
          return bintree<Pregunta>({personajes[personajes_last_index], 1});
     }
     
     string atributo_actual = atributos[indice_atributo];
     int si_count = 0;
     int no_count = 0;
     vector<bool> si_personajes(personajes.size(), 0);
     vector<bool> no_personajes(personajes.size(), 0);

     for (int i = 0; i < personajes.size(); i++) {
          if (personajes_restantes[i]) {
               if (tablero[i][indice_atributo]) {
                    si_personajes[i] = true;
                    si_count++;
               } else {
                    no_personajes[i] = true;
                    no_count++;
               }
          }
     }
     
     bintree<Pregunta> arbol(Pregunta(atributo_actual, personajes_restantes_count));
     bintree<Pregunta> a_si = crear_arbol(atributos, indice_atributo+1, personajes, si_personajes, tablero);
     bintree<Pregunta> a_no = crear_arbol(atributos, indice_atributo+1, personajes, no_personajes, tablero);
     arbol.insert_left(arbol.root(), a_si);
     arbol.insert_right(arbol.root(), a_no);
     return arbol;
}

float calcular_entropia(int si_count, int no_count) {
    int total = si_count + no_count;
    if (total == 0) return 0;

    float p_si = static_cast<float>(si_count) / total;
    float p_no = static_cast<float>(no_count) / total;

    float entropia = 0;
    if (p_si > 0) entropia -= p_si * log2(p_si);
    if (p_no > 0) entropia -= p_no * log2(p_no);

    return entropia;
}

bintree<Pregunta> QuienEsQuien::crear_arbol_inteligente(vector<string> atributos,
                                    int indice_atributo,
                                    vector<string> personajes,
                                    vector<bool> personajes_restantes,
                                    vector<vector<bool>> tablero){
    
    int personajes_restantes_count = 0;
    int personajes_last_index = -1;
    for (int i = 0; i < personajes_restantes.size(); ++i) {
        if (personajes_restantes[i]) {
            personajes_restantes_count++;
            personajes_last_index = i;
        }
    }

    if (personajes_restantes_count == 0) {
        return bintree<Pregunta>();
    }

    if (personajes_restantes_count == 1) {
        return bintree<Pregunta>({personajes[personajes_last_index], 1});
    }

    float max_entropia = -1;
    int mejor_atributo = -1;

    for (int i = 0; i < atributos.size(); ++i) {
        int si_count = 0;
        int no_count = 0;

        for (int j = 0; j < personajes.size(); ++j) {
            if (personajes_restantes[j]) {
                if (tablero[j][i]) {
                    si_count++;
                } else {
                    no_count++;
                }
            }
        }
        
        float entropia = calcular_entropia(si_count, no_count);

        if (entropia > max_entropia) {
            max_entropia = entropia;
            mejor_atributo = i;
        }
    }

    string atributo_actual = atributos[mejor_atributo];
    vector<bool> si_personajes(personajes.size(), false);
    vector<bool> no_personajes(personajes.size(), false);

    for (int i = 0; i < personajes.size(); ++i) {
        if (personajes_restantes[i]) {
            if (tablero[i][mejor_atributo]) {
                si_personajes[i] = true;
            } else {
                no_personajes[i] = true;
            }
        }
    }

    bintree<Pregunta> arbol(Pregunta(atributo_actual, personajes_restantes_count));
    bintree<Pregunta> a_si = crear_arbol_inteligente(atributos, mejor_atributo + 1, personajes, si_personajes, tablero);
    bintree<Pregunta> a_no = crear_arbol_inteligente(atributos, mejor_atributo + 1, personajes, no_personajes, tablero);

    arbol.insert_left(arbol.root(), a_si);
    arbol.insert_right(arbol.root(), a_no);

    return arbol;
}



void QuienEsQuien::eliminar_nodos_redundantes(bintree<Pregunta>::node nodo, bintree<Pregunta>& arbol) {
    if (nodo.null()) {
        return; 
    }
    
    if (nodo.left().null() && nodo.right().null()) {
        return;
    }
    
    if (!nodo.left().null() && !nodo.right().null()) {
        eliminar_nodos_redundantes(nodo.left(), arbol);
        eliminar_nodos_redundantes(nodo.right(), arbol);
    }

    if (!nodo.left().null() && nodo.right().null()) {
        bintree<Pregunta> subarbol_izquierdo;
        arbol.prune_left(nodo, subarbol_izquierdo);

        if (nodo == arbol.root()) { 
            arbol.assign_subtree(subarbol_izquierdo, subarbol_izquierdo.root());
        } else {
            auto padre = nodo.parent();
            if (padre.left() == nodo) {
                arbol.insert_left(padre, subarbol_izquierdo);
                eliminar_nodos_redundantes(padre.left(), arbol);
            } else {
                arbol.insert_right(padre, subarbol_izquierdo);
                eliminar_nodos_redundantes(padre.right(), arbol);
            }
        }
        return;
    }
   
    if (!nodo.right().null() && nodo.left().null()) {
        bintree<Pregunta> subarbol_derecho;
        arbol.prune_right(nodo, subarbol_derecho);

        if (nodo == arbol.root()) { 
            arbol.assign_subtree(subarbol_derecho, subarbol_derecho.root());
        } else {
            auto padre = nodo.parent();
            if (padre.left() == nodo) {
                arbol.insert_left(padre, subarbol_derecho);
                eliminar_nodos_redundantes(padre.left(), arbol);
            } else {
                arbol.insert_right(padre, subarbol_derecho);
                eliminar_nodos_redundantes(padre.right(), arbol);
            }
        }
        return;
    }
}

void QuienEsQuien::eliminar_nodos_redundantes() {
    if (!arbol.empty()) {
        eliminar_nodos_redundantes(arbol.root(), arbol);
    }
}

bintree<Pregunta> QuienEsQuien::crear_arbol(){
     int indice_atributo = 0;
     vector<bool> personajes_restantes;
     for(vector<string>::iterator it = personajes.begin();it!=personajes.end();it++){
          personajes_restantes.push_back(true);
     }

     return crear_arbol( this->atributos, indice_atributo, this->personajes,
                         personajes_restantes, this->tablero);
}

void QuienEsQuien::usar_arbol(bintree<Pregunta> arbol_nuevo){
     arbol = arbol_nuevo;
}

void QuienEsQuien::iniciar_juego(){
     //TODO :D:D
    Ventana v(tg,con,"WisW");
    if (modo_graph){
        v.show();
    }
    //COMPLETAR AQUI
    
    // Paso 1
    jugada_actual = arbol.root();
    
    // Paso 2
    while ( !jugada_actual.null() && (*jugada_actual).obtener_num_personajes() != 1 ) {
    	cout << (*jugada_actual) << endl;
    	
    	string respuesta = "";
    	
    	cin >> respuesta;
    	
    	if ( respuesta == "Si" && !jugada_actual.left().null()){
    		jugada_actual = jugada_actual.left();
    	}
    	else if ( respuesta == "No" && !jugada_actual.right().null() ){
    		jugada_actual = jugada_actual.right();
    	}
    	
        cout << "\n---------------------------------------------------------\n";
    	cout << preguntas_formuladas(jugada_actual);
        cout << "---------------------------------------------------------\n\n";
    }
    
    // Paso 3
    cout << "Nombre del personaje: " 
         << (*jugada_actual).obtener_personaje() << endl;
    
    if (modo_graph){
     con->WriteText("Cuando completes QuienEsQuien, este mensaje lo podrás quitar");
     char c;
     do{
        con->WriteText("Pulsa 0 para salir");
	c = con->ReadChar();
     
     }while (c!='0');
     
    }
    v.cerrar();
}	

set<string> QuienEsQuien::informacion_jugada(bintree<Pregunta>::node jugada_actual){
     //TODO :)
     set<string> personajes_levantados;
     
     // Paso 1
     if (jugada_actual.null()) return personajes_levantados; // devuelve conjunto vacio
         
     // Paso 2
     if ( jugada_actual.left().null() && jugada_actual.right().null() &&
     		(*jugada_actual).obtener_num_personajes() == 1 ) {
     	personajes_levantados.insert((*jugada_actual).obtener_personaje());
     	return personajes_levantados;
     }
     
     // Paso 3
     set<string> conjuntos_personajes_izquierdo = informacion_jugada(jugada_actual.left()),
     		 conjuntos_personajes_derecho = informacion_jugada(jugada_actual.right());
     		 
     personajes_levantados = conjuntos_personajes_derecho;		 
     
     personajes_levantados.insert(conjuntos_personajes_izquierdo.begin(),
     				  conjuntos_personajes_izquierdo.end());
     
     return personajes_levantados;
}

void escribir_esquema_arbol(ostream& ss, const bintree <Pregunta>& a,
                            bintree<Pregunta>::node n, string& pre){
     if (n.null())
          ss <<  pre <<  "-- x" <<  endl;
     else {
          ss <<  pre <<  "-- " <<  (*n) <<  endl;
          if ( !n.right().null() || !n.left().null()) {// Si no es una hoja
               pre += "   |";
               escribir_esquema_arbol(ss,a, n.right(), pre);
               pre.replace(pre.size()-4, 4, "    ");
               escribir_esquema_arbol (ss,a, n.left(), pre);
               pre.erase(pre.size()-4, 4);
          }
     }
}

void QuienEsQuien::escribir_arbol_completo() const{
     string pre = "";
     escribir_esquema_arbol(cout,this->arbol,this->arbol.root(),pre);
}

list<int> recursion_auxiliar( bintree<Pregunta>::node n) {
	list<int> l;
	
	if ( n.null() ) return l;
	else if ( n.left().null() && n.right().null() ) {
		l.push_back(0);
		return l;
	} else {
		list<int> li = recursion_auxiliar(n.left()), // hijo izq.
			  ld = recursion_auxiliar(n.right()); // hijo der.
			  
		list<int> lu(li);
		lu.insert(lu.end(), ld.begin(), ld.end());
		
		for (list<int>::iterator it = lu.begin(); it != lu.end(); ++it)
			(*it)++;
		
		return lu; // lista union
	}
}

float QuienEsQuien::profundidad_promedio_hojas(){
//TODO :)

	// Paso 1: Recursion con funcion auxiliar
	list<int> lista = recursion_auxiliar(arbol.root());
	
	float valor_medio = 0;
	
	for (list<int>::iterator it = lista.begin(); it != lista.end(); ++it)
		valor_medio += *it;


	return valor_medio / lista.size();

}

/**
* @brief Genera numero enteros positivos aleatorios en el rango [min,max].
**/
int generaEntero(int min, int max){
     int tam= max-min;
     return ((rand( )%tam)+min) ;
}

void QuienEsQuien::tablero_aleatorio(int numero_de_personajes){
     srand(0);
     this->limpiar();
     float log_2_numero_de_personajes = log(numero_de_personajes)/log(2);
     int numero_de_atributos = ceil(log_2_numero_de_personajes);

     cout <<  "Peticion para generar " <<  numero_de_personajes << " personajes ";
     cout <<  "con " << numero_de_atributos <<  " atributos" << endl;
     cout <<  "Paso 1: generar " <<  pow(2, numero_de_atributos) <<  " personajes" << endl;

     //Fase 1: completar el tablero con todos los personajes posibles
     //Complete el tablero y los nombres de personajes a la vez
     for(int indice_personaje=0;indice_personaje< pow(2,numero_de_atributos);indice_personaje++){
          vector<bool> atributos_personaje =
                                    convertir_a_vector_bool(indice_personaje,numero_de_atributos);
          string nombre_personaje = "Personaje_"+to_string(indice_personaje);

          this->personajes.push_back(nombre_personaje);
          this->tablero.push_back(atributos_personaje);
     }

     // Completo los nombres de los atributos.
     for( int indice_atributo=0;indice_atributo<numero_de_atributos;indice_atributo++){
          string nombre_atributo = "Atributo_"+to_string(indice_atributo);
          this->atributos.push_back(nombre_atributo);
     }
     cout <<  "Paso 2: eliminar " << (pow(2,numero_de_atributos)-numero_de_personajes) <<  "personajes" << endl;

     //Fase 2. Borrar personajes aleatoriamente hasta que quedan solo los que hemos pedido.
     while(personajes.size()>numero_de_personajes){
          int personaje_a_eliminar = generaEntero(0,personajes.size());
          personajes.erase(personajes.begin()+personaje_a_eliminar);
          tablero.erase(tablero.begin()+personaje_a_eliminar);
     }
}
void QuienEsQuien::ocultar_personajes_graph(const set<string> &personajes_activos){
    //ocultamos los personajes
    int idx=0;
    int ncols=tg->getNcols();
    for (auto it=personajes.begin();it!=personajes.end();++it,idx++){
        if ( personajes_activos.find(*it)==personajes_activos.end())
            tg->putImagen(idx/ncols,idx%ncols,imagen_ocultar.c_str());
    }
}


void QuienEsQuien::setImagenOcultar(const char * n){
    imagen_ocultar=n;
}

void QuienEsQuien::setModoGraph(bool m){
    modo_graph=m;
}

void QuienEsQuien::elimina_personaje(string nombre) {
	int indice = -1;
	
	// buscamos el indice del personaje para obtener las caract.
	for (int i = 0; i < personajes.size() && indice == -1; i++)
		if ( personajes[i] == nombre ) indice = i;
		
	// caracteristicas del nombre
	vector<bool> caracteristicas = tablero[indice];
	
	// recorremos el arbol hasta encontrar el personaje
	bintree<Pregunta>::node n = arbol.root();
	
	while ( (*n).es_pregunta() ) {
		string pregunta = (*n).obtener_pregunta();
	
		indice = -1;
		
		// buscamos el indice de la pregunta
		for (int i = 0; i < atributos.size() && indice == -1; i++)
			if ( atributos[i] == pregunta ) indice = i;
			
		// true -> mover a la izq. , false -> mover a la dcha.
		if ( caracteristicas[indice] ) n = n.left();
		else n = n.right();
	}
	
	bintree<Pregunta>::node nueva_pregunta = n.parent();
	
	// si el nodo estaba a la izq., el nuevo padre esta a la dcha.
	// y viceversa.
	if ( caracteristicas[indice] ) nueva_pregunta = nueva_pregunta.right();
	else nueva_pregunta = nueva_pregunta.left();
	
	// subarbol nuevo con el nuevo padre
	bintree<Pregunta> nuevo_arbol;
	arbol.assign_subtree(nuevo_arbol,nueva_pregunta);
	
	// insercion del subarbol
	if ( indice > 0 && caracteristicas[indice-1]) 
		arbol.insert_left(n.parent().parent(),nuevo_arbol);
	else arbol.insert_right(n.parent().parent(),nuevo_arbol);
}

void QuienEsQuien::aniade_personaje(string nombre, vector<bool> caracteristicas, bintree<Pregunta>::node n, string nombre_imagen_personaje ) {
	
	//bintree<Pregunta>::node n = arbol.root();

	// Paso 1
	if (n.null()) {
		Pregunta pregunta(nombre,1);
		arbol = bintree<Pregunta>(pregunta);
		return;
	} // Paso 2
	else if (n.left().null() && n.right().null() && (*n).es_personaje()) {
		
		int indice = -1; // indice para encontrar al vector de n
		
		// se asume que esta dentro del vector
		for ( int i = 0; i < personajes.size() && indice == -1; i++) {
			if ( (*n).obtener_personaje() == personajes[i] )
				indice = i;
		}
		
		vector<bool> car_per = tablero[indice];
		
		indice = -1; // indice ahora es para la pregunta
		
		// se asume mismo tamanio
		for ( int i = 0; i < car_per.size() && indice == -1; i++) {
			if ( car_per[i] != caracteristicas[i] )
				indice = i;
		}
		
		Pregunta pregunta_distinta(atributos[indice],2); // 2 por defecto
		
		bintree<Pregunta> nuevo_arbol(pregunta_distinta);
		bintree<Pregunta>::node nuevo(Pregunta(nombre,1)),
					father(n.parent());
					
		int lado = -1; // 0 -> izq. 1 -> dcha.
		
		if ( father.left() == n ) lado = 0;
		else lado = 1;
		
		// insertamos hijos izquierda y derecha
		if ( car_per[indice] ) {
			nuevo_arbol.insert_right(n,(*n));
			nuevo_arbol.insert_left(nuevo, (*nuevo));
		} else {
			nuevo_arbol.insert_left(n,(*n));
			nuevo_arbol.insert_right(nuevo, (*nuevo));
		}
		
		if (lado) arbol.insert_right(father, nuevo_arbol);
		else arbol.insert_left(father, nuevo_arbol);
	} else { // Paso 3
		int indice = -1;
		
		for (int i = 0; i < atributos.size() && indice == -1; i++)
			if ( (*n).obtener_pregunta() == atributos[i] ) indice = i;
		
		if (caracteristicas[indice]) 
			aniade_personaje(nombre,caracteristicas,
						n.left(), nombre_imagen_personaje);
		else aniade_personaje(nombre,caracteristicas,
						n.right(), nombre_imagen_personaje);
	}
}

string QuienEsQuien::preguntas_formuladas(bintree<Pregunta>::node jugada){
    vector<string> history;

    while (!jugada.parent().null()) {
        auto padre = jugada.parent();
        string respuesta;

        if (padre.left() == jugada){
            respuesta = "si";
        }else{
            respuesta = "no";
        }
        
        Pregunta pregunta_padre = *padre;
        history.push_back(pregunta_padre.obtener_pregunta() + " - " + respuesta);

        jugada = padre;
    }
   
    string descripcion = "El personaje oculto tiene las siguientes caracteristicas:\n";
    for (auto it = history.rbegin(); it != history.rend(); ++it) {
        descripcion += *it + "\n";
    }

    return descripcion;    
}
