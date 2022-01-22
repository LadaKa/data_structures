#include <cstdint>
#include <limits>
#include <iostream>
using namespace std;

// A node of the tree
class Node {
public:
	int64_t key;
	int64_t value;
	int64_t subtree_value_sum;
	Node* left;
	Node* right;
	Node* parent;

	// Constructor
	Node(int64_t key, int64_t value, Node* parent = nullptr, Node* left = nullptr, Node* right = nullptr) {
		this->key = key;
		this->value = value;
		this->subtree_value_sum = value;
		this->parent = parent;
		this->left = left;
		this->right = right;
		if (left) {
			left->parent = this;
			this->subtree_value_sum = this->subtree_value_sum + left->subtree_value_sum;
		}
		if (right) {
			right->parent = this;
			this->subtree_value_sum = this->subtree_value_sum + right->subtree_value_sum;
		}
	}

	void setLeftChild(Node* child) {
		if (left)
			subtree_value_sum = subtree_value_sum - left->subtree_value_sum;
		left = child;
		subtree_value_sum = subtree_value_sum + child->subtree_value_sum;
	}

	void setRightChild(Node* child) {
		if (right)
			subtree_value_sum = subtree_value_sum - right->subtree_value_sum;
		right = child;
		subtree_value_sum = subtree_value_sum + child->subtree_value_sum;
	}
};

// Splay tree
class Tree {
public:
	// Pointer to root of the tree; nullptr if the tree is empty.
	Node* root;

	Tree(Node* root = nullptr) {
		this->root = root;
	}

	// Rotate the given `node` up. Perform a single rotation of the edge
	// between the node and its parent, choosing left or right rotation
	// appropriately.
	virtual void rotate(Node* node) {
		if (node->parent) {
			if (node->parent->left == node) {
				if (node->right) node->right->parent = node->parent;
				node->parent->left = node->right;
				node->right = node->parent;
			}
			else {
				if (node->left) node->left->parent = node->parent;
				node->parent->right = node->left;
				node->left = node->parent;
			}
			if (node->parent->parent) {
				if (node->parent->parent->left == node->parent)
					node->parent->parent->left = node;
				else
					node->parent->parent->right = node;
			}
			else {
				root = node;
			}

			Node* original_parent = node->parent;
			node->parent = node->parent->parent;
			original_parent->parent = node;
		}
	}

	// Splay the given node.
	virtual void splay(Node* node) {
		while (node->parent && node->parent->parent) {
			if ((node->parent->right == node && node->parent->parent->right == node->parent) ||
				(node->parent->left == node && node->parent->parent->left == node->parent)) {
				rotate(node->parent);
				rotate(node);
			}
			else {
				rotate(node);
				rotate(node);
			}
		}
		if (node->parent)
			rotate(node);
	}

	// Look up the given key in the tree, returning the
	// the node with the requested key or nullptr.
	Node* lookup(int64_t key) {
		Node* node = root;
		Node* node_last = nullptr;
		while (node) {
			node_last = node;
			if (node->key == key)
				break;
			if (key < node->key)
				node = node->left;
			else
				node = node->right;
		}
		if (node_last)
			splay(node_last);
		return node;
	}

	// Insert a (key, value) into the tree.
	// If the key is already present, nothing happens.
	void insert(int64_t key, int64_t value) {
		if (!root) {
			root = new Node(key, value);
			return;
		}

		Node* node = root;
		while (node->key != key) {
			if (key < node->key) {
				if (!node->left)
					node->left = new Node(key, value, node);
				node = node->left;
			}
			else {
				if (!node->right)
					node->right = new Node(key, value, node);
				node = node->right;
			}
		}
		splay(node);
	}

	// Delete given key from the tree.
	// It the key is not present, do nothing.
	//
	// The implementation first splays the element to be removed to
	// the root, and if it has both children, splays the largest element
	// in the left subtree and links it to the original right subtree.
	void remove(int64_t key) {
		if (lookup(key)) {
			Node* right = root->right;
			root = root->left;
			if (!root) {
				root = right;
				right = nullptr;
			}
			if (root)
				root->parent = nullptr;

			if (right) {
				Node* node = root;
				while (node->right)
					node = node->right;
				splay(node);
				root->right = right;
				right->parent = root;
			}
		}
	}

	// Return the sum of elements with keys in range [left, right].
	//
	// Given a closed range [left, right], return the sum of values of elements
	// in the range, i.e., sum(value | (key, value) in tree, left <= key <= right).
	int64_t range_sum(int64_t left, int64_t right) {

		Node* left_node = lookup_nearest_greater(left);
		if (!left_node)
			return 0;

		left_node->left = nullptr;

		Node* right_node = lookup_nearest_smaller(right);
		if (!right_node)
			return 0;

		right_node->right = nullptr;

		return sum_subtree(root);
	}

	// Destructor to free all allocated memory.
	~Tree() {
		Node* node = root;
		while (node) {
			Node* next;
			if (node->left) {
				next = node->left;
				node->left = nullptr;
			}
			else if (node->right) {
				next = node->right;
				node->right = nullptr;
			}
			else {
				next = node->parent;
				delete node;
			}
			node = next;
		}
	}

	Node* lookup_nearest_smaller(int64_t key) {

		Node* node = root;
		Node* node_last = nullptr;
		if (node && node->key < key) {
			node_last = node;
			node = node->right;
		}
		while (node && node->key > key) {
			node_last = node;
			node = node->left;
		}

		node_last = look_up_or_get_possible_parent(node, node_last, key);
		if (node_last)
			splay(node_last);
		if (node_last->key <= key)
			return node_last;	

		return nullptr;
	}

	Node* lookup_nearest_greater(int64_t key) {

		Node* node = root;
		Node* node_last = nullptr;
		if (node && node->key > key) {
			node_last = node;
			node = node->left;
		}
		while (node && node->key < key) {
			node_last = node;
			node = node->right;
		}

		node_last = look_up_or_get_possible_parent(node, node_last, key);
		if (node_last)
			splay(node_last);
		if (node_last->key >= key)
			return node_last;

		return nullptr;
	}

	Node* look_up_or_get_possible_parent(Node* node, Node* node_last, int64_t key) {

		while (node) {
			node_last = node;
			if (node->key == key)
				break;
			if (key < node->key)
				node = node->left;
			else
				node = node->right;
		}
		return node_last;
	}

	int64_t sum_subtree(Node* node) {

		int64_t result = node->value;
		if (node->left) {
			result = result + sum_subtree(node->left);
		}
		if (node->right) {
			result = result + sum_subtree(node->right);
		}
		return result;
	}
};
