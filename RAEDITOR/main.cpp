#include <functions.hpp>

int main(int argc, char ** argv) {

    if (argc<2) {
        printf("%s option <filename>'\n",argv[0]);
        return -1;
    }

    std::string option=std::string() + argv[1];
    transform(option.begin(), option.end(), option.begin(), ::tolower);

    if (option == "readbin") {

        unsigned char* bin = NULL;
        long size;

        if ( ( size = main_readbin( argc, argv, bin ) ) > 0 ) {

            int ret = edit_bin( bin, size );
            if ( ret != 0 ) {
                printf( "Error map-size vs. file-size\n" );
                return ret;
            }
            make_bin( bin, size );
            save_bin( bin, size );
            if ( bin != NULL ) free( bin );

        } else {
            if ( bin != NULL ) free( bin );
            return size;
        }

    } else if ( option == "readtileset" ) {

        if (argc<3) {
            printf("%s readtileset <filename>'\n",argv[0]);
            printf("Example: %s readtileset tilesets/interior.yaml>'\n",argv[0]);

            return -1;
        }

        main_readtileset(argv[2]);

    } else {

        printf( "%s readbin <path_to_map.bin>'\n", argv[0] );
        printf( "%s readtileset <path_to_map.yaml>'\n", argv[0] );

        return -3;

        //printf("\n");
    }
    return 0;
}

int main2()
{
    char map[1000][1000];
    FILE* names;
    if ((names = fopen("output33.txt","r"))!=NULL)
    {
        char line[65000];
        int xx,yy=0;
        int len=0;
        while (fgets(line,65000,names)!=NULL) {
            xx=0;
            printf("%s",line);
            len=strlen(line);
//        printf("len=%d\n",len);
            for (int n=0; n<len; n++) {

                if (line[n]==' ') {
                    map[xx][yy]=' ';
                } else if (line[n]=='x') {
                    map[xx][yy]='x';
                } else {
                    map[xx][yy]='O';
                }
                xx++;
            }
            yy++;
        }
        printf("\n");

        int y_len=yy;

        for (yy=0; yy<y_len; yy++) {
            printf("%3d ",yy);
            for (xx=0; xx<len; xx++) {
                printf("%c",map[xx][yy]);
            }
            printf("\n");
        }

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
        printf("\nx=%d y=%d\n",len,y_len);
        int actor=0;
        for (yy=0; yy<y_len; yy++) {
            for (xx=0; xx<len; xx++) {

                if (map[xx][yy]=='O') {
                    printf("	Actor%d: brik\n",actor);
                    printf("		Owner: Neutral\n");
                    printf("		Location: %d,%d\n",xx,yy);
                    actor++;
//                } else if (map[xx][yy]=='.' && (xx==1 || yy==1 || xx==len-2 || yy==y_len-2) ) {
//                    printf("	Actor%d: brik\n",actor);
//                    printf("	Actor%d: minv\n",actor);
//                    printf("		Owner: Neutral\n");
//                    printf("		Location: %d,%d\n",xx,yy);
//                    actor++;
                } else if (map[xx][yy]=='x') {
                    if ((xx%8)==5 && (yy%8)==5) {
                        printf("	Actor%d: gmine\n",actor);
                        printf("		Owner: Neutral\n");
                        printf("		Location: %d,%d\n",xx,yy);
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
