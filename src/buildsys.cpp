#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

#include "json.h"

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
		char buffer[PATH_MAX];
		if (getcwd(buffer, sizeof(buffer)) == NULL) {
			printer->printColored("Cannot fetch current working dir", COLOR_RED);
			return 1;
		}
		auto cwd = buffer;
		
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
	auto [ status, error, value ] = parseJson(buildConfigContent);	
	
	if (status != 0) {
		printer->printColored("Error: " + error, COLOR_RED);
		return 1;
	}

	printer->printColored("> BuildScript " + value["name"].asString() + " v" + value["version"].asString());
	try {
		printer->printColored("by " + value["author"].asString(), COLOR_GREEN);
	} catch (std::exception e) {}
	try {
		printer->printColored("=> " + value["description"].asString(), COLOR_GREEN);
	} catch (std::exception e) {}
	
	try {
		Json::Value jobs = value["jobs"];
		for (Json::Value::ArrayIndex i = 0; i != jobs.size(); i++) {
			Json::Value job = jobs[i];
			try {
				string tag = std::to_string(i);
				if (i < 10) {
					tag = "0" + tag;
				}
				printer->printColored("# Running task " + tag + " '" + job["name"].asString() + "'");
				
				Json::Value tasks = job["tasks"];
				for (Json::Value::ArrayIndex j = 0; j != tasks.size(); j++) {
					Json::Value taskValue = tasks[j];
					string task = taskValue.asString();
					
					printer->printColored("$> " + task, COLOR_WHITE);
					
					const char *str = task.c_str();
					auto result = system(str);
					
					if (result != 0) {
						printer->printColored("!!! Tasks finished with exit code " + std::to_string(result) + ". Exit...", COLOR_WHITE);
						return 1;
					}
				}
			} catch (std::exception e1) {
				printer->printColored("!!! Error fetching tasks from job '" + job["name"].asString() + "'. Array might not be valid", COLOR_RED);
			}	
		}
	} catch (std::exception e2) {
		printer->printColored("!!! Error fetching jobs. Array might not be valid", COLOR_RED);	
	}

	return 0;
}
