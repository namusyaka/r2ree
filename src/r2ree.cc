/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 namusyaka, shiyanhui
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "r2ree.hh"

namespace r2ree {
  int find(const string &str, char target, int start) {
    auto i = str.find(target, start);
    return i == -1 ? str.size() : i;
  }

  radix_tree_node::radix_tree_node(const string &path) {
    this->id   = -1;
    this->leaf = false;
    this->path = path;
  }

  radix_tree_node::~radix_tree_node() {
    for (auto &c : this->children)
      delete c;
  }

  radix_tree_node* radix_tree_node::insert_child(char index, radix_tree_node *child) {
    auto i = this->get_index_position(index);
    this->indices.insert(this->indices.begin() + i, index);
    this->children.insert(this->children.begin() + i, child);
    return child;
  }

  radix_tree_node* radix_tree_node::get_child(char index) {
    auto i = this->get_index_position(index);
    return this->indices[i] != index ? nullptr : this->children[i];
  }

  int radix_tree_node::get_index_position(char target) {
    int low = 0, high = this->indices.size(), mid;
    while (low < high) {
      mid = low + ((high - low) >> 1);
      if (this->indices[mid] < target)
        low = mid + 1;
      else
        high = mid;
    }
    return low;
  }

  radix_tree::radix_tree() {
    this->cid = -1;
    this->root = new radix_tree_node();
  }

  radix_tree::~radix_tree() {
    delete this->root;
  }

  int radix_tree::insert(const string &path) {
    auto root = this->root;
    int i = 0, n = path.size(), param_count = 0, code = 0, cid = ++this->cid;

    while (i < n) {
      if (!root->indices.empty() && (root->indices[0] == asterisk ||
        path[i] == asterisk ||
        (path[i] != colon && root->indices[0] == colon) ||
        (path[i] == colon && root->indices[0] != colon) ||
        (path[i] == colon && root->indices[0] == colon && path.substr(
          i + 1, find(path, slash, i) - i - 1) != root->children[0]->path))) {
        code = -1;
        break;
      }

      auto child = root->get_child(path[i]);
      if (!child) {
        auto p = find(path, colon, i);
        if (p == n) {
          p = find(path, asterisk, i);

          if (path[i] == slash) {
            root = root->insert_child(slash, new radix_tree_node(path.substr(i, 1)));

            if (i == 0 && path[i + 1] == 0)
              break;

            ++i;
          }

          root = root->insert_child(path[i], new radix_tree_node(path.substr(i, p - i)));

          if (p < n) {
            root = root->insert_child(asterisk, new radix_tree_node(path.substr(p + 1)));
            ++param_count;
          }

          break;
        }

        root = root->insert_child(path[i], new radix_tree_node(path.substr(i, p - i)));
        i = find(path, slash, p);

        root = root->insert_child(colon, new radix_tree_node(path.substr(p + 1, i - p - 1)));
        ++param_count;

        if (i == n)
          break;
      } else {
        root = child;

        if (path[i] == colon) {
          ++param_count;
          i += root->path.size() + 1;

          if (i == n)
            break;
        } else {
          auto j = 0UL, m = root->path.size();

          for (; i < n && j < m && path[i] == root->path[j]; ++i, ++j)
            continue;

          if (j < m) {
            auto child = new radix_tree_node(root->path.substr(j));

            child->indices = root->indices;
            child->children = root->children;

            root->path = root->path.substr(0, j);
            root->indices = child->path[0];
            root->children = {child};
          }

          if (i == n)
            break;
        }
      }
    }

    if (param_count > this->root->max_parameters)
      this->root->max_parameters = param_count;

    root->id = cid;
    root->leaf = true;
    return code;
  }

  parse_result radix_tree::get(const string &path) {
    parameters params = parameters{ new parameter[root->max_parameters], 0};

    auto root = this->root;
    int i = 0, n = path.size(), p;

    while (i < n) {
      if (root->indices.empty()) {
        delete[] params.params;
        return parse_result();
      }

      if (root->indices[0] == colon) {
        root = root->children[0];
        p = find(path, slash, i);
        params.params[params.size++] = parameter{ root->path, path.substr(i, p - i) };
        i = p;
      } else if (root->indices[0] == asterisk) {
        root = root->children[0];
        params.params[params.size++] = parameter{ root->path, path.substr(i) };
        break;
      } else {
        root = root->get_child(path[i]);
        if (!root || path.substr(i, root->path.size()) != root->path) {
          delete[] params.params;
          return parse_result();
        }
        i += root->path.size();
      }
    }
    return parse_result{true, root->id, params, root->leaf};
  }
}  // namespace r2ree
