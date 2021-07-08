#include <math.h>
#include "Unit_Movement.h"

using namespace BWAPI;

    //Circle Unit../
void Unit_Movement::circle_unit(BWAPI::Unit u, BWAPI::Unit targ)
{
	int dn, dp, cx, cy;
        Position *prev = new Position(0,0), *next =  new Position(0,0);
        Position next_click; 
        
        locate(u, targ, prev, next);
        dn = u->getDistance(*next);
        dp = u->getDistance(*prev);
        
        Broodwar->drawBoxMap(next->x - 4, next->y - 4, next->x + 4, next->y + 4,
                        Color::Color(0,255,0), true);
        Broodwar->drawBoxMap(prev->x - 4, prev->y - 4, prev->x + 4, prev->y + 4,
                        Color::Color(255,255,0), true);
        next_click = locate(u, targ, prev, next);
		Broodwar->drawBoxMap(next_click.x - 4, next_click.y - 4, next_click.x + 4, next_click.y + 4,
			Color::Color(0, 255, 255), true);

        u->move(next_click);
}//..

    //Locate Corner Points to Move to and From../
BWAPI::Position Unit_Movement::locate(BWAPI::Unit u, BWAPI::Unit targ,
                                         Position *prev, Position *next)
{//get four corners of target and compare distance to unit to decipher points
        int top, left, right, bottom;
        int unit_x, unit_y, targ_x, targ_y;
        int closest, second;
		int dn, dp, i;
        Position dir[4];

        top = targ->getTop() - 12;
        left = targ->getLeft() - 12;
        right = targ->getRight() + 12;
        bottom = targ->getBottom() + 12;  

        /*ne*/ dir[0] = Position(right, top);
        /*se*/ dir[1] = Position(right, bottom);
        /*sw*/ dir[2] = Position(left, bottom);
        /*nw*/ dir[3] = Position(left, top);

        closest = find_closest(u, dir);
        unit_x = u->getPosition().x;
        unit_y = u->getPosition().y;
        targ_x = targ->getPosition().x;
        targ_y = targ->getPosition().y;

        i = get_prev_next(dir, closest, unit_x, unit_y, targ_x, targ_y, prev, next);

        dn = u->getDistance(*next);
        dp = u->getDistance(*prev);
		if (dp > dist_btwn(prev, next) - 1) {
			if (i == 3) {
				next->x = dir[0].x;
				next->y = dir[0].y;
			}
			else {
				next->x = dir[i + 1].x;
				next->y = dir[i + 1].y;
			}
		}
        return get_click_points(next, dn, dp, unit_x, unit_y, targ_x, targ_y,
                                                                        dir);
}//..

int Unit_Movement::dist_btwn(BWAPI::Position *next, BWAPI::Position *prev) 
{
        return sqrt(pow((next->x - prev->x), 2) + pow((next->y - prev->y), 2));
}

BWAPI::Position Unit_Movement::get_click_points(BWAPI::Position *next, int dn,
                int dp, int unit_x, int unit_y, int targ_x, int targ_y,
                                                        BWAPI::Position dir[4])
{
        int cx = 0, cy = 0;

        if ( unit_y < targ_y )// North half
        {
                if ( unit_x > dir[3].x && unit_x < dir[0].x ) // nw-*-ne
                {
					cx = next->x + (dn + 10) *2;
                        cy = next->y + dp;
                }
        } else { // south side
                if ( unit_x > dir[2].x && unit_x < dir[1].x ) // sw-*-se
                {
					cx = next->x - (dn + 10) *2;
                        cy = next->y - dp;
                }
        }
        if ( unit_x < targ_x ) //west half
        {
                if ( unit_y < dir[2].y && unit_y > dir[3].y ) // sw-*-nw
                {
                        cx = next->x + dp;
						cy = next->y - (dn + 10) *2;
                }
        } else { // east side
                if ( unit_y < dir[1].y && unit_y > dir[0].y ) // se-*-ne
                {
                        cx = next->x - dp;
						cy = next->y + (dn + 10) *2;
                }
        }
        if (cx == 0)
        {
			if (unit_x < targ_x) //west half
			{
				if (unit_y < targ_y) // north
				{
					cx = next->x + 
                         (dist_btwn(next, &Position(targ_x, targ_y))+10) * 2;
					cy = next->y - 
                         (dist_btwn(next, &Position(targ_x, targ_y))+10) * 2;
				} else { //south
					cx = next->x - 
                         (dist_btwn(next, &Position(targ_x, targ_y))+10) * 2;
					cy = next->y - 
                         (dist_btwn(next, &Position(targ_x, targ_y))+10) * 2;
				}
			}
			else { //east side
				if (unit_y < targ_y) // north
				{
					cx = next->x + 
                           (dist_btwn(next, &Position(targ_x, targ_y))+10)*2;
					cy = next->y + 
                           (dist_btwn(next, &Position(targ_x, targ_y))+10)*2;
				}
				else { //south
					cx = next->x -
                           (dist_btwn(next, &Position(targ_x, targ_y))+10)*2;
					cy = next->y + 
                           (dist_btwn(next, &Position(targ_x, targ_y))+10)*2;
				}
			}
        }
        return Position{cx, cy};
}

    //Find Points with Acquired Data../
