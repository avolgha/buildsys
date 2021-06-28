#include "buildconfig.h"

using std::string;

class Printer {
public:
	void print(string message) {
		std::cout << message;
	}

	void print(bool boolean) {
		std::cout << (boolean ? "true" : "false");
	}

	void printColored(string message, int color) {
		print("\x1B[" + std::to_string(color) + "m" + message + "\033[0m\n");
	}
};

string getFileContent(string path) {
	std::ifstream ifs(path);
	string content(
		(std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>())
	);
	return content;
}

bool doesFileExists(string path) {
	FILE* file;
	if (file = fopen(path.c_str(), "r")) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}

const string buildCfgName = ".buildcfg";
const auto printer = new Printer();

const auto COLOR_MAGENTA = 35;
const auto COLOR_RED = 31;
const auto COLOR_GREEN = 32;
const auto COLOR_YELLOW = 33;
const auto COLOR_WHITE = 37;

int main(int argc, char** argv) {
	printer->printColored("> Executing BuildSys by Marius ( github.com/MagicDev-Marius )", COLOR_MAGENTA);

	string dir = ".";
	if (argc == 1) {
		auto cwd = argv[0];
		if (doesFileExists(cwd)) {
			dir = cwd;
		}
		else {
			printer->printColored("Cannot fetch given directory", COLOR_RED);
			return 1;
		}
	}
	else if (argc > 1) {
		printer->printColored("Usage: build [cwd]", COLOR_RED);
		return 1;
	}

	auto cfgPath = dir + "/" + buildCfgName;
	if (!doesFileExists(cfgPath)) {
		printer->printColored("Config file does not exists", COLOR_RED);
		return 1;
	}

	auto buildConfigContent = getFileContent(cfgPath);

	return 0;
}