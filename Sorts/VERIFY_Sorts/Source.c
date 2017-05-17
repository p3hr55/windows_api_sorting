#include <windows.h>
#include <stdio.h>

int main(int argc, LPTSTR argv[]) {
	HANDLE in;
	LARGE_INTEGER size, e, b;
	char * first = calloc(64, sizeof(char *)), *second = calloc(64, sizeof(char *));
	BOOL error = FALSE;

	in = CreateFile(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (in == INVALID_HANDLE_VALUE) printf("Error opening file...");
	GetFileSizeEx(in, &size);
	e.QuadPart = 0;

	ReadFile(in, first, 64, 0, 0);
	for (int i = 1; i < (size.QuadPart - 64) / 64; i++) {
			ReadFile(in, second, 64, 0, 0);
			e.QuadPart += 64;
			if (strcmp(first, second) > 0)
				error = TRUE;
			
			SetFilePointerEx(in, e, 0, FILE_BEGIN);
			ReadFile(in, first, 64, 0, 0);
	}

	if (!error)
		printf("No errors.");
	else
		printf("Contained errors.");

	return 0;
}