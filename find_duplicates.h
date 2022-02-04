#include <bitset>
#include <unordered_map>
#include <unordered_set>

/*
	Data structure that contains hashes of lines
	in unordered set.
*/
class Lines_hashes {

public:

	Lines_hashes()
	{}

	bool look_up(const string &line)
	{
		int32_t hash_key = get_hash_key(line);
		return hashes.find(hash_key) != hashes.end();
	}

	void insert(const string &line)
	{
		int32_t hash_key = get_hash_key(line);
		hashes.insert(hash_key);
	}

	bool is_duplicate(const string &line)
	{
		if (look_up(line))
			return true;

		insert(line);
		return false;
	}


	/*
		Returns set of duplicate hashes of lines
		for given generator
	*/
	unordered_set<int32_t> get_duplicate_hashes_set(DataGenerator& generator)
	{
		Lines_hashes *lines_hashes = new Lines_hashes();
		unordered_set<int32_t> duplicate_hashes_set;

		for (const string& line : generator)
		{
			int32_t hash_key = get_hash_key(line);
			if (lines_hashes->is_duplicate(line))
				duplicate_hashes_set.insert(hash_key);
		}

		delete lines_hashes;

		return duplicate_hashes_set;
	}

	/*
		Returns duplicate lines
		for given generator
	*/
	vector<string> get_duplicates(DataGenerator& generator, unordered_set<int32_t> &duplicate_hashes_set)
	{
		vector<string> duplicates;
		unordered_map<int32_t, string> hash_lines_map;

		for (const string& line : generator)
		{
			int32_t hash_key = get_hash_key(line);

			//	check if line has duplicate hash_key 
			if (is_duplicate_hash(duplicate_hashes_set, hash_key))
			{
				//	check if line with duplicate hash_key 
				//	is duplicate

				bool is_duplicate_line = false;
				while ((!is_duplicate_line) && (has_hash_key(hash_lines_map, hash_key)))
				{
					//	compare line with the string that has same hash_key
					if (line == hash_lines_map[hash_key])
					{
						// new duplicate found
						is_duplicate_line = true;
						duplicates.push_back(line);
					}
					//	increase hash_key to handle possible collision
					hash_key = (hash_key + 1);
				}

				if (!is_duplicate_line)
				{
					//	line is not duplicate, 
					//	will be inserted to the next free position 
					//	after strings with the same hash_key
					hash_lines_map[hash_key] = line;
				}
			}
		}
		return duplicates;
	}


private:

	hash<string> std_hash;
	unordered_set<int32_t> hashes;

	int32_t get_hash_key(const string &line)
	{
		return std_hash(line);
	}


	/*
		Returns whether the hash_key is element of given duplicate_hashes_set
	*/
	bool is_duplicate_hash(unordered_set<int32_t> &duplicate_hashes_set, int32_t hash_key)
	{
		return ((duplicate_hashes_set.find(hash_key)) != duplicate_hashes_set.end());
	}


	/*
		Returns whether there is a line with given hash_key
		in hash_lines_map
	*/
	bool has_hash_key(unordered_map<int32_t, string> &hash_lines_map, int32_t hash_key)
	{
		return (hash_lines_map.find(hash_key) != hash_lines_map.end());
	}

};


/*
	Find duplicates in the given data.
*/
vector<string> find_duplicates(DataGenerator& generator)
{
	Lines_hashes lines_hashes;
	unordered_set<int> duplicate_hashes_set = lines_hashes.get_duplicate_hashes_set(generator);
	return lines_hashes.get_duplicates(generator, duplicate_hashes_set);
}
