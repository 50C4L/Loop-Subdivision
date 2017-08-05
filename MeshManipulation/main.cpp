#include "System.h"

int main( int argc, char **argv )
{
	mm::System *app = new mm::System();
	app->start();
	app->shutdown();

	return 0;
}