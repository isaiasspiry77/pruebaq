#include <GL/glut.h>

#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#define p printf
//ESTE ES EL ARBOL B+ CON CARACTERISTICAS DEL PROYECTO DEL PRIMER PARCIAL:

/*
* Inserte 
* elimine
- fusione
* Busque 
* en modo grafico
* muestre el camino de busqueda.
- de manera animada realice las operaciones.
* El dato de los nodos debe contener informacion de libros en una biblioteca
* los datos deben ser consultados por el clic en el nodo.
* Medir el tiempo de busqueda.
*/

//variables globales para lo grafico
int VPadre=0,M,VHijo, pos=0, renglon=0, nivel=1, posXDibujo=170, VHijo2;
char mostrar[50],letID[100]="",let2[100]="",let3[100]="", label[100]="", instrucciones[100]="";
float largo=140 ,ancho=60, xline=0, yline=0;

//variables para medir el tiempo de ejecucion de la busqueda
clock_t t_ini, t_fin;
double secs;

int id_lib;
char tit_libro[100], desc_libro[100];

//Dibujar Pagina
int TAMpag,espacio,space,xPagina, Xpag3, TAMpag3, espacio3, space3;

typedef struct pagina{
	int orden; //contador de elementos que abra en cada pagina 
	struct pagina *izq;
	struct list *inicio;
	struct coordenadas *cord;
}Pagina;

typedef struct nodo{
	int clave;	//clave o ID del libro
	char titulo[50];//titulo del libro
	char descripcion[100];//descripcion del libro
	bool color;
	struct pagina *der;
	struct coordenadas *centro;
}Nodo;

typedef struct list{
	struct nodo *raiz;
	struct list *sig;
}Lista;

typedef struct coordenadas{
	float x;
	float y;
};

Pagina *raiz=NULL;

Pagina* CreaPagina(){
	Pagina *nuevo=(Pagina*)malloc(sizeof(Pagina));
	if(!nuevo) return NULL;
	
	nuevo->inicio=NULL;
	nuevo->izq=NULL;
	nuevo->orden=0;
	
	nuevo->cord = (coordenadas*) malloc(sizeof(coordenadas));//reservas memorias para las coordenadas
	if(!nuevo->cord) return NULL;
	
	nuevo->cord->x=500.0;
	nuevo->cord->y=80.0;
	
	return nuevo;
}

Nodo* CreaNodo(int id, char titu[], char descrip[]){
	Nodo *nuevo=(Nodo*) malloc (sizeof(Nodo));
	if(!nuevo) return NULL;
	
	nuevo->clave=id;
	strcpy(nuevo->titulo,titu);
	strcpy(nuevo->descripcion,descrip);
	nuevo->der=NULL;
	nuevo->color=false;
	
	nuevo->centro= (coordenadas*) malloc (sizeof(coordenadas));
	if(!nuevo->centro) return NULL;
	
	nuevo->centro->x=0.0;
	nuevo->centro->y=0.0;
	
	return nuevo;
}

int CreaLista(Lista **inicio, int id, char titu[], char descrip[]){
	if(!(*inicio) || (*inicio)->raiz->clave>id){ //si no hay una lista de nodos o si el dato del nodo existente es menor al dato nuevo se crea un nueva 
		Lista *nuevo=(Lista*)malloc(sizeof(Lista));//rama de la lista y se le cuelga un nodo con clave   
		if(!nuevo) return 0;
		
		Nodo *aux= CreaNodo(id,titu,descrip);
		if(!aux) return 0;
		
		nuevo->raiz=aux;
		nuevo->sig=(*inicio);
		(*inicio)=nuevo;
		return 1;
	}
	return CreaLista(&(*inicio)->sig,id,titu,descrip);
}

Lista* Busca_Nodo_Lista(Lista **inicio, int dmitad){
	if(!inicio) return NULL;
	if(dmitad ==0){
		Lista *aux= (*inicio);//creo una apuntador de tipo lista el cual apuntara al nodo de enmedio
		(*inicio)=NULL; //la flecha inicio que apunta al nodo de enmedio la desligo dejando a los datos restantes siendo apuntados por aux
		return aux; //retorno aux
	}
	return Busca_Nodo_Lista(&(*inicio)->sig, dmitad-1);
}

Lista* Busca_NodoIndice_Insertar(Lista **inicio, int db){
	if(!(*inicio)->sig) return (*inicio); //si no tiene un sig quiere decir que es el unico indice en donde se puede insertar el dato
	//si el dato nuevo es mayor al dato del nodo de la pagina indice y a la ves el dato nuevo es menor al siguiente dato de la pagina indice regresa inicio
	if((*inicio)->raiz->clave < db && db < (*inicio)->sig->raiz->clave) return (*inicio);
	//si no se cumple la condicion de arriba se buscara en el siguiente dato de la pagina de indices
	return Busca_NodoIndice_Insertar(&(*inicio)->sig,db);
}

