#include "Worker_Defense.h"
#include "Unit_Mapping.h"

using namespace BWAPI;

void Worker_Defense::worker_engage(Unit_Mapping *map)// ..//
{
        Unitset engage, disengage;
        get_enemies(&engage, &disengage);
        map->clear_worker_engage(disengage);
        map->set_worker_engage(engage, 
                     Broodwar->getUnitsInRectangle(0,0,999999,9999999,
                             Filter::IsOwned && Filter::IsWorker));
        for ( auto & u : engage )
        {
                for ( auto & y : map->get_defenders(u) )
                {
					if (y->getLastCommand().target != u)
					{
						y->attack(u);
					}
                }
        }
}//..

void Worker_Defense::worker_evade(Unit_Mapping *map)// ../
{ //Workers are too slow to evade adequately
        Unitset workers;

        workers = Broodwar->getUnitsInRectangle(0,0,9999999,9999999, 
                        Filter::IsWorker);

        for (auto & u: workers)
        {//check hitpoints and last frame attacked to update status
                if ( map->get_unit_shp(u) < u->getShields() + u->getHitPoints()
                                && map->get_task(u) != SCOUT)
                        map->set_under_fire(u);
				
                else if ( (map->last_frame_attacked(u) + 15 >=
                    Broodwar->getFrameCount()) && map->get_task(u) == EVADE )
                        map->set_not_under_fire(u);

		map->set_unit_shp(u);

		if (map->get_task(u) == EVADE)
			u->gather(u->getClosestUnit(Filter::IsMineralField));
        }
}//.. 

void Worker_Defense::get_enemies(Unitset *engage, Unitset *disengage)// ../
{
        int region_a[4], region_b[4];
        Unitset nexus;
        Unitset tmp, tmp1, tmp2;

        nexus = Broodwar->getUnitsInRectangle(0,0,999999,9999999, 
                                Filter::IsOwned && Filter::IsResourceDepot);
        for ( auto & u : nexus )
        {
                get_region_a(region_a, u); //engage region
                get_region_b(region_b, u); //disengage region (with engage)
                tmp = Broodwar->getUnitsInRectangle(region_a[0], region_a[1],
                                                region_a[2], region_a[3], 
                                         Filter::IsEnemy && !Filter::IsFlying); 
                tmp1 = Broodwar->getUnitsInRectangle(region_b[0], region_b[1],
                                                region_b[2], region_b[3], 
                                         Filter::IsEnemy && !Filter::IsFlying);
                tmp2 = set_difference(tmp, tmp1); // remove engage from dis
                insert_elements(tmp, tmp2, engage, disengage);
        }
}//..

void Worker_Defense::get_region_a(int a[4], BWAPI::Unit u)// ..//
{
        int top_point, left_point, bottom_point, right_point, minerals;

        minerals = get_mineral_direction(u);
        get_points(u, top_point, left_point, bottom_point, right_point);

        a[0] = left_point - 150;
        a[1] = top_point - 150;
        a[2] = right_point + 150;
        a[3] = bottom_point + 150;
        if ( minerals == 0 ) // north
                a[3] = bottom_point - 30;
        if ( minerals == 1 ) // east
                a[0] = left_point + 30;
        if ( minerals == 2 ) // south
                a[1] = top_point + 30;
        if ( minerals == 3 ) // west
                a[2] = right_point - 30;
}//..

void Worker_Defense::get_region_b(int a[4], BWAPI::Unit u)// ..//
{
        int top_point, left_point, bottom_point, right_point;

        get_points(u, top_point, left_point, bottom_point, right_point);

        a[0] = left_point - 200;
        a[1] = top_point - 200;
        a[2] = right_point + 200;
        a[3] = bottom_point + 200;

}//..

BWAPI::Unitset Worker_Defense::set_difference(BWAPI::Unitset a, BWAPI::Unitset b)// ../
{
        Unitset return_set;
		return_set = Broodwar->getUnitsInRectangle(0, 0, 9999999, 9999999, Filter::IsEnemy && !Filter::IsFlying);
        for ( auto & u : return_set)
                if ( b.contains(u) == true )
                        return_set.erase(u);
		for (auto & u : a)
			return_set.insert(u);
        return return_set;
}//..

void Worker_Defense::insert_elements(BWAPI::Unitset tmp, BWAPI::Unitset tmp2, BWAPI::Unitset *engage, BWAPI::Unitset *disengage)// ..//
{
        for (auto & u : tmp)
                engage->insert(u);
        for (auto & u : tmp2)
                disengage->insert(u);
}//..
                
int Worker_Defense::get_mineral_direction(BWAPI::Unit u)// ../
{
        int top_point, left_point, bottom_point, right_point;
        Unitset Minerals; 

        get_points(u, top_point, left_point, bottom_point, right_point);

        Minerals = u->getUnitsInRadius(200, Filter::IsMineralField);
        for ( auto & y : Minerals )
        { //check where minerals are in location to nexus points
                if ( y->getPosition().x > left_point &&
                     y->getPosition().x < right_point)
                {
                        if ( y->getPosition().y < u->getPosition().y)
                                return 0; // north
                        else
                                return 2; //south
                } else {
                        if ( y->getPosition().y > top_point &&
                             y->getPosition().y < bottom_point )
                        {
                                if ( y->getPosition().x > u->getPosition().x )
                                        return 1; //east
                                else
                                        return 3; //west
                        }
                }
        }
}//..

void Worker_Defense::get_points(BWAPI::Unit u, int &top_point,// ..//
                int &left_point, int &bottom_point, int &right_point) 
{
        int top, left, bottom, right;

        top = u->getTop(); left = u->getLeft(); 
        bottom = u->getBottom(); right = u->getRight();

        top_point = top + (bottom - top)/3;
        bottom_point = bottom - (bottom - top)/3;
        left_point = left + (right - left)/3;
        right_point = right - (right - left)/3;
}//..
