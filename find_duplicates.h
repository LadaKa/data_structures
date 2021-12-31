#include <unordered_map>

// TODO:	clean code

class node {
public:
	
	vector<node *> children;
	vector<char> keys;
	node *parent;
	int defaultKeysSize = 30; // TODO - dyn. resize

	// Constructor: initialize a node with one key
	node()
	{
		keys.reserve(defaultKeysSize);
		children.reserve(defaultKeysSize);
	}

	// If this node contains the given key, return true and set i to key's position.
	// Otherwise return false and set i to the first key greater than the given one.
	bool find_branch(char key, int &i)
	{
		i = 0;
		while (i < keys.size() && keys[i] <= key) {
			if (keys[i] == key)
				return true;
			i++;
		}
		return false;
	}

	// Insert a new key and a new child at position i.
	void insert_node(int i, char key, node *child)
	{
		keys.insert(keys.begin() + i, key);
		children.insert(children.begin() + i, child);

	}
};

/*** Trie ***/

class trie {
public:
	         
	node *root; 
    unordered_map<string, int> duplicates;

	// Create a new node and return a pointer to it.
	node *new_node(node* parent)
	{
		node *n = new node();
		
		n->parent = parent;  // TODO: remove this pointer?
		return n;
	}

	// Delete a given node, assuming that its children have been already unlinked.
	void delete_node(node *n)
	{
		delete n;
	}

	// Constructor: initialize an empty tree with just the root.
	trie()
	{
		// The root has one null child pointer.
		root = new_node(nullptr);
	}

	// An auxiliary function for deleting a subtree recursively.
	void delete_trie(node *n)
	{
		for (int i = 0; i < n->children.size(); i++)
			if (n->children[i])
				delete_trie(n->children[i]);
		delete_node(n);
	}

	// Destructor: delete all nodes.
	~trie()
	{
		delete_trie(root);
	}

	void process_line(string line)
	{
		node *current = root;
		int position;
		int line_index = 0;
		char key = line[line_index];
		while (line_index < line.length() && current->find_branch(key, position))
		{
			current = current->children[position];
			line_index++;
			key = line[line_index];
		}
		if (line_index == line.length())
		{
			if (current->children.size() == 0)
			{
				// duplicate line
				duplicates[line]++;
				return;
			}
			// prefix of existing line
			current->insert_node(0, key, new_node(current));
			return;
		}
		// unique suffix
		insert_branch(position, key, line.substr(line_index + 1),current);
	}

private:

	void insert_branch(int i, char key, string suffix, node *current)
	{
		node *last, *next;
		next = new_node(current);
		current->insert_node(i, key, next);
		for (char& c : suffix)
		{
			last = next;
			next = new_node(current);
			last->keys.push_back(c);
			last->children.push_back(next);
		}
	}
};



vector<string> find_duplicates(DataGenerator& generator) {
	/*
	 * Find duplicates in the given data.
	 *
	 * The `generator` provides a forward iterator over strings
	 * for traversing the data, so it can be iterated for example
	 * using a `for` cycle:
	 *
	 *   for (const string& item : generator) {...}
	 *
	 * The `generator` can be traversed multiple times.
	 *
	 * The goal is to return a vector of duplicated entries,
	 * reporting each duplicated entry only once.
	 */

	trie *t = new trie();
	vector<string> duplicates = {};
	for (const string& line : generator)
	{
		t->process_line(line);
	}
	for (auto d : t->duplicates)
		duplicates.push_back(d.first);

	return duplicates;
}
