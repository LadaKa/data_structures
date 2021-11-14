#include <limits>
#include <vector>
#include <tuple>
#include <iostream>

using namespace std;

// If the condition is not true, report an error and halt.
#define EXPECT(condition, message) do { if (!(condition)) expect_failed(message); } while (0)


void expect_failed(const string& message);

/*** One node ***/

class ab_node {
public:
	// Keys stored in this node and the corresponding children
	// The vectors are large enough to accomodate one extra entry
	// in overflowing nodes.
	vector<ab_node *> children;
	vector<int> keys;
	ab_node *parent;

	// Constructor: initialize an empty ab-node.
	ab_node(int a, int b)
	{
		keys.reserve(b);
		children.reserve(b + 1);
	}

	// If this node contains the given key, return true and set i to key's position.
	// Otherwise return false and set i to the first key greater than the given one.
	bool find_branch(int key, int &i)
	{
		i = 0;
		while (i < keys.size() && keys[i] <= key) {
			if (keys[i] == key)
				return true;
			i++;
		}
		return false;
	}

	// Insert a new key at position i and add a new child between keys i and i+1.
	void insert_branch(int i, int key, ab_node *child)
	{
		keys.insert(keys.begin() + i, key);
		children.insert(children.begin() + i + 1, child);

	}

	// An auxiliary function for displaying a sub-tree under this node.
	void show(int indent);
};

/*** Tree ***/

class ab_tree {
public:
	int a;          // Minimum allowed number of children
	int b;          // Maximum allowed number of children
	ab_node *root;  // Root node (even a tree with no keys has a root)
	int num_nodes;  // We keep track of how many nodes the tree has

	// Create a new node and return a pointer to it.
	ab_node *new_node(ab_node* parent)
	{
		ab_node *n = new ab_node(a, b);
		n->parent = parent;
		num_nodes++;
		return n;
	}

	// Delete a given node, assuming that its children have been already unlinked.
	void delete_node(ab_node *n)
	{
		num_nodes--;
		delete n;
	}

	// Constructor: initialize an empty tree with just the root.
	ab_tree(int a, int b)
	{
		EXPECT(a >= 2 && b >= 2 * a - 1, "Invalid values of a,b");
		this->a = a;
		this->b = b;
		num_nodes = 0;
		// The root has no keys and one null child pointer.
		root = new_node(nullptr);
		root->children.push_back(nullptr);
	}

	// An auxiliary function for deleting a subtree recursively.
	void delete_tree(ab_node *n)
	{
		for (int i = 0; i < n->children.size(); i++)
			if (n->children[i])
				delete_tree(n->children[i]);
		delete_node(n);
	}

	// Destructor: delete all nodes.
	~ab_tree()
	{
		delete_tree(root);
		EXPECT(num_nodes == 0, "Memory leak detected: some nodes were not deleted");
	}

	// Find a key: returns true if it is present in the tree.
	bool find(int key)
	{
		ab_node *n = root;
		while (n) {
			int i;
			if (n->find_branch(key, i))
				return true;
			n = n->children[i];
		}
		return false;
	}

	// Display the tree on standard output in human-readable form.
	void show();

	// Check that the data structure satisfies all invariants.
	void audit();

	// Split the node into two nodes: move some children of n into 
	// a newly created node such that n contains exactly size children in the end.
	// Return the new node and the key separating n and the new node.
	virtual pair<ab_node*, int> split_node(ab_node* n, int size)
	{
		int separating_key = n->keys.at(size - 1);

		// create node with keys greater than separating_key and corresponding children
		ab_node* created_node = create_node_from_split_node(n, size);

		// erase items, that were moved to the new node
		n->keys.erase(n->keys.begin() + size - 1, n->keys.end());
		n->children.erase(n->children.begin() + size, n->children.end());

		return { created_node, separating_key };
	}


	// Insert: add key to the tree (unless it was already present).
	virtual void insert(int key)
	{
		ab_node *parent;
		if (find_or_return_nearest_parent(key, parent))
			return;		//  key was already present

		int i;
		parent->find_branch(key, i);
		parent->insert_branch(i, key, nullptr);	// insert key to the nearest parent

		//  split and move up 
		//  until we reach a node with less than b children (or root)
		while (parent->children.size() > b)		
		{
			pair<ab_node*, int> node_key_pair = split_node(parent, parent->children.size() / 2);
			ab_node *last = parent;
			parent = last->parent;
			if (last == root)
			{
				// create new root with two children
				root = create_new_root(last, node_key_pair.first, node_key_pair.second);
				return;
			}

			int i;
			parent->find_branch(key, i);
			parent->insert_branch(i, node_key_pair.second, node_key_pair.first);
			node_key_pair.first->parent = parent;
		}
	}

private:

	//  If this node contains the given key, return true.
	//  Otherwise return false and set 'parent' to a parent of a leaf,
	//	where search ends.
	bool find_or_return_nearest_parent(int key, ab_node* &parent)
	{
		parent = root;
		ab_node *n = root;
		while (n) {
			int i;
			if (n->find_branch(key, i))
				return true;
			parent = n;
			n = n->children[i];
		}
		return false;
	}

	//  Create new root with given key and two children.
	ab_node* create_new_root(ab_node* first_child, ab_node* second_child, int key)
	{
		ab_node *new_root = new ab_node(a, b);
		num_nodes++;
		new_root->children.insert(new_root->children.begin(), first_child);
		first_child->parent = new_root;
		new_root->insert_branch(0, key, second_child);
		second_child->parent = new_root;
		return new_root;
	}
	
	//  Create new node that contains all keys greater than separating_key
    //  and all corresponding children.
	ab_node* create_node_from_split_node(ab_node* n, int size)
	{
		ab_node* created_node = new ab_node(a, b);
		num_nodes++;
		int created_children_size = (n->children.size()) - size;
		int created_keys_size = created_children_size - 1;
		created_node->children = { n->children[size] };
		for (int i = 0; i < created_keys_size; i++)
		{
			int index = size + i;
			created_node->insert_branch(
				i, n->keys[index], n->children[index + 1]);
		}
		for (int i = 0; i < created_children_size; i++)
		{
			if (created_node->children[i])
				created_node->children[i]->parent = created_node;
		}
		return created_node;
	}
};
#pragma once
