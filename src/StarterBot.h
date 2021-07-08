#pragma once
#include <BWAPI.h>
#include "Build_Interface.h"
#include "Unit_Movement.h"
#include "Worker_Defense.h"
#include "Mine_Interface.h"

#define SIZE_BUILDERS 5
#define SIZE_BUILD_STATUS 6

// Remember not to use "Broodwar" in any global class constructor!



class StarterBot : public BWAPI::AIModule
{
public:
  // Virtual functions for callbacks, leave these as they are../
  virtual void onStart();
  virtual void onEnd(bool isWinner);
  virtual void onFrame();
  virtual void onSendText(std::string text);
  virtual void onReceiveText(BWAPI::Player player, std::string text);
  virtual void onPlayerLeft(BWAPI::Player player);
  virtual void onNukeDetect(BWAPI::Position target);
  virtual void onUnitDiscover(BWAPI::Unit unit);
  virtual void onUnitEvade(BWAPI::Unit unit);
  virtual void onUnitShow(BWAPI::Unit unit);
  virtual void onUnitHide(BWAPI::Unit unit);
  virtual void onUnitCreate(BWAPI::Unit unit);
  virtual void onUnitDestroy(BWAPI::Unit unit);
  virtual void onUnitMorph(BWAPI::Unit unit);
  virtual void onUnitRenegade(BWAPI::Unit unit);
  virtual void onSaveGame(std::string gameName);
  virtual void onUnitComplete(BWAPI::Unit unit);
  // Everything below this line is safe to modify.//..


  
    //updates../
  void StarterBot::building_update();
  void StarterBot::gateway_procedures();
  void StarterBot::upgrade();
    //..
    //build buildings../
  void StarterBot::gateway_man();
  void StarterBot::tech_up();
  void StarterBot::get_gas();
  void StarterBot::buildAdditionalSupply();
    //..

  void StarterBot::observer_move();

    //utilities..//
  bool StarterBot::delay(int interval, static int &i);
  BWAPI::Unitset StarterBot::unitset_type(BWAPI::UnitType t);
  void StarterBot::recharge();
  void StarterBot::shuttle_to_batt();
    //..

  void StarterBot::harvester_check();
  void StarterBot::harvester_killed(BWAPI::Unit unit);
  void StarterBot::update_worker_idle(BWAPI::Unit unit);
bool StarterBot::supply_blocked();
BWAPI::Unit StarterBot::unit_of_type(BWAPI::UnitType t);

  //Counting  Stuff../

  BWAPI::Unitset enemy_warp;

  std::map<BWAPI::UnitType, int> granite_unit_count;
  std::map<BWAPI::UnitType, int> granite_warp_count;
  std::map<BWAPI::UnitType, int> enemy_unit_count;
  std::map<BWAPI::UnitType, int> enemy_warp_count;

        
  void StarterBot::add_unit (BWAPI::Unit u,
                  std::map<BWAPI::UnitType, int> &);
  void StarterBot::del_unit (BWAPI::Unit u,
                  std::map<BWAPI::UnitType, int> &);

  //..

    //expansion procedures../
void StarterBot::second_exp_phase(BWAPI::TilePosition site);
void StarterBot::expansion_procedures();
bool StarterBot::can_expand();
BWAPI::TilePosition StarterBot::expand();
BWAPI::TilePosition StarterBot::get_expansion_site();
BWAPI::TilePosition StarterBot::get_middle_mineral(int siteID);
bool StarterBot::is_leftmost(int val[2], BWAPI::Unitset Minerals);
bool StarterBot::is_topmost(int val[2], BWAPI::Unitset Minerals);
BWAPI::TilePosition StarterBot::triangulate_left(int val[2]);
BWAPI::TilePosition StarterBot::triangulate_right(int val[2]);
BWAPI::TilePosition StarterBot::triangulate_top(int val[2]);
BWAPI::TilePosition StarterBot::triangulate_bottom(int val[2]);
    //..

    //nexus procedures../
void StarterBot::nexus_procedures();
    //mine procedures../
void StarterBot::mine(BWAPI::UnitInterface *const u);
void StarterBot::send_to_mineralsV2 (BWAPI::UnitInterface *const y,
                                        BWAPI::Unitset Minerals);
void StarterBot::fill_array (int array[5], BWAPI::Unitset Minerals);
void StarterBot::horizontal_fill (int val[2], 
                int array[5], BWAPI::Unitset Minerals);
void StarterBot::vertical_fill (int val[2], 
                int array[5], BWAPI::Unitset Minerals);
bool StarterBot::get_ends (int val[2], BWAPI::Unitset Minerals);
int StarterBot::get_leftmost_mineral (BWAPI::Unitset Minerals);
int StarterBot::get_rightmost_mineral (BWAPI::Unitset Minerals);
int StarterBot::get_topmost_mineral (BWAPI::Unitset Minerals);
int StarterBot::get_bottommost_mineral (BWAPI::Unitset Minerals);
int StarterBot::get_middle_mineral_lr (int left, int right,
                                         BWAPI::Unitset Minerals);
int StarterBot::get_middle_mineral_tb (int top, int bottom,
                                         BWAPI::Unitset Minerals);
 //..

    //build_probes../
void StarterBot::build_probes(BWAPI::UnitInterface *const u);
    //..
    //..

    //AlbertaBOT StarterBot Building Tools../
void StarterBot::build_supply();
int StarterBot::GetTotalSupply(bool inProgress);
void StarterBot::build(BWAPI::UnitType structure,
	BWAPI::TilePosition desiredPos, int range);
bool StarterBot::BuildBuilding(BWAPI::UnitType type,
                              BWAPI::TilePosition location, int range,
                              bool random, std::map<enum status, int> &map);
BWAPI::TilePosition StarterBot::get_build_position(
                                BWAPI::UnitType type,
                                BWAPI::TilePosition desiredPos, int range,
                                bool random);
void StarterBot::add_building();


   //..
private:
Unit_Mapping* m_map;
Mine_Interface * m_gather;
void StarterBot::SmartRightClick(BWAPI::Unit unit, BWAPI::Unit target);

};
