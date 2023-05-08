#include <iostream>
#include "CSVData.h"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <random>

using namespace std;

int dataConditioning(CSVData data);
void dataConverter(int separator, string* data);

int main(void)
{
    CSVData heart;
    
    if(!readCSV("heart.csv", heart))
    {
        cout << "No pudimos leer heart.csv" << endl;
        return 1;
    }

    //La primer columna contiene los nombres de los campos, no nos interesa conservarla.
    heart.erase(heart.begin());

    
    dataConditioning(heart);

    //Reordeno heart
    std::random_device rd; // semilla aleatoria
    std::mt19937 g(rd());
    std::shuffle(heart.begin(), heart.end(), g);


    return 0;
}

int dataConditioning(CSVData data)
{
    for (auto it = data.begin(); it != data.end() ; it++)
    {
        //Guardo el valor de la edad como int
        int i = 0, age = 0;
        while(it[0][0][i] != 0)
        {
            age = age*10 + it[0][0][i] - '0';       
            i++;
        }
        /*
        Cambio el valor de la edad por los valores 0, 1, 2  dependiendo de si es <50 años, entre 
        50 y 65 años, >65 años respectivamente
        */
        if(age < 50)
        {
            it[0][0] = '0';
        }
        else if(age > 65)
        {
            it[0][0] = '2';
        }
        else
        {
            it[0][0] = '1';
        }

        //Cambio el valor del nivel de azucar por 0 o 1 dependiendo de si es bajo o alto respectivamente.
        dataConverter(120, &it[0][3]);
        
        //Cambio el valor del colesterol
        dataConverter(240, &it[0][4]);

        //Cambio el valor de la tasa
        dataConverter(150, &it[0][7]);
        
        //Cambio el valor de la depresion
        dataConverter(1, &it[0][9]);
    }
    return 1;
}

/*************************************************************************************************************
* Recibe un separador que contiene el valor que separa a los datos en altos y bajos ( 0 y 1 respectivamente),
* y un puntero  a un string con el valor del respectivo atributo
**************************************************************************************************************/
void dataConverter(int separator, string* data)
{
    int i = 0, value = 0;
    while(i != (*data).length() && data[0][i] != '.')
    {
        value = value*10 + data[0][i] - '0';
        i++;
    }

    if(value < separator)
    {
        *data = '0';
    }
    else
    {
        *data = '1';
    }
    return;
}