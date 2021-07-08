//Header../
#include "StarterBot.h"
#include <iostream>
#include <math.h>

#define DRAWSCREEN BWAPI::Broodwar->drawBoxScreen(\
                100+j*2, 100+i*2,100+j*2+2, 100+i*2+2, \
                Color::Color(color,color,color),\
                true);


using namespace BWAPI;
using namespace Filter;

//..

//Defines..//
#define MECH_GRID_SIZE 350
#define MAX_WORKER_LIST 50
#define MAX_WORKERS 65
#define EXPANSION_INTERVAL 500
#define EXPANSION_RANGE 35
#define STANDARD_RANGE 64
//..

//Globals../
int expansion_phase = 0;
int mineral_peak;
int gas_peak;
bool scout = false;
std::map<BWAPI::Unit, BWAPI::Unitset> harvesters;
std::vector<Build> Buildings;
static double game_count[2] = { 0, 0 };


//..

//tech tree../
BWAPI::UnitType tech[20] =
{
        BWAPI::UnitTypes::Protoss_Cybernetics_Core,
        BWAPI::UnitTypes::Protoss_Forge,
        BWAPI::UnitTypes::Protoss_Robotics_Facility,
        BWAPI::UnitTypes::Protoss_Observatory,
        BWAPI::UnitTypes::Protoss_Citadel_of_Adun,
        BWAPI::UnitTypes::Protoss_Templar_Archives,
        BWAPI::UnitTypes::Protoss_Forge,
        BWAPI::UnitTypes::Protoss_Forge,
        BWAPI::UnitTypes::Protoss_Robotics_Support_Bay,
        BWAPI::UnitTypes::Terran_Academy
}; //..

//Initialize../ 
void StarterBot::onStart()
{
        Broodwar->setLocalSpeed(0);
        // Hello World!../
        Broodwar->sendText("Hello world!");
        //..

        // Print the map name../
        // BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
        Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;
        //..

        // Enable the UserInput flag../, which allows us to control the bot and type messages.
        Broodwar->enableFlag(Flag::UserInput);
        //..

        // Set the command optimization level../ so that common commands can be grouped
        // and reduce the bot's APM (Actions Per Minute).
        Broodwar->setCommandOptimizationLevel(2);
        //..

        //Check if this is a replay../
        if ( Broodwar->isReplay() )
        {

                // Announce the players in the replay../
                Broodwar << "The following players are in this replay:" << std::endl;

                // Iterate all the players in the game using a std:: iterator
                Playerset players = Broodwar->getPlayers();
                for(auto p : players)
                {
                        // Only print the player if they are not an observer
                        if ( !p->isObserver() )
                                Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
                }

        }//..
        else // if this is not a replay print match info../
        {
                // Retrieve you and your enemy's races enemy() will just return the first enemy.
                // If you wish to deal with multiple enemies then you must use enemies().
                if ( Broodwar->enemy() ) // First make sure there is an enemy
                        Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
        }//..
        //..

        if (Broodwar->mapFileName() == "(2)Destination1.1.scx")
            game_count[0]++;
        else
            game_count[1]++;

        m_map = new Unit_Mapping();
        m_gather = new Mine_Interface();
        mineral_peak = 0;
        gas_peak = 0;
        Buildings.clear();
}//..

//Main../ Called-> once every game frame
void StarterBot::onFrame()
{
        static int build_interval = 0;
        static int rosy_test = 0;
        static int grid_pattern;
        static Unit scout = nullptr;
        add_building();
        Unitset Workers =
                Broodwar->getUnitsInRectangle(0, 0, 99999, 99999, IsWorker);
        if (Broodwar->self()->minerals() > mineral_peak)
                mineral_peak = Broodwar->self()->minerals();

        //PRINTSTUFF../
        for (auto &start : Broodwar->getStartLocations())  {
                Position center = Position(start) + Position(64, 48);
                Broodwar->drawBoxMap(center.x, center.y,
                                center.x + 50, center.y + 50,
                                Color::Color(100, 0, 100),
                                true);
        }
        //..


        //BoilerPlate../
        // Return if the game is a replay or is paused../
        if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
                return;
        //..

        // Prevent spamming../ run onFrame once every number of latency frames.
        // Latency frames are the number of frames before commands are processed.
        if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
                return;
        //..
        //..

        //Worker_Defense::worker_evade(map);
        Worker_Defense::worker_engage(m_map);
        buildAdditionalSupply();
        harvester_check();
        building_update();
        nexus_procedures();
        gateway_procedures();
        upgrade();
        recharge();
        observer_move();
        if (m_map->bl_empty())
            gateway_man();
        m_gather->update_mine_interface(m_map);
        BWAPI::Unit shield;
        BWAPI::Unitset units = BWAPI::Broodwar->getUnitsInRectangle(0, 0, 99999, 99999, IsOwned);
        for (auto& u : units)
        {
            if (u->getType() != BWAPI::UnitTypes::Protoss_Zealot) { continue; }

            if (u->isIdle() && u->getShields() > 40)
            {
                int random1 = (rand() % 730 + BWAPI::Broodwar->getFrameCount() / 10) * (-1 + (rand() % 2) * 2);
                int random2 = (rand() % 730 + BWAPI::Broodwar->getFrameCount() / 10) * (-1 + (rand() % 2) * 2);

                u->attack({ m_map->get_enemy_base().x + random1, m_map->get_enemy_base().y + random2});
            }
            if (u->getShields() < 10)
            {
                shield = u->getClosestUnit(IsOwned && GetType == UnitTypes::Protoss_Shield_Battery);
                if (!shield)
                    shield = u->getClosestUnit(IsOwned && GetType == UnitTypes::Protoss_Photon_Cannon);
                if (!shield)
                    continue;
                if (u->getLastCommand().getTarget() != shield)
                    u->move(shield->getPosition());
            }
        }
        //expansion_procedures();
        if (Broodwar->getFrameCount()%150 == 0)
        {
                mineral_peak = 0;
                gas_peak = 0;
        }
}//.. 

