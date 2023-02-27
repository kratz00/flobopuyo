#ifndef _PUYOCOMMANDER
#define _PUYOCOMMANDER

extern "C" {
    #include "sofont.h"
    #include "menu.h"
    #include "scrollingtext.h"
    #include "PuyoDoomMelt.h"
}

#include "corona32.h"
#define CORONA_HEIGHT 120

class PuyoDrawable
{
  public:
    virtual void draw() = 0;
};

class PuyoCommander
{
 public:
  PuyoCommander(bool fullscreen,bool sound, bool audio);

  void run();
  void updateAll(PuyoDrawable *starter);

  void showGameOver() { menu_show(gameOverMenu); }
  void hideGameOver() { menu_hide(gameOverMenu); }

  bool changeControlLoop(int controlIndex, PuyoDrawable *starter);
  void controlsMenuLoop(PuyoDrawable *d);
  void optionMenuLoop(PuyoDrawable *d = NULL);
  void backLoop(Menu *menu, PuyoDrawable *starter = NULL);
  void startSingleGameLoop();
  void startTwoPlayerGameLoop();
  void enterStringLoop(Menu *menu, const char *kItem, char out[256]);

  Menu *gameOverMenu;
  
  Menu *gameOver2PMenu;
  Menu *gameOver1PMenu;
  Menu *nextLevelMenu;
  Menu *finishedMenu;
  Menu *looserMenu;
  Menu *highScoresMenu;
  void updateHighScoresMenu(int newOne = -1);

  SoFont *smallFont;
  SoFont *darkFont;
  SoFont *menuFont;
  DoomMelt *melt;

 private:
  Menu *mainMenu;
  Menu *singleGameMenu;
  Menu *twoPlayerGameMenu;
  Menu *optionMenu;
  Menu *controlsMenu;
  Menu *rulesMenu;
  Menu *aboutMenu;
  ScrollingText *scrollingText;
  unsigned int cycle, start_time;
  unsigned int lastRenderedCycle;
  unsigned int maxFrameDrop;

  static const int player1Joystick = 1;
  static const int player2Joystick = 0;
  
  int      *corona_screen;
  Corona32 *corona;
  
};


extern SDL_Surface *display;
extern class PuyoCommander *theCommander;

extern const char *kYouDidIt;
extern const char *kNextLevel;
extern const char *kLooser;
extern const char *kCurrentLevel;
extern const char *kContinueLeft;
extern const char *kGameOver;
extern const char *kYouGotToLevel;
extern const char *kHitActionToContinue;
extern const char *kContinue;
extern const char *kPlayer;
extern const char *kScore;
extern const char *kContinueGame;
extern const char *kAbortGame;
extern const char *kOptions;

#endif // _PUYOCOMMANDER