int DividePagina(Pagina **raiz, int dmitad){
	//se buscara el nodo de enmedio el cual se ara una copia para subirlo como indice
	Lista *aux=Busca_Nodo_Lista(&(*raiz)->inicio,dmitad);
	if(!aux) return 0;
	char caracter[6]="vacio";
	//se crean las 2 paginas que contendran a las nuevas claves
	Pagina *pag1= CreaPagina();
	if(!pag1) return 0;
	Pagina *pag2=CreaPagina();
	if(!pag2) return 0;
	
	//la pagina 1 sera la raiz y la raiz sera la pagina que estara a asu izquierda ya que contendra a los valores menores a la raiz
	pag1->izq=(*raiz);
	//se crea una nueva cajita de lista la cual contendra la copia del nodo de enmedio
	int i=CreaLista(&pag1->inicio, aux->raiz->clave,caracter,caracter);
	if(i==0) return 0;
	//el apuntador a lista de la pagina 2 sera lo que contenga aux  
	pag2->inicio=aux;	
	//como la pagina 1 sera la nueva raiz, a la derecha de su nodo se le colgara la vieja raiz
	pag1->inicio->raiz->der=pag2;
	//como la raiz sera la pagina que estara a la izquierda su orden sera de 2 
	(*raiz)->orden=2;
	//la pagina 1 sera la nueva raiz y como contendra un solo elemento sera de orden 1
	pag1->orden=1;
	//la pagina 2 sera la pagina que estara a la derecha del nodo de la pagina 1 entonces su orden sera de 3
	pag2->orden=3;
	//asignamos como raiz a la pagina 1
	(*raiz)= pag1;
	return 1;
}

int DivideRaiz(Pagina **raiz, int dmitad){
	//Busca el nodo de enmedio de la pagina raiz indice 
	Lista *aux=Busca_Nodo_Lista(&(*raiz)->inicio,dmitad);
	if(!aux) return 0;
	//se crena las 2 paginas nuevas que contendran a los datos menores y mayores de la raizde indice
	Pagina *pag1= CreaPagina();
	if(!pag1) return 0;
	Pagina *pag2= CreaPagina();
	if(!pag2) return 0;
	//se enlaza la pagina 1 que sera la nueva raiz de indices con su apuntador izq a la raiz principal 
	pag1->izq=(*raiz);
	//como un arbol b la pagina 2 su inicio sera el dato sig del nodo de enmedio
	pag2->inicio=aux->sig;
	//la izquierda la pagina 2 apuntara a los datos mayores al nodo de la mitad 
	pag2->izq=aux->raiz->der;
	//con el fin de que solo el nodo de enmedio quede como raiz se le corta su sig
	aux->sig=NULL;
	//la pagina 1 tomara el lugar de la raiz de indice por lo cual en su lista debe ir el dato de enmedio 
	pag1->inicio=aux;
	//una ves teniendo el dato de enmedio en la pagina 1 la derecha de ese nodo apuntara a la pagina 2 que contiene a los datos mayores a el
	pag1->inicio->raiz->der=pag2;
	//se modificara el orden tanto de las paginas nuevas como el de la antigua raiz
	(*raiz)->orden =2;
	pag1->orden=1;
	pag2->orden=2;
	//la pagina 1 toma el lugar como raiz
	(*raiz)=pag1;
	return 1;
}

int Inserta(Pagina **raiz, int id, char titu[], char descrip[], int grado){
	if(!(*raiz)) (*raiz)=CreaPagina();
	
	int m= (2*grado)+1;//numero maximo de elementos
	
	if(!(*raiz)->izq){ //si no hay izquierda entonces quiere decir que la pagina esta vacia o que aun hay espacio en la pagina  
		(*raiz)->orden++; //se ira incrementando el orden de la pagina hasta llegar a (grado*2)+1
		int i=CreaLista(&(*raiz)->inicio,id,titu,descrip);
		if(i==0) return 0;
		
		if((*raiz)->orden == m){//si el grado de la pagina raiz es igual al limite de elementos permitidos se partira la pagina
			//dividir pagina
			//nivel++;
			DividePagina(raiz,grado);
			return 2;//retorna 2 si la pagina de datos se rompe
		}
		return 1;//retorna 1 si la insercion fue simple y no es necesario partir la pagina de datos
	}
	
	if(id < (*raiz)->inicio->raiz->clave){ //si el dato a ingresar es menor al dato que esta en el indice de la pagina raiz, se colocara en la pagina de la izquierda
		int j=Inserta(&(*raiz)->izq,id,titu,descrip,grado);//se insertan los datos con normalidad en solo que ahora lo hacen en la pagina izq de la raiz inidce la cual son los menores
		if(j==2){//si la pagina a la izq de la raiz de inidices se divide 
			//se crea un aux de tipo lista el cual apuntara al inicio de la lista de la pagina raiz inidce creada por la division de la pagina izq de la raiz inidice principal
			Lista *aux= (*raiz)->izq->inicio; 
			//se liga la cajita de la lista de la pagina raiz indice que esta a la izq con la lista de la pagina raiz indice principal
			aux->sig=(*raiz)->inicio;
			//como el dato que esta en la lista apuntada por aux es menor al de la pagna principal inidice, aux tomo el inicio de la lista
			(*raiz)->inicio=aux;
			//el apuntador izq de la pagina principal indice ahora apuntara a la pagina que esta a la izq de la pagina raiz indice izq (ya que dicha no nos interesa)
			(*raiz)->izq=(*raiz)->izq->izq;
			//como la pagina raiz indice tiene una nueva clave se le aumenta el orden 
			(*raiz)->orden++;
			return 1;
		}
		//ReubicaPaginas(raiz);
		return 1;
	}
	//si el dato es mayor al del inicio de la lista de la pagina raiz de indices entonces mando a buscar el indice adecuado para insertar el nuevo dato
	Lista *aux2= Busca_NodoIndice_Insertar(&(*raiz)->inicio,id);
	if(!aux2) return 0;
	//una ves encontrando el el nodo indice adecuado prosigue la insercion a su pagina de claves derecha
	int j=Inserta(&aux2->raiz->der, id, titu, descrip, grado);
	//si la pagina derecha de la raiz de indice se divide 
	if(aux2->raiz->der->orden==1){
		aux2->raiz->der->inicio->sig =aux2->sig;
		aux2->sig=aux2->raiz->der->inicio;
		aux2->raiz->der=aux2->raiz->der->izq;
		(*raiz)->orden++;
		return 1;
	}
	//si la raiz de indices alcanza el numero maximo de nodos 
	if((*raiz)->orden==m){
		//divide la raiz 
		DivideRaiz(raiz,grado);
		return 2;
	}
}
void Imprime(Pagina *raiz);

