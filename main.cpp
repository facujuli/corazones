#include <iostream>
#include "CSVData.h"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <random>
#include <fstream>

using namespace std;
using json = nlohmann::json;

CSVData dataConditioning(CSVData data);
void dataConverter(int separator, string *data);
json saveToJsonFile(CSVData data, string path, string csvPath);
double getEntropy(CSVData data, map<string, string> attributes, vector<string> titles);
double getGain(CSVData data, vector<string> titles, double entropy, map<string, string> attributes, string newAttribute, int outcomes);

int main(void)
{
    CSVData heart;

    if (!readCSV("../../heart.csv", heart))
    {
        cout << "No pudimos leer heart.csv" << endl;
        return 1;
    }

    // La primer columna contiene los nombres de los campos, nos interesa conservarla pero no en el CSVData.
    vector<string> titles;
    int i;
    for (i = 0; i < 14; i++)
    {
        titles.push_back(heart[0][i]);
    }

    heart.erase(heart.begin());

    heart = dataConditioning(heart);

    // Reordeno heart
    std::random_device rd; // semilla aleatoria
    std::mt19937 g(rd());
    std::shuffle(heart.begin(), heart.end(), g);

    map<string, string> attributes = {{"age", "1"}, {"sex", "1"}};

    double entropy = getEntropy(heart, attributes, titles);
    cout << entropy << endl;

    double gain = getGain(heart, titles, entropy, attributes, "fbs", 2);
    cout << gain << endl;

    // json jsonData = saveToJsonFile(heart, "jsonData.json", "../../heart.csv");

    return 0;
}

double getGain(CSVData data, vector<string> titles, double entropy, map<string, string> attributes, string newAttribute, int outcomes)
{
    double gain = 0;

    for (int k = 0; k < outcomes; k++) // por cada Sj
    {
        attributes[newAttribute] = (char)k + '0';

        float wantedCount = 0, totalCount = 0;
        int flagAttr, flagTotal;
        int last = (int)(data.size() * 0.8); // contiene la posicion del ultimo elemento a analizar
        for (int j = 0; j < last; j++)       // recorre las filas de los datos
        {
            flagTotal = 0;
            flagAttr = 0;
            for (int i = 0; i < titles.size() - 1; i++) // recorre las columnas de los datos
            {
                auto it = attributes.find(titles[i]); // verifico si el atributo de la instancia coincide con alguno que debo fijar

                if (it != attributes.end())
                {
                    if ((data[j][i] == it->second))
                    {
                        flagAttr++; // si el caso cumple con todos los atributos pedidos, incremento el flag que cuenta estos casos.
                    }
                    else if (it == attributes.find(newAttribute))
                    {
                        flagTotal++; // si cumple con todos menos el nuevo, incremento el flag que cuenta esos casos
                    }
                }
            }

            if (flagAttr == attributes.size()) // si el caso cumple todos los atributos
            {
                wantedCount++; // cuento casos totales que cumplan las condiciones
                totalCount++;  // cuento casos totales a tener en cuenta
            }
            else if (flagAttr == attributes.size() - 1 && flagTotal > 0) // si cumple todos menos el nuevo
            {
                totalCount++; // agrego a los casos totales a tener en cuenta
            }
        }

        float pj = wantedCount / totalCount; // Proporción de casos queridos.
        if (pj != 0)
        {
            gain += (wantedCount / totalCount) * getEntropy(data, attributes, titles);
        }
    }

    return entropy - gain;
}

/******************************************************************************************************
 * Calcula la entropía de un conjunto de datos
 * data: CSVData que contiene la informacion a analizar
 * attributes: mapa donde la clave es un int que representa la columna del atributo, y el valor
 *  como un string
 *  Si no encuentra casos que cumplan las condiciones solicitadas ( se le solicitan casos inexistentes),
 * devuelve -1 indicando error.
 *********************************************************************************************************/
