#include "model.hpp"
#include "view.hpp"

using namespace cwo;

/*
 * TODO:
 * - Overhaul overviewwindow: bolden out and color some "important" information
 */
int main()
{
  View v;
  if (!v.loadstate()) {
    std::cout << "Please setup a configuration as described on the github page"
              << std::endl;
    return 1;
  }
  v.run();
  return 0;
}