void ImprimePaginas(Lista *inicio){
	if(!inicio) return;
	Imprime(inicio->raiz->der);
	p("\n");
	ImprimePaginas(inicio->sig);
}

void ImprimeLista(Lista *inicio){
	if(!inicio) return;
	//imprimo el dato clave de la lista de nodos de la raiz
	p(" clave-> %d ", inicio->raiz->clave);
	p("titulo= %s ", inicio->raiz->titulo);
	p("descripcion= %s", inicio->raiz->descripcion);
	return ImprimeLista(inicio->sig);
}
void Imprime(Pagina *raiz){
	if(!raiz) return;
	//primero imprimo el orden de mi pagina
	p("orden-> %d ", raiz->orden);
	//llamo a la funcion que imprime la informacion de los nodos de la raiz
	ImprimeLista(raiz->inicio);
	//despues de imprimir la informacion del nodo de la raiz o la raiz de indices preguntamos si existe izquierda
	if(raiz->izq){
		p("\n\n");
		//si existe izquierda imprimimos los datos que estan a la izq
		Imprime(raiz->izq);
		p("\n");
		//despues de imprimir los datos de la raiz izq volvemos a la raiz e imprimimos lo que tiene la pagina der
		ImprimePaginas(raiz->inicio);
	}
	return;
}

Nodo* BuscaNodo(Pagina **raiz, int db, int op);

Nodo* BuscaLista(Lista **inicio,int db,int op){
	if(!(*inicio))return NULL;
	//si el dato que se va a buscar es igual al dato de la raiz y si el nodo raiz no tiene derecha se retornara el nodo 
	(*inicio)->raiz->color=true;
	if((*inicio)->raiz->clave==db && (*inicio)->raiz->der==NULL){
		if(op==0){
			(*inicio)->raiz->color=true;
			return (*inicio)->raiz;//si la op es 0 quiere decir que solo necesitamos regresar el nodo ya sea para mostrar o para editar su informacion
		}
		if(op==1){//si la opcion es 1 entonces se requiere el nodo para eliminarlo
			//antes de eliminar el nodo se debe desenlazar la lista que contiene al nodo 
			Lista *aux=(*inicio);
			(*inicio)=aux->sig;
			//una ves que esta desenlazado la lista el nodo es guardado en una aux de tipo nodo
			Nodo *auxNodo= aux->raiz;
			//procedemos a liberar la memoria que ocupaba el nodo de tipo lista
			free(aux);
			return auxNodo;
		}
	}
	//si no exste siguiente quiere decir que es el unico nodo en la pagina raiz de indices por lo cual se buscara en sus hojas 
	if(!(*inicio)->sig)
		return BuscaNodo(&(*inicio)->raiz->der, db,op);
	//si el dato que estoy buscando es mayor al dato del nodo raiz y a la ves menor que el datos sig del nodo raiz quiere decir que en ese intermedio a la derecha tengo que insertar
	if(db>(*inicio)->raiz->clave && db<(*inicio)->sig->raiz->clave)
		return BuscaNodo(&(*inicio)->raiz->der,db,op);
	//si se llega a este punto se pregunta si la el dato del nodo raiz es el mismo que el dato que quiero encontrar (es la etiqueta copia) por lo cual el dato debe estar en su derecha
	if(db==(*inicio)->raiz->clave)
		return BuscaNodo(&(*inicio)->raiz->der,db,op);
	//si no se cumple ninguna condicion anterior quiere decir que hay mas elementos en la lista a los cuales hay que recorrerlos
	return BuscaLista(&(*inicio)->sig,db,op);
}

Nodo* BuscaNodo(Pagina **raiz, int db, int op){
	int bandera=0;
	
	if(!(*raiz)) return NULL;
	//si el dato que estoy buscando es menor al primer dato de la lista de nodos se ira a la izq que es donde estan los datos menores
	if(db < (*raiz)->inicio->raiz->clave){
		bandera=1;
		Nodo *aux=BuscaNodo(&(*raiz)->izq,db,op);
		if(!aux) return NULL;
		return aux;
	}
	//si el dato que estoy buscando es mayor al primer dato que esta en la lista de nodos se buscara en la lista de nodos de la pagina raiz 
	Nodo *aux= BuscaLista(&(*raiz)->inicio,db,op);
	if(!aux) return NULL;
	//aqui deberia ir la reduccion del orden de la pagina pero si lo hago le bajaria el grado a todo
	return aux;
}

Nodo* BusquedaXY(Pagina *raiz, int px, int py);

Nodo* BusquedaListaXY(Lista *inicio, int px,int py){
	if(!inicio) return NULL;
	
	if(px >= (inicio->raiz->centro->x-10) && px <= (inicio->raiz->centro->x+10) && py >= (inicio->raiz->centro->y-10) && py <= (inicio->raiz->centro->y+10))
		return inicio->raiz;
	
	if(!inicio->sig)
		return BusquedaXY(inicio->raiz->der, px,py);
	
	return BusquedaListaXY(inicio->sig,px,py);
	return BusquedaXY(inicio->raiz->der, px,py);
}

