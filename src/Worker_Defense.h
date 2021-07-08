#pragma once
#include <BWAPI.h>
#include "Unit_Mapping.h"


namespace Worker_Defense
{
        void worker_engage(Unit_Mapping *map);
        void worker_evade(Unit_Mapping *map);
        void get_enemies(BWAPI::Unitset *, BWAPI::Unitset *);
        void get_region_a(int region_a[4], BWAPI::Unit u);
        void get_region_b(int region_a[4], BWAPI::Unit u);
        BWAPI::Unitset set_difference(BWAPI::Unitset, BWAPI::Unitset );
        void insert_elements(BWAPI::Unitset, BWAPI::Unitset,
                                BWAPI::Unitset *, BWAPI::Unitset *);
        int get_mineral_direction(BWAPI::Unit);
        void get_points(BWAPI::Unit u, int &top_point,
                        int &left_point, int &bottom_point, int &right_point); 
};
