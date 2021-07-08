#include "Build_Interface.h"
#include "Unit_Mapping.h"

using namespace BWAPI;
using namespace Filter;

// Build_Interface
//Private
BWAPI::Unit Build_Interface::find_builder(Unit_Mapping * map)// ..//
{
        BWAPI::Unit builder = BWAPI::Broodwar->getClosestUnit(
                        BWAPI::Position(settled_pos),
                        BWAPI::Filter::IsCompleted &&
                        BWAPI::Filter::IsOwned &&
                        BWAPI::Filter::IsWorker &&
                       (BWAPI::Filter::IsGatheringMinerals ||
                        BWAPI::Filter::IsIdle ||
                        BWAPI::Filter::IsConstructing));
        map->set_task(builder, BUILD);
        return builder;
}//..

BWAPI::TilePosition Build_Interface::find_pos()// ..//
{
        bool buildingOnCreep = false;
        if ( random )
               randomize_pos(desired_pos);
        BWAPI::TilePosition buildPos = 
                BWAPI::Broodwar->getBuildLocation( type, 
                                                desired_pos, 
                                                max_range, 
                                                buildingOnCreep
                                                );
        return buildPos;
}//.. 

void Build_Interface::randomize_pos(BWAPI::TilePosition &buildPos)// ..//
{
        if ( rand()%2 )
                buildPos.x = buildPos.x + rand()%5;
        else
                buildPos.x = buildPos.x - rand()%5;
        if ( rand()%2 )
                buildPos.y = buildPos.y + rand()%5;
        else
                buildPos.y = buildPos.y - rand()%5;
}//..

void Build_Interface::set_building(BWAPI::Unit warping)// ..//
{
	if (!warping)
		return;
        ID = warping->getID();
        building = warping;
}//..

//Public 
// Constructor../
Build_Interface::Build_Interface(BWAPI::UnitType a, BWAPI::TilePosition b, Unit_Mapping * map)
        :
                type(a),
                desired_pos(b),
                max_range(64),
                random(false),
                attempt(0),
                attempting(false),
                premoved(false),
                creating(false),
                completed(false)
{
        settled_pos = b;
        builder = find_builder(map);
}//..

// Constructor../
Build_Interface::Build_Interface(BWAPI::UnitType a, BWAPI::TilePosition b,
                                                        bool rand, Unit_Mapping * map)
        :
                type(a),
                desired_pos(b),
                max_range(64),
                random(rand),
                attempt(0),
                attempting(false),
                premoved(false),
                creating(false),
                completed(false)
{
        settled_pos = desired_pos;
        builder = find_builder(map);
}//..

bool Build_Interface::can_build()// ..//
{
if (!builder) {return false;}
        return builder->canBuild(type);
}//..

int Build_Interface::get_id()// ..//
{
        return ID;
}//..

bool Build_Interface::attempt_build(Unit_Mapping * map)// ..//
{
        if (!BWAPI::Broodwar->isVisible(settled_pos))
                return false;
       // std::cout << "attempting build\n";
            attempt++;
        if(!builder) {builder_killed(map);}
        if(!builder) {return false;}
        if (attempt % 300 == 299)
                settled_pos = find_pos();
        if (attempt % 900 == 899)
                random = true;
        //if (attempt % 40 == 39)
              //  builder_killed(map);
        
        if ( builder->build(type, settled_pos) )  // and goes to build
        {
                attempting = true;
                return true;
        }
        return false;
}//..

int Build_Interface::get_status()// ..//
{
        if (is_creating())
                return 1;
        if (is_attempting())
                return 2;
        if (completed == 1)
                return 3;
        if (premoved)
                return 4;
        return 0;
}//..

void Build_Interface::premove()// ..//
{
	if (premoved || !builder)
		return;
	premoved = true;
        builder->returnCargo(true);
        builder->move(BWAPI::Position(settled_pos));
}//..

BWAPI::Unit Build_Interface::get_builder()// ..//
{
        return builder;
}//..

BWAPI::TilePosition Build_Interface::get_position()// ..//
{
        return settled_pos;
}//..

BWAPI::Unit Build_Interface::get_building()// ..//
{
        return building;
}//..

BWAPI::UnitType Build_Interface::get_type()// ..//
{
        return type;
}//..

void Build_Interface::started_creating(BWAPI::Unit unit)// ..//
{
        set_building(unit);
        creating = true;
        attempting = false;
}//..

bool Build_Interface::is_creating()// ..//
{
        return creating;
}//..

void Build_Interface::builder_killed(Unit_Mapping * map)// ..//
{
        if (builder) // This Exception case doesn't do anything...
                map->set_task(builder, IDLE);
        builder = find_builder(map);
}//..

void Build_Interface::failed_creating()// ..//
{
        creating = false;
        building = nullptr;
}//..

bool Build_Interface::is_attempting()// ../
{
        if (!builder)
                return false;
        if ( builder->isIdle() || BWAPI::TilePosition(builder->getOrderTargetPosition()) !=
                                settled_pos )
                attempting = false;
        if ( attempting == true && !builder->isIdle())
                return true;
        return false;
}//..

//..
