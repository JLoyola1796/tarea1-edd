#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

// Importacion de librerias. Los archivos correspondientes deben estar en el mismo directorio que el .cpp.
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <fstream>

using namespace std;

const char* imgName = ("Pikachu.png");

// Struct que almacena la data de una imagen
struct Imagen
{
    unsigned char* data;
    int width, height, channels;
};

// Abrir imagen desde memoria local
Imagen* load(const char* filename) 
{
    Imagen* img = new Imagen();
    img->data = stbi_load(filename, &img->width, &img->height, &img->channels, 0);
    return img;
}

// Guardar imagen en memoria local
void save(Imagen* img, const char* filename) 
{
    cout << "Guardando imagen: " << filename << "\n";
    cout << "Dimensiones de la imagen: " << img->width << "x" << img->height << "\n";
    stbi_write_png(filename, img->width, img->height, img->channels, img->data, img->width * img->channels);
    cout << "Imagen guardada:  " << filename << "\n";
}

// Función liberar memoria dinámica de la imagen
void freeImage(Imagen* img)
{
    if (img)
    {
        if (img->data)
        {
            stbi_image_free(img->data);
        }
        delete img;
    }
}


// Función voltear horizontalmente
void flip()
{
    Imagen* img = load(imgName);
    // Datos de la imagen
    int width = img->width;
    int height = img->height;
    int channels = img->channels;
    
    // Recorrer filas y columnas
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width / 2; x++)
        {
            for (int channel = 0; channel < channels; channel++)
            {
                // Posición del pixel izquierdo y derecho respectivamente
                int leftPos = (y * width + x) * channels + channel;
                int rightPos = (y * width + (width - x - 1)) * channels + channel;
                
                // Variable temporal para guardar la posición izquierda antes de invertirla con la derecha
                unsigned char temp = img->data[leftPos];
                // Invertir posiciones
                img->data[leftPos] = img->data[rightPos];
                img->data[rightPos] = temp;
            }
        }
    }
    save(img, "output_flip.png");
    freeImage(img);
}

// Función rotar
void turn()
{
    Imagen* img = load(imgName);
    // Al rotar, la altura se convierte en ancho y viceversa
    int turnedWidth = img->height;
    int turnedHeight = img->width;

    // Abrir la nueva data
    unsigned char* turnedData = new unsigned char[turnedWidth * turnedHeight * img->channels]();
    
    // Recorrer filas y columnas
    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
        {
            // Calcular los índices de la imagen
            int ogI = (y * img->width + x) * img->channels;
            int newI = ((turnedHeight - x - 1) * turnedWidth + y) * img->channels;

            for (int channel = 0; channel < img->channels; channel++)
            {
                turnedData[newI + channel] = img->data[ogI + channel];
            }
        }
    }

    delete[] img->data;
    
    // Se actualiza la estructura de la imagen
    img->data = turnedData;
    img->height = turnedHeight;
    img->width = turnedWidth;

    save(img, "output_turn.png");
    delete[] turnedData;
    delete img;
}


// Función grado de atenuación
void attenuation(float factor)
{
    Imagen* img = load(imgName);
    // Tamaño de la imagen
    int size = img->width * img->height;
    // Recorrer la imagen
    for (int i = 0; i < size; i++)
    {
        for (int c = 0; c < 3 && c < img->channels; ++c)
        {
            // Obtener valor de color del pixel
            int color = img->data[i * img->channels + c];
            // Iluminar el color
            int colorBright = ((255 - color) * factor + color);
            
            // Asignar el pixel original al nuevo iluminado
            img->data[i * img->channels + c] = (colorBright < 255) ? colorBright : 255;
        }
    }
    
    save(img, "output_attenuation.png");
    freeImage(img);
}

/*****
Pregunta 6: Función errónea
Devuelve una imagen saturada en vez del blanco y negro esperado porque no límita los colores previamente a una escala de grises,
trabaja con los colores originales de la imagen y le aplica el umbral a esos.

void colorLimit(int limit)
{
for (int y = 0; y < img->height; y++)
{
for (int x = 0; x < img->width; x++)
{
int indice = (y * img->width + x) * img->channels;

for (int c = 0; c < img->channels; c++)
{
if (img->data[indice + c] > limit)
{
img->data[indice + c] = 255;
}
else
{
img->data[indice + c] = 0;
}
}
}
}

save(img, "output_limit.png");
}
*****/

