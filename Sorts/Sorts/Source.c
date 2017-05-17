#include <windows.h>
#include <stdio.h>
#define RECORD_SIZE 64

VOID s(HANDLE data, LARGE_INTEGER left, LARGE_INTEGER right)
{
	LARGE_INTEGER left_index = left, right_index = right;
	char * temp = calloc(RECORD_SIZE, sizeof(char *)), *pivot = calloc(RECORD_SIZE, sizeof(char *)), *bar = calloc(RECORD_SIZE, sizeof(char *));
	SetFilePointerEx(data, left, 0, FILE_BEGIN);
	ReadFile(data, pivot, RECORD_SIZE, 0, 0);

	while (left_index.QuadPart <= right_index.QuadPart) {
		SetFilePointerEx(data, left_index, 0, FILE_BEGIN);
		ReadFile(data, temp, RECORD_SIZE, 0, 0);
		while (strcmp(temp, pivot) < 0) {
			left_index.QuadPart += RECORD_SIZE;
			ReadFile(data, temp, RECORD_SIZE, 0, 0);
		}

		SetFilePointerEx(data, right_index, 0, FILE_BEGIN);
		ReadFile(data, temp, RECORD_SIZE, 0, 0);
		while (strcmp(temp, pivot) > 0) {
			right_index.QuadPart -= RECORD_SIZE;
			SetFilePointerEx(data, right_index, 0, FILE_BEGIN);
			ReadFile(data, temp, RECORD_SIZE, 0, 0);
		}

		if (left_index.QuadPart <= right_index.QuadPart) {
			SetFilePointerEx(data, left_index, 0, FILE_BEGIN);
			ReadFile(data, bar, RECORD_SIZE, 0, 0);
			SetFilePointerEx(data, left_index, 0, FILE_BEGIN);
			WriteFile(data, temp, RECORD_SIZE, 0, 0);
			SetFilePointerEx(data, right_index, 0, FILE_BEGIN);
			WriteFile(data, bar, RECORD_SIZE, 0, 0);
			left_index.QuadPart += RECORD_SIZE;
			right_index.QuadPart -= RECORD_SIZE;
		}
	}

	if (left.QuadPart < right_index.QuadPart)
		s(data, left, right_index);
	if (left_index.QuadPart < right.QuadPart)
		s(data, left_index, right);
}

VOID m(LARGE_INTEGER left, LARGE_INTEGER right, LPTSTR source)
{
	LARGE_INTEGER left_index = left, right_index = right;
	char * temp = calloc(RECORD_SIZE, sizeof(char *)), *pivot = calloc(RECORD_SIZE, sizeof(char *)), *bar = calloc(RECORD_SIZE, sizeof(char *));
	memcpy(pivot, source + left.QuadPart, RECORD_SIZE);

	while (left_index.QuadPart <= right_index.QuadPart) {
		memcpy(temp, source + left_index.QuadPart, RECORD_SIZE);
		while (strcmp(temp, pivot) < 0) {
			left_index.QuadPart += RECORD_SIZE;
			memcpy(temp, source + left_index.QuadPart, RECORD_SIZE);
		}
		memcpy(temp, source + right_index.QuadPart, RECORD_SIZE);
		while (strcmp(temp, pivot) > 0) {
			right_index.QuadPart -= RECORD_SIZE;
			memcpy(temp, source + right_index.QuadPart, RECORD_SIZE);
		}
		if (left_index.QuadPart <= right_index.QuadPart) {
			memcpy(temp, source + left_index.QuadPart, RECORD_SIZE);
			memcpy(source + left_index.QuadPart, source + right_index.QuadPart, RECORD_SIZE);
			memcpy(source + right_index.QuadPart, temp, RECORD_SIZE);
			left_index.QuadPart += RECORD_SIZE;
			right_index.QuadPart -= RECORD_SIZE;
		}
	}

	free(temp);
	free(pivot);
	free(bar);

	if (left.QuadPart < right_index.QuadPart)
		m(left, right_index, source);
	if (left_index.QuadPart < right.QuadPart)
		m(left_index, right, source);

	return;
}

int main(int argc, LPTSTR argv[])
{
	HANDLE in, out, in_map, out_map;
	LPTSTR point_in, point_infile, point_out, point_outfile, left, right;
	LARGE_INTEGER size, e, b;
	in = CreateFile(argv[1], GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (in == INVALID_HANDLE_VALUE)
		printf("Error opening file.");

	GetFileSizeEx(in, &size);

	e.QuadPart = 0;
	b.QuadPart = size.QuadPart - 64;

	if (argc == 3 && argv[2][0] == 'm') {
		in_map = CreateFileMapping(in, NULL, PAGE_READWRITE, 0, 0, NULL);
		if (in_map == NULL)
			printf("Error Creating the File Map.");

		point_infile = MapViewOfFile(in_map, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (point_infile == NULL)
			printf("Error Retrieving the Map View of File.");

		m(e, b, point_infile, 0);
	}
	else
		s(in, e, b);

	CloseHandle(in);
	return 0;
}