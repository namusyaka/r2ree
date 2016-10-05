#ifndef r2ree_h
#define r2ree_h

#include <string>
#include <vector>
#include <tuple>

namespace r2ree {
  using std::string;
  using std::vector;
  using std::tuple;
  using std::pair;

  const char asterisk = '*';
  const char colon    = ':';
  const char slash    = '/';

  typedef pair<string, string> parameter;

  struct parameters {
    parameter *params;
    int size;
  };

  typedef tuple<bool, int, parameters, bool> parse_result;

  struct radix_tree_node {
    string path;
    string indices;
    vector<radix_tree_node*> children;
    int id;
    int type;
    int max_parameters;
    bool leaf;

    radix_tree_node() = default;
    explicit radix_tree_node(const string &path);
    ~radix_tree_node();

    radix_tree_node* insert_child(char index, radix_tree_node *child);
    radix_tree_node* get_child(char index);
    int get_index_position(char target);
  };

  int find(const string &str, char target, int start);

  class radix_tree {
    public:
      int cid;
      radix_tree();
      ~radix_tree();
      int insert(const string &path);
      parse_result get(const string &path);
    private:
      radix_tree_node *root;
  };
}; // namespace r2ree

#endif
