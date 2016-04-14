/* 
 * Gaurav Nagarnaik
 * gnagarna@stevens.edu
 * Midterm : ls.c
 * CS631
*/

#include <stdio.h>
#include "ls.h"

void usage()
{
	printf("usage: ls [-AaCcdFfhiklnqRrSstuwx1] [file ...]\n");
}





/*
    program entry
*/

int main ( int argc, char ** argv )
{
	int ch;
    FTS * ftsp;
    FTSENT *p, *chp, *cur;
    char * curr_dir = ".";
    int stat_ret;
    struct stat stat_buf;
    DIR * dp;
	struct dirent * dirp;

    /*
        -A is always set for the super user
    */

    if ( !getuid() )
        flag_A_option = 1;

    /*
        The  tzset()  function initializes the tzname variable 
        from the TZ environment variable. 
    */
	
    tzset();

    /* 
        parse options
    */

	while ( ( ch = getopt(argc, argv, "AaCcdFfhiklnqRrSstuwx1") ) != -1 )
	{
		switch (ch)
		{
            case 'A':
                flag_A_option = 1;
                break;
            case 'a':
                flag_a_option = 1;
                break;
            case 'C':
                flag_C_option = 1;
                flag_x_option = 0;
                flag_l_option = 0;
                flag_n_option = 0;
                break;
            case 'c':
                flag_c_option = 1;
                flag_u_option = 0;     /* override -u */
                break;
            case 'd':
                flag_d_option = 1;
                break;
            case 'F':
                flag_F_option = 1;
                break;
            case 'f':
                flag_f_option = 1;
                break;
            case 'h':
                flag_h_option = 1;
                flag_k_option = 0;     /* override -k */
                break;
            case 'i':
                flag_i_option = 1;
                break;
            case 'k':
                flag_k_option = 1;
                flag_h_option = 0;     /* override -h */
                break;
			case 'l':
				flag_l_option = 1;
                flag_1_option = 0;     /* override */
                flag_C_option = 0;
                flag_x_option = 0;
				break;
            case 'n':
                flag_n_option = 1;
                flag_C_option = 0;
                flag_x_option = 0;
                break;
            case 'q':
                flag_q_option = 1;
                flag_w_option = 0;
                break;
            case 'R':
                flag_R_option = 1;
                break;
            case 'r':
                flag_r_option = 1;
                break;
            case 'S':
                flag_S_option = 1;
                break;
            case 's':
                flag_s_option = 1;
                break;
            case 't':
                flag_t_option = 1;
                break;
			case 'u':
                flag_u_option = 1;
                flag_c_option = 0;     /* override -c */
                break;
            case 'w':
                flag_w_option = 1;
                flag_q_option = 0;
                break;
            case 'x':
                flag_x_option = 1;
                flag_C_option = 0;
                flag_l_option = 0;
                flag_n_option = 0;
                break;
            case '1':
                flag_1_option = 1;
                flag_l_option = 0;
                break;
            default:
				usage();
                exit(1);
		}
	}

	argc -= optind;
	argv += optind;

	/* 
        parse file argument(s)
    */

	/* 
        If no operands are given, the contents of the current 
        directory ( "." ) are displayed. 
    */
	
    if ( argc == 0 )
	{
        /* -d */
        if ( flag_d_option )
        {
            stat_ret = lstat ( curr_dir, &stat_buf );
            if ( stat_ret < 0 )
            {
                fprintf ( stderr, "stat error\n" );
                exit (1);
            }

            record_stat ( &stat_buf, curr_dir );
            print_file_info_list ();
            exit (0);    
        }
        
        /* non-recursive */
        if ( ! flag_R_option )
        {
            /* read directory NAME, and list the files in it */	
            if ( ( dp = opendir(curr_dir) ) == NULL )
            {
                fprintf ( stderr, "can't open '%s'\n", curr_dir );
                exit(1);
            }

            /* enter directory */
            if ( chdir(curr_dir) == -1 )
            {
                fprintf ( stderr, "can't chdir to '%s': %s\n",
                    curr_dir, strerror(errno) );
                exit (1);
            }

            while ( ( dirp = readdir(dp) ) != NULL )
            {
                if ( lstat ( dirp->d_name, &stat_buf ) < 0 )
                {
                    fprintf ( stderr, "lstat() error" );
                    exit (1);
                }
                
                record_stat ( &stat_buf, dirp->d_name );
            }

            if ( closedir(dp) < 0 )
            {
                fprintf ( stderr, "can't close directory\n" );
                exit(1);
            }

            print_file_info_list();
            
            exit (0);
        }
        /* -R : recursive */
        else
        {
            if ( ( ftsp =
                 fts_open ( &curr_dir, FTS_LOGICAL, NULL ) ) == NULL )
            {
                fprintf ( stderr, "fts_open() error" );
                fts_close ( ftsp );
                exit(1);
            }
         
            while ( ( p = fts_read ( ftsp ) ) != NULL )
            {
                switch ( p->fts_info ) 
                {
                    /* directory */ 
                    case FTS_D:
#ifdef DEBUG
                        printf ( "^^%s\n", p->fts_name );
#endif
                        /* print directory path */                        
                        printf ( "%s:\n", p->fts_path ); 

                        // Get files contained in a directory
                        chp = fts_children ( ftsp, 0 );
                        
                        // Loop directory's files
                        for ( cur = chp; cur; cur = cur->fts_link )
                        {
#ifdef DEBUG
                            printf ( "\t@@ %s\n", cur->fts_name );
#endif            
                            record_stat ( cur->fts_statp, cur->fts_name );
                        }
                        
                        print_file_info_list();
                        
                        printf ( "\n" );

                        /* RE-initialize head of file_info linked list */
                        file_info_list_head = NULL;

                        break;

                    default:
                        break;
                }
            }
            
            if ( fts_close ( ftsp ) == -1 )
            {
                fprintf ( stderr, "fts_close() error.\n" );
                exit (1);
            }

            exit (0);
        }
    }

	
    /* save current directory path */
    char save_current_dir [255];
    getcwd ( save_current_dir, sizeof(save_current_dir) / sizeof(save_current_dir[0]) ); 

	while (argc-- > 0)
	{
#ifdef DEBUG
        printf ( "\n## processing argv : %s\n", *argv );
#endif
        
        /* enter original working directory */
        if ( chdir(save_current_dir) == -1 )
        {
            fprintf ( stderr, "can't chdir to '%s': %s\n",
                save_current_dir, strerror(errno) );
            exit (1);
        }

        /* RE-initialize head of file_info linked list */
        file_info_list_head = NULL;

        stat_ret = lstat ( *argv, &stat_buf );
		if ( stat_ret < 0 )
		{
			fprintf ( stderr, "stat error for %s\n", *argv );
            argv++;
			continue;   /* to process the next argv */
		}

        /* argument is a file */
		if ( S_ISREG ( stat_buf.st_mode ) )
		{
            record_stat ( &stat_buf, *argv );
            print_file_info_list();
		}
        /* argument is a directory */
        else if ( S_ISDIR ( stat_buf.st_mode ) )
        {
            /* 
               -d means we need just print directory info
               (not recursively)
            */
            if ( flag_d_option )
            {
                stat_ret = lstat ( *argv, &stat_buf );
                if ( stat_ret < 0 )
                {
                    fprintf ( stderr, "lstat error\n" );
                    argv++;
                    continue;
                }

                record_stat ( &stat_buf, *argv );
                print_file_info_list ();
                argv++; 
                continue;
            }

            /* non-recursive */
            if ( ! flag_R_option )
            {
                /* read directory NAME, and list the files in it */	
                if ( ( dp = opendir(*argv) ) == NULL )
                {
                    fprintf ( stderr, "can't open '%s'\n", *argv );
                    exit(1);
                }

                /* enter directory */
                if ( chdir(*argv) == -1 )
                {
                    fprintf ( stderr, "can't chdir to '%s': %s\n",
                        *argv, strerror(errno) );
                    exit (1);
                }
                
                while ( ( dirp = readdir(dp) ) != NULL )
                {
                    if ( lstat ( dirp->d_name, &stat_buf ) < 0 )
                    {
                        fprintf ( stderr, "stat() error" );
                        exit (1);
                    }
                    
                    record_stat ( &stat_buf, dirp->d_name );
                }

                if ( closedir(dp) < 0 )
                {
                    fprintf ( stderr, "can't close directory\n" );
                    exit(1);
                }
                print_file_info_list();
            }
            /* -R : recursive */
            else
            {
                if ( ( ftsp =
                     fts_open ( &*argv, FTS_LOGICAL, NULL ) ) == NULL )
                {
                    // BUG, never executed!!!
                    fprintf ( stderr, "fts_open error %s", 
                        strerror(errno) );
                    argv++;
                    continue;
                }

                while ( ( p = fts_read ( ftsp ) ) != NULL )
                {
                    switch ( p->fts_info ) 
                    {
                        /* directory */
                        case FTS_D:

                            /* print directory path */                        
                            printf ( "%s:\n", p->fts_path ); 

                            // get files contained in a directory
                            chp = fts_children ( ftsp, 0 );
                            
                            // loop directory's files
                            for ( cur = chp; cur; cur = cur->fts_link )
                            {
                                record_stat ( cur->fts_statp, cur->fts_name );
                            }
                            
                            print_file_info_list();
                            
                            printf ( "\n" );

                            /* RE-initialize head of file_info linked list */
                            file_info_list_head = NULL;
                            
                            break;

                        default:
                            break;
                    }
                }
                
                if ( -1 == fts_close ( ftsp ) )
                {
                    fprintf ( stderr, "fts_close() error.\n" );
                    argv++;
                    continue;
                }

                print_file_info_list();
	        }	
        }

        printf ( "\n" );
		
        argv++;

	} /* endof while ( argc-- > 0 ) */

    /* return with success */
	exit(0);

} /* end of main() */
