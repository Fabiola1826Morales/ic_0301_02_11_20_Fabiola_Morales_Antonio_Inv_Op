

/*-------------------------------------------------------------------------*/
/**
  @file     gnuplot_i.c
  @author   N. Devillard
  @date Sep 1998
  @version  $Revision: 2.10 $
  @brief    C interface to gnuplot.

  gnuplot es una herramienta de visualización gráfica de libre acceso y dirigida por comandos para
  Unix. Compila y funciona bastante bien en una serie de sabores de Unix como
  así como otros sistemas operativos. El siguiente módulo permite enviar
  mostrar las solicitudes de gnuplot a través de simples llamadas C.

*/
/*--------------------------------------------------------------------------*/

/*
    $Id: gnuplot_i.c,v 2.10 2003/01/27 08:58:04 ndevilla Exp $
    $Author: ndevilla $
    $Date: 2003/01/27 08:58:04 $
    $Revision: 2.10 $
 */

/*---------------------------------------------------------------------------
                                Includes
 ---------------------------------------------------------------------------*/

#include "gnuplot_ii.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#ifdef WIN32
#include <io.h>
#endif // #ifdef WIN32

/*---------------------------------------------------------------------------
                                Defines
 ---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                          Prototype Functions
 ---------------------------------------------------------------------------*/

/**
 * Creates a temporary file name for writing
 *
 * @author Peter (12/9/2011)
 *
 * @param handle
 *
 * @return char const * Pointer to file name string.
 */
char const * gnuplot_tmpfile(gnuplot_ctrl * handle);

/**
 * Plot a temporary file.
 *
 * @author Peter (12/9/2011)
 *
 * @param handle
 * @param tmp_filename
 * @param title
 */
void gnuplot_plot_atmpfile(gnuplot_ctrl * handle, char const* tmp_filename, char const* title);

/*---------------------------------------------------------------------------
                            Function codes
 ---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/**
  @brief Abre una sesión de gnuplot, listo para recibir órdenes.
  @return Estructura de control de gnuplot recién asignada.

  Abre una nueva sesión de gnuplot, lista para ser introducida. La estructura
  el control de una sesión de gnuplot debe permanecer opaco y sólo ser
  a las que se accede a través de las funciones proporcionadas.

  The session must be closed using gnuplot_close().
 */
/*--------------------------------------------------------------------------*/

gnuplot_ctrl * gnuplot_init(void)
{
    gnuplot_ctrl *  handle ;
    int i;

#ifndef WIN32
    if (getenv("DISPLAY") == NULL) {
        fprintf(stderr, "cannot find DISPLAY variable: is it set?\n") ;
    }
#endif // #ifndef WIN32


    /*
     * Structure initialization:
     */
    handle = (gnuplot_ctrl*)malloc(sizeof(gnuplot_ctrl)) ;
    handle->nplots = 0 ;
    gnuplot_setstyle(handle, "points") ;
    handle->ntmp = 0 ;

    handle->gnucmd = popen("gnuplot", "w") ;
    if (handle->gnucmd == NULL) {
        fprintf(stderr, "error starting gnuplot, is gnuplot or gnuplot.exe in your path?\n") ;
        free(handle) ;
        return NULL ;
    }

    for (i=0;i<GP_MAX_TMP_FILES; i++)
    {
        handle->tmp_filename_tbl[i] = NULL;
    }
    return handle;
}


/*-------------------------------------------------------------------------*/
/**
  @brief Cierra una sesión de gnuplot previamente abierta por gnuplot_init()
  @param maneja el control de sesión de Gnuplot.
  @return vacío

  Mata al niño PID y borra todos los archivos temporales abiertos.
  Es obligatorio llamar a esta función para cerrar la manija, de lo contrario
  los archivos temporales no se limpian y el proceso de los niños podría sobrevivir.

 */
/*--------------------------------------------------------------------------*/

