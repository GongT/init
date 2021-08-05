#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "file-parser.hpp"
#include "signal-switch.hpp"

using namespace std;

const string whitespaces(" \t\f\v\n\r");

string &trim(string &s)
{
	auto found = s.find_last_not_of(whitespaces);
	if (found != std::string::npos)
	{
		s.erase(found + 1);
	}
	else
	{
		s.clear();
		return s;
	}

	auto found2 = s.find_first_not_of(whitespaces);
	if (found2 != std::string::npos)
		s.erase(0, found2);

	return s;
}

bool Program::set_value(const string key, string value)
{
	if (key == "arg[]")
	{
		_commands.push_back(value);
		cerr << "    add argument: (" << _commands.size() << ") " << value << endl;
		return true;
	}
	if (key == "signal")
	{
		transform(value.begin(), value.end(), value.begin(), ::toupper);
		if (value.find("SIG") == 0)
			value.erase(0, 3);

		_signal = signal_number(value);
		if (!_signal)
			die("Unknown signal: %s", value.c_str());

		cerr << "    stop signal: " << value << "(" << _signal << ") " << endl;
		return true;
	}
	return false;
}

void fail_parse_line(const string line)
{
	die("can not parse line: %s", line.c_str());
}

const vector<const Program *> Program::ParseFile(const string filepath)
{
	ifstream fp(filepath);
	if (!fp.is_open())
		die("failed to open file %s", filepath.c_str());

	vector<const Program *> result;

	string line_buffer;
	Program *last = NULL;

	while (getline(fp, line_buffer))
	{
		if (line_buffer.size() == 0)
			continue;
		if (line_buffer.at(0) == '#')
			continue;

		trim(line_buffer);
		if (line_buffer.size() == 0)
			continue;

		// cerr << " + " << line_buffer << endl;

		if (line_buffer.front() == '[' && line_buffer.back() == ']')
		{
			string title(line_buffer.begin() + 1, line_buffer.end() - 1);
			trim(title);
			last = new Program(string("[") + title + string("]"));
			result.push_back(last);
			cerr << "program: " << last->title() << endl;
		}
		else
		{
			if (last == NULL)
				fail_parse_line(line_buffer);

			size_t sSignPos = line_buffer.find(':');
			if (sSignPos == string::npos)
				fail_parse_line(line_buffer);

			string key(line_buffer.substr(0, sSignPos));
			string value(line_buffer.substr(1 + sSignPos));

			trim(key);
			trim(value);

			if (!last->set_value(key, value))
				fail_parse_line(line_buffer);
		}
	}

	cerr << "config file loaded." << endl;

	fp.close();

	return result;
}