Nodo* BusquedaXY(Pagina *raiz, int px, int py){
	if(!raiz) return NULL;
	
	if(raiz->izq){
		Nodo *aux=BusquedaXY(raiz->izq, px,py);
		if(!aux){
			Nodo* aux=BusquedaListaXY(raiz->inicio, px, py);
			if(!aux) return NULL;
			return aux;
		}
		return aux;
	}
	
	Nodo* aux=BusquedaListaXY(raiz->inicio, px, py);
	if(!aux) return NULL;
	return aux;
}

void RestaurarColores(Pagina *raiz);

void RestauraLista(Lista *inicio){
	if(!inicio) return;
	
	if(inicio->raiz->color==true)
		inicio->raiz->color=false;
	
	RestauraLista(inicio->sig);
	RestaurarColores(inicio->raiz->der);
}

void RestaurarColores(Pagina *raiz){
	if(!raiz) return;
	
	RestauraLista(raiz->inicio);
	
	if(raiz->izq){
		RestaurarColores(raiz->izq);
		return;
	}
	
	return;
}

int Elimina_Hoja(Pagina **raiz, int datoeli){//esta funcion solo elimina hojas y no reestructura
	Nodo *aux=BuscaNodo(&(*raiz),datoeli,1);
	if(!aux) return 0;
	free(aux);
	return 1;
}

//parte grafica

void inline drawString(char *s){//funcion que escribe  
	glColor3f(0.0f,0.0f,1.0f);
	for(unsigned int i=0; i<strlen(s); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,s[i]);	
}

void VentanaInserta(){
	glClearColor(0.8,0.8,0.9,2);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar,"Ingresa los datos del nuevo Libro");
	glRasterPos2f(-0.9f,0.7f);
	drawString(mostrar);
	
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f); //color de la letra
	sprintf(mostrar, "ID Libro:"); //texto que se mostrara en la ventana
	glRasterPos2f(-0.9f,0.5f); //posicion del texto
	drawString(mostrar); //llama a la funcion que se encarga de dibujar lo que contiene mostrar en la ventana
	glRasterPos2f(-0.55f, 0.5f);
	sprintf(mostrar, "%s", letID);//dibuja caracter por caracter en tiempo real
	drawString(mostrar);
	
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar, "Titulo:");
	glRasterPos2f(-0.9f,0.3f);
	drawString(mostrar);
	glRasterPos2f(-0.65f, 0.3f);
	sprintf(mostrar, "%s", let2);
	drawString(mostrar);
	
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar,"Descripcion:");
	glRasterPos2f(-0.9f,0.1f);
	drawString(mostrar);
	glRasterPos2f(-0.45f, 0.1f);
	sprintf(mostrar, "%s", let3);
	drawString(mostrar);
	
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar,"Precione TAB para cerrar la ventana");
	glRasterPos2f(-0.9f,-0.80f);
	drawString(mostrar);
	
	glutSwapBuffers();
}

void VentanaBusqueda(){
	glClearColor(0.8,0.8,0.9,1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar,"Ingresa el ID del Libro a buscar");
	glRasterPos2f(-0.9f,0.7f);
	drawString(mostrar);
	
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f); //color de la letra
	sprintf(mostrar, "ID Libro:"); //texto que se mostrara en la ventana
	glRasterPos2f(-0.9f,0.5f); //posicion del texto
	drawString(mostrar); //llama a la funcion que se encarga de dibujar lo que contiene mostrar en la ventana
	glRasterPos2f(-0.6f, 0.5f);
	sprintf(mostrar, "%s", letID);//dibuja caracter por caracter en tiempo real
	drawString(mostrar);
	
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar,"Precione TAB para cerrar la ventana");
	glRasterPos2f(-0.9f,-0.80f);
	drawString(mostrar);
	
	glutSwapBuffers();
}

void VentanaEliminacion(){
	glClearColor(0.8,0.8,0.9,1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar,"Ingresa el ID del Libro que deseas eliminar");
	glRasterPos2f(-0.9f,0.7f);
	drawString(mostrar);
	
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f); //color de la letra
	sprintf(mostrar, "ID Libro:"); //texto que se mostrara en la ventana
	glRasterPos2f(-0.9f,0.5f); //posicion del texto
	drawString(mostrar); //llama a la funcion que se encarga de dibujar lo que contiene mostrar en la ventana
	glRasterPos2f(-0.6f, 0.5f);
	sprintf(mostrar, "%s", letID);//dibuja caracter por caracter en tiempo real
	drawString(mostrar);
	
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar,"Precione TAB para cerrar la ventana");
	glRasterPos2f(-0.9f,-0.80f);
	drawString(mostrar);
	
	glutSwapBuffers();
}

void VentanaSubBusqueda(){
	glClearColor(0.8,0.8,0.9,1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar,"Datos del libro encontrado");
	glRasterPos2f(-0.9f,0.7f);
	drawString(mostrar);
	
	glColor3f(0.0f,0.8f,1.0f); //color de la letra
	sprintf(mostrar, "ID Libro:"); //texto que se mostrara en la ventana
	glRasterPos2f(-0.9f,0.5f); //posicion del texto
	drawString(mostrar); //llama a la funcion que se encarga de dibujar lo que contiene mostrar en la ventana
	glRasterPos2f(-0.6f, 0.5f);
	sprintf(mostrar, "%d", id_lib);//dibuja caracter por caracter en tiempo real
	drawString(mostrar);
	
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar, "Titulo:");
	glRasterPos2f(-0.9f,0.3f);
	drawString(mostrar);
	glRasterPos2f(-0.65f, 0.3f);
	sprintf(mostrar, "%s", tit_libro);
	drawString(mostrar);
	
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar,"Descripcion:");
	glRasterPos2f(-0.9f,0.1f);
	drawString(mostrar);
	glRasterPos2f(-0.45f, 0.1f);
	sprintf(mostrar, "%s", desc_libro);
	drawString(mostrar);
	
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar,"Tiempo Busqueda en Ms:");
	glRasterPos2f(-0.9f,-0.13f);
	drawString(mostrar);
	glRasterPos2f(-0.0100f, -0.13f);
	sprintf(mostrar, "%.16g", secs*1000.0);
	drawString(mostrar);
	
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar,"Precione TAB para cerrar la ventana");
	glRasterPos2f(-0.9f,-0.80f);
	drawString(mostrar);
	
	glutSwapBuffers();
}