double getEntropy(CSVData data, map<string, string> attributes, vector<string> titles)
{
    double entropy = 0;
    float positiveCounter = 0;
    int last = (int)(data.size() * 0.8); // contiene la posicion del ultimo elemento a analizar
    float pi = 0;
    int totalCount = 0;

    if (attributes.empty()) // Calculo la entropía del nodo raiz
    {
        for (totalCount = 0; totalCount < last; totalCount++)
        {
            if (data[totalCount][titles.size() - 1] == "1")
            {
                positiveCounter++; // cuenta casos de output = 1
            }
        }
        pi = positiveCounter / totalCount; // Proporción de casos positivos.
        if (pi != 0)
            entropy += pi * log2(pi);

        pi = (totalCount - positiveCounter) / totalCount; // Proporción de casos negativos.
        if (pi != 0)
            entropy += pi * log2(pi);

        return (-1) * entropy;
    }

    int i, j, flag;
    for (j = 0; j < last; j++)
    {
        flag = 0;
        for (i = 0; i < titles.size() - 1; i++)
        {
            auto it = attributes.find(titles[i]); // verifico si el atributo de la instancia coincide con alguno que debo fijar
            if (it != attributes.end() && (data[j][i] == it->second))
            {
                flag++; // si el caso cumple con la condicion solicitada, incremento el flag que cuenta estos casos.
            }
        }

        if (flag == attributes.size()) // si el caso cumple todos los atributos
        {
            totalCount++; // cuento casos totales que cumplan las condiciones
            if (data[j][titles.size() - 1] == "1")
            {
                positiveCounter++;
            }
            // cout << data[j][titles.size() - 1 ] << '\t' << positiveCounter << '\t' << totalCount << endl;
        }
    }
    if (totalCount == 0) // no encontré ningun caso que cumpla esas condiciones.
    {
        return -1;
    }
    pi = positiveCounter / totalCount; // Proporción de casos positivos.
    if (pi != 0)
        entropy += pi * log2(pi);

    pi = (totalCount - positiveCounter) / totalCount; // Proporción de casos negativos.
    if (pi != 0)
        entropy += pi * log2(pi);

    return (-1) * entropy;
}

/*************************************************************************************************************
 * Guarda los datos CSVData en formato json en disco y devuelve el json generado
 **************************************************************************************************************/
json saveToJsonFile(CSVData data, string path, string csvPath)
{
    json jsonData = {};
    CSVData heart;

    if (!readCSV(path, heart))
    {
        cout << "No pudimos leer heart.csv" << endl;
        return jsonData;
    }

    int j = 0;
    for (auto it = data.begin(); it != data.end(); it++)
    {
        for (int i = 0; i < it[0].size(); i++)
        {
            jsonData[j][heart[0][i]] = it[0][i];
        }
        j++;
    }

    ofstream of(path);
    of << jsonData.dump(2) << endl;

    return jsonData;
}

CSVData dataConditioning(CSVData data)
{
    for (auto it = data.begin(); it != data.end(); it++)
    {
        // Guardo el valor de la edad como int
        int i = 0, age = 0;
        while (it[0][0][i] != 0)
        {
            age = age * 10 + it[0][0][i] - '0';
            i++;
        }
        /*
        Cambio el valor de la edad por los valores 0, 1, 2  dependiendo de si es <50 años, entre
        50 y 65 años, >65 años respectivamente
        */
        if (age < 50)
        {
            it[0][0] = '0';
        }
        else if (age > 65)
        {
            it[0][0] = '2';
        }
        else
        {
            it[0][0] = '1';
        }

        // Cambio el valor del nivel de azucar por 0 o 1 dependiendo de si es bajo o alto respectivamente.
        dataConverter(120, &it[0][3]);

        // Cambio el valor del colesterol
        dataConverter(240, &it[0][4]);

        // Cambio el valor de la tasa
        dataConverter(150, &it[0][7]);

        // Cambio el valor de la depresion
        dataConverter(1, &it[0][9]);
    }
    return data;
}

/*************************************************************************************************************
 * Recibe un separador que contiene el valor que separa a los datos en altos y bajos ( 0 y 1 respectivamente),
 * y un puntero  a un string con el valor del respectivo atributo
 **************************************************************************************************************/
void dataConverter(int separator, string *data)
{
    int i = 0, value = 0;
    while (i != (*data).length() && data[0][i] != '.')
    {
        value = value * 10 + data[0][i] - '0';
        i++;
    }

    if (value < separator)
    {
        *data = '0';
    }
    else
    {
        *data = '1';
    }
    return;
}