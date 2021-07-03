#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>

#include "json.h"

using std::string;

// Little Printer for outputing content to the console
class Printer {
public:
	// write a string to the console
	void print(string message) {
		std::cout << message;
	}

	// write a boolean to the console
	// (needed for debugging)
	void print(bool boolean) {
		std::cout << (boolean ? "true" : "false");
	}

	// prints a colored message to the console, console is defined by ansii colors
	void printColored(string message, int color) {
		print("\x1B[" + std::to_string(color) + "m" + message + "\033[0m\n");
	}
};

// read a file by its path and return its content
string getFileContent(string path) {
	std::ifstream ifs(path);
	string content(
		(std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>())
	);
	return content;
}

// check if a file with the given path exists
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

std::tuple<int, JSONCPP_STRING, Json::Value> parseJson(string rawJson) {
	const auto jsonLength = static_cast<int>(rawJson.length());

	JSONCPP_STRING err;
	Json::Value root;
	
	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(rawJson.c_str(), rawJson.c_str() + jsonLength, &root, &err)) {
		return std::make_tuple(1, err, root);
	}

	return std::make_tuple(0, err, root);
}

// name of the file we want to use as configuration
const string buildCfgName = ".buildcfg";
// the "logger" we want to use for outputting data to the console
const auto printer = new Printer();

// constants for the colors we use in the console
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

	// raw JSON
	auto buildConfigContent = getFileContent(cfgPath);
	// parsed JSON
	auto [ status, error, value ] = parseJson(buildConfigContent);	
	
	// check if parsing was successfull
	if (status != 0) {
		printer->printColored("Error: " + error, COLOR_RED);
		return 1;
	}



	return 0;
}