// building_update..//
void StarterBot::building_update()
{
        for ( auto & u : Buildings )
        {
                //std::cout << "buildings to place\n";
                if (!u->get_builder())
                {
                        u->builder_killed(m_map);
                }
                if ( u->is_attempting() )
                {
                    //    std::cout << "attempting\n";
                        continue;
                }
                if( u->is_creating() )
                {
                     //   std::cout << "creating\n";
                        continue;
                }
                if (mineral_peak > u->get_type().mineralPrice() - 73 &&
                                gas_peak > u->get_type().gasPrice() - 73)
                        u->premove();
                if ( !u->can_build() )
                {
                     //   std::cout << "cant build" << u->get_type() << std::endl;

                        continue;
                }
                u->attempt_build(m_map);
                // return;
        }
}//..

// add_building../
void StarterBot::add_building()
{
        if (m_map->bl_empty())
        {
                //std::cout << "list empty\n";
                return;
        }
        for (auto &u : Buildings)
        {
                if (!u->is_creating())
                        return;
        }
        Buildings.push_back(new
                        Build_Interface(m_map->get_build_type(),
                                TilePosition(m_map->get_build_loc()), m_map));
        m_map->remove_build();
}//..

// Zealot Move Turn../
//

// Gateway Man../
void StarterBot::gateway_man()
{
        static int threshold = 120;
        if ( mineral_peak > threshold )
        {
                Buildings.push_back(new 
                                Build_Interface(UnitTypes::Protoss_Gateway,
                                        Broodwar->self()->getStartLocation(), m_map));
                threshold += 80*
                        (1 + m_map->g_count(UnitTypes::Protoss_Gateway));
        }
}//..

// tech_up../
void StarterBot::tech_up()
{
        static int i = 0;
        static int threshold = 220;
        if ( m_map->g_total_count(UnitTypes::Protoss_Gateway) < 2)
                return;
        if ( tech[i] == UnitTypes::Terran_Academy )
                return;
        if ( mineral_peak > threshold && 
                        (i == 0 || m_map->g_count(tech[i-1]) > 0 ))
        {
                Buildings.push_back(new 
                                Build_Interface(tech[i++],
                                        Broodwar->self()->getStartLocation(), m_map));
                threshold += 80;
        }
}//..

// get_gas../
void StarterBot::get_gas()
{
        if ( (m_map->g_total_count(UnitTypes::Protoss_Cybernetics_Core) < 1) )
        {
                return;
        }
        BWAPI::Unitset Nexus = Broodwar->
                getUnitsInRectangle(0,0,999999,999999,IsResourceDepot && IsOwned);
        for ( auto & u: Nexus )
        {
                Unitset geysers = u->getUnitsInRadius(150, GetType == 
                                UnitTypes::Resource_Vespene_Geyser);
                if ( geysers.size() == 0)
                        continue;
                if ( Broodwar->self()->minerals() > 80 ) 
                {
                        Buildings.push_back(new 
                                        Build_Interface(
                                                UnitTypes::Protoss_Assimilator,
                                                u->getTilePosition(), m_map));
                }
        }
}//..

// check if supply blocked../
bool StarterBot::supply_blocked()
{
        const int unusedSupply =
                GetTotalSupply(false) - BWAPI::Broodwar->self()->supplyUsed();
        return unusedSupply == 0;
}//..