void gnuplot_close(gnuplot_ctrl * handle)
{
    int     i ;

    if (pclose(handle->gnucmd) == -1) {
        fprintf(stderr, "problem closing communication to gnuplot\n") ;
        return ;
    }
    if (handle->ntmp) {
        for (i=0 ; i<handle->ntmp ; i++) {
            remove(handle->tmp_filename_tbl[i]) ;
            free(handle->tmp_filename_tbl[i]);
            handle->tmp_filename_tbl[i] = NULL;

        }
    }
    free(handle) ;
    return ;
}


/*-------------------------------------------------------------------------*/
/**
  @brief Envía un comando a una sesión de gnuplot activa.
  @param maneja el control de la sesión de gnuplot
  Comando para enviar, igual que una declaración impresa.

  Esto envía una cadena a una sesión gnuplot activa, para ser ejecutada.
  No hay estrictamente ninguna manera de saber si el comando ha sido
  ejecutado con éxito o no.
  La sintaxis del comando es la misma que la de printf.

  Ejemplos:

  @code
  gnuplot_cmd(g, "plot %d*x", 23.0);
  gnuplot_cmd(g, "parcela %.18e * cos(%.18e * x)", 32.0, -3.0);
  @endcode

Traducción realizada con la versión gratuita del traductor www.DeepL.com/Translator
 */
/*--------------------------------------------------------------------------*/

void gnuplot_cmd(gnuplot_ctrl *  handle, char const *  cmd, ...)
{
    va_list ap ;

    va_start(ap, cmd);
    vfprintf(handle->gnucmd, cmd, ap);
    va_end(ap);

    fputs("\n", handle->gnucmd) ;
    fflush(handle->gnucmd) ;
    return ;
}


/*-------------------------------------------------------------------------*/
/**
   Cambiar el estilo de trazado de una sesión de gnuplot.
  @param h Manejador de control de sesión de gnuplot
  @param plot_style Estilo de trazado a usar (cadena de caracteres)
  @retorno vacío

  El estilo de trama proporcionado es una cadena de caracteres. Debe ser una de
  lo siguiente:

  - lines
  - points
  - linespoints
  - impulses
  - dots
  - steps
  - errorbars
  - boxes
  - boxeserrorbars
 */
/*--------------------------------------------------------------------------*/

void gnuplot_setstyle(gnuplot_ctrl * h, char * plot_style)
{
    if (strcmp(plot_style, "lines") &&
        strcmp(plot_style, "points") &&
        strcmp(plot_style, "linespoints") &&
        strcmp(plot_style, "impulses") &&
        strcmp(plot_style, "dots") &&
        strcmp(plot_style, "steps") &&
        strcmp(plot_style, "errorbars") &&
        strcmp(plot_style, "boxes") &&
        strcmp(plot_style, "boxerrorbars")) {
        fprintf(stderr, "warning: unknown requested style: using points\n") ;
        strcpy(h->pstyle, "points") ;
    } else {
        strcpy(h->pstyle, plot_style) ;
    }
    return ;
}


/*-------------------------------------------------------------------------*/
/**
  @brief    Sets the x label of a gnuplot session.
  @param    h Gnuplot session control handle.
  @param    label Character string to use for X label.
  @return   void

  Sets the x label for a gnuplot session.
 */
/*--------------------------------------------------------------------------*/

void gnuplot_set_xlabel(gnuplot_ctrl * h, char * label)
{
    gnuplot_cmd(h, "set xlabel \"%s\"", label) ;
}


/*-------------------------------------------------------------------------*/
/**
  @brief    Sets the y label of a gnuplot session.
  @param    h Gnuplot session control handle.
  @param    label Character string to use for Y label.
  @return   void

  Sets the y label for a gnuplot session.
 */
/*--------------------------------------------------------------------------*/

void gnuplot_set_ylabel(gnuplot_ctrl * h, char * label)
{
    gnuplot_cmd(h, "set ylabel \"%s\"", label) ;
}


