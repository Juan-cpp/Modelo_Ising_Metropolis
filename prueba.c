///Programa de evolución de un sistema ferromagnético en función de la temperatura (Ising)
# include <stdio.h>
# include <math.h>
# include "gsl_rng.h"
//Librería para generación de números aleatorios para compilar un programa
//con la librería anterior de números aleatorios tengo que añadirle al comando
//de compilación típico:gcc random.c -o random.exe -lm -O3
//la ubicacion de la libreria externa: -L/usr/lib/x86_64-linux-gnu -lgsl -lgslcblas -lm
//quedando finalmente:gcc random.c -o random.exe -lm -O3 -L/usr/lib/x86_64-linux-gnu -lgsl -lgslcblas -lm
//como comando de compilacion.

# define temp 1.5 // es la temperatura del sistema, la variamos entre 0º y 5º
# define N 128   // defino fuera el tamaño de las matrices por si quiero cambiarlo rápido
# define TIEMPO_MONTECARLO 1000000   // es el número de pasos montecarlo, defino fuera por lo mismo
# define TIEMPO_CALCULO 100
# define numero_temperaturas 10
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////funciones/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void INICIAL(int matriz[N][N], FILE *file); // es una funcion que me genera una configuracion
// inicial aleatoria de espines en una matriz de NxN nodos, cada nodo sería
// el spin de una particula distinta. Luego me lo escribe en un archivo
void CAMBIO(int matriz[N][N],double T); // aquí está el meollo del programa, haciendo
// el posible cambio de signo a un solo spin por cada ejecución de esta función.
// Ese posible cambio se dará si se complen las condiciones escritas en la función
// la propia función ya realiza el cambio
void ESCRIBIR(int matriz[N][N], FILE *file); // función para escribir mi matriz en un fichero
//para cada paso montecarlo
double CALCULAR_MN(int s[N][N]);
//
double CALCULAR_EN(int s[N][N]);
//
double CALCULAR_CN(int s[N][N]);
//
void FUNCION_CORRELACION(double f[1+N/2], int s[N][N]);


////////////////////////////////////////////////////////////////////////////////
///// Tengo que declarar mi variable aleatoria fuera del main, así será una/////
///// variable general y podré usarla en distintas funciones sin problema./////
////////////////////////////////////////////////////////////////////////////////
gsl_rng *tau;

int main()
{
    int i,j,k,l,m,cont;  //Variables auxiliares
    double p,x,y,z,mn,en,cn;  //Variables auxiliares
    double desmn, desen;
    int s[N][N]; //matriz que recoge todos los espines
    double f[1+N/2],g[1+N/2];//
    extern gsl_rng *tau; //Puntero al estado del número aleatorio
    double T;
    int semilla=413785; //Semilla del generador de números aleatorios
    FILE *f1, *f2, *f3, *f4; // punteros a ficheros

    f1=fopen("estado_inicial413785.txt","w"); //abro fichero para el estado inicial
    f2=fopen("estados_413785.txt","w"); //abro fichero donde escribiré los estados
    f3=fopen("soluciones_promedio.txt","w");
    f4=fopen("correlacion.txt","w");
    ///////////////////////////////////////////////////////////////////////
    //para utilizar los numeros aleatorios generados con gsl tengo que   //
    //inicializar la semilla y el puntero, después podré generar números //
    ///////////////////////////////////////////////////////////////////////
    tau=gsl_rng_alloc(gsl_rng_taus); //Inicializamos el puntero
    gsl_rng_set(tau,semilla); //Inicializamos la semilla

    INICIAL(s,f1);
    fclose(f1);

    T=temp;
    for(l=0;l<=numero_temperaturas;l++){
    m=1;// inicializo el contador m que cada 100 pasos montecarlos calcula lo que me piden y escribe en fichero.
    cont=0;//contador que me sirve para al final de mi programa hacer el promedio total.
    for(k=1;k<=TIEMPO_MONTECARLO;k++) // son 1000000 pasos montecarlos
    {
        for(j=1;j<=N*N;j++) // 64*64=4096 posibles cambios que ejecuta un paso montecarlo
        {
            CAMBIO(s,T);
        }

        if(m==TIEMPO_CALCULO)
        {
            x=CALCULAR_MN(s);
            mn=mn+x;
            y=CALCULAR_EN(s);
            en=en+y;
            z=CALCULAR_CN(s);
            cn=cn+z;
            FUNCION_CORRELACION(f,s);
            for(i=1;i<=N/2;i++)
            {
                g[i]=g[i]+f[i];
            }

            ESCRIBIR(s,f2);

            m=0;
            cont=cont+1;
        }
        m++;

    }

    //desmn=(mn*mn/cont)-(mn*mn/cont*cont);
    //desmn=sqrt(-desmn);
    mn=mn/(1.0*cont);
    cn=((cn/cont)-en*en/(cont*cont))/(N*N*T);
    //desen=(en*en/cont)-(en*en/cont*cont);
    //desen=sqrt(-desen/(2*N));
    en=en/(2.0*N*cont);
    for(i=1;i<=N/2;i++)
    {
        g[i]=g[i]/(N*N*cont);
        fprintf(f4, "%lf\t%lf\n", g[i],temp );
    }
    printf("%lf\t%lf\t%lf\t%i\n",mn,en,cn,l);
    fprintf(f3, "%lf\t%lf\t%lf\t%lf\n",T,mn,en,cn );
    T=T+0.2;
    }
    fclose(f2);
    fclose(f3);
    fclose(f4);

    return 0;
}