// Función umbral de color
void colorLimit(int limit)
{
    Imagen* img = load(imgName);
    // Recorrer la imagen
    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
        {
            // Obtener indice del pixel
            int indice = (y * img->width + x) * img->channels;
            // Asignar color gris para transformar el canal
            unsigned char gris = 0;
            
            if (img->channels >= 3)
            {
                // Transfomar la imagen a escala de grises, usando la fórmula de la pregunta teórica 4
                gris = (0.3 * img->data[indice] + 0.59 * img->data[indice + 1] + 0.11 * img->data[indice + 2]);
            }
            else if (img->channels == 1)
            {
                // Si solo tiene 1 canal, ya está en escala de grises
                gris = img->data[indice];
            }
            
            // Aplica el umbral
            unsigned char color = (gris > limit) ? 255 : 0;
            
            for (int c = 0; c < img->channels; c++)
            {
                // Recorre los canales y transforma los pixeles
                img->data[indice + c] = color; 
            }
            
        }
    }
    
    save(img, "output_limit.png");
    freeImage(img);
}

// Función to_ascii
char** to_ascii(char ascii[])
{
    Imagen* img = load(imgName);

    // Crear el arreglo dinámico bimensional
    char** savedAscii = new char*[img->height];
    for (int i = 0; i < img->height; i++) {
        savedAscii[i] = new char[img->width];
    }
    
    // Recorrer la imagen
    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
        {
            // Repetir el proceso para transformar en escala de grises usado en colorLimit
            int indice = (y * img->width + x) * img->channels;
            unsigned char gris = 0;
            if (img->channels >= 3)
            {
                gris = (0.3 * img->data[indice] + 0.59 * img->data[indice + 1] + 0.11 * img->data[indice + 2]);
            }
            else if (img->channels == 1)
            {
                gris = img->data[indice];
            }
            
            // Evalúa el valor de color de cada índice y le asigna un carácter ASCII
            if (img->data[indice] > 0 && img->data[indice] < 64)
            {
                savedAscii[y][x] = {ascii[0]};
            }
            else if (img->data[indice] >= 64 && img->data[indice] < 128)
            {
                savedAscii[y][x] = {ascii[1]};
            }
            else if (img->data[indice] >= 128 && img->data[indice] < 192)
            {
                savedAscii[y][x] = {ascii[2]};
            }
            else if (img->data[indice] >= 192 && img->data[indice] < 256)
            {
                savedAscii[y][x] = {ascii[3]};
            }
            else
            {
                savedAscii[y][x] = {ascii[4]};
            }
        }
    }
    freeImage(img);
    return savedAscii;
}

// Función save_ascii
void save_ascii(char* savedAscii[])
{
    Imagen* img = load(imgName);
    // Abre el archivo para escritura
    fstream asciiImage;
    asciiImage.open("output_ascii.txt", ios::out);
    
    // Error en caso de que no se pudiera abrir
    if (!asciiImage.is_open())
    {
        cerr << "No se pudo abrir el archivo" << endl;
    }
    
    // Recorre la imagen
    for (int y = 0; y < img->height; y++)
    {
        for (int x = 0; x < img->width; x++)
        {
            // Escribe sobre el archivo acorde a lo guardado en to_ascci
            asciiImage << savedAscii[y][x];
        }
        asciiImage << endl;
    }

    // Libera la memoria y cierra el archivo
    cout << "Archivo guardado: output_ascii.txt" << "\n";
    delete[] savedAscii;

    asciiImage.close();
}

int main() 
{
    float factor = 0.8;
    int limit = 180;

    flip();
    turn();
    attenuation(factor);    
    colorLimit(limit);
    char ascii[5] = {'.', '-', '+', '#', ' '};
    char** savedAscii = to_ascii(ascii);
    save_ascii(savedAscii);
    
    return 0;
}
