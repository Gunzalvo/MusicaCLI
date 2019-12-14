#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Map.h"
#include "list.h"

const int TALLA_MAX = 300;

typedef struct cancion Cancion;
typedef struct artista Artista;
typedef struct album Album;

struct artista {
    list* canciones;
    char* nombre;
};

struct cancion {
    Artista* artista;
    Album* album;
    char* nombre;
    char* duracion;
};

struct album {
    Artista* artista;
    list* canciones;
    char* nombre;
    char* fecha;
};

char * _strdup (const char *s) {
    size_t len = strlen (s) + 1;
    void *new = malloc (len);

    if (new == NULL)
        return NULL;

    return (char *) memcpy (new, s, len);
}

const char *get_csv_field (char * tmp, int i) {
    //se crea una copia del string tmp
    char * line = _strdup (tmp);
    const char * tok;
    for (tok = strtok (line, ","); tok && *tok; tok = strtok (NULL, ",\n")) {
        if (!--i) {
            return tok;
        }
    }
    return NULL;
}

long long stringHash(const void * key) {
    long long hash = 5381;

    const char * ptr;

    for (ptr = key; *ptr != '\0'; ptr++) {
        hash = ((hash << 5) + hash) + tolower(*ptr); /* hash * 33 + c */
    }

    return hash;
}

int stringEqual(const void * key1, const void * key2) {
    const char * A = key1;
    const char * B = key2;

    return strcmp(A, B) == 0;
}

int ingresarMusica(char* filename, Map* canciones, Map* albums, Map* artistas){
    char linea[1024];
    char nombre[260] = "\0";
    char csv[5] = ".csv\0";

    strcat(nombre, filename);
    strcat(nombre, csv);

    FILE* archivo = fopen(nombre, "r");

    if ( archivo != NULL)
    {
        fgets(linea, 1024, archivo);

        while( fgets(linea, 1024, archivo) )
        {
            char *songName = _strdup(get_csv_field(linea, 1) );
            char *artistName = _strdup(get_csv_field(linea, 2) );
            char *songDuration = _strdup(get_csv_field(linea, 3) );
            char *albumName = _strdup(get_csv_field(linea, 4) );
            char *albumDate = _strdup(get_csv_field(linea, 5) );

            if( (searchMap(canciones, songName) != NULL) &&
                (searchMap(artistas, artistName) != NULL) )
            {
                continue;
            }

            Artista *artista = (Artista*) searchMap(artistas, artistName);

            if( artista == NULL )
            {
                artista = (Artista*) malloc( sizeof(Artista) );
                artista->nombre = artistName;
                artista->canciones = list_create_empty();
                insertMap(artistas, artista->nombre, artista);
            }

            Album *album = searchMap(albums, albumName);

            if(  album == NULL )
            {
                album = (Album*) malloc( sizeof(Album) );
                album->artista = artista;
                album->nombre = albumName;
                album->fecha = albumDate;
                album->canciones = list_create_empty();
                insertMap(albums, album->nombre, album);
            }

            Cancion *cancion = (Cancion*) malloc(sizeof(Cancion));
            cancion->nombre = songName;
            cancion->artista = artista;
            cancion->duracion = songDuration;
            cancion->album = album;

            list_push_back(artista->canciones, cancion);
            list_push_back(album->canciones, cancion);
            insertMap(canciones, cancion->nombre, cancion);
        }
        fclose( archivo );
        return 1;
    }
    fclose( archivo );
    return 0;
}

int leerNombre(char *string, int talla)
{
    char c = '\0';

    // leemos caracter a caracter hasta la talla
    for(int i = 0; i < talla; i++)
    {
        scanf("%c", &c);
        // Si encontramos un salto de linea antes de llegar a la talla
        // devolvemos 1 (Exito).
        if ( c == '\n' ) return 1;
        // Si llegamos al largo (-2 caracteres) y no hay un salto de linea
        // el nombre es muy largo...
        if ( (i == (talla - 3) ) && ( c != '\n' ) )
        {
            // Leemos lo que resta de la entrada
            // para luego hacer otro ingreso limpio.
            do {
                scanf("%c", &c);
            } while (c != '\n');

            // Limpiamos la variable
            for (int k = 0; k < talla; k++) {
                string[k] = '\0';
            }
            return 0;
        }
        string[i] = c;
    }
    return 1; // este return no es necesario pero la IDE me molestaba.
}