int Unit_Movement::get_prev_next(Position dir[4], int closest, int unit_x,
                                        int unit_y, int targ_x, int targ_y, 
                                                Position *prev, Position *next)
{//Adhere to different conditions based on unit position
        if ( unit_y < targ_y ) // North half
        {
                if ( unit_x > dir[closest].x )
                {
                        prev->x = dir[closest].x;
                        prev->y = dir[closest].y;
                        if (closest == 3)
                        {
                                next->x = dir[0].x;
                                next->y = dir[0].y;
								return 0;
                        } else {
                                next->x = dir[closest+1].x;
                                next->y = dir[closest+1].y;
								return closest + 1;
                        }
                } else {
                        next->x = dir[closest].x;
                        next->y = dir[closest].y;
                        if (closest == 0)
                        {
                                prev->x = dir[3].x;
                                prev->y = dir[3].y;
                        } else {
                                prev->x = dir[closest-1].x;
                                prev->y = dir[closest-1].y;
                        }
						return closest;
                }
        } else { //south side
                if ( unit_x < dir[closest].x )
                {
                        prev->x = dir[closest].x;
                        prev->y = dir[closest].y;
                        if (closest == 3)
                        {
                                next->x = dir[0].x;
                                next->y = dir[0].y;
								return 0;
                        } else {
                                next->x = dir[closest+1].x;
                                next->y = dir[closest+1].y;
								return closest+1;
                        }
                } else {
                        next->x = dir[closest].x;
                        next->y = dir[closest].y;
                        if (closest == 0)
                        {
                                prev->x = dir[3].x;
                                prev->y = dir[3].y;
                        } else {
                                prev->x = dir[closest-1].x;
                                prev->y = dir[closest-1].y;
                        }
						return closest;
						
                }
        }
}//..
        
    //Get Closest Corner Point../
int Unit_Movement::find_closest(BWAPI::Unit u, Position dir[4])
{//simple min loop
        int closest_distance;
        int closest_point;
       
        closest_distance = u->getDistance(dir[0]);
        closest_point = 0;
        for ( int i = 1; i < 4; i++ )
        {
                if ( u->getDistance(dir[i]) < closest_distance )
                {
                        closest_distance = u->getDistance(dir[i]);
                        closest_point = i;
                } 
        }

        return closest_point;
}//..

    //Get Second Closest Corner Point../
int Unit_Movement::find_second(BWAPI::Unit u, Position dir[4], int closest)
{//simple min loop skipping over closest
        int closest_distance = 9999999;
		int closest_point;
        
        for ( int i = 0; i < 4; i++ )
        {
                if (i == closest)
                        continue;
                if ( u->getDistance(dir[i]) < closest_distance )
                {
                        closest_distance = u->getDistance(dir[i]);
                        closest_point = i;
                } 
        }
        return closest_point;
}//..
































//OLD RING AROUND THE ROSY../
/* 
 * Create a ring of positions
 *
 *  ring[12] 
 *    each number is an iteration for array of positions
 *
 *              3
 *          2       4
 *       0             6
 *        11          7
 *          10      8
 *              9
 *
 *    To move clockwise move towards the higher iteration of 2 closest points
 *    counter clockwise towards lower.
 *
 *    include bounds check for 0 and max to complete loop
 */

