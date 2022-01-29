#include <bitset>
#include <unordered_map>

/*** Bit array of hashes for given data ***/

class hash_bit_array {

public:

	static const int bit_array_size = 30000000;

	bool look_up(string line, int hash_key)
	{
		return (bit_array[hash_key]);
	}

	void insert(string line, int hash_key)
	{
		bit_array[hash_key] = true;
	}


	bool is_duplicate(string line, int hash_key)
	{

		if (look_up(line, hash_key))
			return true;

		insert(line, hash_key);
		return false;
	}


private:

	bitset<bit_array_size> bit_array;
};

int get_hash_key(string line, int array_size)
{
	hash<string> std_hash;
	return std_hash(line) % array_size;
}

unordered_map<int, bool> get_duplicate_hashes_map(DataGenerator& generator, int& size)
{
	hash_bit_array *bit_array = new hash_bit_array();
	unordered_map<int, bool> hash_isPossibleDuplicate_map;


	for (const string& line : generator)
	{
		int hash_key = get_hash_key(line, bit_array->bit_array_size);
		hash_isPossibleDuplicate_map[hash_key] = bit_array->is_duplicate(line, hash_key);
	}

	size = bit_array->bit_array_size;
	delete bit_array;

	unordered_map<int, bool> duplicate_hashes_map;
	for (auto key_value : hash_isPossibleDuplicate_map)
	{
		if (key_value.second)
			duplicate_hashes_map[key_value.first] = true;
	}

	return duplicate_hashes_map;
}

vector<string> get_duplicates(DataGenerator& generator, int size, unordered_map<int, bool> duplicate_hashes_map)
{
	vector<string> duplicates;
	unordered_map<int, string> hash_lines_map;

	for (const string& line : generator)
	{

		int hash_key = get_hash_key(line, size);
		if (duplicate_hashes_map[hash_key])
		{
			string prefix = line.substr(0, 5);
			if (prefix == hash_lines_map[hash_key])
			{
				duplicates.push_back(line);

			}
			else
			{
				hash_key++;
				bool is_duplicate = false;
				while (( !is_duplicate ) && (hash_lines_map.find(hash_key)!=hash_lines_map.end()))
				{
					if (prefix == hash_lines_map[hash_key])
					{
						is_duplicate = true;
						duplicates.push_back(line);
					}
					hash_key++;
				}
				if (!is_duplicate)
				{
					hash_lines_map[hash_key] = prefix;
				}
			}
		}
	}
	return duplicates;
}


vector<string> find_duplicates(DataGenerator& generator) {
	/*
	 * Find duplicates in the given data.
	 */

	int bit_array_size = 0;
	unordered_map<int, bool> duplicate_hashes_map = get_duplicate_hashes_map(generator, bit_array_size);
	return get_duplicates(generator, bit_array_size, duplicate_hashes_map);
}
