#pragma once

#include <BWAPI.h>

#define MAX_I 20 



namespace Unit_Movement
{
        
    //Circle Unit
        void circle_unit(BWAPI::Unit u, BWAPI::Unit targ);
    //Locate Corner Points to Move to and From
        BWAPI::Position locate(BWAPI::Unit u, BWAPI::Unit targ,
                                 BWAPI::Position *prev, BWAPI::Position *next);
    //Find Points with Acquired Data
        int get_prev_next(BWAPI::Position dir[4], int, int, int, int, int, 
                                  BWAPI::Position *prev, BWAPI::Position *next);
		int dist_btwn(BWAPI::Position *next, BWAPI::Position *prev);
    //Calculate click points
		BWAPI::Position get_click_points(BWAPI::Position *next, int dn,
			int dp, int unit_x, int unit_y, int targ_x, int targ_y,
			BWAPI::Position dir[4]);
    //Get Closest Corner Point
        int find_closest(BWAPI::Unit u, BWAPI::Position dir[4]);
    //Get Second Closest Corner Point
        int find_second(BWAPI::Unit u, BWAPI::Position dir[4], int closest);



        //Ring Around The Rosy Insanity../
        void ring_around_the_rosy(BWAPI::Unit u, BWAPI::Unit rosy, bool);
        int unit_radius(BWAPI::Unit u);
        void get_xcalc_points( int radius, int center_x,
                               int calcpoint[MAX_I/2+1]);
        void get_ycalc_points( int radius, int center_y,
                                      int calcpoint[MAX_I/2+1]);
        void get_travel_points(BWAPI::Position center, int radius,
                       int calcpoint[MAX_I/2+1], int calcpoint2[MAX_I/2+1],
                       BWAPI::Position ring[MAX_I]);
        void to_next(BWAPI::Unit u, BWAPI::Position ring[MAX_I],
                           BWAPI::Position click_points[MAX_I], bool );
        void move_clockwise(BWAPI::Unit u, BWAPI::Position ring[MAX_I],int,int);
        void move_counter_clockwise(BWAPI::Unit u,
                                        BWAPI::Position clicpts[MAX_I],int,int);
        int distance(BWAPI::Position a, BWAPI::Position b);
        int get_closest(BWAPI::Unit u, BWAPI::Position ring[MAX_I]);
        int get_second_closest(int closest,
                        BWAPI::Unit u, BWAPI::Position ring[MAX_I]);
        void draw_ring(BWAPI::Unit rosy);
        void draw(BWAPI::Position center, BWAPI::Position ring[MAX_I]);
        void get_click_points(BWAPI::Position ring[], bool, int radius,
                BWAPI::Position center, BWAPI::Position click_points[MAX_I]);
        //..
}


