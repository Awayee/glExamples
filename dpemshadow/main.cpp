#include <iostream>

#include "glapp.h"

void TestGL() {
	glapp::GLApp app;
	app.InitWindow();
	app.InitGL();
	app.Run();
	app.Clear();
}

int main() {
	TestGL();
	system("pause");
	return 0;
}