#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>

#define STORE_FILE "../store/dumpFile"
#define NODE_TYPE Node<K, V>
#define SKIPLIST_TYPE SkipList<K, V, KeyComparator>

std::mutex mtx; // mutex for critical section
std::string delimiter = ":";
int count_for_test = 0;

// Class template to implement node
template <typename K, typename V>
class Node
{
public:
  Node() {}

  Node(K k, V v, int);

  ~Node();

  K get_key() const;

  V get_value() const;

  void set_value(V);

  // Linear array to hold pointers to next node of different level
  NODE_TYPE **forward;

  int node_level;

private:
  K key;
  V value;
};

template <typename K, typename V>
NODE_TYPE::Node(const K k, const V v, int level)
{
  this->key = k;
  this->value = v;
  this->node_level = level;

  // level + 1, because array index is from 0 - level
  this->forward = new NODE_TYPE *[level + 1];

  // Fill forward array with 0(NULL)
  memset(this->forward, 0, sizeof(NODE_TYPE *) * (level + 1));
};

template <typename K, typename V>
NODE_TYPE::~Node()
{
  // std::cout << "In the ~Node(), now delete []forward, the level is "
  //           << node_level << std::endl;
  delete[] forward;
};

template <typename K, typename V>
K NODE_TYPE::get_key() const
{
  return key;
};

template <typename K, typename V>
V NODE_TYPE::get_value() const
{
  return value;
};
template <typename K, typename V>
void NODE_TYPE::set_value(V value)
{
  this->value = value;
};

// Class template for Skip list
template <typename K, typename V, typename KeyComparator>
class SkipList
{
public:
  SkipList(int);
  ~SkipList();
  int get_random_level();
  NODE_TYPE *create_node(K, V, int);
  bool insert_element(K, V);
  void display_list();
  bool search_element(K);
  void delete_element(K);
  void dump_file();
  void load_file();
  int size();

private:
  void get_key_value_from_string(const std::string &str, std::string *key,
                                 std::string *value);
  bool is_valid_string(const std::string &str);

private:
  // Maximum level of the skip list
  int _max_level;

  // current level of skip list
  int _skip_list_level;

  // pointer to header node
  NODE_TYPE *_header;

  // file operator
  std::ofstream _file_writer;
  std::ifstream _file_reader;

  // skiplist current element count
  int _element_count;

  KeyComparator _comparator;
};

// create new node
template <typename K, typename V, typename KeyComparator>
NODE_TYPE *SKIPLIST_TYPE::create_node(const K k, const V v, int level)
{
  NODE_TYPE *node = new NODE_TYPE(k, v, level);
  return node;
}

// Insert given key and value in skip list
// return 1 means element exists
// return 0 means insert successfully
/*
                           +------------+
                           |  insert 50 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |                      insert +----+
level 3         1+-------->10+---------------> | 50 |          70       100
                                               |    |
                                               |    |
level 2         1          10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 1         1    4     10         30       | 50 |          70       100
                                               |    |
                                               |    |
level 0         1    4   9 10         30   40  | 50 |  60      70       100
                                               +----+

*/
template <typename K, typename V, typename KeyComparator>
bool SKIPLIST_TYPE::insert_element(const K key, const V value)
{
  const std::lock_guard<std::mutex> lock(mtx);
  NODE_TYPE *current = this->_header;

  // create update array and initialize it
  // update is array which put node that the node->forward[i] should be operated
  // later
  NODE_TYPE *update[_max_level + 1];
  memset(update, 0, sizeof(NODE_TYPE *) * (_max_level + 1));

  // start form highest level of skip list
  for (int i = _skip_list_level; i >= 0; i--)
  {
    while (current->forward[i] != nullptr &&
           _comparator(current->forward[i]->get_key(), key) < 0)
    {
      current = current->forward[i];
    }
    update[i] = current;
  }
  // count_for_test++;

  // reached level 0 and forward pointer to right node, which is desired to
  // insert key.
  current = current->forward[0];

  // if current node have key equal to searched key, we get it
  if (current != NULL && _comparator(current->get_key(), key) == 0)
  {
    std::cout << "key: " << key << ", exists" << std::endl;
    return 1;
  }

  // if current is NULL that means we have reached to end of the level
  // if current's key is not equal to key that means we have to insert node
  // between update[0] and current node
  if (current == NULL || current->get_key() != key)
  {
    // Generate a random level for node
    int random_level = get_random_level();

    // If random level is greater thar skip list's current level, initialize
    // update value with pointer to header
    if (random_level > _skip_list_level)
    {
      for (int i = _skip_list_level + 1; i < random_level + 1; i++)
      {
        update[i] = _header;
      }
      _skip_list_level = random_level;
    }

    // create new node with random level generated
    NODE_TYPE *inserted_node = create_node(key, value, random_level);

    // insert node
    for (int i = 0; i < random_level + 1; i++)
    {
      inserted_node->forward[i] = update[i]->forward[i];
      update[i]->forward[i] = inserted_node;
    }
    std::cout << "Successfully inserted key:" << key << ", value:" << value
              << std::endl;
    _element_count++;
  }
  return 0;
}

