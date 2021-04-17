#ifndef INCLUDE_VIEW_HPP
#define INCLUDE_VIEW_HPP
#include "json.hpp"
#include "model.hpp"
#include "observable.hpp"
#include "observer.hpp"
#include <filesystem>
#include <fstream>
#include <ncurses.h>
#include <sstream>
#include <stdlib.h>
#include <string>
namespace cwo {

/*
 * Class responsible for displaying gathered information for
 * CoinWalletOverview
 */
class View : public Observer {
public:
  View();

  ~View();

  /*
   * Implemented virtual function of Observer class to trigger visual
   * update
   */
  void update();

  /*
   * Check if terminalwindow was resized
   */
  bool checkresize();

  /*
   * Load $USER/.config/cwo/state.json file to retrieve wallet information
   */
  bool loadstate();

  /*
   * Register a model to use with this application
   */
  void registermodel(Model *m);

  /*
   * Displays walletinformation for alle registered wallets
   * @param win : Window in which to draw
   * @param h : height of available window
   * @param w : widht of available window
   */
  void displaywalletinfo(WINDOW *win, int h, int w);

  /*
   * How are graphwindows expanded if space below or to the side is
   * available
   */
  void hgraphexpansion();
  void vgraphexpansion();

  /*
   * Main run function for this class
   */
  void run();

private:
  int _basex, _basey, _ux, _uy;
  Model *_m;
  WINDOW *overviewwin, *coinviewwin, *debugwin;
  int _state;
  bool _wait;
  bool _running;
  bool _szchanged;
  bool _updated;
  nlohmann::json _jdata;
  std::thread *_worker;
  std::vector<std::pair<WINDOW *, WINDOW *>> _graphs;

  int MINGRAPHWIDTH = 50;
  int LOFFSETIG = 8;
  int BOFFESTIG = 0;
  ALIGNMENT GRAPHEXPANSION = HORIZONTAL;
  bool FILL = true;
  DATARES DATARESOLUTION = M;
  const char *GRAPH_SYMBOL = "*";

  /*
   * Returns a ascii representation of a cryptoasset if available
   */
  std::array<std::string, 8> asciicrypto(CRYPTOTYPE t);

  /*
   * Creates a simple window with a label and a box surrounding it
   */
  WINDOW *createbutton(int h, int w, int y, int x, const char *label);

  /*
   * Create graph windows for each registered cryptocurrency
   */
  void creategraphs();

  /*
   * Draw points with provided data v into graphwindow g with range
   * specifying min and max values
   */
  void drawgraph(std::pair<WINDOW *, WINDOW *> g,
                 const std::vector<Statistic> &v,
                 const std::array<double, 2> range);

  /*
   * Draw metainformation for cryptoassets
   */
  void drawmetainfo();

  /*
   * Draw coordinate system for graphwindow g
   */
  void drawcoord(std::pair<WINDOW *, WINDOW *> g);

  /*
   * Draw ascii representation of crypto asset
   */
  void drawascii(WINDOW *w, CRYPTOTYPE t);

  /*
   * Wrapperfunction to draw coordinate system and graphinformation
   * for each registered cryptoasset
   */
  void displaygraphinfo();

  /*
   * Handle main menu windows after resize event happened
   */
  void adjustmainwins();

  /*
   * Handle graphwindows after resize event happened
   */
  void adjustgraphs();

  /*
   * Graphsymbol
   */
  const char *choosegraphsymbol(double oldp, double curp, double nexp);

  /*
   * Bitmask has 1 where a window should be and 0 where space is left
   * General layout will be a multiple of the most windows fitting in one
   * line e.g.:
   *          v.size() = 4   | v.size() = 2  | v.size = 3
   *          | 1 | 1 | 1 |  | | 1 | 1 |     | | 1 | 1 | 1 |
   *          | 1 | 0 | 0 |  |               |
   * @returns number of graphs per line
   */
  int setbitmask(std::vector<uint8_t> *v, int maxy, int maxx);

  /*
   * Draw main menu
   */
  void mainmenu();

  /*
   * Auxiliary function for deciding width/height of a graph-window
   * and y and x position
   * @returns bool -> true if a window is to be created
   */
  bool calcgraphdimensions(std::vector<uint8_t> *v, int gpl, int &houtergraph,
                           int &woutergraph, int hratio, int wratio,
                           uint32_t ln, int32_t j, int &y, int &x);

  /*
   * Auxiliary function to decide on which currency symbol to use
   */
  const char *currencysymbol(CURRENCY c);

  /*
   * Auxiliary function for debugwin
   */
  void debugmessage(const char *);

};     // class View
};     // namespace cwo
#endif // INCLUDE_VIEW_HPP