/*-------------------------------------------------------------------------*/
/**
  @brief    Resets a gnuplot session (next plot will erase previous ones).
  @param    h Gnuplot session control handle.
  @return   void

  Resets a gnuplot session, i.e. the next plot will erase all previous
  ones.
 */
/*--------------------------------------------------------------------------*/

void gnuplot_resetplot(gnuplot_ctrl * h)
{
    int     i ;
    if (h->ntmp) {
        for (i=0 ; i<h->ntmp ; i++) {
            remove(h->tmp_filename_tbl[i]) ;
            free(h->tmp_filename_tbl[i]);
            h->tmp_filename_tbl[i] = NULL;

        }
    }
    h->ntmp = 0 ;
    h->nplots = 0 ;
    return ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief    Plots a 2d graph from a list of doubles.
  @param    handle  Gnuplot session control handle.
  @param    d       Array of doubles.
  @param    n       Number of values in the passed array.
  @param    title   Title of the plot.
  @return   void

  Plots out a 2d graph from a list of doubles. The x-coordinate is the
  index of the double in the list, the y coordinate is the double in
  the list.

  Example:

  @code
    gnuplot_ctrl    *h ;
    double          d[50] ;
    int             i ;

    h = gnuplot_init() ;
    for (i=0 ; i<50 ; i++) {
        d[i] = (double)(i*i) ;
    }
    gnuplot_plot_x(h, d, 50, "parabola") ;
    sleep(2) ;
    gnuplot_close(h) ;
  @endcode
 */
/*--------------------------------------------------------------------------*/

void gnuplot_plot_x(
    gnuplot_ctrl    *   handle,
    double          *   d,
    int                 n,
    char            *   title
)
{
    int     i ;
    FILE*   tmpfd ;
    char const * tmpfname;

    if (handle==NULL || d==NULL || (n<1)) return ;

    /* Open temporary file for output   */
    tmpfname = gnuplot_tmpfile(handle);
    tmpfd = fopen(tmpfname, "w");

    if (tmpfd == NULL) {
        fprintf(stderr,"cannot create temporary file: exiting plot") ;
        return ;
    }

    /* Write data to this file  */
    for (i=0 ; i<n ; i++) {
      fprintf(tmpfd, "%.18e\n", d[i]);
    }
    fclose(tmpfd) ;

    gnuplot_plot_atmpfile(handle,tmpfname,title);
    return ;
}



/*-------------------------------------------------------------------------*/
/**
  @brief    Plot a 2d graph from a list of points.
  @param    handle      Gnuplot session control handle.
  @param    x           Pointer to a list of x coordinates.
  @param    y           Pointer to a list of y coordinates.
  @param    n           Number of doubles in x (assumed the same as in y).
  @param    title       Title of the plot.
  @return   void

  Plots out a 2d graph from a list of points. Provide points through a list
  of x and a list of y coordinates. Both provided arrays are assumed to
  contain the same number of values.

  @code
    gnuplot_ctrl    *h ;
    double          x[50] ;
    double          y[50] ;
    int             i ;

    h = gnuplot_init() ;
    for (i=0 ; i<50 ; i++) {
        x[i] = (double)(i)/10.0 ;
        y[i] = x[i] * x[i] ;
    }
    gnuplot_plot_xy(h, x, y, 50, "parabola") ;
    sleep(2) ;
    gnuplot_close(h) ;
  @endcode
 */
/*--------------------------------------------------------------------------*/

void gnuplot_plot_xy(
    gnuplot_ctrl    *   handle,
    double          *   x,
    double          *   y,
    int                 n,
    char            *   title
)
{
    int     i ;
    FILE*   tmpfd ;
    char const * tmpfname;

    if (handle==NULL || x==NULL || y==NULL || (n<1)) return ;

    /* Open temporary file for output   */
    tmpfname = gnuplot_tmpfile(handle);
    tmpfd = fopen(tmpfname, "w");

    if (tmpfd == NULL) {
        fprintf(stderr,"cannot create temporary file: exiting plot") ;
        return ;
    }

    /* Write data to this file  */
    for (i=0 ; i<n; i++) {
        fprintf(tmpfd, "%.18e %.18e\n", x[i], y[i]) ;
    }
    fclose(tmpfd) ;

    gnuplot_plot_atmpfile(handle,tmpfname,title);
    return ;
}



/*-------------------------------------------------------------------------*/
/**
  @brief    Open a new session, plot a signal, close the session.
  @param    title   Plot title
  @param    style   Plot style
  @param    label_x Label for X
  @param    label_y Label for Y
  @param    x       Array of X coordinates
  @param    y       Array of Y coordinates (can be NULL)
  @param    n       Number of values in x and y.
  @return

  This function opens a new gnuplot session, plots the provided
  signal as an X or XY signal depending on a provided y, waits for
  a carriage return on stdin and closes the session.

  It is Ok to provide an empty title, empty style, or empty labels for
  X and Y. Defaults are provided in this case.
 */
/*--------------------------------------------------------------------------*/

void gnuplot_plot_once(
  char    *   title,
  char    *   style,
  char    *   label_x,
  char    *   label_y,
  double  *   x,
  double  *   y,
  int         n
)
{
  gnuplot_ctrl    *   handle ;

  if (x==NULL || n<1) return ;

  if ((handle = gnuplot_init()) == NULL) return ;
  if (style!=NULL) {
      gnuplot_setstyle(handle, style);
  } else {
      gnuplot_setstyle(handle, "lines");
  }
  if (label_x!=NULL) {
      gnuplot_set_xlabel(handle, label_x);
  } else {
      gnuplot_set_xlabel(handle, "X");
  }
  if (label_y!=NULL) {
      gnuplot_set_ylabel(handle, label_y);
  } else {
      gnuplot_set_ylabel(handle, "Y");
  }
  if (y==NULL) {
      gnuplot_plot_x(handle, x, n, title);
  } else {
      gnuplot_plot_xy(handle, x, y, n, title);
  }
  printf("press ENTER to continue\n");
  while (getchar()!='\n') {}
  gnuplot_close(handle);
  return ;
}

void gnuplot_plot_slope(
    gnuplot_ctrl    *   handle,
    double              a,
    double              b,
    char            *   title
)
{
    char const *    cmd    = (handle->nplots > 0) ? "replot" : "plot";
    title                  = (title == NULL)      ? "(none)" : title;

    gnuplot_cmd(handle, "%s %.18e * x + %.18e title \"%s\" with %s",
                  cmd, a, b, title, handle->pstyle) ;

    handle->nplots++ ;
    return ;
}


void gnuplot_plot_equation(
    gnuplot_ctrl    *   h,
    char            *   equation,
    char            *   title
)
{
    char const *    cmd    = (h->nplots > 0) ? "replot" : "plot";
    title                  = (title == NULL)      ? "(none)" : title;

    gnuplot_cmd(h, "%s %s title \"%s\" with %s",
                  cmd, equation, title, h->pstyle) ;
    h->nplots++ ;
    return ;
}


int gnuplot_write_x_csv(
    char const * fileName,
    double const * d,
    int n,
    char const * title)
{
    int     i;
    FILE*   fileHandle;

    if (fileName==NULL || d==NULL || (n<1))
    {
        return -1;
    }

    fileHandle = fopen(fileName, "w");

    if (fileHandle == NULL)
    {
        return -1;
    }

    // Write Comment.
    if (title != NULL)
    {
        fprintf(fileHandle, "# %s\n", title) ;
    }

    /* Write data to this file  */
    for (i=0 ; i<n; i++)
    {
        fprintf(fileHandle, "%d, %.18e\n", i, d[i]) ;
    }

    fclose(fileHandle) ;

    return 0;
}

int gnuplot_write_xy_csv(
    char const *        fileName,
    double const    *   x,
    double const    *   y,
    int                 n,
    char const      *   title)
{
    int     i ;
    FILE*   fileHandle;

    if (fileName==NULL || x==NULL || y==NULL || (n<1))
    {
        return -1;
    }

    fileHandle = fopen(fileName, "w");

    if (fileHandle == NULL)
    {
        return -1;
    }

    // Write Comment.
    if (title != NULL)
    {
        fprintf(fileHandle, "# %s\n", title) ;
    }

    /* Write data to this file  */
    for (i=0 ; i<n; i++)
    {
        fprintf(fileHandle, "%.18e, %.18e\n", x[i], y[i]) ;
    }

    fclose(fileHandle) ;

    return 0;
}

int gnuplot_write_multi_csv(
    char const *        fileName,
    double const    **  xListPtr,
    int                 n,
    int                 numColumns,
    char const      *   title)
{
    int     i;
    int     j;
    FILE*   fileHandle;

    if (fileName==NULL || xListPtr==NULL || (n<1) || numColumns <1)
    {
        return -1;
    }

    for (j=0;j<numColumns;j++)
    {
        if (xListPtr[j] == NULL)
        {
            return -1;
        }
    }

    fileHandle = fopen(fileName, "w");

    if (fileHandle == NULL)
    {
        return -1;
    }

    // Write Comment.
    if (title != NULL)
    {
        fprintf(fileHandle, "# %s\n", title) ;
    }

    /* Write data to this file  */
    for (i=0 ; i<n; i++)
    {
        fprintf(fileHandle, "%d, %.18e", i, xListPtr[0][i]) ;
        for (j=1;j<numColumns;j++)
        {
            fprintf(fileHandle, ", %.18e", xListPtr[j][i]) ;
        }
        fprintf(fileHandle, "\n");
    }

    fclose(fileHandle) ;

    return 0;
}

char const * gnuplot_tmpfile(gnuplot_ctrl * handle)
{
    static char const * tmp_filename_template = "gnuplot_tmpdatafile_XXXXXX";
    char *              tmp_filename = NULL;
    int                 tmp_filelen = strlen(tmp_filename_template);

#ifndef WIN32
    int                 unx_fd;
#endif // #ifndef WIN32

    assert(handle->tmp_filename_tbl[handle->ntmp] == NULL);

    /* Open one more temporary file? */
    if (handle->ntmp == GP_MAX_TMP_FILES - 1) {
        fprintf(stderr,
                "maximum # of temporary files reached (%d): cannot open more",
                GP_MAX_TMP_FILES) ;
        return NULL;
    }

    tmp_filename = (char*) malloc(tmp_filelen+1);
    if (tmp_filename == NULL)
    {
        return NULL;
    }
    strcpy(tmp_filename, tmp_filename_template);

#ifdef WIN32
    if (_mktemp(tmp_filename) == NULL)
    {
        return NULL;
    }
#else // #ifdef WIN32
    unx_fd = mkstemp(tmp_filename);
    if (unx_fd == -1)
    {
        return NULL;
    }
    close(unx_fd);

#endif // #ifdef WIN32

    handle->tmp_filename_tbl[handle->ntmp] = tmp_filename;
    handle->ntmp ++;
    return tmp_filename;
}

void gnuplot_plot_atmpfile(gnuplot_ctrl * handle, char const* tmp_filename, char const* title)
{
    char const *    cmd    = (handle->nplots > 0) ? "replot" : "plot";
    title                  = (title == NULL)      ? "(none)" : title;
    gnuplot_cmd(handle, "%s \"%s\" title \"%s\" with %s", cmd, tmp_filename,
                  title, handle->pstyle) ;
    handle->nplots++ ;
    return ;
}


/* vim: set ts=4 et sw=4 tw=75 */