// Display skip list
template <typename K, typename V, typename KeyComparator>
void SKIPLIST_TYPE::display_list()
{
  std::cout << "\n<-----Skip List----->"
            << "\n";
  for (int i = 0; i <= _skip_list_level; i++)
  {
    NODE_TYPE *node = this->_header->forward[i];
    std::cout << "Level " << i << ": ";
    while (node != NULL)
    {
      std::cout << node->get_key() << ":" << node->get_value() << "; ";
      node = node->forward[i];
    }
    std::cout << std::endl;
  }
}

// Dump data in memory to file
template <typename K, typename V, typename KeyComparator>
void SKIPLIST_TYPE::dump_file()
{
  {
    std::cout << "dump_file-----------------" << std::endl;
    _file_writer.open(STORE_FILE);
    NODE_TYPE *node = this->_header->forward[0];

    while (node != NULL)
    {
      _file_writer << node->get_key() << ":" << node->get_value() << "\n";
      std::cout << node->get_key() << ":" << node->get_value() << ";\n";
      node = node->forward[0];
    }

    _file_writer.flush();
    _file_writer.close();
  }
}

// Load data from disk
template <typename K, typename V, typename KeyComparator>
void SKIPLIST_TYPE::load_file()
{
  {
    std::cout << "load_file-----------------" << std::endl;
    _file_reader.open(STORE_FILE);
    std::string line;
    std::string *key = new std::string();
    std::string *value = new std::string();
    while (getline(_file_reader, line))
    {
      get_key_value_from_string(line, key, value);
      if (key->empty() || value->empty())
      {
        continue;
      }
      insert_element(*key, *value);
      std::cout << "key:" << *key << "value:" << *value << std::endl;
    }
    delete key;
    delete value;
    _file_reader.close();
  }
}

// Get current SkipList size
template <typename K, typename V, typename KeyComparator>
int SKIPLIST_TYPE::size()
{
  return _element_count;
}

template <typename K, typename V, typename KeyComparator>
void SKIPLIST_TYPE::get_key_value_from_string(const std::string &str,
                                              std::string *key,
                                              std::string *value)
{
  if (!is_valid_string(str))
  {
    return;
  }
  *key = str.substr(0, str.find(delimiter));
  *value = str.substr(str.find(delimiter) + 1, str.length());
}

template <typename K, typename V, typename KeyComparator>
bool SKIPLIST_TYPE::is_valid_string(const std::string &str)
{
  if (str.empty())
  {
    return false;
  }
  if (str.find(delimiter) == std::string::npos)
  {
    return false;
  }
  return true;
}