//buildAdditionalSupply../
void StarterBot::buildAdditionalSupply()
{
        // Get the amount of supply supply we currently have unused
        const int unusedSupply =
                GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

        if (unusedSupply > (8 + 4*
                                m_map->g_count(UnitTypes::Protoss_Gateway)))
        {
                return;
        }
        if (GetTotalSupply(true) >= 400)
                return;

        // Otherwise, we are going to build a supply provider

        Buildings.push_back( new Build_Interface(UnitTypes::Protoss_Pylon,
                                Broodwar->self()->getStartLocation(), m_map)
                        );
}//..

//GetTotalSupply../
int StarterBot::GetTotalSupply(bool inProgress)
{
        // start the calculation with completed supply
        int totalSupply = Broodwar->self()->supplyTotal();
        if (!inProgress)
                return totalSupply;

        // Check For supply units in creation
        totalSupply += m_map->g_warping_count(UnitTypes::Protoss_Pylon)*8;

        // Check if a unit has been ordered by building_interface
        for (auto& unit : Buildings)
                if (unit->get_type() == UnitTypes::Protoss_Pylon)
                        totalSupply += UnitTypes::Protoss_Pylon.supplyProvided();

        return totalSupply;
}//..
/*
//Create Expansion.../
//expansion_procedures (Main).../ 
void StarterBot::expansion_procedures()
{
static int current_expansion = 0;
static TilePosition site;
if ( expansion_phase )
{
second_exp_phase(site);
return;
}
if ( !can_expand() )
return;
if ( (Broodwar->self()->minerals() > 600) && 
(Broodwar->getFrameCount() >
current_expansion + EXPANSION_INTERVAL))
{
site = expand();
current_expansion = Broodwar->getFrameCount();
}
}//..
//second_exp_phase../
void StarterBot::second_exp_phase(BWAPI::TilePosition site)
{
static int time;
if ( expansion_phase == 1 )
{
build(UnitTypes::Protoss_Pylon, site,
EXPANSION_RANGE, true, builders[EXPAND]);
time = Broodwar->getFrameCount();
}
else if ( (expansion_phase > 6) &&
(Broodwar->getFrameCount() > (time + 50 +
UnitTypes::Protoss_Photon_Cannon.buildTime())))
{
build(UnitTypes::Protoss_Photon_Cannon, site,
STANDARD_RANGE, true, builders[EXPAND]);
}
else if ( (expansion_phase < 7) &&
(Broodwar->getFrameCount() > (time + 50 +
UnitTypes::Protoss_Pylon.buildTime())))
{
build(UnitTypes::Protoss_Shield_Battery, site,
STANDARD_RANGE, true, builders[EXPAND]);
}
return;
} 
//..
//can_expand../
bool StarterBot::can_expand()
{
Unitset Minerals =
Broodwar->getUnitsInRectangle(0,0,99999,99999, IsMineralField);

int max = 280;
for ( auto &u : Minerals )
{
if (u->getDistance( u->getClosestUnit(IsResourceDepot) ) > max)
{
return true;
}
}
return false;
}//..
//expand()../
BWAPI::TilePosition StarterBot::expand()
{
TilePosition site = get_expansion_site();
build(UnitTypes::Protoss_Nexus, site, EXPANSION_RANGE, false,
builders[EXPAND]);
return site;

} //..
//get_expansion_site../
BWAPI::TilePosition StarterBot::get_expansion_site()
{
        UnitType supplyProviderType = UnitTypes::Protoss_Nexus;
        Unitset Minerals =
                Broodwar->getUnitsInRectangle(0,0,99999,99999, IsMineralField);
        int dist = 99999;
        int siteID = 0;
        for ( auto &u : Minerals ) 
        {
                if ( u->getDistance(u->getClosestUnit(IsResourceDepot)) < 400 )
                        continue;
                if ( u->getDistance(u->getClosestUnit(IsResourceDepot)) < dist )
                {
                        dist = 
                                u->getDistance(u->getClosestUnit(IsResourceDepot));
                        siteID = u->getID();
                }
        }
        BWAPI::TilePosition targetBuildLocation = get_middle_mineral(siteID);
        // Register an event that draws the target build location
        Broodwar->registerEvent([targetBuildLocation,supplyProviderType](Game*)
                        {
                        Broodwar->drawBoxMap( Position(targetBuildLocation),
                                        Position(targetBuildLocation + supplyProviderType.tileSize()),
                                        Colors::Blue);
                        },
                        nullptr,  // condition
                        300 );  // frames to run

        Broodwar->sendText("Dist To Expansion = %i",
                        Broodwar->getUnit(siteID)->getDistance(
                                Broodwar->getUnit(siteID)->getClosestUnit(IsResourceDepot) ) );
        return targetBuildLocation;
}
//../
//get_middle_location../
BWAPI::TilePosition StarterBot::get_middle_mineral(int siteID)
{
        int val[2];
        Unit min = Broodwar->getUnit(siteID);
        Unitset Minerals = Broodwar->getUnitsInRectangle(min->getLeft() - 200,
                        min->getTop() - 200,
                        min->getRight() + 200,
                        min->getBottom() + 200,
                        IsMineralField);
        if ( get_ends(val, Minerals) )
        {
                if ( is_topmost(val, Minerals) )
                        return triangulate_top(val);
                else
                        return triangulate_bottom(val);
        } else {
                if ( is_leftmost(val, Minerals) )
                        return triangulate_left(val);
                else
                        return triangulate_right(val);
        }
}//..
//is_leftmost../
bool StarterBot::is_leftmost(int val[2], BWAPI::Unitset Minerals)
{
        int orig;
        if ( Broodwar->getUnit(val[0])->getLeft() <
                        Broodwar->getUnit(val[1])->getLeft() )
        {
                orig = Broodwar->getUnit(val[0])->getLeft();
        } else {
                orig = Broodwar->getUnit(val[1])->getLeft();
        }

        for ( auto &u : Minerals )
                if ( u->getLeft() < orig )
                        return false;
        return true;
}//..
//is_topmost../
bool StarterBot::is_topmost(int val[2], BWAPI::Unitset Minerals)
{
        int orig;
        if ( Broodwar->getUnit(val[0])->getTop() <
                        Broodwar->getUnit(val[1])->getTop() )
        {
                orig = Broodwar->getUnit(val[0])->getTop();
        } else {
                orig = Broodwar->getUnit(val[1])->getTop();
        }

        for ( auto &u : Minerals )
                if ( u->getTop() < orig )
                        return false;
        return true;
}//..
//triangulate_right../
BWAPI::TilePosition StarterBot::triangulate_right(int val[2])
{
        Unit a = Broodwar->getUnit(val[0]);
        Unit b = Broodwar->getUnit(val[1]);

        int dist = a->getDistance(b);

        int x = 100 + a->getRight();
        int y = dist/2 + a->getTop();

        Position ret{x,y};

        return TilePosition(ret);
}//..
//triangulate_left../
BWAPI::TilePosition StarterBot::triangulate_left(int val[2])
{
        Unit a = Broodwar->getUnit(val[0]);
        Unit b = Broodwar->getUnit(val[1]);

        int dist = a->getDistance(b);

        int x = a->getLeft() - 100;
        int y = dist/2 + a->getTop();

        Position ret{x,y};

        return TilePosition(ret);
}//..
//triangulate_top../
BWAPI::TilePosition StarterBot::triangulate_top(int val[2])
{
        Unit a = Broodwar->getUnit(val[0]);
        Unit b = Broodwar->getUnit(val[1]);

        int dist = a->getDistance(b);

        int x = dist/2 + a->getLeft();
        int y = a->getTop() - 100;

        Position ret{x,y};

        return TilePosition(ret);
}//..
//triangulate_bottom../
BWAPI::TilePosition StarterBot::triangulate_bottom(int val[2])
{
        Unit a = Broodwar->getUnit(val[0]);
        Unit b = Broodwar->getUnit(val[1]);

        int dist = a->getDistance(b);

        int x = dist/2 + a->getLeft();
        int y = 100 + a->getBottom();

        Position ret{x,y};

        return TilePosition(ret);
}//..
//..
*/
//nexus_procedures../
void StarterBot::nexus_procedures()
{
        Unitset Nexus = 
                Broodwar->getUnitsInRectangle(0,0,99999,99999, IsResourceDepot &&
                                IsCompleted &&
                                IsOwned);
        if (Nexus.empty())
                return;
        for ( auto &u : Nexus ) {
                build_probes(u);
                //mine(u);
        }
}
//build_probes../
void StarterBot::build_probes(BWAPI::UnitInterface *const u)
{
        if (supply_blocked())
                return;
        if (!u)
                return;
        if ( !u->isTraining() &&
                        Broodwar->canMake(UnitTypes::Protoss_Probe) &&
                        (m_map->g_count(UnitTypes::Protoss_Probe) < 14) )
        {
                u->train(UnitTypes::Protoss_Probe);
        }
} //..
//mine procedures../
//mine (Main Procedure)../
void StarterBot::mine(BWAPI::UnitInterface *const u)
{
        Unitset Workers = 
                Broodwar->getUnitsInRadius(u->getPosition(), 640, IsWorker &&
                                IsIdle && !IsEnemy);
        Unitset Minerals = 
                Broodwar->getUnitsInRadius(u->getPosition(), 280,
                                IsMineralField);
        for ( auto &y : Workers )
        {
                if (!y)
                        continue;
                if ( m_map->get_task(y) == BUILD || m_map->get_task(y) == SCOUT)
                        continue;
                if ( (!y->isCarryingGas() &&
                                        !y->isCarryingMinerals()) )
                {
                        send_to_mineralsV2(y, Minerals);
                        m_map->set_task(y, MINE);
                }
                else 
                        y->returnCargo();
        }
}//..
//send_to_mineralsV2../
void StarterBot::send_to_mineralsV2 (BWAPI::UnitInterface *const y,
                BWAPI::Unitset Minerals)
{
        static int i = 0;
        int array[5];
        fill_array(array, Minerals);
        y->move( Broodwar->getUnit(array[i++])->getPosition() ); 
        if ( i == 5 )
                i = 0;
}
//..
//fill_array../
void StarterBot::fill_array (int array[5], BWAPI::Unitset Minerals)
{
        int val[2];
        if ( get_ends(val, Minerals) ) //true for horizontal
                horizontal_fill(val, array, Minerals);
        else
                vertical_fill(val, array, Minerals);
}//..
//get_ends../
bool StarterBot::get_ends (int val[2], BWAPI::Unitset Minerals)
{
        int leftmost = get_leftmost_mineral(Minerals);
        int rightmost = get_rightmost_mineral(Minerals);
        int topmost = get_topmost_mineral(Minerals);
        int bottommost = get_bottommost_mineral(Minerals);
        int xdist = Broodwar->getUnit(rightmost)->getRight() -
                Broodwar->getUnit(leftmost)->getLeft();
        int ydist = Broodwar->getUnit(bottommost)->getBottom() -
                Broodwar->getUnit(topmost)->getTop();

        if ( ydist > xdist )
        {
                val[0] = topmost;
                val[1] = bottommost; 
                return false;
        }
        else
        {
                val[0] = leftmost;
                val[1] = rightmost;
                return true;
        }
}//..
//horizontal_fill../
void StarterBot::horizontal_fill (int val[2], 
                int array[5], BWAPI::Unitset Minerals)
{
        array[0] = val[0];
        array[4] = val[1];
        array[2] = get_middle_mineral_lr( 
                        Broodwar->getUnit(array[0])->getLeft(),
                        Broodwar->getUnit(array[4])->getRight(),
                        Minerals);
        array[1] = get_middle_mineral_lr( 
                        Broodwar->getUnit(array[0])->getLeft(),
                        Broodwar->getUnit(array[2])->getRight(),
                        Minerals);
        array[3] = get_middle_mineral_lr( 
                        Broodwar->getUnit(array[2])->getLeft(),
                        Broodwar->getUnit(array[4])->getRight(),
                        Minerals);
}//..
//vertical_fill../
void StarterBot::vertical_fill (int val[2], 
                int array[5], BWAPI::Unitset Minerals)
{
        array[0] = val[0];
        array[4] = val[1];
        array[2] = get_middle_mineral_tb( 
                        Broodwar->getUnit(array[0])->getTop(),
                        Broodwar->getUnit(array[4])->getBottom(),
                        Minerals);
        array[1] = get_middle_mineral_tb( 
                        Broodwar->getUnit(array[0])->getTop(),
                        Broodwar->getUnit(array[2])->getBottom(),
                        Minerals);
        array[3] = get_middle_mineral_tb( 
                        Broodwar->getUnit(array[2])->getTop(),
                        Broodwar->getUnit(array[4])->getBottom(),
                        Minerals);
}//..
//get_leftmost_mineral../
int StarterBot::get_leftmost_mineral (BWAPI::Unitset Minerals)
{
        int leftID = 0, left_min = 999999, left;
        for ( auto & x : Minerals )
        {
                left = x->getLeft();
                if ( left < left_min )
                {
                        left_min = left;
                        leftID = x->getID();
                }
        }
        return leftID;
}//..
//get_rightmost_mineral../
int StarterBot::get_rightmost_mineral (BWAPI::Unitset Minerals)
{
        int rightID = 0, right_max = -1, right;
        for ( auto & x : Minerals )
        {
                right = x->getRight();
                if ( right > right_max )
                {
                        right_max = right;
                        rightID = x->getID();
                }
        }
        return rightID;
}//..
//get_topmost_mineral../
int StarterBot::get_topmost_mineral (BWAPI::Unitset Minerals)
{
        int topID = 0, top_min = 99999, top;
        for ( auto & x : Minerals )
        {
                top = x->getTop();
                if ( top < top_min )
                {
                        top_min = top;
                        topID = x->getID();
                }
        }
        return topID;
}//..
//get_bottommost_mineral../
int StarterBot::get_bottommost_mineral (BWAPI::Unitset Minerals)
{
        int bottomID = 0, bottom_max = -1, bottom;
        for ( auto & x : Minerals )
        {
                bottom = x->getBottom();
                if ( bottom > bottom_max )
                {
                        bottom_max = bottom;
                        bottomID = x->getID();
                }
        }
        return bottomID;
}//..
//get_middle_mineral_lr../
int StarterBot::get_middle_mineral_lr (int left, int right,
                BWAPI::Unitset Minerals)
{
        int middleID = 0, close_dist = 999999, dist;
        for ( auto & x : Minerals )
        {
                dist = abs((left + (right - left)/2) - x->getPosition().x);
                if ( dist < close_dist )
                {
                        close_dist = dist;
                        middleID = x->getID();
                }
        }

        return middleID;
}
//..
//get_middle_mineral_tb../
int StarterBot::get_middle_mineral_tb (int top, int bottom,
                BWAPI::Unitset Minerals)
{
        int middleID = 0, close_dist = 999999, dist;
        for ( auto & x : Minerals )
        {
                dist = abs((top + (bottom - top)/2) - x->getPosition().y);
                if ( dist < close_dist )
                {
                        close_dist = dist;
                        middleID = x->getID();
                }
        }

        return middleID;
}
//..
//..
// Mine2 Procedures../
// Get all minerals within range of nexus
// Order minerals based on their distance from nexus
// Assign minerals in order an idle worker until there are no idle workers
// Send worker to minerals
//      ->
// Hold position at mineral if it is being mined
// mine the mineral.
//
//
//mine (Main Procedure)../
//..

