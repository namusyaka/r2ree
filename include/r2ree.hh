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

  struct request {
    string path;
    string method;
    parameters params;

    request(const string &path, const string &method);
  };

  typedef void (*handle_func)(request *req);
  typedef tuple<bool, int, handle_func, parameters, bool> parse_result;

  struct handler {
    string method;
    handle_func handler;
  };

  struct radix_tree_node {
    string path;
    vector<handler> handlers;
    string indices;
    vector<radix_tree_node*> children;
    int id;
    int type;
    int max_parameters;
    bool leaf;

    radix_tree_node() = default;
    explicit radix_tree_node(const string &path);
    ~radix_tree_node();

    handle_func get_handler(const string &method);
    int add_handler(handle_func handler, const vector<string> &methods);

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
      int insert(const string &path, handle_func handler, const vector<string> &method);
      parse_result get(const string &path, const string &method);
    private:
      radix_tree_node *root;
  };
}; // namespace r2ree

#endif
