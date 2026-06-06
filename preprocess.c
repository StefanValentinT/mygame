#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

bool is_line_empty(const char *line) {
	while (*line) {
		if (!isspace((unsigned char)*line)) {
			return false;
		}
		line++;
	}
	return true;
}

bool has_prefix(const char *line, const char *prefix) {
	while (*line && isspace((unsigned char)*line)) {
		line++;
	}
	return strncmp(line, prefix, strlen(prefix)) == 0;
}

int main(int argc, char *argv[]) {
	if (argc < 4) {
		fprintf(stderr, "Usage: %s <filename.c> <src_path> <dest_path>\n", argv[0]);
		return EXIT_FAILURE;
	}

	const char *filename = argv[1];
	const char *src_path = argv[2];
	const char *dest_path = argv[3];

	FILE *src = fopen(src_path, "r");
	if (!src) {
		fprintf(stderr, "Error: Could not open source file %s\n", src_path);
		return EXIT_FAILURE;
	}

	FILE *dest = fopen(dest_path, "w");
	if (!dest) {
		fprintf(stderr, "Error: Could not open destination file %s\n", dest_path);
		fclose(src);
		return EXIT_FAILURE;
	}

	char base_name[256] = {0};
	strncpy(base_name, filename, sizeof(base_name) - 1);
	char *dot = strrchr(base_name, '.');
	if (dot) *dot = '\0';

	char line[1024];
	int line_num = 0;
	bool in_imports = false;
	bool header_included = false;

	fprintf(dest, "#if INTERFACE\n\n");

	while (fgets(line, sizeof(line), src)) {
		line_num++;

		if (has_prefix(line, "#Imports")) {
			in_imports = true;
			continue; 
		}
		if (has_prefix(line, "#Types")) {
			in_imports = false;
			continue; 
		}
		if (has_prefix(line, "#Impl")) {
			in_imports = false;
			
			if (!header_included) {
				fprintf(dest, "\n#endif\n\n#include \"%s.h\"\n", base_name);
				header_included = true;
			}
			continue;
		}

		if (in_imports) {
			if (!is_line_empty(line) && 
				!has_prefix(line, "#include") && 
				!has_prefix(line, "//") && 
				!has_prefix(line, "/*")) {
				
				line[strcspn(line, "\r\n")] = '\0';
				fprintf(stderr, "Error in %s:%d -> Every line under #Imports must be empty or start with #include!\n", filename, line_num);
				fprintf(stderr, "   Found: \"%s\"\n", line);
				
				fclose(src);
				fclose(dest);
				remove(dest_path); 
				return EXIT_FAILURE;
			}
		}

		fputs(line, dest);
	}

	if (!header_included) {
		fprintf(dest, "\n#endif\n\n#include \"%s.h\"\n", base_name);
	}

	fclose(src);
	fclose(dest);
	return EXIT_SUCCESS;
}