//Ring Around the Rosy../
void Unit_Movement::ring_around_the_rosy(BWAPI::Unit u, BWAPI::Unit rosy,
                                                          bool clockwise)
{
	if (!rosy)
		return;
	BWAPI::Position unit_pos;
	int radius;
	int center_x, left_x_bound, right_x_bound;
	BWAPI::Position ring[MAX_I];
	int calcpoint[MAX_I / 2 + 1];
	int calcpoint2[MAX_I / 2 + 1];
        BWAPI::Position click_points[MAX_I];

	unit_pos = rosy->getPosition(); // center of circular path
	radius = unit_radius(rosy);     // radius of circular path
	center_x = unit_pos.x;          // center x coordinate

	if (center_x - radius < 0)
		return;
	get_xcalc_points(radius, center_x, calcpoint);
	get_ycalc_points(radius, unit_pos.y, calcpoint2);
	get_travel_points(unit_pos, radius, calcpoint, calcpoint2, ring);
        get_click_points(ring, clockwise, radius, unit_pos, click_points);
	draw(unit_pos, ring);
    to_next(u, ring, click_points, clockwise);
}//..

    //Find  Radius (corner to corner length of unit)../
int Unit_Movement::unit_radius(BWAPI::Unit u)
{
        double result;

        result = sqrt( pow(u->getRight() - u->getLeft(), 2) +
                       pow(u->getBottom() - u->getTop(), 2) );
        return int(result/1.5);        
}//..

    //Find x values  to calculate points in circle../
void Unit_Movement::get_xcalc_points( int radius, int center_x,
                                                 int calcpoint[MAX_I/2+1])
{
        int tmp, i, seg_len, start;

        seg_len = (2*radius)/(MAX_I/2);
        start = center_x - radius; 

        for ( i = 0; i <= MAX_I/2; i++ )
        {
                calcpoint[i] = start;
                start += seg_len;
        }
}//..

    //Find y values  to calculate points in circle../
void Unit_Movement::get_ycalc_points( int radius, int center_y,
                                                 int calcpoint[MAX_I/2+1])
{
        int tmp, i, seg_len, start;

        seg_len = (2*radius)/(MAX_I/2);
        start = center_y - radius; 

        for ( i = 0; i <= MAX_I/2; i++ )
        {
                calcpoint[i] = start;
                start += seg_len;
        }
}//..

    //Get Points To travel to../
void Unit_Movement::get_travel_points(BWAPI::Position center, int radius,
                        int calcpoint[MAX_I/2+1], int calcpoint2[MAX_I/2+1],
                                                BWAPI::Position ring[MAX_I])
{
	double trv_pts[MAX_I][2];
        int xprime, yprime, i;
        int x = center.x;
        int y = center.y;

        for ( i = 0; i < MAX_I/4; i++)
        {
                trv_pts[i][0] = calcpoint[i+3];
                trv_pts[i][1] = y + 
                sqrt( pow(radius, 2) - pow((calcpoint[i+3]-x), 2) );
        }
        for (; i < MAX_I/2; i++)
        {
                trv_pts[i][1] = calcpoint2[MAX_I/2+2-i];
                trv_pts[i][0] = x + 
                sqrt( pow(radius, 2) - 
                                  pow((calcpoint2[MAX_I/2+2-i] - y), 2) );
                
        }
        for (; i < MAX_I - MAX_I/4; i++)
        {
                trv_pts[i][0] = calcpoint[(MAX_I/2 + MAX_I/4) + 2 - i];
                trv_pts[i][1] = y - sqrt( pow(radius, 2) -
                     pow((calcpoint[(MAX_I / 2 + MAX_I / 4) + 2 - i] - x), 2) );
        }
        for (; i < MAX_I; i++)
        {
                trv_pts[i][1] = calcpoint2[( i - (MAX_I/2 + MAX_I/4))+3];
                trv_pts[i][0] = x -
                sqrt(pow(radius, 2) -
                        pow((calcpoint2[(i - (MAX_I/2 + MAX_I/4))+3] - y), 2));
        }

        for (i = 0; i < MAX_I; i++)
        {
                xprime = x + (x - trv_pts[i][0]) * cos(.79) -
                                     (y - trv_pts[i][1]) * sin(.79);
                yprime = y + (x - trv_pts[i][0]) * sin(.79) +
                                (y - trv_pts[i][1]) * cos(.79);
               if (xprime > x)
                        xprime += radius/4;
                else
                        xprime -= radius/4;
                if (yprime > y)
                        yprime += radius/4;
                else
                        yprime -= radius/4;
                ring[i] = BWAPI::Position{ xprime, yprime };
        }
}//..

    //Get Click Points../
