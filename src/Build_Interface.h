#pragma once
#include <BWAPI.h>
#include "Unit_Mapping.h"

class Build_Interface// ../
{
        private:
                BWAPI::UnitType type; //type of building to build
                BWAPI::TilePosition desired_pos; //position set to build
                BWAPI::TilePosition settled_pos; //position algorithm finds
                BWAPI::Unit builder;    //worker assigned to build
                BWAPI::Unit building;   //building created
                int max_range;          //range in which to build
                bool random;            //option to randomize build position
                bool attempting;        //set when build is ordered
                bool creating;          //set when building warping
                int attempt;            //# of times tried to build building
                bool completed;         //set when build is complete
                bool premoved;
                int ID;

                BWAPI::Unit find_builder(Unit_Mapping * map);
                BWAPI::TilePosition find_pos();
                void randomize_pos(BWAPI::TilePosition &buildPos);
                void set_building(BWAPI::Unit warping);

         public:
                Build_Interface(BWAPI::UnitType a, BWAPI::TilePosition b, Unit_Mapping * map);
                Build_Interface(BWAPI::UnitType a, BWAPI::TilePosition b,
					bool rand, Unit_Mapping * map);
                bool can_build();
                bool attempt_build(Unit_Mapping * map);
                int get_id();
                void premove();
                BWAPI::Unit get_builder();
                BWAPI::Unit get_building();
                BWAPI::UnitType get_type();
                void started_creating(BWAPI::Unit unit);
                bool is_creating();
                void builder_killed(Unit_Mapping * map);
                void failed_creating();
                bool is_attempting();
                int get_status();
                BWAPI::TilePosition get_position();

				
};//..

typedef Build_Interface *Build;
