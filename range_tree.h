#include <cstdint>
#include <limits>
#include <iostream>
using namespace std;

// A node of the tree
class Node {
public:
	int64_t key;
	int64_t value;
	int64_t subtree_value_sum; // = value + subtree_value_sum of children 
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

	// Add left child and update subtree_value_sum
	void addLeftChild(Node* child) {
		if (left)
			subtree_value_sum = subtree_value_sum - left->subtree_value_sum;
		left = child;
		if (child)
			subtree_value_sum = subtree_value_sum + child->subtree_value_sum;
	}

	// Add left child and update subtree_value_sum
	void addRightChild(Node* child) {
		if (right)
			subtree_value_sum = subtree_value_sum - right->subtree_value_sum;
		right = child;
		if (child)
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
				if (node->right)
					node->right->parent = node->parent;
				node->parent->addLeftChild(node->right);
				node->addRightChild(node->parent);
			}
			else {
				if (node->left)
					node->left->parent = node->parent;
				node->parent->addRightChild(node->left);
				node->addLeftChild(node->parent);
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
			if (node)
				node->subtree_value_sum = node->subtree_value_sum + value;
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

		int64_t sum = root->subtree_value_sum;

		// Look up and splay the node with the nearest greater key.
		// Subtract the values in its left subtree.
		Node* left_node = lookup_nearest_greater(left);
		if (!left_node)
			return 0;	// all keys are smaller than left
		if (left_node->left)
			sum = sum - left_node->left->subtree_value_sum;

		// Look up and splay the node with the nearest smaller key.
		// Subtract the values in its right subtree.
		Node* right_node = lookup_nearest_smaller(right);
		if (!right_node)
			return 0;	// all keys are greater than right
		if (right_node->right)
			sum = sum - right_node->right->subtree_value_sum;

		return sum;
	}


	// Given a closed range [left, right], 
	// adds delta to the values of elements in the range.
	void range_update(int64_t left, int64_t right, int64_t delta) {

		// Empty range
		if (left > right)
			return;

		// Look up and splay the node with the nearest greater key.
		Node* left_node = lookup_nearest_greater(left);
		if (!left_node)
			return;	// all keys are smaller than left

		// Look up and splay the node with the nearest smaller key.
		Node* right_node = lookup_nearest_smaller(right);
		if (!right_node)
			return;	// all keys are greater than right


		update_values(delta, right_node, left_node);
	}

	int test_range_update(int64_t left, int64_t right, int64_t delta) {

		int nodes_in_range_count = get_count_nodes_in_range(left, right);

		int64_t sum = sum_subtree_values();

		range_update(left, right, delta);
		int64_t sum_after_update = sum_subtree_values();

		int64_t expected_sum_after_update = sum + (delta * nodes_in_range_count);
		EXPECT(
			expected_sum_after_update == sum_after_update,
			error_msg_expected_sum_after_update(sum_after_update, expected_sum_after_update));

		return 0;
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

private:

	// Look up and splay the node with given key,
	// or node with the nearest smaller key
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
		if (!node && node_last->parent && node_last->parent->key <= key)
		{
			node_last = node_last->parent;
		}
	
		node_last = look_up_or_get_possible_parent(node, node_last, key);
		if (node_last)
			splay(node_last);
		if (node_last->key <= key)
			return node_last;

		// all keys are smaller than key
		return nullptr;
	}

	// Look up and splay the node with given key,
	// or node with the nearest greater key
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

		// all keys are smaller than key
		return nullptr;
	}

	// Look up the node with given key,
	// or possible parent of this node
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

	void update_values(int64_t delta, Node* rigth_bound_node, Node* left_bound_node) {

		if (rigth_bound_node->key == left_bound_node->key) {
			rigth_bound_node->value = rigth_bound_node->value + delta;
			return;
		}

		rigth_bound_node->value = rigth_bound_node->value + delta;
		update_values_from_right_to_left(delta, rigth_bound_node->key, rigth_bound_node->left, left_bound_node);
		if (rigth_bound_node->right) {
			rigth_bound_node->subtree_value_sum = rigth_bound_node->subtree_value_sum + rigth_bound_node->right->subtree_value_sum;
		}
		if (rigth_bound_node->left) {
			rigth_bound_node->subtree_value_sum = rigth_bound_node->subtree_value_sum + rigth_bound_node->left->subtree_value_sum;
		}
	}

	void update_values_from_right_to_left(int64_t delta, int64_t right_bound, Node* rigth_bound_node, Node* left_bound_node) {

		if (!rigth_bound_node)
			return; // the only one node in range is rigth_bound_node->parent

		rigth_bound_node->value = rigth_bound_node->value + delta;
		rigth_bound_node->subtree_value_sum = rigth_bound_node->value;

		if (rigth_bound_node->right) {

			update_values_from_right_to_left(delta, right_bound, rigth_bound_node->right, left_bound_node);
			rigth_bound_node->subtree_value_sum = rigth_bound_node->subtree_value_sum + rigth_bound_node->right->subtree_value_sum;
		}
		if (rigth_bound_node->left) {

			if (rigth_bound_node->left->key < left_bound_node->key) {
				return;
			}

			update_values_from_right_to_left(delta, right_bound, rigth_bound_node->left, left_bound_node);
			rigth_bound_node->subtree_value_sum = rigth_bound_node->subtree_value_sum + rigth_bound_node->left->subtree_value_sum;
		}

	}


	/* TEST methods for range_update */

	// If the condition is not true, report an error and halt.
#define EXPECT(condition, message) do { if (!(condition)) expect_failed(message); } while (0)


	int64_t sum_subtree_values() {

		return sum_subtree_values(root);
	}


	int64_t sum_subtree_values(Node* node) {

		if (!node)
			return 0;

		int64_t result = node->value;
		if (node->left) {
			result = result + sum_subtree_values(node->left);
		}
		if (node->right) {
			result = result + sum_subtree_values(node->right);
		}

		return result;
	}

	string error_msg_sum_subtree_values(Node* node, int64_t result)
	{
		return "Expected subtree_value_sum of node ("
			+ to_string(node->key) + ", " + to_string(node->value) + ") "
			+ "is " + to_string(result)
			+ "; node->subtree_value_sum is " + to_string(node->subtree_value_sum);
	}

	string error_msg_expected_sum_after_update(
		int64_t sum_after_update,
		int64_t expected_sum_after_update)
	{
		return "Expected subtree_value_sum of root after range update is "
			+ to_string(expected_sum_after_update)
			+ "; subtree_value_sum of root after range update is "
			+ to_string(sum_after_update);
	}

	int get_count_nodes_in_range(int64_t left, int64_t right) {

		return get_count_nodes_in_range(left, right, root, 0);
	}

	int get_count_nodes_in_range(int64_t left, int64_t right, Node* node, int current_count) {

		int count = 0;
		if (node->left)
			count = count + get_count_nodes_in_range(left, right, node->left, count);

		if (node->right)
			count = count + get_count_nodes_in_range(left, right, node->right, count);

		if ((left <= node->key) && (node->key <= right))
			count = count + 1;

		return count;
	}

};