void INICIAL(int matriz[N][N],FILE *file)
{
    int i,j; // contadores
    int a; // variable aleatoria
    //extern gsl_rng *tau; //Puntero al estado del número aleatorio


    for(i=0;i<N;i++)
    {
        for(j=0;j<N;j++)
        {
            //a=gsl_rng_uniform_int(tau,2); // numero entero entre 0 y 1;

            //if(a==0)
        //    {matriz[i][j]=-1;} //así obtengo los espines -1
            //else
            {matriz[i][j]=1;} //así obtengo los espines 1

            if(j!=N-1)
            {fprintf(file,"%i\t", matriz[i][j]);}
            //escribo la matriz en el fichero de texto  estado_inicialSEMILLA.txt
            // con un formato de que en cada nueva fila, haga un salto de linea
            // y escriba la siguiente fila en la linea siguiente (aunque yo no
            // lo veré en mi fichero de texto en este formato por el tamaño de mi pantalla)
            else
            {fprintf(file,"%i\n", matriz[i][j]);}
            // los datos de una misma fila vienen separados por un tabulador
        }
    }
    return;
}

void CAMBIO(int matriz[N][N],double T)
{
    int i,j; //contadores
    double p,E,x; // E hace referencia a la diferencia de energía entre un estado
    // y el siguiente estado, p sería la probabilidad de que dicho paso ocurriera
    // x es un número aleatorio entre 0 y 1
    extern gsl_rng *tau; //Puntero al estado del número aleatorio

    i=gsl_rng_uniform_int(tau,N);  //número aleatorio entero [0,63]
    j=gsl_rng_uniform_int(tau,N);  //número aleatorio entero [0,63]
    // Así escogería un spin aleatorio de la matriz[64][64] que luego evaluaría
    // para ver si cambia.

    ////////////////////////////////////////////////////////////////////////////
    // Tengo el problema de las condiciones de contorno                       //
    // lo vamos a resolver a continuación con condiciones if, hay 8 casos     //
    // especiales y el general                                                //
    ////////////////////////////////////////////////////////////////////////////
    if(i==0 && j==0) //caso [0,0]
    {
        E=2*matriz[i][j]*(matriz[i+1][j]+matriz[N-1][j]+matriz[i][j+1]+
        matriz[i][N-1]); // así se calcula la variación de energía entre dos
        //estados en un paso de markov en concreto para el punto [0,0], el caso
        //general lo he puesto el último
    }
    else if(i==0 && j==N-1) // caso [0,63]
    {
        E=2*matriz[i][j]*(matriz[i+1][j]+matriz[N-1][j]+matriz[i][0]+
        matriz[i][j-1]);
    }
    else if(i==N-1 && j==0) //caso [63,0]
    {
        E=2*matriz[i][j]*(matriz[0][j]+matriz[i-1][j]+matriz[i][j+1]+
        matriz[i][N-1]);
    }
    else if((i==N-1) && (j==N-1)) // caso [63,63]
    {
        E=2*matriz[i][j]*(matriz[0][j]+matriz[i-1][j]+matriz[i][0]+
        matriz[i][j-1]);
    }
    else if(i==0)
    {
        E=2*matriz[i][j]*(matriz[i+1][j]+matriz[N-1][j]+matriz[i][j+1]+
        matriz[i][j-1]);
    }
    else if(j==0)
    {
        E=2*matriz[i][j]*(matriz[i+1][j]+matriz[i-1][j]+matriz[i][j+1]+
        matriz[i][N-1]);
    }
    else if(i==N-1)
    {
        E=2*matriz[i][j]*(matriz[0][j]+matriz[i-1][j]+matriz[i][j+1]+
        matriz[i][j-1]);
    }
    else if(j==N-1)
    {
        E=2*matriz[i][j]*(matriz[i+1][j]+matriz[i-1][j]+matriz[i][0]+
        matriz[i][j-1]);
    }
    else
    {
        E=2*matriz[i][j]*(matriz[i+1][j]+matriz[i-1][j]+matriz[i][j+1]+
        matriz[i][j-1]);// así se calcula la variación de energía entre dos
        //estados en un paso de markov
    }

    p=exp(-E/T); // así se calcula la probabilidad de que dicho paso ocurra.

    if(p>1) // si la probabilidad saliera mayor que uno yo la igualo a 1 porque no puede ser mayor
    {p=1;}

    x=gsl_rng_uniform(tau);  //número aleatorio real [0,1] es el que evalua si
    //dicho paso de markov se produce

    if(x<p)
    {matriz[i][j]=-1*matriz[i][j];} // hago el cambio si x<p en caso contrario el
    // sistema no cambia

    return;
}