void VentanaClics(){
	glClearColor(0.8,0.8,0.9,1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar,"Datos del libro");
	glRasterPos2f(-0.9f,0.7f);
	drawString(mostrar);
	
	glColor3f(0.0f,0.8f,1.0f); //color de la letra
	sprintf(mostrar, "ID Libro:"); //texto que se mostrara en la ventana
	glRasterPos2f(-0.9f,0.5f); //posicion del texto
	drawString(mostrar); //llama a la funcion que se encarga de dibujar lo que contiene mostrar en la ventana
	glRasterPos2f(-0.6f, 0.5f);
	sprintf(mostrar, "%d", id_lib);//dibuja caracter por caracter en tiempo real
	drawString(mostrar);
	
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar, "Titulo:");
	glRasterPos2f(-0.9f,0.3f);
	drawString(mostrar);
	glRasterPos2f(-0.65f, 0.3f);
	sprintf(mostrar, "%s", tit_libro);
	drawString(mostrar);
	
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar,"Descripcion:");
	glRasterPos2f(-0.9f,0.1f);
	drawString(mostrar);
	glRasterPos2f(-0.45f, 0.1f);
	sprintf(mostrar, "%s", desc_libro);
	drawString(mostrar);
	
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar,"Precione TAB para cerrar la ventana");
	glRasterPos2f(-0.9f,-0.80f);
	drawString(mostrar);
	
	glutSwapBuffers();
}

void VentanaComprobacion(){
	glClearColor(0.8,0.8,0.9,1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glColor3f(0.0f,0.8f,1.0f); //color de la letra
	sprintf(mostrar, "Libro Agregado Exitosamente! :v"); //texto que se mostrara en la ventana
	glRasterPos2f(-0.9f,0.5f); //posicion del texto
	drawString(mostrar); //llama a la funcion que se encarga de dibujar lo que contiene mostrar en la ventana
	
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar,"Tiempo Insercion en Ms:");
	glRasterPos2f(-0.9f,-0.13f);
	drawString(mostrar);
	glRasterPos2f(-0.0100f, -0.13f);
	sprintf(mostrar, "%.16g", secs*1000.0);
	drawString(mostrar);
		
	mostrar[0]='\0';
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(mostrar,"Precione TAB para cerrar la ventana");
	glRasterPos2f(-0.9f,-0.80f);
	drawString(mostrar);
	
	glutSwapBuffers();
}

void actualiza(){
	glutSetWindow(VPadre);
	glClear(GL_COLOR_BUFFER_BIT);
	glutPostRedisplay();	
	if(VHijo!=0){
		glutSetWindow(VHijo);
		glutPostRedisplay();	
	}
}

void TecladoInsertar(unsigned char letra, int x, int y);
void TecladoBusqueda(unsigned char letra, int x, int y);
void TecladoEliminacion(unsigned char letra, int x, int y);
void TecladoSubBusqueda(unsigned char letra, int x, int y);
void TecladoClics(unsigned char letra, int x, int y);
void TecladoComprobacion(unsigned char letra, int x, int y);

void EventosMouse(int boton, int estado, int x, int y){

	if(boton==GLUT_LEFT_BUTTON && estado== GLUT_DOWN){ //si el clic izquierdo es precionado
		if(x>=250 && x<=285  && y>=37 && y<=52)
			exit(0);
		
		if(x>=10 && x<=65 && y>=37 && y<=52){ //si el clic es dado en el rango tanto en x,y del label se ejecuta las funciones
			VHijo=glutCreateSubWindow(VPadre, 20,20,500,250);//creas la subventana
			glutDisplayFunc(VentanaInserta);//inicializas la subventana
			glutKeyboardFunc(TecladoInsertar);//cada ventana o subventana debe tener un keyboard propio
		}
		if(x>=90 && x<=141 && y>37 && y<=52){
			VHijo=glutCreateSubWindow(VPadre, 20,20,500,250);
			glutDisplayFunc(VentanaBusqueda);
			glutKeyboardFunc(TecladoBusqueda);
		}
		if(x>=170 && x<=250 && y>=37 && y<=52){
			VHijo=glutCreateSubWindow(VPadre, 20,20,500,250);
			glutDisplayFunc(VentanaEliminacion);
			glutKeyboardFunc(TecladoEliminacion);
		}
		if(y>=81 && x>=0 && x<=1000 ){
			Nodo *aux=BusquedaXY(raiz, x, y);
			if(!aux) return;
			
			id_lib=aux->clave;
			strcpy(tit_libro, aux->titulo);
			strcpy(desc_libro, aux->descripcion);
			
			VHijo=glutCreateSubWindow(VPadre, 20,400,500,250);
			glutDisplayFunc(VentanaClics);
			glutKeyboardFunc(TecladoClics);
		}
	}
}