//gateway_procedures../
void StarterBot::gateway_procedures()
{
        if (supply_blocked())
                return;
        Unitset Gateways = Broodwar->getUnitsInRectangle(0,0,999999,999999,
                        IsOwned && (GetType == UnitTypes::Protoss_Gateway) &&
                        IsCompleted);
        for ( auto & u : Gateways )
        {
                if (!u)
                        continue;
                if ( u->isTraining() || Broodwar->self()->minerals() < 100 )
                        continue;
                if ( m_map->g_count(UnitTypes::Protoss_Cybernetics_Core) == 0)
                {
                        u->train(UnitTypes::Protoss_Zealot);
                        continue;
                }
                if ( rand()% 20 < 15 )
                        u->train(UnitTypes::Protoss_Dragoon);
                else
                        u->train(UnitTypes::Protoss_Zealot);
        }
}//..

//Text Processing../
void StarterBot::onSendText(std::string text)
{

        // Send the text to the game if it is not being processed.
        Broodwar->sendText("%s", text.c_str());


        // Make sure to use %s and pass the text as a parameter,
        // otherwise you may run into problems when you use the %(percent) character!

}

void StarterBot::onReceiveText(BWAPI::Player player, std::string text)
{
        // Parse the received text
        Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}//..

//Player Left../
void StarterBot::onPlayerLeft(BWAPI::Player player)
{
        // Interact verbally with the other players in the game by
        // announcing that the other player has left.
        Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}//..

