#include <unordered_map>

/*** Bloom Filter ***/

class bloom_filter {
public:


	unordered_map<int, bool> map_0;

	// Constructor: initialize an empty .......
	bloom_filter()
	{
		//..
	}


	// Destructor
	~bloom_filter()
	{
		// TODO
	}

	bool is_duplicate(string line)
	{
		int hash_0 = std::hash<std::string>{}(line);
		//cout << hash_0 << endl;
		if (map_0[hash_0]==true)
		{
			return true;
		}
		map_0[hash_0] = true;
		return false;
	}



private:

	
};



vector<string> find_duplicates(DataGenerator& generator) {
	/*
	 * Find duplicates in the given data.
	 */


	bloom_filter *filter = new bloom_filter();
	vector<string> duplicates = {};
	for (const string& line : generator)
	{
		if (filter->is_duplicate(line))
		{
			duplicates.push_back(line);
		}
	}
	
	// TODO: delete
	return duplicates;
}
