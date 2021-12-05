#pragma once

#include <unordered_map>

#include "sl_types.hpp"

namespace tb
{
namespace colors
{
// bgcolor(Color(Color::Palette256(it.index_256)))
struct Gruvbox
{
    static constexpr std::tuple<U32, U32, U32> dark0_hard = {29, 32, 33};
    static constexpr std::tuple<U32, U32, U32> dark0 = {40, 40, 40};
    static constexpr std::tuple<U32, U32, U32> dark0_soft = {50, 48, 47};
    static constexpr std::tuple<U32, U32, U32> dark1 = {60, 56, 54};
    static constexpr std::tuple<U32, U32, U32> dark2 = {80, 73, 69};
    static constexpr std::tuple<U32, U32, U32> dark3 = {102, 92, 84};
    static constexpr std::tuple<U32, U32, U32> dark4 = {124,111,100};
    static constexpr std::tuple<U32, U32, U32> gray_245 = {146,131,116};
    static constexpr std::tuple<U32, U32, U32> gray_244 = {146,131,116};
    static constexpr std::tuple<U32, U32, U32> light0_hard = {249,245,215};
    static constexpr std::tuple<U32, U32, U32> light0 = {251,241,199};
    static constexpr std::tuple<U32, U32, U32> light0_soft = {242,229,188};
    static constexpr std::tuple<U32, U32, U32> light1 = {235,219,178};
    static constexpr std::tuple<U32, U32, U32> light2 = {213,196,161};
    static constexpr std::tuple<U32, U32, U32> light3 = {189,174,147};
    static constexpr std::tuple<U32, U32, U32> light4 = {168,153,132};
    static constexpr std::tuple<U32, U32, U32> bright_red = {251, 73, 52};
    static constexpr std::tuple<U32, U32, U32> bright_green = {184,187, 38};
    static constexpr std::tuple<U32, U32, U32> bright_yellow = {250,189, 47};
    static constexpr std::tuple<U32, U32, U32> bright_blue = {131,165,152};
    static constexpr std::tuple<U32, U32, U32> bright_purple = {211,134,155};
    static constexpr std::tuple<U32, U32, U32> bright_aqua = {142,192,124};
    static constexpr std::tuple<U32, U32, U32> bright_orange = {254,128, 25};
    static constexpr std::tuple<U32, U32, U32> neutral_red = {204, 36, 29};
    static constexpr std::tuple<U32, U32, U32> neutral_green = {152,151, 26};
    static constexpr std::tuple<U32, U32, U32> neutral_yellow = {215,153, 33};
    static constexpr std::tuple<U32, U32, U32> neutral_blue = {69,133,136};
    static constexpr std::tuple<U32, U32, U32> neutral_purple = {177, 98,134};
    static constexpr std::tuple<U32, U32, U32> neutral_aqua = {104,157,106};
    static constexpr std::tuple<U32, U32, U32> neutral_orange = {214, 93, 14};
    static constexpr std::tuple<U32, U32, U32> faded_red = {157,  0,  6};
    static constexpr std::tuple<U32, U32, U32> faded_green = {121,116, 14};
    static constexpr std::tuple<U32, U32, U32> faded_yellow = {181,118, 20};
    static constexpr std::tuple<U32, U32, U32> faded_blue = {7,102,120};
    static constexpr std::tuple<U32, U32, U32> faded_purple = {143, 63,113};
    static constexpr std::tuple<U32, U32, U32> faded_aqua = {66,123, 88};
    static constexpr std::tuple<U32, U32, U32> faded_orange = {175, 58,  3};
};

} // namespace colors
} // namespace tb

