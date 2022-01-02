#include <unordered_map>

// TODO:	clean code

class node {
public:
	
	unordered_map<char, node*> children;

	// Constructor: initialize an empty node
	node()
	{
	}

	bool has_key(char key)
	{
		return children.find(key) != children.end();
	}

	// Insert a new key and a new child
	void insert_node(char key, node *child)
	{
		children[key] = child;
	}
};

/*** Trie ***/

class trie {
public:
	         
	node *root; 
    unordered_map<string, int> duplicates;

	// Create a new node and return a pointer to it.
	node *new_node()
	{
		node *n = new node();
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
		root = new_node();
	}

	// An auxiliary function for deleting a subtree recursively.
	void delete_trie(node *n)
	{
		for (auto item : n->children)
			delete_trie(item.second);
		delete_node(n);
	}

	// Destructor: delete all nodes.
	~trie()
	{
		delete_trie(root);
	}

	void process_line(string line)
	{

		// traverse trie by line prefix
		node *current = root;
		int line_index = 0;
		char key = line[line_index];
		while (line_index < line.length() && current->has_key(key))
		{
			current = current->children[key];
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
			current->insert_node(key, new_node());
			return;
		}
		// unique suffix
		insert_branch(key, line.substr(line_index + 1), current);		
	}

private:

	void insert_branch(char key, string suffix, node *current)
	{
		node *last, *next;
		next = new_node();
		current->insert_node(key, next);
		for (char& c : suffix)
		{
			last = next;
			next = new_node();
			last->insert_node(c, next);
		}
	}
};



vector<string> find_duplicates(DataGenerator& generator) {
	/*
	 * Find duplicates in the given data.
	 */

	trie *t = new trie();
	vector<string> duplicates = {};
	for (const string& line : generator)
	{
		t->process_line(line);
		cout << line << endl;
	}
	for (auto d : t->duplicates)
		duplicates.push_back(d.first);
	t->delete_trie(t->root);
	return duplicates;
}
