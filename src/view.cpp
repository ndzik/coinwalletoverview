#include "view.hpp"
#include <chrono>
#include <curses.h>
#include <sstream>
#include <thread>
#include <vector>

namespace cwo {
  /********************************* CTORS ***********************************/
  View::View()
    : _state(0), _wait(true), _running(true), _szchanged(true),
    _updated(true)
  { }

  View::~View()
  {
    _m->stop();
    _worker->join();
    delete _m;
  }

  /********************************* PUBLIC **********************************/

  void View::update()
  {
    _updated = true;
  }

  void View::registermodel(Model *m)
  {
    _m = m;
  }

  void View::run()
  {
    /* start model */
    _worker = new std::thread([this] (View *v) {
        Model *m = new Model();
        m->addobs(v);
        v->registermodel(m);
        m->token(_jdata["blockcypher"]["token"]);
        for (auto c : _jdata["wallets"])
        m->registerwallet(STOC.at(c["crypto"]), c["address"]);
        for (auto c : _jdata["investments"])
        m->investment(STOC.at(c["crypto"]), c["amount"]);
        m->updateinterval(_jdata["updateinterval"]);
        m->run();
        }, this);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    /* ncurses init */
    initscr();
    getmaxyx(stdscr, _uy, _ux);
    _basex = _ux;
    _basey = _uy;
    nodelay(stdscr, true);
    noecho();
    nocbreak();
    curs_set(0);

    /* initialize colors */
    start_color();
    init_pair(1, COLOR_YELLOW, 0);
    init_pair(2, COLOR_CYAN, 0);
    init_pair(3, COLOR_GREEN, 0);
    init_pair(4, COLOR_RED, 0);

    /* create windows */
    overviewwin = newwin((_uy*0.2), _ux, _uy-(_uy*0.2), 0);
    coinviewwin = newwin((_uy*0.8), _ux, 0, 0);
//    debugwin = newwin(3, 30, _uy-(_uy*0.2)-4, _ux*0.8+1);

    /* draw box around coinviewwindow */
    box(coinviewwin, 0, 0);
    wnoutrefresh(coinviewwin);

    /* draw box around walletoverview */
    box(overviewwin, 0, 0);
    wnoutrefresh(overviewwin);

    /* draw box around debugwin */
//    box(debugwin, 0, 0);
//    wnoutrefresh(debugwin);


    creategraphs();
    _updated = true;
    uint64_t counter = 0;
    while (_running) {
      ++counter;
      /* check if terminal was resized */
      getmaxyx(stdscr, _uy, _ux);
      if (checkresize()) {
        _basex = _ux;
        _basey = _uy;
        _szchanged = true;
      }

      /* keyboard input */
      switch (getch()) {
        case 'q' :
          _running = false;
          break;
        case 'v' :
          GRAPHEXPANSION = VERTICAL;
          _szchanged = true;
          break;
        case 'h' :
          GRAPHEXPANSION = HORIZONTAL;
          _szchanged = true;
          break;
        case 'f':
          FILL = !FILL;
          _szchanged = true;
          break;
        case '1':
          DATARESOLUTION = M;
          _szchanged = true;
          break;
        case '2':
          DATARESOLUTION = H;
          _szchanged = true;
          break;
        case '3':
          DATARESOLUTION = D;
          _szchanged = true;
          break;
        default:
          break;
      }

      /* main rendering */
      switch (_state) {
        case 0 :
          mainmenu();
          break;
        default:
          break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    /* delete windows when done */
    for (auto &a : _graphs) {
      delwin(a.first);
      delwin(a.second);
    }

    delwin(overviewwin);
    delwin(coinviewwin);
//    delwin(debugwin);
    endwin();
  }

  void View::mainmenu()
  {
    if (_szchanged) {
      wclear(overviewwin);
      wclear(coinviewwin);
      for (auto &a : _graphs) {
        wclear(a.first);
        wclear(a.second);
        wrefresh(a.first);
        wrefresh(a.second);
      }
      wrefresh(overviewwin);
      wrefresh(coinviewwin);
      _szchanged = false;
      _updated = true;
      adjustmainwins();
      adjustgraphs();
    }

    /* update contents of windows */
    if (_updated) {
      _updated = false;
      for (auto &a : _graphs) {
        werase(a.first);
        werase(a.second);
      }
      /* draw graphs */
      displaygraphinfo();
      //debugmessage("entering drawmetainfo");
      drawmetainfo();
      wnoutrefresh(coinviewwin);

      /* draw wallet information */
      int x, y;
      getmaxyx(overviewwin, y, x);
      //debugmessage("entering displaywalletinfo");
      displaywalletinfo(overviewwin, y-1, x);
      //debugmessage("done loop");
      wnoutrefresh(overviewwin);

      /* draw updated content to screen */
      doupdate();
    }
  }

  void View::hgraphexpansion()
  {
    GRAPHEXPANSION = HORIZONTAL;
  }

  void View::vgraphexpansion()
  {
    GRAPHEXPANSION = VERTICAL;
  }

  /************************ PRIVATE ******************************************/

  std::array<std::string, 8> View::asciicrypto(CRYPTOTYPE t)
  {
    std::array<std::string, 8> r;
    switch(t) {
      case ETH:
        r ={
          " ******** ********** **      **",
          "/**///// /////**/// /**     /**",
          "/**          /**    /**     /**",
          "/*******     /**    /**********",
          "/**////      /**    /**//////**",
          "/**          /**    /**     /**",
          "/********    /**    /**     /**",
          "////////     //     //      // ",
        };
        break;
      case BTC:
        r = {
          " ******   **********   ****** ",
          "/*////** /////**///   **////**",
          "/*   /**     /**     **    // ",
          "/******      /**    /**       ",
          "/*//// **    /**    /**       ",
          "/*    /**    /**    //**    **",
          "/*******     /**     //****** ",
          "///////      //       //////  ",
        };
        break;
      case VET:
        r = {
          " **      ** ******** **********",
          "/**     /**/**///// /////**/// ",
          "/**     /**/**          /**    ",
          "//**    ** /*******     /**    ",
          " //**  **  /**////      /**    ",
          "  //****   /**          /**    ",
          "   //**    /********    /**    ",
          "    //     ////////     //     ",
        };
        break;
      case CKB:
        r = {
          "   ******   **    **  ******  ",
          "  **////** /**   **  /*////** ",
          " **    //  /**  **   /*   /** ",
          "/**        /*****    /******  ",
          "/**        /**//**   /*//// **",
          "//**    ** /** //**  /*    /**",
          " //******  /**  //** /******* ",
          "  //////   ///   /// ///////  ",
        };
        break;
      default:
        r = {CRTOS.at(t)};
        break;
    }
    return r;
  }

  void View::adjustmainwins()
  {
    werase(overviewwin);
    wborder(overviewwin, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wresize(overviewwin, _uy*0.2, _ux);
    mvwin(overviewwin, _uy-(_uy*0.2), 0);
    box(overviewwin, 0, 0);
    wnoutrefresh(overviewwin);
    werase(coinviewwin);
    wborder(coinviewwin, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wresize(coinviewwin, _uy*0.8, _ux);
    mvwin(coinviewwin, 0, 0);
    box(coinviewwin, 0, 0);
    wnoutrefresh(coinviewwin);
  }

  void View::adjustgraphs()
  {
    int locy,locx,py,px,begy,begx;
    getmaxyx(coinviewwin, py, px);
    px = px*0.8;
    py = py-2; // substract win border top and bottom
    std::vector<uint8_t> bitmask(_graphs.size());
    int gpl = setbitmask(&bitmask, py, px);
    int hratio =  py/(bitmask.size()/gpl);

    int wratio = px/gpl;
    int houtergraph, woutergraph;

    /* for line... */
    for (uint32_t ln=0; ln<bitmask.size()/gpl; ++ln) {
      /* ...for col... */
      for (int32_t j=0; j<gpl; ++j) {
        if (!calcgraphdimensions(&bitmask, gpl, houtergraph, woutergraph,
              hratio, wratio, ln, j, begy, begx)) break;

        werase(_graphs[ln*gpl+j].first);
        wborder(_graphs[ln*gpl+j].first,
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        werase(_graphs[ln*gpl+j].second);
        wresize(_graphs[ln*gpl+j].first, houtergraph, woutergraph);
        mvwin(_graphs[ln*gpl+j].first, begy, begx);
        getmaxyx(_graphs[ln*gpl+j].first, locy, locx);
        getbegyx(_graphs[ln*gpl+j].first, begy, begx);
        wresize(_graphs[ln*gpl+j].second, locy-2, locx-2);
        mvwin(_graphs[ln*gpl+j].second, begy+1, begx+1);
        box(_graphs[ln*gpl+j].first, 0, 0);
        wnoutrefresh(_graphs[ln*gpl+j].first);
        wnoutrefresh(_graphs[ln*gpl+j].second);
      }
    }
  }

  bool View::calcgraphdimensions(std::vector<uint8_t> *bitmask, int gpl,
      int &houtergraph, int &woutergraph, int hratio, int wratio,
      uint32_t ln, int32_t j, int &y, int &x)
  {
    uint64_t maxln = bitmask->size()/gpl;
    y = 1+ln*hratio; // offset each line by 1 because of winborder
    x = 1+j*wratio; // offset each col by 1 because of winborder
    houtergraph = hratio;
    woutergraph = wratio;
    /* ...if 0 in bitmask then no window is to be created... */
    if (bitmask->at(ln*gpl+j) == 0) return false;
    /* ...else create two new windows... */
    if (GRAPHEXPANSION == HORIZONTAL) { /* horizontal expansion */
      if ((j<gpl-1) && (bitmask->at(ln*gpl+j+1) != 1)) {
        for (int k=j; k<gpl-1; ++k) {
          woutergraph += wratio;
        }
      }
    } else { /* vertical expansion */
      if ((ln<(maxln-1) && (bitmask->at((ln+1)*gpl+j) != 1))) {
        houtergraph += hratio;
      }
    }
    return true;
  }

  void View::displaywalletinfo(WINDOW *win, int h, int w)
  {
    /* Prevent crash due to (idx%h==0) leading to FPE */
    if (h<=0) return;
////    debugmessage("getting wallets");
    auto wallet = _m->wallets();
////    debugmessage("got wallets");
    uint32_t numofwallets = wallet.size();
    uint32_t numofrows = numofwallets/h + 1;
    uint32_t widthofrow = w/numofrows;
    uint32_t row = 0;
    std::string addr;
    int ypos = 0;
    for(uint32_t idx = 1; (idx<=numofwallets); ++idx) {
      if (idx%h==0)
        ++row;
      addr = wallet[idx-1]->address();
      ypos = (idx%h == 0) ? 1 : idx%h;
      CRYPTOTYPE t = wallet[idx-1]->crypto();
      wattron(win, A_BOLD);
      /* Print crypto asset name */
      mvwprintw(win, ypos, row*widthofrow+1, "%s",
          CRTOS.at(t).c_str());
      wattroff(win, A_BOLD);
      /* Print addr of wallet */
      mvwprintw(win, ypos, row*widthofrow+1+3, " : %s",
          addr.c_str());
      wattron(win, A_BOLD);
      /* Print balance of wallet */
      mvwprintw(win, ypos, row*widthofrow+1+3+46, " %5.6f",
          wallet[idx-1]->balance());
      wattroff(win, A_BOLD);
      /* Print estimated value of wallet in specified currency */
      mvwprintw(win, ypos, row*widthofrow+1+3+55, " est. value: ");
      wattron(win, A_BOLD);
      mvwprintw(win, ypos, row*widthofrow+1+3+54+14, "%6.2f %s",
          wallet[idx-1]->balance()*wallet[idx-1]->value(),
          currencysymbol(wallet[idx-1]->currency())
          );
      wattroff(win, A_BOLD);
    }
  }

  WINDOW* View::createbutton(int h, int w, int y, int x, const char* label)
  {
    WINDOW *btnwin = newwin(h, w, y, x);
    box(btnwin, 0, 0);
    mvwprintw(btnwin, 1, 1, label);
    wrefresh(btnwin);
    return btnwin;
  }

  void View::creategraphs()
  {
    int locy,locx,py,px,begy,begx;
    getmaxyx(coinviewwin, py, px);
    px = px*0.8;
    std::vector<CRYPTOTYPE> v = _m->regcryptos();

    /* Calculate how many lines and cols we need */
    std::vector<uint8_t> bitmask(v.size());
    int gpl = setbitmask(&bitmask, py, px);

    /* Create graphs with appropriate dimensions according to bitmask
     * If a 1 has a 0 beneath/next to it, the window is supposed to be extended
     * to fit the rest of the column/line */
    int hratio = py/(bitmask.size()/gpl);
    int wratio = px/gpl;
    int houtergraph, woutergraph;

    /* for line... */
    for (uint32_t ln=0; ln<bitmask.size()/gpl; ++ln) {
      /* ...for col... */
      for (int32_t j=0; j<gpl; ++j) {
        if (!calcgraphdimensions(&bitmask, gpl, houtergraph, woutergraph,
              hratio, wratio, ln, j, begy, begx)) break;
        WINDOW *graphout = newwin(houtergraph-2, woutergraph,
            1+ln*hratio, 1+j*wratio);
        getmaxyx(graphout, locy, locx);
        getbegyx(graphout, begy, begx);
        WINDOW *graphin = newwin(locy-2, locx-2, begy+1, begx+1);
        _graphs.push_back(std::make_pair(graphout, graphin));
      }
    }
  }

  int View::setbitmask(std::vector<uint8_t> *v, int maxy, int maxx)
  {
    int numofgraphs = v->size();
    /* how many graphwindows fit in one line? */
    int graphsperline = maxx/MINGRAPHWIDTH;
    graphsperline = (graphsperline > numofgraphs) ? numofgraphs : graphsperline;
    if (graphsperline == 0)
      graphsperline = 1;
    /* how many lines do we need? Minimum of one line */
    int numoflines = numofgraphs/graphsperline + 1;
    /* resize vector to fit correct amount of potential graphs if necessary */
    /* fill last line if necessary */
    if (numofgraphs%graphsperline!=0)
      v->resize(numoflines * graphsperline);
    /* write 1's where a window is supposed to be, else stay 0 */
    for (int i=0; i<numofgraphs; ++i)
      v->at(i) = 1;
    return graphsperline;
  }

  void View::drawgraph(std::pair<WINDOW*, WINDOW*> g,
      const std::vector<Statistic> &v, const std::array<double, 2> range)
  {
    drawcoord(std::make_pair(g.first, g.second));
    if (v.size() == 0) {
      wnoutrefresh(g.first);
      wnoutrefresh(g.second);
      return;
    }
    double maxx, maxy, cnt=0;
    double diff = range[1] - range[0];
    uint64_t i = v.size()-1;
    getmaxyx(g.second, maxy, maxx);
    maxy = maxy-3;
    maxx = maxx-1;
    /* draw min and max value to y-axis */
    const char *strformat = v[i].price > 1 ? "%4.3f" : "%1.6f";
    mvwprintw(g.second, 0, 0, strformat, range[1]);
    mvwprintw(g.second, maxy, 0, strformat, range[0]);
    for (int x=maxx; x>LOFFSETIG && i>=0 && cnt<v.size(); ++cnt, --x, --i) {
      int y = maxy*(v[i].price-range[0])/diff;
      y = maxy-y; /* invert axis */
      if (i!=0 && i!=v.size()-1) {
        GRAPH_SYMBOL = choosegraphsymbol(v[i-1].price,
            v[i].price, v[i+1].price);
      } else {
        GRAPH_SYMBOL = "*";
      }
      wattron(g.second, COLOR_PAIR(2));
      mvwprintw(g.second, y, x, GRAPH_SYMBOL);
      if (FILL) {
        wmove(g.second, y+1, x);
        wvline(g.second, ACS_BLOCK, maxy-y);
      }
      wattroff(g.second, COLOR_PAIR(2));
      strformat = v[i].price > 1 ? "%4.3f" : "%1.6f";
      mvwprintw(g.second, y, 0, strformat, v[i].price);
    }
    std::string s;
    switch (DATARESOLUTION) {
      case M:
        s = "5 Min / Point";
        break;
      case H:
        s = "1 Hour / Point";
        break;
      case D:
        s = "1 Day / Point";
        break;
      default:
        s = "5 Min / Point";
    }
    mvwprintw(g.second, maxy+2, maxx-(1+s.length()), s.c_str(), v[i].price);
    wnoutrefresh(g.first);
    wnoutrefresh(g.second);
  }

  void View::drawcoord(std::pair<WINDOW*, WINDOW*> g)
  {
    int y, x;
    getmaxyx(g.second, y, x);
    box(g.first, 0, 0);
    wmove(g.second, 0, LOFFSETIG);
    wvline(g.second, ACS_VLINE, y-1);
    wmove(g.second, y-2, LOFFSETIG);
    wvline(g.second, ACS_LLCORNER, 1);
    wmove(g.second, y-2, LOFFSETIG+1);
    whline(g.second, ACS_HLINE, x-1);
  }

  void View::displaygraphinfo()
  {
    std::vector<CRYPTOTYPE> v = _m->regcryptos();
    std::array<double, 2> minmax;
    int x,y;
    for (uint32_t i=0; i<v.size(); ++i) {
      drawcoord(_graphs[i]);
      getmaxyx(_graphs[i].second, y, x);
      std::vector<Statistic> s;
      //debugmessage("entering selectdatabyX");
      switch (DATARESOLUTION) {
        case M:
          _m->selectdatabymin(v[i], &s, x+4);
            minmax = _m->minmaxmin(v[i], x+4);
          break;
        case H:
          _m->selectdatabyhour(v[i], &s, x+4);
          minmax = _m->minmaxhour(v[i], x+4);
          break;
        case D:
          _m->selectdatabyday(v[i], &s, x+4);
          minmax = _m->minmaxday(v[i], x+4);
          break;
        default:
          _m->selectdatabymin(v[i], &s, x+4);
          minmax = _m->minmaxmin(v[i], x+4);
      }
      //debugmessage("entering drawgraph");
      drawgraph(_graphs[i], s, minmax);
      std::array<std::string, 8> title = asciicrypto(v[i]);
      wattron(_graphs[i].second, COLOR_PAIR(1));
      for (uint8_t j=0; j<title.size(); ++j) {
        for (uint8_t k=0; k<title[j].size(); ++k) {
          wmove(_graphs[i].second, y-10+j, LOFFSETIG+1+k);
          if (title[j][k] != ' ')
            waddch(_graphs[i].second, title[j][k] | A_UNDERLINE);
        }
      }
      wattroff(_graphs[i].second, COLOR_PAIR(1));
      wnoutrefresh(_graphs[i].first);
      wnoutrefresh(_graphs[i].second);
    }
  }

  bool View::checkresize()
  {
    return ((_ux != _basex) || (_uy != _basey));
  }

  bool View::loadstate()
  {
    /* Get home directory and construct filepath */
    std::string line;
    std::stringstream pofile;
    std::filesystem::path p;
    const char* homedir = getenv("HOME");
    pofile << homedir << "/.config/cwo/state.json";
    p = std::filesystem::path(pofile.str());
    if (!std::filesystem::exists(p)) {
      return false;
    }
    std::ifstream data(pofile.str().c_str());
    pofile.str(std::string());
    if (data.is_open()) {
      while (getline(data, line)) {
        pofile << line << '\n';
      }
      data.close();
      _jdata = nlohmann::json::parse(pofile);
    }
    return true;
  }

  void View::drawmetainfo()
  {
    int x, y, col, height;
    double change;
    bool newline;
    const char* currencysym;
    CURRENCY currency;
    std::vector<CRYPTOTYPE> c = _m->regcryptos();
////    debugmessage("getting walletmap");
    std::multimap<CRYPTOTYPE, Wallet*> w = _m->walletmap();
////    debugmessage("got walletmap");
    getmaxyx(coinviewwin, y, x);
    wmove(coinviewwin, 1, x*0.8+1);
    whline(coinviewwin, ACS_HLINE, x-x*0.8-2);
    /* decide if a newline is needed to print information */
    if ((x-(x*0.8+2) > 32)) {
      newline = false;
      height = 6;
    } else {
      newline = true;
      height = 9;
    }
    for (uint32_t i=0; i<c.size(); ++i) {
      wattron(coinviewwin, A_BOLD);
      mvwprintw(coinviewwin, 2 + i*height, x*0.8+1, "Cryptoasset: %s",
          CRTOS.at(c[i]).c_str());
      wattroff(coinviewwin, A_BOLD);
      wattron(coinviewwin, COLOR_PAIR(1));
      currency = w.find(c[i])->second->currency();
      currencysym = currencysymbol(currency);
      /* split print statements if width of metainfo is not wide enough */
      mvwprintw(coinviewwin, 3 + i*height, x*0.8+1, "Price: %f %s",
          w.find(c[i])->second->value(), currencysym);
      wattroff(coinviewwin, COLOR_PAIR(1));
      change = w.find(c[i])->second->onehourchange();
      col = (change >= 0) ? 3 : 4;
      wattron(coinviewwin, COLOR_PAIR(col));
      if (newline) {
        mvwprintw(coinviewwin, 4 + i*height, x*0.8+1, "1-hour change (%):");
        wattron(coinviewwin, A_BOLD);
        mvwprintw(coinviewwin, 5 + i*height, x*0.8+1, "    %+5.6f", change);
        wattroff(coinviewwin, A_BOLD);
      } else {
        mvwprintw(coinviewwin, 4 + i*height, x*0.8+1, "1-hour change (%):");
        wattron(coinviewwin, A_BOLD);
        mvwprintw(coinviewwin, 4 + i*height, x*0.8+1+18, " %+5.6f", change);
        wattroff(coinviewwin, A_BOLD);
      }
      wattroff(coinviewwin, COLOR_PAIR(col));
      change = w.find(c[i])->second->onedaychange();
      col = (change >= 0) ? 3 : 4;
      wattron(coinviewwin, COLOR_PAIR(col));
      if (newline) {
        mvwprintw(coinviewwin, 6 + i*height, x*0.8+1, "1-day  change (%):");
        wattron(coinviewwin, A_BOLD);
        mvwprintw(coinviewwin, 7 + i*height, x*0.8+1, "    %+5.6f", change);
        wattroff(coinviewwin, A_BOLD);
      } else {
        mvwprintw(coinviewwin, 5 + i*height, x*0.8+1, "1-day  change (%):");
        wattron(coinviewwin, A_BOLD);
        mvwprintw(coinviewwin, 5 + i*height, x*0.8+1+18, " %+5.6f", change);
        wattroff(coinviewwin, A_BOLD);
      }
      wattroff(coinviewwin, COLOR_PAIR(col));
      change = w.find(c[i])->second->oneweekchange();
      col = (change >= 0) ? 3 : 4;
      wattron(coinviewwin, COLOR_PAIR(col));
      if (newline) {
        mvwprintw(coinviewwin, 8 + i*height, x*0.8+1, "1-week change (%):");
        wattron(coinviewwin, A_BOLD);
        mvwprintw(coinviewwin, 9 + i*height, x*0.8+1, "    %+5.6f", change);
        wattroff(coinviewwin, A_BOLD);
      } else {
        mvwprintw(coinviewwin, 6 + i*height, x*0.8+1, "1-week change (%):");
        wattron(coinviewwin, A_BOLD);
        mvwprintw(coinviewwin, 6 + i*height, x*0.8+1+18, " %+5.6f", change);
        wattroff(coinviewwin, A_BOLD);
      }
      wattroff(coinviewwin, COLOR_PAIR(col));
      wmove(coinviewwin, height+1+i*height, x*0.8+1);
      whline(coinviewwin, ACS_HLINE, x-x*0.8-2);
    }
    /* Print last time updated stamp */
    auto time = std::chrono::system_clock::now();
    auto real_time = std::chrono::system_clock::to_time_t(time);
    wattron(coinviewwin, A_BOLD);
    mvwprintw(coinviewwin, y-3, x*0.8+1, "Last Update:");
    mvwprintw(coinviewwin, y-2, x*0.8+1, std::ctime(&real_time));
    wattroff(coinviewwin, A_BOLD);
  }

  const char* View::currencysymbol(CURRENCY c)
  {
    switch (c) {
      case EUR:
        return "€";
        break;
      case USD:
        return "$";
        break;
      default:
        return CTOS.at(c).c_str();
    }
  }

  const char* View::choosegraphsymbol(double oldp, double curp, double nexp)
  {
    if (oldp > curp && nexp < curp) {
      return "\\";
    } else if (oldp < curp && nexp > curp) {
      return "/";
    } else if ((oldp >= curp && nexp >= curp)
        || (oldp <= curp && nexp <= curp)) {
      return "_";
    } else {
      return "*";
    }
  }

  void View::debugmessage(const char *msg)
  {
    wclear(debugwin);
    box(debugwin, 0, 0);
    mvwprintw(debugwin, 1, 1, msg);
    wrefresh(debugwin);
  }

}; // namespace cwo