void BotonesEspeciales(int tecla, int x, int y){
	if(tecla == GLUT_KEY_UP){// tecla direccion arriba
		glTranslated(0,10,0);
	}
	if(tecla== GLUT_KEY_DOWN){// tecla direccion abajo
		glTranslated(0,-10,0);
	}
	if(tecla == GLUT_KEY_RIGHT){ // tecla direccion derecha
		glTranslated(-10.0,0,0);
	}
	if(tecla == GLUT_KEY_LEFT){ // tecla direccion izquierda
		glTranslated(10.0,0,0);
	}
}

void TecladoInsertar(unsigned char letra, int x, int y){
	if(letra==9){ //al apretar Esc se destruye la subventana
		glutDestroyWindow(glutGetWindow());
		VHijo=0;
	}
	if(renglon==0){
		
		if(letra>=48 && letra<=57 ){ //intervalo de ascii donde las teclas son numeros
			letID[pos]=letra;
			pos++;
		}
		if(letra == 8 && pos!=0){ //por si apretas la tecla de borrado
			letID[pos]=32;
			pos--;
		}
		
		if(letra==13){//enter
			renglon++;
			pos=0;
		}
	}
	if(renglon==1){
		if(letra>=97 && letra<=122 || letra==32){
			let2[pos]=letra;
			pos++;
		}
		if(letra == 8 && pos!=0){
			let2[pos]=32;
			pos--;
		}
		
		if(letra==13 && pos!=0){
			renglon++;
			pos=0;
		}
	}
	if(renglon==2){
		if(letra>=97 && letra<=122 || letra==32){
			let3[pos]=letra;
			pos++;
		}
		if(letra == 8 && pos!=0){
			let3[pos]=32;
			pos--;
		}
		if(letra==13 && pos!=0){
			renglon++;
			pos=0;
			
			int dato=atoi(letID);//convertir la cadena de ID en un entero
			t_ini=clock();
			Inserta(&raiz, dato, let2, let3,2);//inserta 
			t_fin=clock();
			
			secs=(double)(t_fin - t_ini) / CLOCKS_PER_SEC;
			
			Imprime(raiz);
			
			p("\n");
			//limpiar los buffers utilizados para guardar los datos
			memset(letID,'\0',sizeof(letID));
			memset(let2, '\0', sizeof(let2));
			memset(let3, '\0', sizeof(let3));
			
			glutDestroyWindow(glutGetWindow());//al insertar el ultimo dato me destruira la ventana
			VHijo=0;//al destruir la subventana el valor de VHijo regresa a ser 0
			renglon=0;//renglon vuelve a ser 0 para poder insertar los datos en la primera posicion
			
			VHijo2=glutCreateSubWindow(VPadre, 20,20,500,250);
			glutDisplayFunc(VentanaComprobacion);
			glutKeyboardFunc(TecladoComprobacion);
		}
	}
}

void TecladoBusqueda(unsigned char letra, int x, int y){
	if(letra == 9){
		glutDestroyWindow(glutGetWindow());
		VHijo=0;
	}
	if(renglon==0){
		if(letra>=48 && letra<=57){ //intervalo de ascii donde las teclas son numeros
			letID[pos]=letra;
			pos++;
		}
		if(letra == 8 && pos!=0){ //por si apretas la tecla de borrado
			letID[pos]=32;
			pos--;
		}
		if(letra==13){//enter
			renglon++;
			pos=0;
			
			int dato=atoi(letID);
			t_ini=clock();
			Nodo *aux=BuscaNodo(&raiz,dato,0);
			t_fin=clock();
			if(!aux){
				memset(letID,'\0',sizeof(letID));
				renglon=0;
				RestaurarColores(raiz);
				return;
			}
			
			id_lib=aux->clave;
			strcpy(tit_libro, aux->titulo);
			strcpy(desc_libro, aux->descripcion);	
			secs=(double)(t_fin - t_ini) / CLOCKS_PER_SEC;
			
			memset(letID,'\0',sizeof(letID));
			glutDestroyWindow(glutGetWindow());
			
			VHijo=0;
			renglon=0;
			
			VHijo2=glutCreateSubWindow(VPadre,20,400,500,250);
			glutDisplayFunc(VentanaSubBusqueda);
			glutKeyboardFunc(TecladoSubBusqueda);
		}
	}
}

void TecladoEliminacion(unsigned char letra, int x, int y){
	if(letra==9){
		glutDestroyWindow(glutGetWindow());
		VHijo=0;
	}
	if(renglon==0){
		if(letra>=48 && letra<=57){ //intervalo de ascii donde las teclas son numeros
			letID[pos]=letra;
			pos++;
		}
		if(letra == 8 && pos!=0){ //por si apretas la tecla de borrado
			letID[pos]=32;
			pos--;
		}
		if(letra==13){//enter
			renglon++;
			pos=0;
			
			int dato=atoi(letID);
			Nodo *aux=BuscaNodo(&raiz,dato,0);
			if(Elimina_Hoja(&raiz, dato)==1)
				p("Eliminacion del dato exitosa");
			
			memset(letID,'\0',sizeof(letID));
			glutDestroyWindow(glutGetWindow());
			RestaurarColores(raiz);
			VHijo=0;
		}
	}
}

void TecladoSubBusqueda(unsigned char letra, int x, int y){
	if(letra == 9){
		RestaurarColores(raiz);
		glutDestroyWindow(glutGetWindow());
		VHijo2=0;
	}
}

void TecladoClics(unsigned char letra, int x, int y){
	if(letra == 9){
		glutDestroyWindow(glutGetWindow());
		VHijo=0;
	}
}
void TecladoComprobacion(unsigned char letra, int x, int y){
	if(letra==9){
		glutDestroyWindow(glutGetWindow());
		VHijo2=0;
	}
}

