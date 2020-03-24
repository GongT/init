#pragma once
#include <string>
#include <vector>
#include <csignal>
#include "debug.hpp"

using namespace std;

class Program
{
protected:
	string _title;
	vector<string> _commands;
	int _signal;
	Program(const string title) : _title(title), _signal(SIGINT){};

private:
	bool set_value(const string key, string value);

public:
	const string &title() const { return this->_title; };
	const vector<string> &commands() const { return this->_commands; };
	const int &signal() const { return this->_signal; };

	static const vector<const Program *> ParseFile(const string fp);
};
