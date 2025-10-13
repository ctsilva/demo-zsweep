/* cc GLjunk.c -lGL -lGLU -lglut -lXext -lXmu -lXi -lm */

#include <GL/glut.h>
#include <stdlib.h>

void init( void )
{
  glClearColor(0,0,1,1);
  glShadeModel( GL_FLAT );
}

void display( void )
{
  glClear( GL_COLOR_BUFFER_BIT );
  glColor3f( 1,1,1 );
  glLoadIdentity();
  gluLookAt( 0,0,5, 0,0,0, 0,1,0 );
  glScalef( 1,2,1 );
  glutWireCube( 1.0 );

  //glLoadIdentity();
  glTranslated( 1.5, 0.0, 0.0 );
  glScalef( 1,2.5,1 );
  glutWireCube( 1.0 );

  //glLoadIdentity();
  glTranslated( -3.0, 0.0, 0.0 );
  //glScalef( 1,2.5,1 );
  glutWireCube( 1.0 );

  glutSwapBuffers();

}

void reshape( int w, int h )
{
  glViewport( 0,0,(GLsizei)w, (GLsizei)h );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glFrustum( -1, 1, -1, 1, 1.5, 20 );
  glMatrixMode( GL_MODELVIEW );
}

int main( int argc, char **argv )
{
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
  glutInitWindowSize( 500, 500);
  glutInitWindowPosition( 100,100 );
  glutCreateWindow( argv[0] );
  init();
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutMainLoop();
  return 0;
}