void DibujaTODO(Pagina **raiz);
void dibujaHijos(Lista *inicio, int bandera);
void DibujaHijosTercer(Pagina *raiz){
	glColor3f(0.7, 0.7, 0.7);
	glBegin(GL_QUADS);
	glVertex2f(Xpag3-70, 240);
	glVertex2f(Xpag3-70, 240+60);
	glVertex2f(Xpag3+70, 240+60);
	glVertex2f(Xpag3+70, 240);
	glEnd();
	glPushMatrix();
	glColor3f(1.0f,0.5f,0.2f);
	glBegin(GL_LINES);
	glVertex2f(Xpag3+70, 240+60);
	glVertex2f(Xpag3+70, 240);
	glEnd();
	glPopMatrix();
	
	Lista *aux= raiz->inicio;
	float line=0;
	raiz->cord->x = Xpag3;
	raiz->cord->y = 180;
	
	while(aux){
		float XMitad=(Xpag3-70)+(line+(125/raiz->orden)/2);
		float YMitad=240+(ancho/1.8);
		
		aux->raiz->centro->x=XMitad;
		aux->raiz->centro->y=YMitad;
			
		glPushMatrix();
		glColor3f(1.0f,0.5f,0.2f);
		glBegin(GL_LINES); //linea izq de 
		glVertex2f((Xpag3-(largo/2))+line, 240);
		glVertex2f((Xpag3-(largo/2))+line, 240+ancho);
		glEnd();
		glPopMatrix();
		glPushMatrix();
		if(aux->raiz->color==false)
			glColor3f(1.0f,1.0f,1.0f);
		else glColor3f(0.51f,0.0f,1.0f);
		sprintf(label, "%d", aux->raiz->clave);
		glRasterPos2f(XMitad,YMitad);
		drawString(label);
		glPopMatrix();
		memset(label,'\0', sizeof(label));
		aux = aux->sig;
		line+=largo/raiz->orden;
	}
	
	Xpag3+=150;
}

void dibujaHijo(Pagina *raiz){

	glColor3f(0.7,0.7,0.7);
	glBegin(GL_QUADS);
	glVertex2f(xPagina-70,160);
	glVertex2f(xPagina-70,160+60);
	glVertex2f(xPagina+70,160+60);
	glVertex2f(xPagina+70,160);
	glEnd();
	
	glPushMatrix();
	glColor3f(1.0f,0.5f,0.2f);
	glBegin(GL_LINES);
	glVertex2f(xPagina+70, 160+60);
	glVertex2f(xPagina+70, 160);
	glEnd();
	glPopMatrix();

	Lista *aux=raiz->inicio;
	float line=0;
	
	int xAux = xPagina;
	raiz->cord->x = xPagina;
	raiz->cord->y = 160;
	
	if(raiz->izq){
		glColor3f(1.0f,0.5f,0.2f);
		glBegin(GL_LINES);
		glVertex2f(raiz->cord->x-(largo/2),yline+(2*ancho)+20);
		glVertex2f(raiz->izq->cord->x,raiz->izq->cord->y+ancho+20);
		glEnd();
	}
	
	while(aux){
		float XMitad=(xPagina-70)+(line+(125/raiz->orden)/2);
		float YMitad=160+(ancho/1.8);
		
		aux->raiz->centro->x=XMitad;
		aux->raiz->centro->y=YMitad;
		
		glPushMatrix();
		glColor3f(1.0f,0.5f,0.2f);
		glBegin(GL_LINES); //linea izq de 
		glVertex2f((xPagina-(largo/2))+line, 160);
		glVertex2f((xPagina-(largo/2))+line, 160+ancho);
		glEnd();
		glPopMatrix();
		glPushMatrix();
		if(aux->raiz->color==false)
			glColor3f(1.0f,1.0f,1.0f);
		else glColor3f(0.51f,0.0f,1.0f);
		sprintf(label, "%d", aux->raiz->clave);
		glRasterPos2f(XMitad,YMitad);
		drawString(label);
		glPopMatrix();
		memset(label,'\0', sizeof(label));
		if(aux->raiz->der){
			glColor3f(1.0f,0.5f,0.2f);
			glBegin(GL_LINES);
			glVertex2f(xPagina-(largo/2)+(largo/raiz->orden)+line,220);
			glVertex2f(aux->raiz->der->cord->x,aux->raiz->der->cord->y+80);
			glEnd();
		}
		aux = aux->sig;
		line+=largo/raiz->orden;
	}
	
	if(raiz->izq){
		nivel++;
		TAMpag3 = (raiz->orden + 1)*50;
		espacio3= (raiz->orden * 10);
		space3 = TAMpag3+espacio3 / 2;
		Xpag3 = raiz->cord->x-space3;
		
		DibujaHijosTercer(raiz->izq);
		dibujaHijos(raiz->inicio,3);
		
	}
	if(nivel >=1 && nivel<3) xPagina += (70+300);
	
	if(nivel>=3) xPagina+=(70+500);
}

void dibujaHijos(Lista *inicio, int bandera){
	if(!inicio) return;
	
	if(bandera==2){
		//xPagina += 140+espacio;
		dibujaHijo(inicio->raiz->der);
	}
	if(bandera==3)DibujaHijosTercer(inicio->raiz->der);
	dibujaHijos(inicio->sig,bandera);
}