void ESCRIBIR(int matriz[N][N], FILE *file)
{
    int i,j; // contadores

    for(i=0;i<N;i++)
    {
        for(j=0;j<N;j++)
        {
            if((j==N-1) && (i==N-1))
            {
                fprintf(file,"%i\t%i\t%i\n", i,j, matriz[i][j]);
            }
            else
            {
                fprintf(file,"%i\t%i\t%i\t", i,j, matriz[i][j]);
                //escribo la matriz en el fichero de texto  estado_inicialSEMILLA.txt
                // con un formato de que escribo todos la x la y y el espín seguidos
                // y toda la matriz seguida, el salto de linea se da entre matrices
                // diferentes
            }

        }
    }
    return;
}

double CALCULAR_MN(int s[N][N])
{
    int i,j;
    double sol;

    sol=0.0;
    for(i=0;i<N;i++)
    {
        for(j=0;j<N;j++)
        {
            sol=sol+s[i][j];
        }
    }
    sol=fabs(sol*1.0)/(N*N);

    return sol;
}

double CALCULAR_EN(int matriz[N][N])
{
    int i,j;
    double E;

    E=0.0;
    for(i=0;i<N;i++)
    {
        for(j=0;j<N;j++)
        {
            ////////////////////////////////////////////////////////////////////////////
            // Tengo el problema de las condiciones de contorno                       //
            // lo vamos a resolver a continuación con condiciones if, hay 8 casos     //
            // especiales y el general                                                //
            ////////////////////////////////////////////////////////////////////////////
            if(i==0 && j==0) //caso [0,0]
            {
                E=E-0.5*matriz[i][j]*(matriz[i+1][j]+matriz[N-1][j]+matriz[i][j+1]+
                matriz[i][N-1]); // así se calcula la variación de energía entre dos
                //estados en un paso de markov en concreto para el punto [0,0], el caso
                //general lo he puesto el último
            }
            else if(i==0 && j==N-1) // caso [0,63]
            {
                E=E-0.5*matriz[i][j]*(matriz[i+1][j]+matriz[N-1][j]+matriz[i][0]+
                matriz[i][j-1]);
            }
            else if(i==N-1 && j==0) //caso [63,0]
            {
                E=E-0.5*matriz[i][j]*(matriz[0][j]+matriz[i-1][j]+matriz[i][j+1]+
                matriz[i][N-1]);
            }
            else if((i==N-1) && (j==N-1)) // caso [63,63]
            {
                E=E-0.5*matriz[i][j]*(matriz[0][j]+matriz[i-1][j]+matriz[i][0]+
                matriz[i][j-1]);
            }
            else if(i==0)
            {
                E=E-0.5*matriz[i][j]*(matriz[i+1][j]+matriz[N-1][j]+matriz[i][j+1]+
                matriz[i][j-1]);
            }
            else if(j==0)
            {
                E=E-0.5*matriz[i][j]*(matriz[i+1][j]+matriz[i-1][j]+matriz[i][j+1]+
                matriz[i][N-1]);
            }
            else if(i==N-1)
            {
                E=E-0.5*matriz[i][j]*(matriz[0][j]+matriz[i-1][j]+matriz[i][j+1]+
                matriz[i][j-1]);
            }
            else if(j==N-1)
            {
                E=E-0.5*matriz[i][j]*(matriz[i+1][j]+matriz[i-1][j]+matriz[i][0]+
                matriz[i][j-1]);
            }
            else
            {
                E=E-0.5*matriz[i][j]*(matriz[i+1][j]+matriz[i-1][j]+matriz[i][j+1]+
                matriz[i][j-1]);// así se calcula la variación de energía entre dos
                //estados en un paso de markov
            }
        }
    }
    return E;
}

double CALCULAR_CN(int s[N][N])
{
    double E;

    E=CALCULAR_EN(s);
    return E*E;
}

void FUNCION_CORRELACION(double f[1+N/2], int s[N][N])
{
    int i,j,k;
    double a;

    for(k=1;k<=(N/2);k++)
    {
        for(i=0;i<N;i++)
        {
            for(j=0;j<N;j++)
            // la distancia maxima entre dos puntos de la red en la misma
            //fila es N/2 porque la red tiene los extremos conectados.
            {
                //si tenemos un s[i][j], cercano al final de la linea, tendremos que empezarla de nuevo, es decir
                if((k+i)<N)
                {
                    f[k]=f[k]+s[i][j]*s[i+k][j];
                }
                if((k+i)>=N)
                {
                    f[k]=f[k]+s[i][j]*s[k+i-N][j];
                }
            }
        }
    }
return;
}
