#include <functions.hpp>

#include <SFML/System/Thread.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>

long main_readbin(int argc, char ** argv, unsigned char* &bin) {
    long size;

    if (argc<3) {

        printf("%s option <filename>'\n",argv[0]);
        return -2;

    } else {

        if ( ( size = read_bin_file( std::string() + argv[2], bin ) ) >= 0 ) {
//                printf( "%ld bytes read.\n", size );
            return size;
        } else {
            printf("Error %ld reading file %s\n", size, argv[2]);
            if ( bin != NULL ) free( bin );
            return (int)size;
        }

    }
}

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
//      Total: 2 bytes * size_x * size_y
//      Every 2 bytes: One 16 bit integer for the tile status, like emerald or shroud / damage stuff or so.
//      Values need to be determined what is what. On map with no spice they are all 0x0000 (hex)


int edit_bin(unsigned char* bin, long filesize) {
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

    if ( filesize != bytes ) {

        printf( "bin=( %d x %d ) x 5 + 17 = %d bytes. FAIL!!!\n", size_x, size_y, bytes );

        printf( "Filesize does not match map resolution.\n" );

        return -1;

    } else {

        printf( "bin=( %d x %d ) x 5 + 17 = %d bytes. OK!\n", size_x, size_y, bytes );

    }

//    sf::Image image;
//    image.create( size_x,  size_y, sf::Color( 255, 0, 0, 255 ) );
//    sf::Color color = sf::Color( 0, 0, 0, 0 );

//    unsigned char* val1a_ptr = ( bin + 17 );
//    unsigned char* val1b_ptr = ( bin + 17 + 2 );
//    unsigned char* val2_ptr  = ( bin + 17 + ( size_x * size_y ) * 3 );

    unsigned int offset;
    uint16_t val1a;
    uint8_t  val1b;
    uint16_t val2;

    FILE* scan;
    char str[4096];

    if ( ( scan = fopen( "scan.txt", "w" ) ) == NULL ) {

        printf( "Can't open %s for writing.\n", "scan.txt " );
        return -1;

    }
    for (int y=0; y < size_y; y++) {

        fprintf( scan, "%04d ", y );

        for (int x=0; x < size_x; x++) {
            offset = x + size_x * y;

            val1a = (int16_t) ( *( (uint16_t*) ( bin + 17                            + offset*3  )  ) );
            val1b = (int8_t)  ( *( (uint8_t*)  ( bin + 17 + 2                        + offset*3  )  ) );
            val2  = (int16_t) ( *( (uint16_t*) ( bin + 17 + ( size_x * size_y ) * 3  + offset*3  )  ) );


            if ( val1a >= 351 && val1a <= 377 ) {

                fprintf( scan, "*%04X-%02X",  val1a, val1b );

            } else {

                fprintf( scan, "|%04X-%02X",  val1a, val1b );

            }

            if ( val1a == 275 ) {

                printf( "." );

            } else {

                if ( val1a >= 351 && val1a <= 377 ) {
//                    printf("x=%4d y=%4d val1a=%04X=%5d val1b=%02X=%3d val2=%04X=%5d\n", x, y, val1a, val1a, val1b, val1b, val2, val2 );

                    sprintf( str, "echo \"| x=%4d y=%4d | val1a=0x%04X (%5d) | val1b=0x%02X (%3d) | val2=0x%04X (%5d) |\" >> fail.txt\n", x, y, val1a, val1a, val1b, val1b, val2, val2 );
                    int ret = system( str );
                    if ( ret != 0 ) printf( "Error system( %s )\n" ,str);

                    printf( "\033[1;31m*\033[0m" );

                } else {

                    printf( "*" );

                }

            }
        }
        printf( "\n" );
        fprintf( scan, "|\n" );
    }

    fclose( scan );
    return 0;
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

int make_bin( unsigned char* bin, long size ) {

    char map[1000][1000];
    FILE* names;

    int16_t size_x = *(int16_t*) (bin + 1);
//    int16_t size_y = *(int16_t*) (bin + 3);

    // wall 340-346
    if ( ( names = fopen( "output33.txt", "r" ) ) != NULL )
    {
        char line[65000];
        int xx, yy = 0;
        int len = 0;

        while ( fgets( line, 65000, names ) != NULL ) {

            xx = 0;
            printf( "%s", line );
            len = strlen( line );
//        printf("len=%d\n",len);
            for ( int n = 0; n < len; n++ ) {

                if ( line[n] == ' ' ) {
                    map[xx][yy] = ' ';
                } else if ( line[n] == 'x' ) {
                    map[xx][yy] = 'x';
                } else {
                    map[xx][yy] = 'O';
                }
                xx++;
            }
            yy++;
        }
        printf( "\n" );

        int y_len = yy;

        for ( yy = 0; yy < y_len; yy++) {
            printf( "%3d ", yy);
            for ( xx = 0; xx < len; xx++ ) {
                printf( "%c", map[xx][yy] );
            }
            printf( "\n" );
        }

/*
        for (yy=0; yy<y_len; yy++) {
            for (xx=0; xx<len; xx++) {

                if ( ! (xx==0 || xx==len-1 || yy==0 || yy==y_len-1) ) {
                    if (map[xx][yy]=='O') {
                        int f=0;
                        for (int y1=-1; y1<=1; y1++) {
                            for (int x1=-1; x1<=1; x1++) {
                                if (map[xx+x1][yy+y1]==' ' || map[xx+x1][yy+y1]=='x') {
                                    f=1;
                                }
                            }
                        }
                        if (f==1) {
                            map[xx][yy]='O';
                        } else {
                            map[xx][yy]='.';
                        }
                    }
                } else {
                    map[xx][yy]='O';
                }
                printf("%c",map[xx][yy]);
            }
            printf("\n");
        }
*/
        uint16_t* val1a_ptr;
        uint8_t*  val1b_ptr;
//        uint16_t* val2_ptr;

        printf( "\nx=%d y=%d\n", len,y_len );
        int actor = 0;

        unsigned int offset;

        for (yy = 0; yy < y_len; yy++ ) {
            for ( xx = 0; xx < len; xx++ ) {

                offset = xx + size_x * yy;

//                val1a = (int16_t) ( *( (uint16_t*) ( bin + 17                            + offset*3  )  ) );
//                val1b = (int8_t)  ( *( (uint8_t*)  ( bin + 17 + 2                        + offset*3  )  ) );

                val1a_ptr =    (uint16_t*) ( bin + 17                            + offset*3  )  ;
                val1b_ptr =    (uint8_t*)  ( bin + 17 + 2                        + offset*3  )  ;
//                val2  = (int16_t) ( *( (uint16_t*) ( bin + 17 + ( size_x * size_y ) * 3  + offset*3  )  ) );


                if (map[xx][yy]=='O') {
    // wall 340-346 0
                    *val1a_ptr = (uint16_t) ( 340 + ( rand() % 7 ) );
                    *val1b_ptr = (uint8_t) ( 0 );
//                    printf("	Actor%d: brik\n",actor);
//                    printf("		Owner: Neutral\n");
//                    printf("		Location: %d,%d\n",xx,yy);
//                    actor++;
//                } else if (map[xx][yy]=='.' && (xx==1 || yy==1 || xx==len-2 || yy==y_len-2) ) {
//                    printf("	Actor%d: brik\n",actor);
//                    printf("	Actor%d: minv\n",actor);
//                    printf("		Owner: Neutral\n");
//                    printf("		Location: %d,%d\n",xx,yy);
//                    actor++;
                } else if (map[xx][yy]==' ') {

    // floor 268 0-11
                    *val1a_ptr = (uint16_t) ( 268 );
                    *val1b_ptr = (uint8_t) (  ( rand() % 12 )  );

                } else if (map[xx][yy]=='x') {

                    *val1a_ptr = (uint16_t) ( 268 );
                    *val1b_ptr = (uint8_t) (  ( rand() % 12 )  );

                    if ( (xx % 8 ) == 5 && ( yy % 8 ) == 5 ) {
                        printf( "	Actor%d: gmine\n", actor );
                        printf( "		Owner: Neutral\n" );
                        printf( "		Location: %d,%d\n", xx, yy);
                        actor++;
                    }
                }
            }
//            printf("\n");
        }

    }

    cout << "Hello world!" << endl;
    return 0;
}


int save_bin(unsigned char* bin, long filesize) {
    FILE* f_ptr;

    if ( ( f_ptr = fopen( "map.bin.new", "w" ) ) == NULL ) {

        printf( "Error opening %s for writing.\n", "map.bin.new" );
        return -1;

    } else {

        size_t bytes = fwrite( bin, 1, filesize, f_ptr );

        printf( "File %s %lu bytes written\n", "map.bin.new", bytes );
        fclose( f_ptr );
        return 0;

    }
}

//	Template@362:
//		Id: 362
//		Images: wall0034.int
//		Size: 2,2
//		Categories: Wall
//		Tiles:
//			0: Wall
//			2: Wall
//			3: Wall

struct Template {
    int Id;
    std::string Images;
    sf::Vector2i Size;
    sf::Vector2i Realsize;
    std::string Catagories;
    std::map< int, std::string > Tiles;
    sf::Image Image;

    bool operator < (const Template &B) const
    {
        return Id < B.Id;
    }
};

struct Tileset {
    long Lookup;    // id * 256 + tile (3 bytes ( * (x + x*y) ) from first block (offset 17) from .bin file)
};

int main_readtileset(char* filename) {

//    std::vector< struct Template > Templates;
//    std::vector< struct Template >::iterator it_Templates;

    struct Template Template;
    std::map< int, struct Template> Templates;
    std::map< int, struct Template>::iterator it_Templates;
    std::map< int, std::string>::iterator it_Tiles;

    if ( !file_exists( filename ) ) {

        printf( "File does not exist: %s\n", filename );
        return -1;

    }

    printf( "Filename: %s\n", filename );

    FILE* f;

    if ( ( f = fopen( filename, "r" ) ) == NULL ) {

        printf( "Can not open file: %s\n", filename );
        return -1;

    } else {

        char line[65000];
        int n = 1;
        char a[5][50];
        bool tiles = false;

        while ( fgets( line, 65000, f ) != NULL ) {
//            printf( "%d: ", 0);
//            printf( "%s", line );
//            n++;

            int num=sscanf( line, "%50[\t]%50[^ ] %50[^ ] %50[^ ] %50[^ ] %50[^ ]", a[0], a[1], a[2], a[3], a[4], a[5] );

            printf( "%3d: %s", n, a[0] );

            a[ num - 1 ][ strcspn( a[ num-1 ], "\n" ) ]  = 0;

            for ( int i = 1; i < num ; i++) {

                sprintf( line, "\033[1;%lum%s\033[0m" , 31 + (strlen( a[0] ) + i*2 +1) % 6 , a[i] );
                printf( "%s ", line);
//                printf( "%s ", a[i]);
            }
            printf("\n");

            if ( strcmp( a[1], "Id:" ) == 0 ) {

                if ( tiles == true ) {
                    tiles = false;
                    Templates.insert( std::make_pair( Template.Id, Template ) );
                    Template.Tiles.clear();
                }

                tiles = false;
                printf( "Id: %s\n", a[2] );
                Template.Id = atoi(a[2]);

            } else if ( strcmp( a[1], "Images:" ) == 0 ) {

                tiles = false;
                Template.Images = std::string() + a[2];
                printf( "Images: %s\n", a[2] );

            } else if ( strcmp( a[1], "Size:") == 0) {

                tiles = false;
                sscanf( a[2], "%d,%d", &(Template.Size.x), &(Template.Size.y ) );
                printf( "Size: %d,%d\n", Template.Size.x, Template.Size.y );

            } else if ( strcmp( a[1], "Tiles:") == 0) {

                tiles = true;

            } else if ( tiles == true) {

                printf( "%s %s\n", a[1], a[2] );
                Template.Tiles.insert( std::make_pair( atoi( a[1] ), a[2] ) );

            }
            n++;
        }
        if ( tiles == true ) {
            tiles = false;
            Templates.insert( std::make_pair( Template.Id, Template ) );
        }

        fclose( f );
        printf( "\n" );
        printf( "SIZE=%lu\n", Templates.size() );

        int error = 0;
        int loaded = 0;

        for (it_Templates = Templates.begin(); it_Templates != Templates.end(); it_Templates++) {

            Template = it_Templates->second;

            printf( "Id: %5d     Size: %d,%d     Images: %15s ",
                Template.Id,
                Template.Size.x,
                Template.Size.y,
                Template.Images.c_str()
            );
            for (it_Tiles = Template.Tiles.begin(); it_Tiles != Template.Tiles.end(); it_Tiles++) {
                if (  it_Tiles != Template.Tiles.begin() ) {
                    printf( "                                                    " );
                }
                printf( "%2d: %s\n", it_Tiles->first, it_Tiles->second.c_str()  );
            }
            std::string path=std::string() + "tiles/" + Template.Images;

            if ( path.find_last_of( "." ) != std::string::npos ) {
                path = path.substr( 0, path. find_last_of( "." ) ) + ".png";
            }
//            path = "tiles/" + Template.Images;
            if ( file_exists( path.c_str() ) ) {
                printf( "Loading image Id: %d  %s ", Template.Id, path.c_str() );
                if ( Template.Image.loadFromFile( path.c_str() ) ) {
                    int x,y;
                    x = Template.Image.getSize().x / 24;
                    y = Template.Image.getSize().y / 24;
                    Template.Realsize.x = x;
                    Template.Realsize.y = y;
                    printf( "Ok. Size: %d,%d / %d,%d  Resolution: %d x %d\n",
                        Template.Size.x,
                        Template.Size.y,
                        Template.Realsize.x,
                        Template.Realsize.y,
                        Template.Image.getSize().x,
                        Template.Image.getSize().y
                    );
                    loaded++;

                } else {
                    printf( "Error.\n" );
                    error++;
                }
                it_Templates->second = Template;
            } else {
                printf( "File does noet exist: %s\n", path.c_str() );
                error++;
            }
//            printf( "\n" );
//    std::map< int, std::string>::iterator it_tiles;



        }

        for (it_Templates = Templates.begin(); it_Templates != Templates.end(); it_Templates++) {

            Template = it_Templates->second;



            printf( "Id: %5d\tSize: %d,%d / %d,%d \tResolution: %d x %d\n",
                Template.Id,
                Template.Size.x,
                Template.Size.y,
                Template.Realsize.x,
                Template.Realsize.y,
                Template.Image.getSize().x,
                Template.Image.getSize().y
            );
            for (it_Tiles = Template.Tiles.begin(); it_Tiles != Template.Tiles.end(); it_Tiles++) {
                int X_START = ( it_Tiles->first % Template.Realsize.x ) * 24;
                int Y_START = (int)( it_Tiles->first / Template.Realsize.x ) * 24;

//                if (  it_Tiles == Template.Tiles.begin() ) {
                    printf( "                                                    " );
//                }
                printf( "%2d: pos(%3d,%3d) %s\n", it_Tiles->first, X_START, Y_START, it_Tiles->second.c_str()  );
            }


        }

        printf( "%d errors loading %d images.\n", error, loaded );

        printf("Templates.size()=%lu\n",Templates.size());

        int n1=Templates.size();

        it_Templates=Templates.begin();

        for ( int n2=0; n2 < n1; n2++ ) {

            struct Template* Template_ptr;
            Template_ptr = &(it_Templates->second);

            printf("n2=%d Id=%d\n",n2,Template_ptr->Id);

            it_Templates++;

        }


//        for ( long unsigned int i=0; i < Templates.size(); i++ ) {
//            Template = Templates[i];
//            printf("i=%lu Id=%d\n",i,Templates[i].Id);
//        }

    }
    return 0;
}


bool file_exists( const char * filename ) {

    struct stat stat_buffer;

    if ( stat( filename, &stat_buffer ) == 0 ) return true;
    return false;
}

