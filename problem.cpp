#include <bits/stdc++.h>

using namespace std;

// Function to decode a number from given base to decimal
long long decodeBase(const string &value, int base)
{
	long long result = 0;
	for (char c : value)
	{
		int digit;
		if (isdigit(c))
			digit = c - '0';
		else if (isalpha(c))
			digit = tolower(c) - 'a' + 10;
		else
			digit = 0;
		result = result * base + digit;
	}
	return result;
}

// Lagrange interpolation to find P(0)
long double lagrangeInterpolation(const vector<long long> &x, const vector<long long> &y, long double targetX)
{
	int k = x.size();
	long double result = 0.0;

	for (int i = 0; i < k; i++)
	{
		long double term = y[i];
		for (int j = 0; j < k; j++)
		{
			if (i != j)
			{
				term *= (targetX - x[j]) / (long double)(x[i] - x[j]);
			}
		}
		result += term;
	}
	return result;
}

// Simple JSON parser for our specific format
map<string, string> parseJsonFile(const string &filename)
{
	ifstream file(filename);
	if (!file)
	{
		cerr << "Error: Could not open " << filename << endl;
		exit(1);
	}

	map<string, string> result;
	string line;
	string currentSection = "";

	while (getline(file, line))
	{
		// Remove whitespace
		line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
		if (line.empty() || line == "{" || line == "}" || line == "{," || line == "},")
			continue;

		// Remove trailing comma
		if (!line.empty() && line.back() == ',')
		{
			line.pop_back();
		}

		size_t colonPos = line.find(':');
		if (colonPos != string::npos)
		{
			string key = line.substr(0, colonPos);
			string value = line.substr(colonPos + 1);

			// Remove quotes
			key.erase(remove(key.begin(), key.end(), '"'), key.end());
			value.erase(remove(value.begin(), value.end(), '"'), value.end());

			if (key == "keys")
			{
				currentSection = "keys";
			}
			else if (currentSection == "keys")
			{
				result["keys_" + key] = value;
			}
			else if (key == "base" || key == "value")
			{
				result[currentSection + "_" + key] = value;
			}
			else
			{
				// This is a numbered section
				currentSection = key;
			}
		}
	}

	file.close();
	return result;
}

int main()
{
	// Parse JSON file
	map<string, string> data = parseJsonFile("input.json");

	int n = stoi(data["keys_n"]);
	int k = stoi(data["keys_k"]);

	vector<pair<long long, long long>> points;

	// Extract and decode points
	for (const auto &entry : data)
	{
		const string &key = entry.first;

		// Skip keys entries
		if (key.find("keys_") == 0)
			continue;

		// Find base entries
		if (key.find("_base") != string::npos)
		{
			string pointKey = key.substr(0, key.find("_base"));

			long long x = stoll(pointKey);
			int base = stoi(entry.second);
			string val = data[pointKey + "_value"];

			long long y = decodeBase(val, base);
			points.push_back({x, y});
		}
	}

	// Sort by x-coordinate
	sort(points.begin(), points.end());

	// Use only first k points
	vector<long long> X, Y;
	for (int i = 0; i < k; i++)
	{
		X.push_back(points[i].first);
		Y.push_back(points[i].second);
	}

	// Get constant term (P(0))
	long double secretC = lagrangeInterpolation(X, Y, 0.0);

	cout << fixed << setprecision(0) << secretC << endl;

	return 0;
}
