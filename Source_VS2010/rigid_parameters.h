
//floor
float floorMinX = -10.0;//-50.0;
float floorMaxX =  10.0;//50.0;
float floorMinY = -10.0;//-50.0;
float floorMaxY =  10.0;//50.0;


enum Kind {SPHERE, CUBE, CYLINDER, BOX, COLUMN, PLATE };

enum Color {BLACK, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE, GRAY,
            DARK_RED, DARK_GREEN, DARK_BLUE, DARK_YELLOW, DARK_CYAN, DARK_MAGENTA };

float black[] = {0.0, 0.0, 0.0, 1.0};
float red[] = {0.9, 0.1, 0.1, 1.0};
float green[] = {0.1, 0.9, 0.1, 1.0};
float blue[] = {0.1, 0.1, 0.9, 1.0};
float yellow[] = {0.9, 0.9, 0.1, 1.0};
float cyan[] = {0.1, 0.9, 0.9, 1.0};
float magenta[] = {0.9, 0.1, 0.9};
float white[] = {0.9, 0.9, 0.9, 1.0};
float gray[] = {0.6, 0.6, 0.6, 1.0};
float dark_red[] = {0.6, 0.05, 0.05, 1.0};
float dark_green[] = {0.05, 0.6, 0.05, 1.0};
float dark_blue[] = {0.05, 0.05, 0.6, 1.0};
float dark_yellow[] = {0.6, 0.6, 0.05, 1.0};
float dark_cyan[] = {0.05, 0.6, 0.6, 1.0};
float dark_magenta[] = {0.6, 0.05, 0.6, 1.0};

//display list
int sphere, cube, cylinder, box, column, plate, floor0, floor1;
//int sphereTex, cubeTex, boxTex, cylinderTex, columnTex, torusTex, coneTex, pyramidTex;

void createSphere()
{
	sphere = glGenLists(1);
	if(sphere == 0) { printf("display list を確保できませんでした\n"); return; }
	glNewList(sphere, GL_COMPILE);
	drawSphere(0.5, 10, 10);
	glEndList();
}

void createCube()
{
	cube = glGenLists(1);
	if(cube == 0) {printf("display list を確保できませんでした\n"); return;}
	glNewList(cube, GL_COMPILE);
	drawCube(1.0);
	glEndList();
}

void createCylinder()
{
	cylinder = glGenLists(1);
	if(cylinder == 0) {printf("display list を確保できませんでした\n"); return;}
	glNewList(cylinder, GL_COMPILE);
	drawCylinder(0.5, 0.5, 1.0, 12, 4);
	glEndList();
}
//角に丸みのある直方体(衝突判定時はCUBEとおなじデータ構造）
float boxEps1 = 0.1;
float boxEps2 = 0.1;
void createBox()
{
	box = glGenLists(1);
	if(box == 0) {printf("display list を確保できませんでした\n"); return;}
	glNewList(box, GL_COMPILE);
	drawSuper(0.5, 10, 10, boxEps1, boxEps2);
	glEndList();
}
//角に丸みのある円柱
float columnEps1 = 0.1;//0.5;
float columnEps2 = 1.0;//0.5;//1.0;
void createColumn()
{
	column = glGenLists(1);
	if(column == 0) {printf("display list を確保できませんでした\n"); return;}
	glNewList(column, GL_COMPILE);
	drawSuper(0.5, 10, 10, columnEps1, columnEps2);
	glEndList();
}
void createPlate()
{
	plate = glGenLists(1);
	if(plate == 0) {printf("display list を確保できませんでした\n"); return;}
	glNewList(plate, GL_COMPILE);
	drawPlate();
	glEndList();
}

void createFloor()
{
	floor0 = glGenLists(1);
	if(floor0 == 0) {printf("display list を確保できませんでした\n"); return;}
	glNewList(floor0, GL_COMPILE);
	drawFloorZ(floorMaxX - floorMinX, floorMaxY - floorMinY, 10, 10);
	glEndList();
}

void createFloorY()
{
	floor1 = glGenLists(1);
	if(floor1 == 0) {printf("display list を確保できませんでした\n"); return;}
	glNewList(floor1, GL_COMPILE);
	drawFloorY(floorMaxX - floorMinX, floorMaxY - floorMinY, 10, 10);
	glEndList();
}


void createDisplayLists()
{
	createSphere();
	createCube();
	createCylinder();
	createBox();
	createColumn();
	createPlate();
	createFloor();
	createFloorY();
}
void deleteDisplayLists()
{
	glDeleteLists(sphere, 1);
	glDeleteLists(cube, 1);
	glDeleteLists(cylinder, 1);
	glDeleteLists(box, 1);
	glDeleteLists(column, 1);
	glDeleteLists(plate, 1);
	glDeleteLists(floor0, 1);
	glDeleteLists(floor1, 1);
}
