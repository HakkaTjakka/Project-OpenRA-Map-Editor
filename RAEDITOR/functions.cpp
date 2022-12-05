
#include <functions.hpp>
#include <SFML/System/Thread.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>

// File layout .bin file (inside the .oramap files, .zip file...)
// First 17 bytes: map info
//      Byte 1+2: 16 bit integer x-size map
//      Byte 3+4: 16 bit integer y-size map
//      Byte 0, 5-16: Unknown for now
//
// From byte 17: 3 bytes * size_x * size_y
//      Every 3 bytes: One 16 bit (2 bytes) integer for the TILENUMBER,
//          and one 8 bit (1 byte) integer for the VARIATIONS of this tile.
//
//      List of filenames of tiles and their corresponding (16 bin integer) code used in .bin file comes from:
//      File: OpenRA/mods/ra/tilesets/interior.yaml
//      GitHub source: https://github.com/OpenRA/OpenRA/mods/ra/tilesets/interior.yaml
//      And looks like:
/*
    ...
	Template@350:                   // one tile tile
		Id: 350                     // 16 bit integer from .bin
		Images: wall0022.int
		Size: 1,1                   // dimensions (# of tiles)
		Categories: Wall
		Tiles:                      // tiles / layout
			0: Wall                 // numbering (dimentions) left-right top-down (depending on shape... see the build in editor)
	Template@351:                   // 4 tile tile (2 x 2)
		Id: 351
		Images: wall0023.int
		Size: 2,2
		Categories: Wall
		Tiles:
			0: Wall                 // upper left  (0,0)
			1: Wall                 // upper right (0,1)
			2: Wall                 // lower left  (1,0) (3 would be 1,1)
    ...
*/
//
//      Starting with the interior tiles: (there are also winter desert etc.)
//      Tiles: the .png tiles comes from some .mix file of the OpenRA (interior.mix) found in the gamedir,
//           and where extracted with a tool called XCC to .png
// From there (byte 17 + 3 bytes * size_x * size_y)
//      Every 2 bytes: One 16 bit integer for the tile status, like emerald or shroud / damage stuff or so.
//      Values need to be determined what is what. On map with no spice they are all 0x0000 (hex)


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
    if ( filesize!= bytes ) {
        printf("bin=( %d x %d ) x 5 + 17 = %d bytes. FAIL!!!\n", size_x, size_y, bytes);
        printf("Filesize does not match map resolution.\n");
    } else {
        printf("bin=( %d x %d ) x 5 + 17 = %d bytes. OK!\n", size_x, size_y, bytes);
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
                return -10;
            } else {
                printf( "0x%p, reading, ", mem );
                fflush( stdout );
            }

            if ( ( n = (long) fread( mem, 1, fz, file ) ) != fz ) {
                printf( "FATAL ERROR: unable to read file \"%s\".\n", filename.c_str() );
                free(mem);
                mem=NULL;
                fclose( file );
                return -11;
            } else {
                printf( "%lu bytes read, ", n );
                fflush( stdout );
                fclose( file );
                printf( "ok.\n" );
                return n;
            }
        } else {
            printf( "ERROR OPENING FILE: %s\n", filename.c_str() );
            return -12;
        }
    } else {
        printf( "File \"%s\" does not exist.\n", filename.c_str() );
        return -13;
    }
}

bool file_exists( const char * filename ) {
    struct stat stat_buffer;
    if ( stat(filename,&stat_buffer) == 0 ) return true;
    return false;
}