void Unit_Movement::get_click_points(BWAPI::Position ring[MAX_I],
                bool clockwise, int radius,
                BWAPI::Position center, BWAPI::Position click_points[MAX_I])
{
        int x = center.x;
        int y = center.y;

        for ( int i = 0; i < MAX_I; i++ )
        {
                int xprime = x + (x - ring[i].x) * cos(.85) -
                                     (y - ring[i].y) * sin(.85);
                int yprime = y + (x - ring[i].x) * sin(.85) +
                                (y - ring[i].y) * cos(.85);
                if (xprime > x)
                        xprime += 400 -radius*6;
                else
                        xprime -= 400 - radius * 6;
                if (yprime > y)
                        yprime += 400 -radius *6;
                else
                        yprime -= 400 - radius * 6;
                click_points[i] = BWAPI::Position{ xprime, yprime };
        }
}//..

    //Move to next point on ring../
void Unit_Movement::to_next(BWAPI::Unit u, BWAPI::Position ring[MAX_I],
                           BWAPI::Position click_points[MAX_I], bool clockwise)
{
        int closest, second;

        closest = get_closest(u, ring);
        second = get_second_closest(closest, u, ring);
        if (clockwise)
                move_clockwise(u, ring, closest, second);
        else
                move_counter_clockwise(u, ring, closest, second);
        
}//..

    //Move Clockwise../
void Unit_Movement::move_clockwise(BWAPI::Unit u, BWAPI::Position ring[MAX_I],
                                                         int closest, int second)
{

        if ( closest > second )
        {
                if (closest == 19)
                        u->move(ring[0]);
                else
                        u->move(ring[closest+1]);
        } else {
                if (second == 19)
                        u->move(ring[0]);
                else
                        u->move(ring[second+1]);
        }
}//..

    //Move Counter Clockwise../
void Unit_Movement::move_counter_clockwise(BWAPI::Unit u,
                   BWAPI::Position click_points[MAX_I], int closest, int second)
{
        if ( closest < second )
        {
			if (closest < 2)
				closest = 19;
      
                        u->move(click_points[closest-1]);
        } else {
			if (second < 2)
				second = 19;

                        u->move(click_points[second-1]);
        }
}//..

    //Distance between two positions../
int Unit_Movement::distance(BWAPI::Position a, BWAPI::Position b)
{
	int x, y;
        x = b.x - a.x;
        y = b.y - a.y;
        return int(sqrt( pow(x, 2) + pow(y, 2) ));
}//..

    //Get Closest../
int Unit_Movement::get_closest(BWAPI::Unit u, BWAPI::Position ring[MAX_I])
{
        int closest;
        int dist, distance_a, distance_b;

        for ( int i = 0; i < MAX_I; i++ )
        {
                dist = u->getDistance(ring[i]);
                if ( dist < distance_a)
                {
                        distance_a = dist;
                        closest = i;
                }
        }
        return closest;
}//..

    //Get Second Closest../
int Unit_Movement::get_second_closest(int closest,
                        BWAPI::Unit u, BWAPI::Position ring[MAX_I])
{
        int second;
        int dist, distance_a, distance_b;
        for ( int i = 0; i < MAX_I; i++ )
        {
                if ( i == closest )
                        continue;
                dist = u->getDistance(ring[i]);
                if ( dist < distance_b)
                {
                        distance_b = dist;
                        second = i;
                }
        }
        return second;
}//..

    //Draw Ring../
void Unit_Movement::draw_ring(BWAPI::Unit rosy)
{
	if (!rosy)
		return;
        BWAPI::Position unit_pos;
        int radius;
        int center_x, left_x_bound, right_x_bound;
        BWAPI::Position ring[MAX_I];
        int calcpoint[MAX_I/2+1];
        int calcpoint2[MAX_I/2+1];

        unit_pos = rosy->getPosition(); // center of circular path
        radius = unit_radius(rosy);     // radius of circular path
        center_x = unit_pos.x;          // center x coordinate

        if ( center_x - radius < 0 )
                return;
        get_xcalc_points(radius, center_x, calcpoint);
	get_ycalc_points(radius, unit_pos.y, calcpoint2);
	get_travel_points(unit_pos, radius, calcpoint, calcpoint2, ring);
        draw(unit_pos, ring);

}//..

    //Draw to map../
void Unit_Movement::draw(BWAPI::Position center, BWAPI::Position ring[MAX_I])
{
        BWAPI::Broodwar->drawCircleMap(center.x, center.y, 5,
                                        BWAPI::Color::Color(200,0,0), true);
        for ( int i = 0; i < MAX_I; i++ )
                BWAPI::Broodwar->drawTextMap(ring[i].x, ring[i].y, "%i", i,
					BWAPI::Color::Color(0, 12*i, 180/(i+1)), true);
}//..
 //..   