/*  GRUVCOLR         HEX       RELATV ALIAS   TERMCOLOR      RGB           ITERM RGB     OSX HEX */
/*  --------------   -------   ------------   ------------   -----------   -----------   ------- */
/*  dark0_hard       #1d2021   [   ]  [   ]   234 [h0][  ]    29- 32- 33    22- 24- 25   #161819 */
/*  dark0            #282828   [bg0]  [fg0]   235 [ 0][  ]    40- 40- 40    30- 30- 30   #1e1e1e */
/*  dark0_soft       #32302f   [   ]  [   ]   236 [s0][  ]    50- 48- 47    38- 36- 35   #262423 */
/*  dark1            #3c3836   [bg1]  [fg1]   237 [  ][15]    60- 56- 54    46- 42- 41   #2e2a29 */
/*  dark2            #504945   [bg2]  [fg2]   239 [  ][  ]    80- 73- 69    63- 57- 53   #3f3935 */
/*  dark3            #665c54   [bg3]  [fg3]   241 [  ][  ]   102- 92- 84    83- 74- 66   #534a42 */
/*  dark4            #7c6f64   [bg4]  [fg4]   243 [  ][ 7]   124-111-100   104- 92- 81   #685c51 */
/*  gray_245         #928374   [gray] [   ]   245 [ 8][  ]   146-131-116   127-112- 97   #7f7061 */
/*  gray_244         #928374   [   ] [gray]   244 [  ][ 8]   146-131-116   127-112- 97   #7f7061 */
/*  light0_hard      #f9f5d7   [   ]  [   ]   230 [  ][h0]   249-245-215   248-244-205   #f8f4cd */
/*  light0           #fbf1c7   [fg0]  [bg0]   229 [  ][ 0]   251-241-199   250-238-187   #faeebb */
/*  light0_soft      #f2e5bc   [   ]  [   ]   228 [  ][s0]   242-229-188   239-223-174   #efdfae */
/*  light1           #ebdbb2   [fg1]  [bg1]   223 [15][  ]   235-219-178   230-212-163   #e6d4a3 */
/*  light2           #d5c4a1   [fg2]  [bg2]   250 [  ][  ]   213-196-161   203-184-144   #cbb890 */
/*  light3           #bdae93   [fg3]  [bg3]   248 [  ][  ]   189-174-147   175-159-129   #af9f81 */
/*  light4           #a89984   [fg4]  [bg4]   246 [ 7][  ]   168-153-132   151-135-113   #978771 */
/*  bright_red       #fb4934   [red]   [  ]   167 [ 9][  ]   251- 73- 52   247- 48- 40   #f73028 */
/*  bright_green     #b8bb26   [green] [  ]   142 [10][  ]   184-187- 38   170-176- 30   #aab01e */
/*  bright_yellow    #fabd2f   [yellow][  ]   214 [11][  ]   250-189- 47   247-177- 37   #f7b125 */
/*  bright_blue      #83a598   [blue]  [  ]   109 [12][  ]   131-165-152   113-149-134   #719586 */
/*  bright_purple    #d3869b   [purple][  ]   175 [13][  ]   211-134-155   199-112-137   #c77089 */
/*  bright_aqua      #8ec07c   [aqua]  [  ]   108 [14][  ]   142-192-124   125-182-105   #7db669 */
/*  bright_orange    #fe8019   [orange][  ]   208 [  ][  ]   254-128- 25   251-106- 22   #fb6a16 */
/*  neutral_red      #cc241d   [   ]  [   ]   124 [ 1][ 1]   204- 36- 29   190- 15- 23   #be0f17 */
/*  neutral_green    #98971a   [   ]  [   ]   106 [ 2][ 2]   152-151- 26   134-135- 21   #868715 */
/*  neutral_yellow   #d79921   [   ]  [   ]   172 [ 3][ 3]   215-153- 33   204-136- 26   #cc881a */
/*  neutral_blue     #458588   [   ]  [   ]    66 [ 4][ 4]    69-133-136    55-115-117   #377375 */
/*  neutral_purple   #b16286   [   ]  [   ]   132 [ 5][ 5]   177- 98-134   160- 75-115   #a04b73 */
/*  neutral_aqua     #689d6a   [   ]  [   ]    72 [ 6][ 6]   104-157-106    87-142- 87   #578e57 */
/*  neutral_orange   #d65d0e   [   ]  [   ]   166 [  ][  ]   214- 93- 14   202- 72- 14   #ca480e */
/*  faded_red        #9d0006   [   ]   [red]   88 [  ][ 9]   157-  0-  6   137-  0-  9   #890009 */
/*  faded_green      #79740e   [   ] [green]  100 [  ][10]   121-116- 14   102- 98- 13   #66620d */
/*  faded_yellow     #b57614   [   ][yellow]  136 [  ][11]   181-118- 20   165- 99- 17   #a56311 */
/*  faded_blue       #076678   [   ]  [blue]   24 [  ][12]     7-102-120    14- 83-101   #0e5365 */
/*  faded_purple     #8f3f71   [   ][purple]   96 [  ][13]   143- 63-113   123- 43- 94   #7b2b5e */
/*  faded_aqua       #427b58   [   ]  [aqua]   66 [  ][14]    66-123- 88    53-106- 70   #356a46 */
/*  faded_orange     #af3a03   [   ][orange]  130 [  ][  ]   175- 58-  3   157- 40-  7   #9d2807 */
