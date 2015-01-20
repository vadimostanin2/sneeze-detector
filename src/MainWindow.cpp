#include <GL/glut.h>
#include "MainWindow.h"

static sound_processing_status_e sneeze_status = SOUND_STATUS_PREPARED_E;
#define RGB(r, g, b) (r<<16) + (g<<8) + b
int windh, windw; // window height and width

void draw_string(void *font, const char* string)
{
	while(*string)
	{
		glutStrokeCharacter(font, *string++);
	}
}

void draw_string_bitmap(void *font, const char* string)
{
	while (*string)
	{
		glutBitmapCharacter(font, *string++);
	}
}

void reshape(int wid, int ht)
{
  glViewport(0, 0, wid, ht);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-wid/2, wid/2, -ht/2, ht/2);
  glMatrixMode(GL_MODELVIEW);
  windw = wid;
  windh = ht;
}

void draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glLineWidth(.05f);
	glColor3b( 255, 255, 255 );

	glRasterPos2f(-200, 150);

    if( sneeze_status == SOUND_STATUS_PREPARED_E )
	{
    	glColor3f( 1.0f, 1.0f, 1.0f );
    	draw_string_bitmap(GLUT_BITMAP_TIMES_ROMAN_24, "Listening");
	}
    else if( sneeze_status == SOUND_STATUS_PROCESSING_E )
	{
		glColor3f( 1.0f, 1.0f, 0.0f );
		draw_string_bitmap(GLUT_BITMAP_TIMES_ROMAN_24, "Processing");
	}
	else if( sneeze_status == SOUND_STATUS_RECOGNIZED_NOISE_E )
	{
		glColor3f( 1.0f, 0.0f, 0.0f );
		draw_string_bitmap(GLUT_BITMAP_TIMES_ROMAN_24, "Noise");
	}
	else if( sneeze_status == SOUND_STATUS_RECOGNIZED_SNEEZE_E )
	{
		glColor3f( 0.0f, 1.0f, 0.0f );
		draw_string_bitmap(GLUT_BITMAP_TIMES_ROMAN_24, "Sneezing");
	}

    glBegin(GL_POLYGON);
       	glVertex2f( -1.0*windw, -1.0*windh );
       	glVertex2f( -1.0*windw, 1.0*windh );
       	glVertex2f( 1.0*windw, 1.0*windh );
       	glVertex2f( 1.0*windw, -1.0*windh );
    glEnd();

    glFlush();

	glutSwapBuffers();

	glutPostRedisplay();
}

void sneeze_status_set( sound_processing_status_e status )
{
	sneeze_status = status;
}

void KeyFunc(unsigned char key, int x, int y)
{
  if (key == 27)
    exit(0);
}

//Main program

int mainwindow_show()
{
	windw = 400;
	windh = 400;

	int argc = 0;
	char * argv[2] = { "", NULL };
    glutInit(&argc, argv);
    glutInitWindowSize( windw, windh );
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    (void)glutCreateWindow("Sneezing Detector");
    glutReshapeFunc(reshape);
    glutDisplayFunc(draw);
//    glutIdleFunc( draw );
    glutKeyboardFunc(KeyFunc);
    glClearColor(  0, 0, 0, 1 );
    glutMainLoop();
    return 0;
}
