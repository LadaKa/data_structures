#include <bitset>

/*** Bloom Filter ***/

class bloom_filter {

public:

	bloom_filter(int hash_functions_count)
	{
		hash_count = hash_functions_count;
	}


	bool look_up(string line)
	{
		vector<int> hash_keys = get_keys(line);
		bool all_true = bit_array[hash_keys[0]];

		int index = 1;
		while (all_true && index < hash_count)
		{
			all_true = all_true && bit_array[hash_keys[index]];
			index++;
		}

		return all_true;
	}


	void insert(string line)
	{
		vector<int> hash_keys = get_keys(line);

		for (int i = 0; i < hash_count; i++)
		{
			bit_array[hash_keys[i]] = true;
		}
	}


	bool is_duplicate(string line)
	{
		if (look_up(line))
			return true;

		insert(line);
		return false;
	}

private:

	int hash_count;
	int primes[8] = { 7, 13, 17, 23, 31, 37, 41 };
	static const int bit_array_size = 512000000;
	bitset<bit_array_size> bit_array;

	int hash_function_std(string line)
	{
		hash<string> std_hash;
		return std_hash(line) % bit_array_size;
	}


	int hash_function_multiply(string line, int index)
	{
		hash<string> hash;
		int prime = primes[index];

		long long hash_value = 0LL;
		for (int i = 0; i < line.length(); i++)
		{
			hash_value = ((hash_value * prime) + line[i]);
			hash_value = hash_value % bit_array_size;
		}

		return hash_value;
	}


	vector<int> get_keys(string line)
	{
		vector<int> hash_keys;

		//  std hash function:
		//	hash_keys[0]
		hash_keys.push_back(hash_function_std(line));

		//  hash functions using multiplication with primes[i]:
		//	hash_keys[1, .., hash_count - 1]
		for (int i = 1; i < hash_count; i++)
		{
			hash_keys.push_back(hash_function_multiply(line, i));
		}

		return hash_keys;
	}
};


vector<string> find_duplicates(DataGenerator& generator) {
	/*
	 * Find duplicates in the given data.
	 */

	int hash_functions_count = 3;
	bloom_filter *filter = new bloom_filter(hash_functions_count);
	vector<string> duplicates = {};
	for (const string& line : generator)
	{
		if (filter->is_duplicate(line))
		{
			duplicates.push_back(line);
		}
	}

	delete filter;

	return duplicates;
}