//Nuke Detect../
void StarterBot::onNukeDetect(BWAPI::Position target)
{

        // Check if the target is a valid position
        if ( target )
        {
                // if so, print the location of the nuclear strike target
                Broodwar << "Nuclear Launch Detected at " << target << std::endl;
        }
        else 
        {
                // Otherwise, ask other players where the nuke is!
                Broodwar->sendText("Where's the nuke?");
        }

        // You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}//..

//Unit Events../
//Discover../
void StarterBot::onUnitDiscover(BWAPI::Unit unit)
{
        if ( unit->getPlayer()->isEnemy(Broodwar->self()) )
                m_map->add_to_count(unit);
}//..
//Evade../
void StarterBot::onUnitEvade(BWAPI::Unit unit)
{
}//..
//Show../
void StarterBot::onUnitShow(BWAPI::Unit unit)
{
}//..
//Hide../
void StarterBot::onUnitHide(BWAPI::Unit unit)
{
}//..
//Create../
void StarterBot::onUnitCreate(BWAPI::Unit unit)
{
        m_map->add_to_count(unit); // have to check interference with discover
        if ( !unit )
                return;
        if ( unit->getType().isBuilding() )
        {
                update_worker_idle(unit);
        }

        //Verbose../
        if ( Broodwar->isReplay() )
        {
                // if we are in a replay, then we will print out the build order of the structures
                if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
                {
                        int seconds = Broodwar->getFrameCount()/24;
                        int minutes = seconds/60;
                        seconds %= 60;
                        Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
                }
        }//..
}//..
//Destroy../
void StarterBot::onUnitDestroy(BWAPI::Unit unit)
{
        m_map->remove_from_count(unit, unit->isCompleted()); 
        if(!unit->getPlayer()->isEnemy(Broodwar->self()))
        {
                for ( auto & b : Buildings )
                {
                        if ( b->get_building() == unit )
                                b->failed_creating();
                        if ( b->get_builder() == unit )
                                b->builder_killed(m_map);
                }
                harvester_killed(unit);
        }
}//..
//Morph../
void StarterBot::onUnitMorph(BWAPI::Unit unit)
{
        m_map->add_to_count(unit);
        if ( unit->getType().isBuilding() )
        {
                update_worker_idle(unit);
        }
        if ( Broodwar->isReplay() )
        {
                // if we are in a replay, then we will print out the build order
                if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
                {
                        int seconds = Broodwar->getFrameCount()/24;
                        int minutes = seconds/60;
                        seconds %= 60;
                        Broodwar->sendText("%.2d:%.2d: %s morphs a %s",
                                        minutes, seconds, unit->getPlayer()->getName().c_str(),
                                        unit->getType().c_str());
                }
        }
}//..
//Renegade../
void StarterBot::onUnitRenegade(BWAPI::Unit unit)
{
}//..
//Complete../
void StarterBot::onUnitComplete(BWAPI::Unit unit)
{   
        m_map->set_unit_shp(unit);
        if ( unit->getPlayer()->isEnemy(Broodwar->self()) )
        {
                m_map->add_to_count(unit);
                return;
        }
        Unitset empty;
        if (!unit->getPlayer()->isEnemy(Broodwar->self()))
        {
                m_map->add_to_count(unit);
                m_map->remove_from_count(unit, !unit->isCompleted());
                if (unit->getType() == UnitTypes::Protoss_Probe)
                    m_map->set_task(unit, IDLE);

                if (unit->getType() == UnitTypes::Protoss_Nexus)
                    m_gather->new_depot(unit);

                if (unit->getType() == UnitTypes::Protoss_Assimilator)
                        harvesters[unit] = empty;
                for (auto b : Buildings)
                {
                        if (b->get_id() == unit->getID())
                        {
                                Buildings.erase(std::remove(Buildings.begin(),
                                                        Buildings.end(), b));
                                break;
                        }
                }
        }
}//..
//..

//Save Game../
void StarterBot::onSaveGame(std::string gameName)
{
        Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}//..

//Match End../
void StarterBot::onEnd(bool isWinner)
{
    static double game_wins[2] = { 0, 0 };
        delete(m_map);
        // Called when the game ends
        if ( isWinner )
        {
            

            if (Broodwar->mapFileName() == "(2)Destination1.1.scx")
                game_wins[0]++;
            else
                game_wins[1]++;
                // Log your win here!
        }
        std::cout << "Destination win/loss ratio = " << ( (game_wins[0] + 1) / (game_count[0] + 1) ) * 100 << std::endl;
        std::cout << "Heartbreaker win/loss ratio = " << ( (game_wins[1] + 1) / (game_count[1] + 1) ) * 100 << std::endl;

}//..

//Delay../
bool StarterBot::delay(int interval, static int &i)
{//only can be called once per frame because of static symbol
        if (Broodwar->getFrameCount() > interval + i)
        {
                i = Broodwar->getFrameCount();
                return true;
        }
        return false;
}//..

//harvester_check../
void StarterBot::harvester_check()
{
        Unitset assimilators =
                Broodwar->getUnitsInRectangle(0,0,999999,999999, GetType ==
                                UnitTypes::Protoss_Assimilator &&
                                IsCompleted);
        for ( auto & u: assimilators )
                for ( int i = harvesters[u].size(); i < 3; i++)
                {
                        Unit worker = u->getClosestUnit(IsWorker && IsOwned &&
                                        !IsCarryingMinerals &&
                                        (IsGatheringMinerals || IsIdle));
                        if (!worker)
                                continue;
                        worker->gather(u);
                        harvesters[u].emplace(worker);
                }
}//..

//harvester_killed../
void StarterBot::harvester_killed(BWAPI::Unit unit)
{
        Unitset assimilators =
                Broodwar->getUnitsInRectangle(0,0,999999,999999, 
                                GetType == UnitTypes::Protoss_Assimilator);
        for (auto & u : assimilators)
        {
                for (auto y : harvesters[u])
                {
                        if ( unit == y )
                        {
                                harvesters[u].erase(y);
                        }
                }
        }
}//..

//update_worker_idle../
void StarterBot::update_worker_idle(BWAPI::Unit unit)
{
        Unit builder;
        for (auto & b : Buildings)
        {
                if (b->get_position() == unit->getTilePosition() &&
                                b->get_type() == unit->getType())
                {
                        b->started_creating(unit);
                        builder = b->get_builder();
                        m_map->set_task(builder, IDLE);
                        break;
                }
        }
        for (auto & b : Buildings)
        {
                if ( b->get_builder() == builder &&
                                b->get_building() != unit &&
                                !b->is_creating())
                {
                        m_map->set_task(builder, BUILD);
                }
        }
}//..

//upgrade../
void StarterBot::upgrade()
{
        Unitset units = 
                Broodwar->getUnitsInRectangle(0,0,999999,999999,
                                IsOwned && !IsResearching);
        for ( auto & u : units)
        {
                if (u->isUpgrading())
                        continue;
                if ( u->getType() == UnitTypes::Protoss_Forge )
                {
                        u->upgrade(UpgradeTypes::Protoss_Ground_Weapons);
                        u->upgrade(UpgradeTypes::Protoss_Ground_Armor);
                        u->upgrade(UpgradeTypes::Protoss_Plasma_Shields);
                }
                if ( u->getType() == UnitTypes::Protoss_Cybernetics_Core)
                        u->upgrade(UpgradeTypes::Singularity_Charge);
                if ( u->getType() == UnitTypes::Protoss_Observatory)
                {
                        u->upgrade(UpgradeTypes::Sensor_Array);
                        u->upgrade(UpgradeTypes::Gravitic_Boosters);
                }
                if ( u->getType() == UnitTypes::Protoss_Citadel_of_Adun)
                        u->upgrade(UpgradeTypes::Leg_Enhancements);
                if ( u->getType() == UnitTypes::Protoss_Robotics_Support_Bay)
                        u->upgrade(UpgradeTypes::Gravitic_Drive);
        }
}//..

//Observer_Move ../
void StarterBot::observer_move()
{
        Unit enemy;
        Unitset observers = unitset_type(UnitTypes::Protoss_Observer);

        for (auto & u : observers)
        {
                if (!u)
                        continue;
                enemy = u->getClosestUnit(IsEnemy &&
                                (GetType == UnitTypes::Protoss_Dark_Templar ||
                                 GetType == UnitTypes::Zerg_Lurker ||
                                 GetType == UnitTypes::Protoss_Observer ||
                                 GetType == UnitTypes::Terran_Ghost ||
                                 GetType == UnitTypes::Terran_Wraith));
                if(!enemy)
                        continue;
                if ( u->getShields() > 20 )
                        u->move(enemy->getPosition());
        }
}//..

//recharge../
void StarterBot::recharge()
{
        static int interval = 0;
        static int interval2 = 0;
        bool flag = false;
        Unitset batteries = unitset_type(UnitTypes::Protoss_Shield_Battery);
        if ( delay(120, interval))
                return;
       // if ( delay(150, interval2))
      //          flag = true;
        for ( auto & u : batteries )
        {

                Unit attacker = nullptr;
                attacker = u->getClosestUnit( 
                                !IsBuilding &&
                                IsOwned &&
                                (Shields_Percent < 100)
                                );
                // Don't order to recharge shields if these conditions
                if (!attacker) { continue; }
                if ( u->getDistance(attacker) > 150 ) { continue; }
                if (u->getEnergy() < 50 ) { continue; }
                if (u->getLastCommand().getTarget() == attacker) { continue; }
                if (attacker->getLastCommand().getTarget() == u) { continue; }

                attacker->rightClick(u);
        }
}
//../

//shuttle_to_batt..//
void StarterBot::shuttle_to_batt()
{
        Unitset shuttles = unitset_type(UnitTypes::Protoss_Shuttle);
        for ( auto & u : shuttles )
        {
                Unit battery = u->getClosestUnit(GetType ==
                                UnitTypes::Protoss_Shield_Battery &&
                                IsOwned &&
                                (Energy > 80)
                                );
                if(!battery)
                        continue;
                Position batt_loc = battery->getPosition();
                if (u->getSpaceRemaining() != 0 && !u->isIdle())
                        continue; //if there is space or not idle
                if(u->getDistance(battery) < 50 &&
                                u->getLastCommand().getType() != u->unloadAll())
                        u->unloadAll();//unload if within range
                else if (u->getDistance(battery) > 30)
                        u->move(batt_loc);//move to battery
        }
}//..

//unit_of_type../
BWAPI::Unit StarterBot::unit_of_type(BWAPI::UnitType t)
{
        Unitset units = unitset_type(t);
        return *begin(units);
}//.. 

//unitset_type../
BWAPI::Unitset StarterBot::unitset_type(BWAPI::UnitType t)
{
        return Broodwar->getUnitsInRectangle(0,0,9999999,9999999, GetType == t
                        && IsOwned && IsCompleted);
}//.. 