int main() {
    char op;
    Map* mapa_canciones = createMap(stringHash, stringEqual);
    Map* mapa_album = createMap(stringHash, stringEqual);
    Map* mapa_artistas = createMap(stringHash, stringEqual);

    printf("------------- Menu MusikCLI -------------\n");
    printf("1. Importar musica desde un archivo CSV\n");
    printf("2. Exportar musica a archivo CSV\n");
    printf("3. Agregar album\n");
    printf("4. Agregar cancion\n");
    printf("5. Eliminar canciones de un artista\n");
    printf("6. Buscar cancion\n");
    printf("7. Buscar canciones de un artista\n");
    printf("8. Buscar album\n");
    printf("9. Salir\n");
    printf("Seleccione una opcion:\n");
    scanf("%c", &op);

    while( op != '9')
    {
        char filename[255] = "\0";
        char songName[TALLA_MAX];
        for(int i = 0; i < TALLA_MAX; i++) songName[i]='\0';
        Cancion *cancion;
        Artista *artista;
        Album *album;

        switch(op)
        {
            case '1':
                scanf("%c", &op);
                printf("Ingrese nombre del archivo sin extension .csv\n");
                leerNombre(filename, 257);

                if ( ingresarMusica(filename, mapa_canciones,
                                              mapa_album,
                                              mapa_artistas) )
                {
                    printf("Musica ingresada correctamente.\n");
                }
                else
                {
                    printf("No se pudo ingresar la musica.\n");
                }
                break;
            case '2':
                break;
            case '3':
                break;
            case '4':
                break;
            case '5':
                break;
            case '6':
                scanf("%c", &op);
                if ( mapCount(mapa_canciones) != 0 )
                {
                    printf("Ingrese nombre de la cancion a buscar:\n");
                    leerNombre(songName, TALLA_MAX + 2);

                    Cancion *cancion = searchMap(mapa_canciones, songName);

                    if( cancion != NULL )
                    {
                        printf("Artista: %s\nDuracion: %s\n",
                               cancion->artista->nombre,
                               cancion->duracion);

                        if ( !strcmp(cancion->album->nombre, "-") )
                        {
                            printf("Album: Desconocido\n");
                        }
                        else
                        {
                            printf("Album: %s\n", cancion->album->nombre);
                        }

                        if( !strcmp(cancion->album->fecha, "0") )
                        {
                            printf("Año: Desconodido\n");
                        }
                        else
                        {
                            printf("Año: %s\n", cancion->album->fecha);
                        }

                    }
                    else printf("No se encontro la cancion.\n");
                }
                else
                {
                    printf("No hay canciones para buscar!.\n");
                }
                break;
            case '7':
                scanf("%c", &op);
                if ( mapCount(mapa_artistas) != 0 )
                {
                    printf("Ingrese nombre del artista a buscar:\n");
                    leerNombre(songName, TALLA_MAX + 2);

                    artista = (Artista*) searchMap(mapa_artistas, songName);
                    if(artista != NULL)
                    {
                        int i = 1;
                        cancion = (Cancion*) list_first(artista->canciones);
                        while( cancion )
                        {
                            printf("%d. %s / %s / %s / %s.\n", i,
                                    cancion->nombre, cancion->duracion,
                                    cancion->album->nombre,
                                    cancion->album->fecha);
                            i++;
                            cancion = (Cancion*) list_next(artista->canciones);
                        }
                    }
                    else
                    {
                        printf("Artista no encontrado.\n");
                    }
                }
                else
                {
                    printf("No hay artistas para buscar!.\n");
                }
                break;
            case '8':
                scanf("%c", &op);
                if ( mapCount(mapa_album) != 0 )
                {
                    printf("Ingrese nombre del album a buscar:\n");
                    leerNombre(songName, TALLA_MAX + 2);

                    album = (Album*) searchMap(mapa_album, songName);
                    if(album != NULL)
                    {
                        int i = 1;
                        printf("Artista: %s | Año: %s\n",
                                album->artista->nombre,
                                album->fecha);
                        cancion = (Cancion*) list_first(album->canciones);
                        while( cancion )
                        {
                            printf("%d. %s / %s\n", i, cancion->nombre,
                                                  cancion->duracion);
                            i++;
                            cancion = (Cancion*) list_next(album->canciones);
                        }
                    }
                    else
                    {
                        printf("Album no encontrado.\n");
                    }
                }
                else
                {
                    printf("No hay albums para buscar!.\n");
                }
                break;
            case '9':
                return 0;
            default:
                printf("Opcion incorrecta.");
        }
        printf("Ingrese una opcion:\n");
        scanf("%c", &op);
    }

    return 0;
}