// Delete element from skip list
/*
                                    +------------+
                                    |  Delete 50 |
                                    +------------+
level 4     +-->1+ 100
                 |
                 |
level 3         1+---------->10+======================>(50)+============>70 100
                               |
                               |
level 2         1            10+------->30+===========>(50)+============>70 100
                                          |
                                          |
level 1         1    4       10         30+===========>(50)+============>70 100
                                          |
                                          |
level 0         1    4   9   10         30+--->40+====>(50)+====>60      70 100
*/
template <typename K, typename V, typename KeyComparator>
void SKIPLIST_TYPE::delete_element(K key)
{
  const std::lock_guard<std::mutex> lock(mtx);
  NODE_TYPE *current = this->_header;
  NODE_TYPE *update[_max_level + 1];
  memset(update, 0, sizeof(NODE_TYPE *) * (_max_level + 1));

  // start from highest level of skip list
  for (int i = _skip_list_level; i >= 0; i--)
  {
    while (current->forward[i] != NULL &&
           _comparator(current->forward[i]->get_key(), key) < 0)
    {
      current = current->forward[i];
    }
    update[i] = current;
  }

  current = current->forward[0];
  if (current != NULL && _comparator(current->get_key(), key) == 0)
  {
    // From current level down to 0, change the pointer
    for (int i = current->node_level; i >= 0; --i)
    {
      update[i]->forward[i] = current->forward[i];
    }

    // Remove levels which have no elements
    while (_skip_list_level > 0 && _header->forward[_skip_list_level] == 0)
    {
      _skip_list_level--;
    }

    std::cout << "Successfully deleted key " << key << std::endl;

    delete current;
    _element_count--;
  }
  return;
}

// Search for element in skip list
/*
                           +------------+
                           |  select 60 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |
level 3         1+-------->10+------------------>50+           70       100
                                                   |
                                                   |
level 2         1          10         30         50|           70       100
                                                   |
                                                   |
level 1         1    4     10         30         50|           70       100
                                                   |
                                                   |
level 0         1    4   9 10         30   40    50+-->60      70       100
*/
template <typename K, typename V, typename KeyComparator>
bool SKIPLIST_TYPE::search_element(K key)
{
  NODE_TYPE *current = _header;

  // start from highest level of skip list
  for (int i = _skip_list_level; i >= 0; i--)
  {
    while (current->forward[i] && _comparator(current->forward[i]->get_key(), key) < 0)
    {
      current = current->forward[i];
    }
  }

  // reached level 0 and advance pointer to right node, which we search
  current = current->forward[0];

  // if current node have key equal to searched key, we get it
  if (current && _comparator(current->get_key(), key) == 0)
  {
    std::cout << "Found key: " << key << ", value: " << current->get_value()
              << std::endl;
    return true;
  }

  std::cout << "Not Found Key:" << key << std::endl;
  return false;
}

// construct skip list
template <typename K, typename V, typename KeyComparator>
SKIPLIST_TYPE::SkipList(int max_level)
{
  this->_max_level = max_level;
  this->_skip_list_level = 0;
  this->_element_count = 0;

  // create header node and initialize key and value to null
  K k;
  V v;
  this->_header = new NODE_TYPE(k, v, _max_level);
};

template <typename K, typename V, typename KeyComparator>
SKIPLIST_TYPE::~SkipList()
{
  if (_file_writer.is_open())
  {
    _file_writer.close();
  }
  if (_file_reader.is_open())
  {
    _file_reader.close();
  }
  // Remember to traverse and delete
  // Delete all node based on level 0
  NODE_TYPE *current = _header->forward[0]; // Start from next node of _header
  while (current)
  {
    NODE_TYPE *temp = current->forward[0];
    delete current;
    current = temp;
  }

  // std::cout << "In the ~SkipList(), now delete []_header, the level is "
  //           << _skip_list_level << std::endl;
  delete _header;
}

template <typename K, typename V, typename KeyComparator>
int SKIPLIST_TYPE::get_random_level()
{
  int k = 0;
  while (rand() & 1)
  {
    k++;
  }
  k = (k < _max_level) ? k : _max_level;
  return k;
};
// vim: et tw=100 ts=4 sw=4 cc=120
