﻿#define STB_IMAGE_IMPLEMENTATION
#include "C:/Users/Damir/source/repos/CGLab_6/CGLab_6/stb_image.h"
#include <gl\glut.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <locale.h>
#include <D:/Learning/glaux/Glaux.h>
#pragma comment(lib, "legacy_stdio_definitions.lib")
#pragma comment(lib, "D:/Learning/glaux/Glaux.lib")

/*Структура для хранения заголовка файла изображения*/
struct Zagolovok
{
	GLint shirina; /*Ширина*/
	GLint vysota; /*Высота*/
	GLenum formatCveta; /*Формат представления цвета*/
	GLenum formatKomponenty; /*Формат данных компоненты цвета*/
	int kol_voKomponent; /*Количество компонент цвета*/
};
/*Структура для хранения изображения*/
struct Izobrajenie
{
	unsigned char* pikseli;
	Zagolovok zagolovok;
};
/*Функция чтения изображения из фалйа .kai*/
Izobrajenie* ChtenieIzobrajeniyaIzFaila(const char* szFileName)
{
	FILE* pFile; // Указатель файла 
	Izobrajenie* iz = (Izobrajenie*) malloc(sizeof(Izobrajenie)); //Создание структуры для хранения изображения
		// Открытие файла
		fopen_s(&pFile,szFileName, "rb");
	if (pFile == NULL) return NULL;
	// Считываем заголовок
	fread(&(iz->zagolovok), sizeof(Zagolovok), 1, pFile);
	//Создание массива для битов
	iz->pikseli = (unsigned char*)malloc(iz->zagolovok.shirina * iz->zagolovok.vysota * iz -> zagolovok.kol_voKomponent);
	// Считывание битов
	fread(iz->pikseli, iz->zagolovok.shirina * iz->zagolovok.vysota * iz -> zagolovok.kol_voKomponent, 1, pFile);
	// Работа с файлом завершается 
	fclose(pFile);
	// Возвращает указатель на данные изображения
	return iz;
}
/*Функция записи изображения в файл .kai*/
GLint SohranenieIzobrajeniyaVfail(const char* szFileName)
{
	FILE* pFile; // Указатель файла 
	//Создание структуры для хранения изображения
	Izobrajenie* iz = (Izobrajenie*)malloc(sizeof(Izobrajenie));
	GLint iViewport[4]; //Массив для хранения размеров порта просмотра
	// Получение размеров порта просмотра
	glGetIntegerv(GL_VIEWPORT, iViewport);
	// Считывание битов из буфера цвета 
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	// Переключение на передний буфер
	glReadBuffer(GL_FRONT);
	//Установка параметров изображения
	iz->zagolovok.shirina = iViewport[2];
	iz->zagolovok.vysota = iViewport[3];
	iz->zagolovok.formatCveta = GL_RGB;
	iz->zagolovok.formatKomponenty = GL_UNSIGNED_BYTE;
	iz->zagolovok.kol_voKomponent = 3;
	//Выделение памяти для хранения битов
	iz->pikseli = (unsigned char*)malloc(iz->zagolovok.shirina * iz->zagolovok.vysota * iz -> zagolovok.kol_voKomponent);
	//Чтение битов
	glReadPixels(0, 0, iz->zagolovok.shirina, iz->zagolovok.vysota, iz->zagolovok.formatCveta, iz -> zagolovok.formatKomponenty, iz->pikseli);
	// Открытие файла
	fopen_s(&pFile, szFileName, "wb");
	// Запись заголовка
	fwrite(&(iz->zagolovok), sizeof(Zagolovok), 1, pFile);
	// Запись данных об изображении
	fwrite(iz->pikseli, iz->zagolovok.shirina * iz->zagolovok.vysota * iz -> zagolovok.kol_voKomponent, 1, pFile);
	//Закрытие файла
	fclose(pFile);
	// Успех
	return 1;
}
Izobrajenie* izobr; //Текущее изображение
Izobrajenie* izobr_bmp; //Изображение из файла .bmp
Izobrajenie* izobr_kai; //Изображение из файла .kai
Izobrajenie* izobr_ch_b; //Черно-белое изображение
AUX_RGBImageRec* pImage = NULL; //Изображение AUX_RGB
//Переменная для хранения режима рисования
static GLint rejim = 1;
// Должным образом обновляем флаги в ответ на выбор позиции из меню
void ObrabotkaMenu(int punktMenu)
{
	// меняем индекс режима визуализации на индекс,
	// соответствующий позиции меню
	rejim = punktMenu;
	// Активизируем перерисовывание изображения glutPostRedisplay(); }
	glutPostRedisplay();
}
int k;
int iz = 0;
void Pererisovka(void)
{
	GLint iViewport[4];
	//GLbyte *pModifiedBytes = NULL;
	GLfloat invertMap[256];
	GLint i;
	// Очищаем окно текущим цветом очистки
	glClear(GL_COLOR_BUFFER_BIT);
	// Текущее растровое положение всегда соответствует левому нижнему углу окна 
	glRasterPos2i(0, 128);
	bool run = true;
	// В зависимости от индекса режима визуализации выполняются необходимые операции с изображением
		switch (rejim)
		{
		case 0: /*Очистка экрана*/
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			break;
		case 1: /*Загрузка изображения из файла *.bmp*/
			pImage = auxDIBImageLoadA("CGLab_6.bmp");
			izobr_bmp = (Izobrajenie*)malloc(sizeof(Izobrajenie));
			izobr_bmp->zagolovok.shirina = pImage->sizeX;
			izobr_bmp->zagolovok.vysota = pImage->sizeY;
			izobr_bmp->pikseli = pImage->data;
			izobr_bmp->zagolovok.formatCveta = GL_RGB;
			izobr_bmp->zagolovok.formatKomponenty = GL_INT;
			izobr_bmp->zagolovok.kol_voKomponent = 3;
			izobr = izobr_bmp;
			izobr_bmp->zagolovok.formatCveta = GL_RGB;
			izobr_bmp->zagolovok.formatKomponenty = GL_UNSIGNED_BYTE;
			break;
		case 2: /*Сохранение текущего буфера в файл*/

			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			SohranenieIzobrajeniyaVfail("picture_txt.kai");
			break;
		case 3: /*Загрузка изображения из собственного файла*/
			
			izobr_kai = ChtenieIzobrajeniyaIzFaila("picture_txt.kai");
			glRasterPos2i(0, 0);
			izobr = izobr_kai;
			break;

		case 4: /*Отзеркаливание*/
			glReadBuffer(GL_FRONT);
			glPixelZoom(1, -1);
			break;

		case 5: /*Инверсия цветов*/

			invertMap[0] = 1.0f;
			for (i = 1; i < 256; i++)
				invertMap[i] = 1.0f - (1.0f / 255.0f * (GLfloat)i);
			glPixelMapfv(GL_PIXEL_MAP_R_TO_R, 255, invertMap);
			glPixelMapfv(GL_PIXEL_MAP_G_TO_G, 255, invertMap); //изменение цвета через массив
			glPixelMapfv(GL_PIXEL_MAP_B_TO_B, 255, invertMap);
			glPixelTransferi(GL_MAP_COLOR, GL_TRUE); // изменение 
			break;

		case 6:/* Отображение компоненты R и B*/
			glPixelTransferf(GL_RED_SCALE, 1.0f); //изменение яркости (по цветовой модели)
			glPixelTransferf(GL_GREEN_SCALE, 0.0f);
			glPixelTransferf(GL_BLUE_SCALE, 1.0f);


		default:
			break;
		}
	if (rejim != 0)
		// Рисуются пиксели
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glDrawPixels(izobr->zagolovok.shirina, izobr->zagolovok.vysota, izobr -> zagolovok.formatCveta, izobr->zagolovok.formatKomponenty, izobr->pikseli);
	}
	//Переключает буферы
	glutSwapBuffers();
}
void IzmenenieRazmera(int w, int h)
{
	//Предотвращает деление на ноль, когда окно слишком маленькое
	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);
	//Система координат обновляется перед модификацией
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}
//Точка входа основной программы
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GL_DOUBLE);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Операции над пикселями");
	glutReshapeFunc(IzmenenieRazmera);
	glutDisplayFunc(Pererisovka);
	//Создается меню и добавляются опции выбора
	glutCreateMenu(ObrabotkaMenu);
	glutAddMenuEntry("Очистить экран", 0);
	glutAddMenuEntry("Загрузить изображение .bmp", 1);
	glutAddMenuEntry("Сохранить текущее изображение", 2);
	glutAddMenuEntry("Загрузить изображение из файла .kai", 3);
	glutAddMenuEntry("Отобразить изображение относитель оси X", 4);
	glutAddMenuEntry("Инверсия цветов", 5);
	glutAddMenuEntry("Отображение R и B компонент", 6);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	gluOrtho2D(0, 512, 0, 512);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	rejim = 0;
	glutMainLoop();
	// Освобождаем исходные данные изображений
	free(izobr);
	free(izobr_ch_b);
	free(izobr_kai);
	free(izobr_bmp);
	free(pImage);
	return 0;
}