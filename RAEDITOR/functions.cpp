
#include <functions.hpp>
#include <SFML/System/Thread.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>

void edit_bin(unsigned char* bin, long filesize) {
//    printf( "bin=0x%p filesize=%ld\n", bin, filesize );
    for ( int n=0; n<17; n++ ) {
        if ( n>0 && n<17 ) printf(" ");
        printf("%02X", (unsigned char)bin[n]);
    }
    printf("\n");

    int16_t size_x = *(int16_t*)(bin + 1);
    int16_t size_y = *(int16_t*)(bin + 3);

    printf("size_x=%4d (%04X)\nsize_y=%4d (%04X)\n", size_x, size_y, size_x, size_y);
    int bytes=17 + 5 * size_x * size_y;
    printf("bin=( %d x %d ) x 5 + 17 = %d bytes.\n", size_x, size_y, bytes);
    if ( filesize!= bytes ) {
        printf("Filesize does not match map resolution.\n");
    }
    sf::Image image;
    image.create( size_x,  size_y, sf::Color(255,0,0,255));
    sf::Color color=sf::Color(0,0,0,0);

//    unsigned char* val1a_ptr = ( bin + 17 );
//    unsigned char* val1b_ptr = ( bin + 17 + 2 );
//    unsigned char* val2_ptr  = ( bin + 17 + ( size_x * size_y ) * 3 );


    int16_t offset;
    int16_t val1a;
    int8_t  val1b;
    int16_t val2;


    for (int y=0; y < size_y; y++) {
        for (int x=0; x < size_x; x++) {
            offset = x + size_x * y;

            val1a = (int16_t) ( *( (int16_t*) ( bin + 17                            + offset*3  )  ) );
            val1b = (int8_t)  ( *( (int16_t*) ( bin + 17 + 2                        + offset*3  )  ) );
            val2  = (int16_t) ( *( (int16_t*) ( bin + 17 + ( size_x * size_y ) * 3  + offset*3  )  ) );

//            val1a = (int16_t) ( *( val1a_ptr  ) );
//            val1b = (int8_t)  ( *( val1b_ptr  ) );
//            val2  = (int16_t) ( *( val2_ptr   ) );


//            val1a = (int16_t) ( (int16_t)  *( val1a_ptr + offset*3 ) );
//            val1b = (int8_t)  ( (int8_t)   *( val1b_ptr + offset*3 ) );
//            val2  = (int16_t) ( (int16_t)  *( val2_ptr  + offset*2 ) );
//            val1a = (int16_t) ( *( val1a_ptr + offset*3 ) );
//            val1b = (int8_t)  ( *( val1b_ptr + offset*3 ) );
//            val2  = (int16_t) ( *( val2_ptr  + offset*2 ) );
//            if ( val1a == 385 ) {
                if (val1a == 275) {
                    printf(".");
                } else {
                    printf("*");
                }
//            if ( x==1 && y==1 ) {
//                printf("x=%4d y=%4d val1a=%04X=%5d val1b=%02X=%3d val2=%04X=%5d\n", x, y, val1a, val1a, val1b, val1b, val2, val2 );
//            }
        }
        printf("\n");
    }

}

long read_bin_file( string filename, unsigned char* &mem) {
//    printf( "filename: %s\n", filename.c_str() );
//    char* mem=bin;
    if ( file_exists( filename.c_str() ) ) {
        std::filesystem::path p{ filename.c_str() };
        long fz = std::filesystem::file_size(p);
        printf(
            "Filesize of \"%s\" is: %lu bytes. malloc()="
            ,filename.c_str()
            ,fz
        );
        fflush(stdout);
        if (FILE* file = fopen(filename.c_str(), "r")) {
            long n;

            mem=(unsigned char*)	calloc(fz,1);
            if (mem==NULL) {
                printf("ERROR ALLECATING MEMORY\n");
                return -1;
            } else {
                printf( "0x%p, reading, ", mem );
                fflush( stdout );
            }

            if ( ( n = (long) fread( mem, 1, fz, file ) ) != fz ) {
                printf( "FATAL ERROR: unable to read file \"%s\".\n", filename.c_str() );
                free(mem);
                mem=NULL;
                fclose( file );
                return -2;
            } else {
                printf( "%lu bytes read, ", n );
                fflush( stdout );
                fclose( file );
                printf( "ok.\n" );
                return n;
            }
        } else {
            printf( "ERROR OPENING FILE: %s\n", filename.c_str() );
            return -3;
        }
    } else {
        printf( "File \"%s\" does not exist.\n", filename.c_str() );
        return -4;
    }
}

bool file_exists( const char * filename ) {
    struct stat stat_buffer;
    if ( stat(filename,&stat_buffer) == 0 ) return true;
    return false;
}