void DibujaTODO(Pagina **raiz){
	if(!(*raiz)) return;
	//funciones para pintar la pagina
	glPushMatrix();
	glColor3f(0.7,0.7,0.7);
	glBegin(GL_QUADS);
	glVertex2f((*raiz)->cord->x-(largo/2),(*raiz)->cord->y); //primer punto de origen
	glVertex2f((*raiz)->cord->x-(largo/2),(*raiz)->cord->y+ancho); //segundo punto arriba del origen
	glVertex2f((*raiz)->cord->x+(largo/2),(*raiz)->cord->y+ancho); //tercer punto 
	glVertex2f((*raiz)->cord->x+(largo/2),(*raiz)->cord->y);//cuarto punto arriba del tercero
	glEnd();
	glPopMatrix();
	//funciones para pintar la linea derecha limite de la pagina
	glPushMatrix();
	glColor3f(1.0f,0.5f,0.2f);
	glBegin(GL_LINES);
	glVertex2f((*raiz)->cord->x+(largo/2), (*raiz)->cord->y+ancho);
	glVertex2f((*raiz)->cord->x+(largo/2), (*raiz)->cord->y);
	glEnd();
	glPopMatrix();
	
	float line=0; //variable que controla la posicion de la linea
	Lista *AuxPrint= (*raiz)->inicio; 
	
	if((*raiz)->izq){
		glColor3f(1.0f,0.5f,0.2f);
		glBegin(GL_LINES);
		glVertex2f((*raiz)->cord->x-(largo/2),yline+ancho);
		glVertex2f((*raiz)->izq->cord->x,(*raiz)->izq->cord->y);
		glEnd();
	}
	
	//codigo que recorre la lista para poder pintar las claves del arbol
	while(AuxPrint){
		float MitadX=((*raiz)->cord->x-(largo/2))+(line+(125/(*raiz)->orden)/2); //formula para sacar el punto x para poner la clave del nodo
		float MitadY=(*raiz)->cord->y+(ancho/1.8); //formula para sacar el punto y para poner la clave del nodo
		
		//asigno los valores del centro de cada nodo 
		AuxPrint->raiz->centro->x=MitadX;
		AuxPrint->raiz->centro->y=MitadY;
		
		xline=((*raiz)->cord->x-(largo/2))+line; 
		yline=(*raiz)->cord->y;
		
		glPushMatrix();
		glColor3f(1.0f,0.5f,0.2f);
		glBegin(GL_LINES); //linea izq de 
		glVertex2f(((*raiz)->cord->x-(largo/2))+line, (*raiz)->cord->y);
		glVertex2f(((*raiz)->cord->x-(largo/2))+line, (*raiz)->cord->y+ancho);
		glEnd();
		glPopMatrix();
		glPushMatrix();
		if(AuxPrint->raiz->color==false)
			glColor3f(1.0f,1.0f,1.0f);
		else glColor3f(0.51f,0.0f,1.0f);
		sprintf(label, "%d", AuxPrint->raiz->clave);
		glRasterPos2f(MitadX,MitadY);
		drawString(label);
		glPopMatrix();
		memset(label,'\0', sizeof(label));
		if(AuxPrint->raiz->der){
			glColor3f(1.0f,0.5f,0.2f);
			glBegin(GL_LINES);
			glVertex2f((*raiz)->cord->x-(largo/2)+line+largo/(*raiz)->orden,yline+ancho);
			glVertex2f(AuxPrint->raiz->der->cord->x,AuxPrint->raiz->der->cord->y);
			glEnd();
		}
		AuxPrint = AuxPrint->sig;
		line+=largo/(*raiz)->orden;
	}
	
	if((*raiz)->izq){
		nivel++;
		TAMpag = ((*raiz)->orden + 1)*50;
		espacio = ((*raiz)->orden * 250);
		space = TAMpag+espacio / 2;
		xPagina = 500-space;
		
		
		dibujaHijo((*raiz)->izq);
		dibujaHijos((*raiz)->inicio,2);
	}
}

static void display(void){	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3d(1,1,1);	
	glPushMatrix();
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluOrtho2D (0,1000,700,0);
	glPopMatrix();
	
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(instrucciones,"Precione una opcion");
	glRasterPos2f(10.0f,20.0f);
	drawString(instrucciones);
	
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(instrucciones,"Inserta");
	glRasterPos2f(10.0f,50.0f);
	drawString(instrucciones);
	
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(instrucciones,"Busca");
	glRasterPos2f(90.0f,50.0f);
	drawString(instrucciones);
	
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(instrucciones,"Elimina");
	glRasterPos2f(170.0f,50.0f);
	drawString(instrucciones);
	
	glColor3f(0.0f,0.8f,1.0f);
	sprintf(instrucciones,"Salir");
	glRasterPos2f(250.0f,50.0f);
	drawString(instrucciones);
	
	DibujaTODO(&raiz);
	glutSwapBuffers();
}

static void resize(int width, int height){ // volver resposiva la ventana
	//if(width==0 || height==0) return;
	const float ar = (float) width / (float) height;
	
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity() ;
}

int main(int argc, char *argv[]){
	int grado=2;
	
	//configuraciones de inicio de open gl 
	glutInit(&argc, argv);
	glutInitWindowSize(1000,700);
	glutInitWindowPosition(0,0);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	VPadre=glutCreateWindow("IEB7_Proyecto Arbol B+");
	glutReshapeFunc(resize);
	glutDisplayFunc(display);	
	glClearColor(0.1,0.1,0.2,1);

	glutIdleFunc(actualiza);
	glutSpecialFunc(BotonesEspeciales); //funcion que hace funcionar las teclas especiales
	glutMouseFunc(EventosMouse); //funcion que capta los eventos del mouse 
	
	glutMainLoop();
	return EXIT_SUCCESS;

	return 0;
}
